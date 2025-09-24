package web_meta2.models;

import app_meta1.Configuration;
import app_meta1.GatewayRemote;
import jakarta.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.rmi.Naming;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.concurrent.atomic.AtomicBoolean;

@Service
@EnableScheduling
public class StatsService {

    @Autowired
    private SimpMessagingTemplate messagingTemplate;

    private String rmiUrl;
    private Map<String, Integer> ultimasPesquisasFrequentes = new HashMap<>();
    private Map<String, Double> ultimosTemposMediosBarrels = new HashMap<>();
    private Map<String, Integer> ultimosBarrelsInfo = new HashMap<>();
    private AtomicBoolean firstLoad = new AtomicBoolean(true);

    @PostConstruct
    public void init() {
        rmiUrl = "rmi://" + Configuration.HOST_RMI + ":" + Configuration.PORTA_RMI;
        System.out.println("StatsService iniciado com RMI URL: {" + rmiUrl + "}");

        // Coletar estatísticas assim que o serviço iniciar
        try {
            coletarEstatisticas();
        } catch (Exception e) {
            System.err.println("Erro ao coletar estatísticas iniciais: " + e.getMessage());
        }
    }

    /**
     * Coleta estatísticas periodicamente e envia atualizações pelo WebSocket
     * quando houver mudanças ou é a primeira carga
     */
    @Scheduled(fixedRate = 5000) // Verifica a cada 5 segundos
    public void coletarEstatisticas() {
        try {
            System.out.println("Coletando estatísticas...");
            GatewayRemote gateway = (GatewayRemote) Naming.lookup(rmiUrl + "/GATEWAY");
            boolean isFirstLoad = firstLoad.get();

            // Definir o estado como já carregado
            if (isFirstLoad) {
                firstLoad.set(false);
            }

            // Obter top 10 pesquisas
            Map<String, Integer> top10Pesquisas = gateway.getTop10Pesquisas();
            if (isFirstLoad || !top10Pesquisas.equals(ultimasPesquisasFrequentes)) {
                ultimasPesquisasFrequentes = top10Pesquisas;

                // Converter para formato mais amigável para o frontend
                List<SearchTerm> searchTerms = top10Pesquisas.entrySet().stream()
                        .map(entry -> new SearchTerm(entry.getKey(), entry.getValue()))
                        .collect(Collectors.toList());

                // Enviar atualização via WebSocket
                System.out.println("Enviando atualização de top pesquisas: " + searchTerms.size() + " itens");
                messagingTemplate.convertAndSend("/topic/top-searches", searchTerms);
            }

            // Obter informações dos barrels
            Map<String, Integer> barrelsInfo = gateway.getBarrelsInfo();
            Map<String, Double> tempoMedioPorBarrel = gateway.getTempoMedioPorBarrel();

            // Sempre enviar atualizações de barrels na primeira carga
            // ou se houver mudanças nos dados
            if (isFirstLoad || !barrelsInfo.equals(ultimosBarrelsInfo) ||
                    !tempoMedioPorBarrel.equals(ultimosTemposMediosBarrels)) {

                ultimosBarrelsInfo = barrelsInfo;
                ultimosTemposMediosBarrels = tempoMedioPorBarrel;

                // Converter para formato mais amigável para o frontend
                List<BarrelInfo> barrelInfos = new ArrayList<>();
                for (Map.Entry<String, Integer> entry : barrelsInfo.entrySet()) {
                    String barrelId = entry.getKey();
                    Integer tamanho = entry.getValue();
                    Double tempoResposta = tempoMedioPorBarrel.getOrDefault(barrelId, 0.0);

                    barrelInfos.add(new BarrelInfo(
                            barrelId,
                            tamanho,
                            true, // assumimos que está ativo se temos informações
                            tempoResposta
                    ));
                }

                // Enviar atualização via WebSocket
                System.out.println("Enviando atualização de barrels: " + barrelInfos.size() + " barrels");
                messagingTemplate.convertAndSend("/topic/barrels", barrelInfos);

                // Calcular tempo médio geral (média dos tempos médios de todos os barrels)
                double tempoMedioGeral = tempoMedioPorBarrel.values().stream()
                        .mapToDouble(Double::doubleValue)
                        .average()
                        .orElse(0.0);

                // Criar objeto com estatísticas gerais
                Map<String, Object> statsGerais = new HashMap<>();
                statsGerais.put("totalPages",
                        barrelsInfo.values().stream().mapToInt(Integer::intValue).sum());
                statsGerais.put("averageSearchTime", String.format("%.2f", tempoMedioGeral * 100)); // converter para ms

                // Enviar estatísticas gerais
                System.out.println("Enviando estatísticas gerais: totalPages=" +
                        statsGerais.get("totalPages") + ", averageSearchTime=" +
                        statsGerais.get("averageSearchTime"));
                messagingTemplate.convertAndSend("/topic/stats", statsGerais);
            }

            // Status do sistema (componentes ativos)
            Map<String, Boolean> systemStatus = new HashMap<>();
            systemStatus.put("downloaderActive", true);
            systemStatus.put("barrelActive", true);
            systemStatus.put("gatewayActive", true);
            systemStatus.put("springBootActive", true);

            System.out.println("Enviando status do sistema");
            messagingTemplate.convertAndSend("/topic/system-status", systemStatus);

        } catch (Exception e) {
            System.err.println("Erro ao coletar estatísticas: " + e.getMessage());
            e.printStackTrace();

            // Se ocorrer um erro, enviar status offline
            Map<String, Boolean> systemStatus = new HashMap<>();
            systemStatus.put("downloaderActive", false);
            systemStatus.put("barrelActive", false);
            systemStatus.put("gatewayActive", false);
            systemStatus.put("springBootActive", true); // Ainda estamos executando

            messagingTemplate.convertAndSend("/topic/system-status", systemStatus);
        }
    }

    // Classes para serializar os dados para o frontend

    public static class SearchTerm {
        private String term;
        private int count;

        public SearchTerm(String term, int count) {
            this.term = term;
            this.count = count;
        }

        public String getTerm() {
            return term;
        }

        public int getCount() {
            return count;
        }
    }

    public static class BarrelInfo {
        private String id;
        private int size;
        private boolean active;
        private double tempoResposta;

        public BarrelInfo(String id, int size, boolean active, double tempoResposta) {
            this.id = id;
            this.size = size;
            this.active = active;
            this.tempoResposta = tempoResposta;
        }

        public String getId() {
            return id;
        }

        public int getSize() {
            return size;
        }

        public boolean isActive() {
            return active;
        }

        public double getTempoResposta() {
            return tempoResposta;
        }
    }
}
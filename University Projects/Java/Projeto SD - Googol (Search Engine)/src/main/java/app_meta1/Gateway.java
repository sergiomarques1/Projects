package app_meta1;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.concurrent.ConcurrentHashMap;
import java.io.*;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;


public class Gateway extends UnicastRemoteObject implements GatewayRemote{

    // Estatísticas do sistema
    private Map<String, Integer> pesquisasFrequentes; // Armazena as pesquisas e suas contagens
    private Map<String, Long> temposResposta; // Armazena os tempos de resposta totais por pesquisa
    private Map<String, Integer> contagemPesquisas; // Conta o número de vezes que cada pesquisa foi feita
    private Map<String, Map<String, Long>> temposRespostaBarrels; // Tempos por barrel
    private Map<String, Integer> contagemBarrels; // Contagem de pesquisas por barrel

    private static final String ESTATISTICAS_FILE = "estatisticas.txt";

    protected Gateway() throws RemoteException{
        super();
        this.pesquisasFrequentes = new ConcurrentHashMap<>();
        this.temposResposta = new ConcurrentHashMap<>();
        this.contagemPesquisas = new ConcurrentHashMap<>();
        this.temposRespostaBarrels = new ConcurrentHashMap<>();
        this.contagemBarrels = new ConcurrentHashMap<>();

        carregarEstatisticas();
    }

    public void enviarURL(String url){
        try{
            URLQueueRemote queueRemote = (URLQueueRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/QUEUE");
            queueRemote.adicionarURL(url);
        } catch(Exception e){
            System.err.println("Erro ao enviar URL para a queue: "+e.getMessage());
        }
    }

    // Método modificado para suportar paginação e registrar estatísticas
    public ArrayList<String> efetuarPesquisa(String pesquisa, int pagina){
        ArrayList<String> resultadoPesquisa = new ArrayList<>();
        BarrelRemote barrel = null;
        long tempoInicio = System.currentTimeMillis();
        String barrelId = "BARREL"; // ID padrão - pode ser alterado quando múltiplos barrels forem implementados

        try{
            barrel = (BarrelRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/BARREL");

            if(pagina == 1){
                registarPesquisa(pesquisa);
            }

            resultadoPesquisa = barrel.pesquisa(pesquisa, pagina, Configuration.TAM_PAGINA);

            // Registrar o tempo de resposta
            long tempoFim = System.currentTimeMillis();
            long tempoTotal = tempoFim - tempoInicio;
            registarTempoResposta(pesquisa, barrelId, tempoTotal);

        } catch(Exception e){
            System.err.println("Erro ao tentar pesquisar informação nos barrels: "+e.getMessage());
        }

        return resultadoPesquisa;
    }

    // Método adicional para obter o total de resultados
    public int getTotalResultados(String pesquisa) {
        int totalResultados = 0;
        BarrelRemote barrel = null;

        try{
            barrel = (BarrelRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/BARREL");
            totalResultados = barrel.getTotalResultados(pesquisa);
        } catch(Exception e){
            System.err.println("Erro ao tentar obter total de resultados: "+e.getMessage());
        }

        return totalResultados;
    }

    // Método adicional para obter o total de páginas
    public int getTotalPaginas(String pesquisa) {
        int totalResultados = getTotalResultados(pesquisa);
        return (int) Math.ceil((double) totalResultados / Configuration.TAM_PAGINA);
    }

    public ArrayList<String> ligacoesPagina(String url){
        ArrayList<String> ligacoes = new ArrayList<>();
        BarrelRemote barrel = null;

        try{
            barrel = (BarrelRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/BARREL");
            ligacoes = barrel.ligacoes(url);
        } catch(Exception e){
            System.err.println("Erro ao tentar pesquisar informação nos barrels: "+e.getMessage());
        }

        return ligacoes;
    }

    // Métodos para estatísticas
    private void registarPesquisa(String pesquisa) {
        pesquisasFrequentes.put(pesquisa, pesquisasFrequentes.getOrDefault(pesquisa, 0) + 1);
    }

    private void registarTempoResposta(String pesquisa, String barrelId, long tempo) {
        // Registrar tempo total por pesquisa
        temposResposta.put(pesquisa, temposResposta.getOrDefault(pesquisa, 0L) + tempo);
        contagemPesquisas.put(pesquisa, contagemPesquisas.getOrDefault(pesquisa, 0) + 1);

        // Registrar tempo por barrel
        temposRespostaBarrels.putIfAbsent(barrelId, new ConcurrentHashMap<>());
        Map<String, Long> temposBarrel = temposRespostaBarrels.get(barrelId);
        temposBarrel.put(pesquisa, temposBarrel.getOrDefault(pesquisa, 0L) + tempo);

        contagemBarrels.put(barrelId, contagemBarrels.getOrDefault(barrelId, 0) + 1);
    }

    // Métodos para retornar estatísticas ao cliente
    public Map<String, Integer> getTop10Pesquisas() throws RemoteException {
        // Ordenar as pesquisas por frequência e pegar as 10 mais comuns
        return pesquisasFrequentes.entrySet().stream()
                .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
                .limit(10)
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        Map.Entry::getValue,
                        (e1, e2) -> e1,
                        LinkedHashMap::new
                ));
    }

    public Map<String, Integer> getBarrelsInfo() throws RemoteException {
        Map<String, Integer> barrelsInfo = new HashMap<>();

        try {
            BarrelRemote barrel = (BarrelRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/BARREL");
            barrelsInfo.put("BARREL", barrel.getTamanhoIndice());
        } catch (Exception e) {
            System.err.println("Erro ao obter informações dos barrels: " + e.getMessage());
        }

        return barrelsInfo;
    }

    public Map<String, Double> getTempoMedioPorBarrel() throws RemoteException {
        Map<String, Double> tempoMedio = new HashMap<>();

        for (String barrelId : contagemBarrels.keySet()) {
            int contagem = contagemBarrels.get(barrelId);
            if (contagem > 0) {
                double tempoTotal = 0;
                Map<String, Long> temposBarrel = temposRespostaBarrels.get(barrelId);
                if (temposBarrel != null) {
                    for (long tempo : temposBarrel.values()) {
                        tempoTotal += tempo;
                    }
                }
                // Converte milissegundos para décimos de segundo
                tempoMedio.put(barrelId, (tempoTotal / contagem) / 100.0);
            }
        }

        return tempoMedio;
    }

    // Método para salvar todas as estatísticas em um único arquivo de texto
    public void salvarEstatisticas() {
        try {
            PrintWriter writer = new PrintWriter(new FileWriter(ESTATISTICAS_FILE));
            DateTimeFormatter dtf = DateTimeFormatter.ofPattern("yyyy/MM/dd HH:mm:ss");
            LocalDateTime now = LocalDateTime.now();

            writer.println("===== ESTATÍSTICAS DO GOOGOL =====");
            writer.println("Data de Gravação: " + dtf.format(now));
            writer.println();

            // Pesquisas Frequentes
            writer.println("--- PESQUISAS FREQUENTES ---");
            for (Map.Entry<String, Integer> entry : pesquisasFrequentes.entrySet()) {
                writer.println("\"" + entry.getKey() + "\": " + entry.getValue() + " pesquisas");
            }
            writer.println();

            // Tempos de Resposta
            writer.println("--- TEMPOS DE RESPOSTA POR PESQUISA (ms) ---");
            for (Map.Entry<String, Long> entry : temposResposta.entrySet()) {
                String pesquisa = entry.getKey();
                long tempoTotal = entry.getValue();
                int contagem = contagemPesquisas.getOrDefault(pesquisa, 1);
                double media = (double) tempoTotal / contagem;
                writer.println("\"" + pesquisa + "\": tempo total " + tempoTotal + " ms, média " + String.format("%.2f", media) + " ms");
            }
            writer.println();

            // Estatísticas por Barrel
            writer.println("--- ESTATÍSTICAS POR BARREL ---");
            for (String barrelId : contagemBarrels.keySet()) {
                int contagem = contagemBarrels.get(barrelId);
                writer.println("Barrel " + barrelId + ": " + contagem + " pesquisas processadas");

                Map<String, Long> temposBarrel = temposRespostaBarrels.get(barrelId);
                if (temposBarrel != null) {
                    double tempoTotal = 0;
                    for (long tempo : temposBarrel.values()) {
                        tempoTotal += tempo;
                    }
                    double media = tempoTotal / contagem;
                    writer.println("Tempo médio de resposta: " + String.format("%.2f", media) + " ms (" + String.format("%.2f", media/100.0) + " décimos de segundo)");
                }
                writer.println();
            }

            writer.close();
            System.out.println("Estatísticas salvas com sucesso em " + ESTATISTICAS_FILE);
        } catch (IOException e) {
            System.err.println("Erro ao salvar estatísticas: " + e.getMessage());
            e.printStackTrace();
        }
    }

    // Método para carregar estatísticas do arquivo
    private void carregarEstatisticas() {
        File file = new File(ESTATISTICAS_FILE);
        if (!file.exists()) {
            System.out.println("Arquivo de estatísticas não encontrado. Iniciando com dados vazios.");
            return;
        }

        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String linha;
            String secaoAtual = "";

            while ((linha = br.readLine()) != null) {
                linha = linha.trim();

                // Identificar seção atual
                if (linha.startsWith("--- PESQUISAS FREQUENTES ---")) {
                    secaoAtual = "pesquisas";
                    continue;
                } else if (linha.startsWith("--- TEMPOS DE RESPOSTA POR PESQUISA (ms) ---")) {
                    secaoAtual = "tempos";
                    continue;
                } else if (linha.startsWith("--- ESTATÍSTICAS POR BARREL ---")) {
                    secaoAtual = "barrels";
                    continue;
                } else if (linha.isEmpty() || linha.startsWith("===") || linha.startsWith("Data de")) {
                    continue;
                }

                // Processar linhas baseado na seção atual
                if (secaoAtual.equals("pesquisas") && linha.contains(":")) {
                    try {
                        int posInicio = linha.indexOf("\"");
                        int posFim = linha.lastIndexOf("\"");
                        if (posInicio >= 0 && posFim > posInicio) {
                            String pesquisa = linha.substring(posInicio + 1, posFim);
                            int contador = Integer.parseInt(linha.substring(posFim + 2, linha.indexOf(" pesquisas")).trim());
                            pesquisasFrequentes.put(pesquisa, contador);
                        }
                    } catch (Exception e) {
                        System.err.println("Erro ao processar linha de pesquisas frequentes: " + linha);
                    }
                } else if (secaoAtual.equals("tempos") && linha.contains(":")) {
                    try {
                        int posInicio = linha.indexOf("\"");
                        int posFim = linha.lastIndexOf("\"");
                        if (posInicio >= 0 && posFim > posInicio) {
                            String pesquisa = linha.substring(posInicio + 1, posFim);
                            String[] partes = linha.substring(posFim + 2).split(",");

                            // Extrair tempo total
                            String tempoTotalStr = partes[0].trim().replace("tempo total ", "").replace(" ms", "");
                            long tempoTotal = Long.parseLong(tempoTotalStr);
                            temposResposta.put(pesquisa, tempoTotal);

                            // Calcular contagem baseada na média
                            String mediaStr = partes[1].trim().replace("média ", "").replace(" ms", "");
                            double media = Double.parseDouble(mediaStr);
                            if (media > 0) {
                                int contagem = (int)Math.round(tempoTotal / media);
                                contagemPesquisas.put(pesquisa, contagem);
                            } else {
                                contagemPesquisas.put(pesquisa, 1);
                            }
                        }
                    } catch (Exception e) {
                        System.err.println("Erro ao processar linha de tempos de resposta: " + linha);
                    }
                } else if (secaoAtual.equals("barrels")) {
                    try {
                        if (linha.startsWith("Barrel ")) {
                            String barrelId = linha.substring("Barrel ".length(), linha.indexOf(":"));
                            int contagem = Integer.parseInt(linha.substring(linha.indexOf(": ") + 2, linha.indexOf(" pesquisas")).trim());
                            contagemBarrels.put(barrelId, contagem);
                        } else if (linha.startsWith("Tempo médio")) {
                            // Esta linha pertence ao barrelId atual
                            String barrelId = getLastProcessedBarrelId();
                            if (barrelId != null) {
                                String mediaStr = linha.substring(linha.indexOf(": ") + 2, linha.indexOf(" ms"));
                                double media = Double.parseDouble(mediaStr);

                                // Inicializa o mapa para este barrel se não existir
                                if (!temposRespostaBarrels.containsKey(barrelId)) {
                                    temposRespostaBarrels.put(barrelId, new HashMap<>());
                                }

                                // Adiciona um tempo fictício baseado na média
                                int contagem = contagemBarrels.getOrDefault(barrelId, 0);
                                long tempoTotal = (long)(media * contagem);
                                temposRespostaBarrels.get(barrelId).put("media", tempoTotal);
                            }
                        }
                    } catch (Exception e) {
                        System.err.println("Erro ao processar linha de estatísticas por barrel: " + linha);
                    }
                }
            }

            System.out.println("Estatísticas carregadas com sucesso:");
            System.out.println(" - " + pesquisasFrequentes.size() + " pesquisas frequentes");
            System.out.println(" - " + temposResposta.size() + " registos de tempo de resposta");
            System.out.println(" - " + contagemBarrels.size() + " barrels com estatísticas");
        } catch (IOException e) {
            System.err.println("Erro ao carregar estatísticas: " + e.getMessage());
        }
    }

    // Método auxiliar para obter o último barrelId processado
    private String getLastProcessedBarrelId() {
        if (contagemBarrels.isEmpty()) {
            return null;
        }
        // Retorna o último barrelId adicionado
        return contagemBarrels.keySet().stream().reduce((first, second) -> second).orElse(null);
    }

    public static void main(String[] args) {

        Gateway gateway = null;
        try{
            gateway = new Gateway();
            LocateRegistry.createRegistry(Configuration.PORTA_RMI);
            Naming.rebind("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/GATEWAY", gateway);
        } catch(Exception e){
            System.err.println("Falha ao tentar fazer rebind da gateway: "+e.getMessage());
            System.exit(-1);
        }
        System.out.println("Gateway iniciada com sucesso!");

        try {
            URLQueue queue = new URLQueue();
            Naming.rebind("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/QUEUE", queue);
        } catch (Exception e) {
            System.err.println("Falha ao tentar fazer rebind da queue: "+e.getMessage());
            System.exit(-1);
        }
        System.out.println("URLQueue iniciada com sucesso!");

        // Adicionar um hook de encerramento para garantir que os dados sejam salvos quando o programa for fechado
        Gateway finalGateway = gateway;
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Salvando estatisticas antes de encerrar...");
            finalGateway.salvarEstatisticas();
        }));
    }
}
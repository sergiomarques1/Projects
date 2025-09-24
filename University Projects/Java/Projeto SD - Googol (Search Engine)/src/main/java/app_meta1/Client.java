package app_meta1;

import java.util.Scanner;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.lang.Thread;


public class Client {

    private static Scanner scanner;

    private static GatewayRemote gateway;


    private static void conectar(){

        System.out.println("\nA tentar conectar o cliente ...\n");

        scanner = new Scanner(System.in);

        try{
            gateway = (GatewayRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/GATEWAY");
        } catch(MalformedURLException e){
            System.err.println("\nEndereço da gateway está errado!");
        } catch(NotBoundException | RemoteException e){
            conexaoFalhada();
        }

        System.out.println("\nCliente conectado com sucesso!");
    }

    private static void desconectar(){

        scanner.close();

        System.out.println("\nCliente desconectado!");
        System.exit(0);
    }

    private static void conexaoFalhada(){

        System.err.println("Impossível conectar ao Googol. Nova tentativa de conexão dentro de 5 segundos!");

        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
            System.out.println("Espera interrompida. A tentar conectar à queue!");
        }
        conectar();
    }

    private static int menu(){

        int opcao = -1;

        try{

            System.out.println("\n-----------------------------------\n"
                    + "| 1 - Inserir URL                   |\n"
                    + "| 2 - Pesquisa                      |\n"
                    + "| 3 - Listar Ligações Página        |\n"
                    + "| 4 - Estatísticas (Página Admin)   |\n"
                    + "| 5 - Sair                          |\n"
                    +"-----------------------------------\n");
            System.out.print("\nOpção: ");

            String opcaoString = scanner.nextLine();

            try{
                opcao = Integer.parseInt(opcaoString);
                if(opcao < 1 || opcao > 5){
                    throw new NumberFormatException();
                }
            } catch(NumberFormatException e){
                System.out.println("Input Inválido!");
                return 0;
            }
        } catch(NoSuchElementException | IllegalStateException e){
            return -1;
        }

        return opcao;
    }

    private static String inserirURL(){

        try{
            System.out.print("\nInsira um URL: ");
            return scanner.nextLine();
        } catch(NoSuchElementException | IllegalStateException e){
            desconectar();
            return null;
        }

    }

    private static String inputPesquisa(){

        try{
            System.out.print("\nPesquisa: ");
            return scanner.nextLine();
        } catch(NoSuchElementException | IllegalStateException e){
            desconectar();
            return null;
        }

    }

    private static boolean urlValido(String url){

        return(url.startsWith("https://") || url.startsWith("http://"));
    }

    // Método modificado para usar a paginação do Barrel
    private static void pesquisa(String pesquisa){
        try {
            // Verificar se existem resultados para a pesquisa
            int totalPaginas = gateway.getTotalPaginas(pesquisa);

            if (totalPaginas == 0) {
                System.out.println("Não foram encontrados resultados para a sua pesquisa!");
                return;
            }

            int paginaAtual = 1;
            boolean resultados = true;

            while (resultados) {
                // Obter resultados para a página atual diretamente do Barrel
                List<String> resultadosPagina = gateway.efetuarPesquisa(pesquisa, paginaAtual);

                if (resultadosPagina == null || resultadosPagina.isEmpty()) {
                    System.out.println("Não há mais resultados disponíveis.");
                    break;
                }

                System.out.println("\nResultados da pesquisa -> Página " + paginaAtual + " de " + totalPaginas + "\n");

                // Exibir os resultados da página atual
                for (int i = 0; i < resultadosPagina.size(); i++) {
                    try {
                        String[] dados = resultadosPagina.get(i).split("#");
                        int indiceGlobal = (paginaAtual - 1) * Configuration.TAM_PAGINA + (i + 1);
                        System.out.println(indiceGlobal + "->");
                        System.out.println("   Link: " + dados[0] + " | Título: "+dados[1]);
                        System.out.println("   Descricao: " + dados[2] +"\n");
                    } catch (ArrayIndexOutOfBoundsException e) {
                        System.out.println("Dados incompletos");
                    }
                }

                // Opções de navegação
                if (paginaAtual > 1) {
                    System.out.println("A: Página Anterior");
                }
                if (paginaAtual < totalPaginas) {
                    System.out.println("D: Próxima Página");
                }
                System.out.println("V: Voltar Ao Menu");

                System.out.print("Opcao: ");
                String opcao = scanner.nextLine().toUpperCase();

                switch (opcao) {
                    case "A":
                        if (paginaAtual > 1) {
                            paginaAtual--;
                        }
                        break;
                    case "D":
                        if (paginaAtual < totalPaginas) {
                            paginaAtual++;
                        }
                        break;
                    case "V":
                        resultados = false;
                        break;
                    default:
                        System.out.println("Opção inválida!");
                }
            }
        } catch (Exception e) {
            System.err.println("Erro na pesquisa: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private static void ligacoes(String url){

        ArrayList<String> ligacoes = new ArrayList<>();

        try {
            try {
                ligacoes = gateway.ligacoesPagina(url);
            } catch (RemoteException e) {
                System.err.println("Pesquisa não disponível de momento!");
                Thread.sleep(1000);
            }

            if (ligacoes == null || ligacoes.isEmpty()) {
                System.out.println("Não foram encontrados links que apontem para esta página!");
                return;
            }

            System.out.println("\n == Lista dos links que apontam para o URL "+url+" == \n");
            int indice = 1;
            for(String ligacao : ligacoes){
                System.out.println(indice+": "+ligacao);
                indice++;
            }

        } catch(Exception e){
            System.err.println("Erro na pesquisa: " + e.getMessage());
            e.printStackTrace();
        }
    }

    // Implementação da página de estatísticas
    private static void mostrarEstatisticas() {
        try {
            System.out.println("\n========== ESTATÍSTICAS DO SISTEMA ==========\n");

            // Top 10 pesquisas mais comuns
            Map<String, Integer> top10Pesquisas = gateway.getTop10Pesquisas();
            System.out.println("\n-- TOP 10 PESQUISAS MAIS COMUNS --");
            if (top10Pesquisas.isEmpty()) {
                System.out.println("Ainda não foram realizadas pesquisas.");
            } else {
                int posicao = 1;
                for (Map.Entry<String, Integer> entry : top10Pesquisas.entrySet()) {
                    System.out.println(posicao + ". \"" + entry.getKey() + "\" - " + entry.getValue() + " vezes");
                    posicao++;
                }
            }

            // Lista de Barrels ativos com tamanhos de índice
            Map<String, Integer> barrelsInfo = gateway.getBarrelsInfo();
            System.out.println("\n-- BARRELS ATIVOS --");
            if (barrelsInfo.isEmpty()) {
                System.out.println("Nenhum barrel ativo no momento.");
            } else {
                for (Map.Entry<String, Integer> entry : barrelsInfo.entrySet()) {
                    System.out.println("Barrel: " + entry.getKey() + " - Tamanho do índice: " + entry.getValue() + " páginas");
                }
            }

            // Tempo médio de resposta por Barrel
            Map<String, Double> temposMedios = gateway.getTempoMedioPorBarrel();
            System.out.println("\n-- TEMPO MÉDIO DE RESPOSTA --");
            if (temposMedios.isEmpty()) {
                System.out.println("Ainda não há dados sobre tempos de resposta.");
            } else {
                for (Map.Entry<String, Double> entry : temposMedios.entrySet()) {
                    System.out.println("Barrel: " + entry.getKey() + " - Tempo médio: " + String.format("%.1f", entry.getValue()) + " décimos de segundo");
                }
            }

            System.out.println("\nPressione Enter para voltar ao menu...");
            scanner.nextLine();

        } catch (Exception e) {
            System.err.println("Erro ao obter estatísticas: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static void main(String[] args){

        conectar();

        while(true){

            int opcaoMenu = menu();

            if(opcaoMenu == 0){
                continue;
            }
            else if(opcaoMenu == -1){
                desconectar();
            }
            else{
                try{
                    switch(opcaoMenu){
                        case 1:
                            String url = inserirURL();
                            if(urlValido(url)){
                                gateway.enviarURL(url);
                            }
                            else{
                                System.out.println("\nLink inválido!\n");
                            }
                            break;
                        case 2:
                            String pesquisa = inputPesquisa();
                            pesquisa(pesquisa);
                            break;
                        case 3:
                            String ligacoesURL = inserirURL();
                            if(urlValido(ligacoesURL)){
                                ligacoes(ligacoesURL);
                            }
                            else{
                                System.out.println("\nLink inválido!\n");
                            }
                            break;
                        case 4:
                            mostrarEstatisticas();
                            break;
                        case 5:
                            desconectar();
                            break;
                    }
                } catch(java.rmi.ConnectException e){
                    conexaoFalhada();
                } catch(RemoteException e){
                    e.printStackTrace();
                }
            }
        }
    }
}
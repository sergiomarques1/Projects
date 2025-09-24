package app_meta1;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.io.*;

public class Barrel extends UnicastRemoteObject implements BarrelRemote {

    // HashMap que associa palavras às URLs que contêm essas palavras
    private HashMap<String, HashSet<String>> index;

    // HashMap que associa URLs a informações da página (título, descrição, links)
    private HashMap<String, ArrayList<String>> links;

    private static final String INDEX_FILE = "barrel_index.dat";
    private static final String LINKS_FILE = "barrel_links.dat";

    protected Barrel() throws RemoteException {
        super();
        this.index = new HashMap<>();
        this.links = new HashMap<>();

        carregarDados();
    }


    public void inserirDados(String url, String titulo, String descricao, ArrayList<String> palavras, ArrayList<String> ligacoes) {
        // Indexa cada palavra encontrada na página
        for (String palavra : palavras) {
            palavra = palavra.toLowerCase();
            index.putIfAbsent(palavra, new HashSet<>());
            if (!index.get(palavra).contains(url)) {
                index.get(palavra).add(url);
            }
        }

        // Armazena informações da página
        ArrayList<String> infoURL = new ArrayList<>();
        infoURL.add(titulo);               // Posição 0: Título
        infoURL.add(descricao);            // Posição 1: Descrição

        // Adiciona ligações a partir da posição 2
        for (String link : ligacoes) {
            if (!link.equals(url)) { // Evita adicionar o próprio URL como ligação
                infoURL.add(link);
            }
        }

        links.put(url, infoURL);

        salvarDados();
    }


    public int numLigacoes(String url) {
        int numLigacoes = 0;

        for (Map.Entry<String, ArrayList<String>> entrada : links.entrySet()) {
            ArrayList<String> info = entrada.getValue();

            // Ignora as duas primeiras posições (título e descrição)
            for (int i = 2; i < info.size(); i++) {
                if (info.get(i).equals(url)) {
                    numLigacoes++;
                    break;
                }
            }
        }

        return numLigacoes;
    }


    public int getTotalResultados(String pesquisa) throws RemoteException {
        ArrayList<String> resultados = processarPesquisa(pesquisa);
        return resultados.size();
    }


    private ArrayList<String> processarPesquisa(String pesquisa) {
        String[] palavras = pesquisa.split(" ");
        HashMap<String, Integer> aux = new HashMap<>();

        System.out.println("A procurar resultados para a pesquisa: " + pesquisa + " ...");

        // Encontra URLs que contêm todas as palavras da pesquisa
        for (String link : links.keySet()) {
            boolean contemTodasPalavras = true;

            for (String palavra : palavras) {
                palavra = palavra.toLowerCase();

                if (!index.containsKey(palavra) || !index.get(palavra).contains(link)) {
                    contemTodasPalavras = false;
                    break;
                }
            }

            if (contemTodasPalavras) {
                aux.put(link, numLigacoes(link));
            }
        }

        ArrayList<String> resultadoCompleto = new ArrayList<>();

        // Ordena os resultados por número de ligações (mais citados primeiro)
        aux.entrySet().stream()
                .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
                .forEach(entry -> {
                    String link = entry.getKey();
                    ArrayList<String> urlInfo = links.get(link);

                    // Obter título e descrição corretamente
                    String titulo = urlInfo.size() > 0 ? urlInfo.get(0) : "";
                    String descricao = urlInfo.size() > 1 ? urlInfo.get(1) : "";

                    // Formato: URL#TITULO#DESCRICAO#
                    String resultado = link + "#" + titulo + "#" + descricao + "#";
                    resultadoCompleto.add(resultado);
                });

        return resultadoCompleto;
    }


    public ArrayList<String> pesquisa(String pesquisa, int pagina, int tamanhoPagina) {
        ArrayList<String> resultadoCompleto = processarPesquisa(pesquisa);
        ArrayList<String> resultadoPaginado = new ArrayList<>();

        int inicio = (pagina - 1) * tamanhoPagina;
        int fim = Math.min(inicio + tamanhoPagina, resultadoCompleto.size());

        if (inicio < resultadoCompleto.size()) {
            for (int i = inicio; i < fim; i++) {
                resultadoPaginado.add(resultadoCompleto.get(i));
            }
        }

        System.out.println("Enviando página " + pagina + " com " + resultadoPaginado.size() + " resultados");
        return resultadoPaginado;
    }

    public ArrayList<String> ligacoes(String url) {
        ArrayList<String> ligacoes = new ArrayList<>();

        for (Map.Entry<String, ArrayList<String>> entrada : links.entrySet()) {
            String link = entrada.getKey();
            ArrayList<String> info = entrada.getValue();

            // Verifica nas ligações (a partir do índice 2)
            for (int i = 2; i < info.size(); i++) {
                if (info.get(i).equals(url)) {
                    ligacoes.add(link);
                    break;
                }
            }
        }

        return ligacoes;
    }

    public int getTamanhoIndice() throws RemoteException {
        return links.size();
    }

    private void salvarDados() {
        try {
            ObjectOutputStream outIndex = new ObjectOutputStream(new FileOutputStream(INDEX_FILE));
            outIndex.writeObject(index);
            outIndex.close();

            ObjectOutputStream outLinks = new ObjectOutputStream(new FileOutputStream(LINKS_FILE));
            outLinks.writeObject(links);
            outLinks.close();

            System.out.println("Dados do barrel salvos com sucesso!");
        } catch (IOException e) {
            System.err.println("Erro ao salvar dados do barrel: " + e.getMessage());
            e.printStackTrace();
        }
    }

    @SuppressWarnings("unchecked")
    private void carregarDados() {
        try {
            File indexFile = new File(INDEX_FILE);
            File linksFile = new File(LINKS_FILE);

            if (indexFile.exists() && linksFile.exists()) {
                ObjectInputStream inIndex = new ObjectInputStream(new FileInputStream(INDEX_FILE));
                index = (HashMap<String, HashSet<String>>) inIndex.readObject();
                inIndex.close();

                ObjectInputStream inLinks = new ObjectInputStream(new FileInputStream(LINKS_FILE));
                links = (HashMap<String, ArrayList<String>>) inLinks.readObject();
                inLinks.close();

                System.out.println("Dados do barrel carregados com sucesso!");
                System.out.println("Índice carregado com " + index.size() + " palavras e " + links.size() + " URLs");
            } else {
                System.out.println("Nenhum arquivo de dados encontrado. Iniciando com índice vazio.");
            }
        } catch (IOException | ClassNotFoundException e) {
            System.err.println("Erro ao carregar dados do barrel: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        Barrel barrel;
        try {
            barrel = new Barrel();
            Naming.rebind("rmi://" + Configuration.HOST_RMI + ":" + Configuration.PORTA_RMI + "/BARREL", barrel);
            System.out.println("Barrel iniciado com sucesso!");

            // Adicionar um hook de encerramento para garantir que os dados sejam salvos quando o programa for fechado
            Runtime.getRuntime().addShutdownHook(new Thread(() -> {
                System.out.println("Salvando dados antes de encerrar...");
                barrel.salvarDados();
            }));

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
package app_meta1;

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.util.ArrayList;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

public class DownloaderThread extends Thread{
    
    private Document doc;

    private int indice;

    private URLQueueRemote queue;

    private volatile boolean executando = true;

    public DownloaderThread(int indice, URLQueueRemote queue){
        this.indice = indice;
        this.queue = queue;
    }

    public void processarURL(String url, URLQueueRemote queue){

        String descricao;
        String titulo;
        ArrayList<String> palavras = new ArrayList<>();
        ArrayList<String> ligacoes = new ArrayList<>();

        try {
            this.doc = Jsoup.connect(url).get();
        } catch (Exception e) {
            System.out.println("Downloader "+this.indice+": Falha ao conectar com o URL: " + url);
        }

        
        try {
            titulo = doc.title();
            if (titulo == null){
                titulo = "Sem Título";
            }

            Element elementoDescricao = doc.select("meta[name=description]").first();
            if (elementoDescricao != null) {
                descricao = elementoDescricao.attr("content");
            } else {
                descricao = "Sem descricao";
            }
        } catch (NullPointerException e) {
            return;
        }

        //ligacoes.add(url);
        Elements links = doc.select("a[href]");
        for (Element link : links) {
            String ligacao = link.attr("abs:href");
            if (!ligacao.isEmpty()) {
                ligacoes.add(ligacao);
                try {
                    queue.adicionarURL(ligacao);
                } catch (Exception e) {
                    System.err.println("Downloader "+this.indice+": Falha ao enviar o URL para a queue: " + e.getMessage());
                }
            }
        }
    
        String[] palavrasURL = doc.text().toLowerCase().split(" ");

        for (String palavra : palavrasURL) {

            palavra = palavra.replaceAll("[^\\p{L}\\p{Nd}\\p{Punct}]+", "").toLowerCase();
            
            if (!palavra.isEmpty()) {
                palavras.add(palavra);
            }
        }

        try {
            BarrelRemote barrel = (BarrelRemote) Naming.lookup("//"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/BARREL");
            barrel.inserirDados(url, titulo, descricao, palavras, ligacoes);
            System.out.println("Downloader " +this.indice+": Dados enviados para o barrel com sucesso");
        } catch (Exception e) {
            System.err.println("Downloader "+this.indice+": Falha ao enviar os dados para o barrel: " + e.getMessage());
        }
        
    }

    public void run(){

        while (executando) {
            try {
                String url = queue.retirarURL();

                if(url == null) {
                    System.out.println("Downloader " + this.indice + ": Sem URLs disponíveis. Aguardando...");
                    Thread.sleep(2000);
                    continue;
                }

                System.out.println("Downloader " + this.indice + ": Url " + url + " recebido!");
                processarURL(url, this.queue);

            } catch (RemoteException e) {
                System.err.println("Downloader " + this.indice + ": Erro ao comunicar com a queue: " + e.getMessage());
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException ie) {
                    Thread.currentThread().interrupt();
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            }
        }
    }

}

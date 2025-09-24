package web_meta2.models;

import app_meta1.Configuration;
import app_meta1.GatewayRemote;
import jakarta.annotation.PostConstruct;
import org.springframework.stereotype.Service;

import java.rmi.Naming;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

@Service
public class GoogolService {

    private String rmiUrl;

    @PostConstruct
    public void init() {
        rmiUrl = "rmi://" + Configuration.HOST_RMI + ":" + Configuration.PORTA_RMI;
        System.out.println("RMI URL configurado: {"+rmiUrl+"}");
    }

    public UrlSubmission submitUrl(String url) {
        UrlSubmission submission = new UrlSubmission();
        submission.setUrl(url);

        try {
            GatewayRemote gateway = (GatewayRemote) Naming.lookup(rmiUrl + "/GATEWAY");
            gateway.enviarURL(url);

            submission.setStatus("success");
            submission.setMessage("URL enviado com sucesso para indexação!");

            System.out.println("URL '{" +url+"}' enviado com sucesso");
        } catch (Exception e) {
            System.out.println("Erro ao enviar URL '{"+url+"}': {"+e.getMessage()+"}");
            submission.setStatus("error");
            submission.setMessage("Erro ao enviar URL: " + e.getMessage());
        }

        return submission;
    }

    public SearchResponse search(String query, int page) {
        SearchResponse response = new SearchResponse();
        response.setQuery(query);
        response.setCurrentPage(page);

        try {
            GatewayRemote gateway = (GatewayRemote) Naming.lookup(rmiUrl + "/GATEWAY");

            // Obter total de páginas
            int totalPages = gateway.getTotalPaginas(query);
            response.setTotalPages(totalPages);

            // Obter total de resultados
            int totalResults = gateway.getTotalResultados(query);
            response.setTotalResults(totalResults);

            // Obter resultados para a página atual
            ArrayList<String> resultStrings = gateway.efetuarPesquisa(query, page);

            // Converter para objetos SearchResult
            List<SearchResult> results = resultStrings.stream()
                    .map(SearchResult::fromBarrelString)
                    .collect(Collectors.toList());

            response.setResults(results);

            System.out.println("Pesquisa '{"+query+"}' retornou {"+totalResults+"} resultados, página {"+page+"}/{"+totalPages+"}");
        } catch (Exception e) {
            System.out.println("Erro ao pesquisar '{"+query+"}': {"+e.getMessage()+"}");
            response.setResults(new ArrayList<>());
        }

        return response;
    }

    public PageLinks getPageLinks(String url) {
        PageLinks pageLinks = new PageLinks();
        pageLinks.setUrl(url);

        try {
            GatewayRemote gateway = (GatewayRemote) Naming.lookup(rmiUrl + "/GATEWAY");
            ArrayList<String> links = gateway.ligacoesPagina(url);
            pageLinks.setLinks(links);

            System.out.println("URL '{+"+url+"}' tem {"+links.size()+"} links");
        } catch (Exception e) {
            System.out.println("Erro ao obter links para '{"+url+"}': {"+e.getMessage()+"}");
            pageLinks.setLinks(new ArrayList<>());
        }

        return pageLinks;
    }
}
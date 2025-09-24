package web_meta2.controllers;

import web_meta2.models.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;

import java.util.HashMap;
import java.util.Map;

@Controller
public class WebController {

    @Autowired
    private GoogolService googolService;

    @Autowired
    private HackerNewsService hackerNewsService;

    @Autowired
    private AIService aiService;

    @GetMapping("/")
    public String index() {
        return "index";
    }

    @GetMapping("/search")
    public String search(
            @RequestParam(value = "q", required = false) String query,
            @RequestParam(value = "page", defaultValue = "1") int page,
            @RequestParam(value = "analysis", defaultValue = "false") boolean showAnalysis,
            Model model) {

        if (query != null && !query.trim().isEmpty()) {
            SearchResponse searchResponse = googolService.search(query, page);
            model.addAttribute("searchResponse", searchResponse);
            System.out.println("Pesquisa executada: '{"+query+"}', Página: {"+page+"}, Total de resultados: {"+searchResponse.getTotalResults()+"}");

            // Se solicitado, gera a análise contextualizada
            if (showAnalysis && !searchResponse.getResults().isEmpty()) {
                String analysis = aiService.generateContextualAnalysis(query, searchResponse.getResults());
                model.addAttribute("analysis", analysis);
            }
        }

        return "search";
    }

    @GetMapping("/submit")
    public String submitForm() {
        return "submit_url";
    }

    @PostMapping("/submit-url")
    public String handleSubmit(@RequestParam("url") String url, Model model) {
        UrlSubmission resultado = googolService.submitUrl(url);
        model.addAttribute("message", resultado.getMessage());
        model.addAttribute("status", resultado.getStatus());
        return "submit_url";
    }

    @GetMapping("/links")
    public String links(@RequestParam("url") String url, Model model) {
        if (url != null && !url.trim().isEmpty()) {
            model.addAttribute("pageLinks", googolService.getPageLinks(url));
            model.addAttribute("url", url);
        }

        return "links";
    }

    /**
     * Endpoint para a página de backlinks
     */
    @GetMapping("/backlinks")
    public String backlinks() {
        return "backlinks";
    }

    /**
     * Endpoint para a página de estatísticas
     * Não é necessário carregar estatísticas iniciais aqui, pois elas serão
     * obtidas via WebSocket assim que a página carregar
     */
    @GetMapping("/stats")
    public String stats() {
        return "stats";
    }

    @GetMapping("/hackernews")
    public String hackerNewsForm() {
        return "hacker_news";
    }

    /**
     * Endpoint para indexar as top stories do Hacker News que contêm os termos de pesquisa
     */
    @PostMapping("/hackernews/index-top-stories")
    @ResponseBody
    public ResponseEntity<IndexationSummary> indexTopStories(@RequestParam("searchTerms") String searchTerms) {
        IndexationSummary summary = hackerNewsService.indexTopStoriesWithTerms(searchTerms);
        return ResponseEntity.ok(summary);
    }

    /**
     * Endpoint para indexar as histórias de um usuário específico do Hacker News
     */
    @PostMapping("/hackernews/index-user-stories")
    @ResponseBody
    public ResponseEntity<IndexationSummary> indexUserStories(
            @RequestParam("username") String username,
            @RequestParam(value = "searchTerms", required = false) String searchTerms) {

        IndexationSummary summary = hackerNewsService.indexUserStoriesWithTerms(
                username,
                searchTerms != null ? searchTerms : ""
        );

        return ResponseEntity.ok(summary);
    }

    /**
     * Endpoint para a página de IA
     */
    @GetMapping("/ai")
    public String aiPage() {
        return "ai";
    }

    /**
     * API para obter análise contextualizada sem renderizar a página completa
     */
    @GetMapping("/ai/analysis")
    @ResponseBody
    public ResponseEntity<Map<String, String>> getAnalysis(@RequestParam("q") String query) {
        SearchResponse searchResponse = googolService.search(query, 1);
        String analysis = aiService.generateContextualAnalysis(query, searchResponse.getResults());

        Map<String, String> response = new HashMap<>();
        response.put("query", query);
        response.put("analysis", analysis);

        return ResponseEntity.ok(response);
    }
}
package web_meta2.models;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Service
public class AIService {

    @Value("${http://localhost:11434/api/generate}")
    private String ollamaApiUrl;

    @Value("${llama3.2:latest}")
    private String ollamaApiModel;

    private final RestTemplate restTemplate = new RestTemplate();

    /**
     * Gera uma análise contextualizada baseada nos termos de pesquisa e resultados
     */
    public String generateContextualAnalysis(String searchQuery, List<SearchResult> searchResults) {
        try {
            // Preparando o conteúdo dos resultados para enviar à API
            String resultSummary = prepareResultSummary(searchResults);

            // Preparando o prompt para a API
            List<Map<String, String>> messages = new ArrayList<>();

            // Mensagem do sistema (instruções)
            Map<String, String> systemMessage = new HashMap<>();
            systemMessage.put("role", "system");
            systemMessage.put("content", "Você é um assistente especializado em analisar resultados de pesquisa " +
                    "e fornecer uma análise concisa e informativa. Crie um resumo contextualizado " +
                    "com base nos termos de pesquisa e nos resultados fornecidos. Seu resumo deve " +
                    "ser informativo, objetivo e ter no máximo 3 parágrafos.");
            messages.add(systemMessage);

            // Mensagem do usuário (com a consulta e resultados)
            Map<String, String> userMessage = new HashMap<>();
            userMessage.put("role", "user");
            userMessage.put("content", "Por favor, forneça uma análise contextualizada para a pesquisa \"" +
                    searchQuery + "\" com base nos seguintes resultados de pesquisa:\n\n" + resultSummary);
            messages.add(userMessage);

            // Configurando o corpo da requisição
            Map<String, Object> requestBody = new HashMap<>();
            requestBody.put("model", ollamaApiModel);
            requestBody.put("messages", messages);
            requestBody.put("temperature", 0.7);
            requestBody.put("max_tokens", 500);

            // Configurando os cabeçalhos
            HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.APPLICATION_JSON);

            HttpEntity<Map<String, Object>> request = new HttpEntity<>(requestBody, headers);

            // Fazendo a chamada à API
            ResponseEntity<Map> response = restTemplate.postForEntity(ollamaApiUrl, request, Map.class);

            // Processando a resposta
            if (response.getBody() != null) {
                List<Map<String, Object>> choices = (List<Map<String, Object>>) response.getBody().get("choices");
                if (choices != null && !choices.isEmpty()) {
                    Map<String, Object> firstChoice = choices.get(0);
                    Map<String, String> message = (Map<String, String>) firstChoice.get("message");
                    if (message != null) {
                        return message.get("content");
                    }
                }
            }

            return "Não foi possível gerar uma análise contextualizada neste momento.";

        } catch (Exception e) {
            System.out.println("Erro ao gerar análise contextualizada: " + e.getMessage());
            return "Ocorreu um erro ao tentar gerar a análise contextualizada: " + e.getMessage();
        }
    }

    /**
     * Prepara um resumo dos resultados para incluir no prompt
     */
    private String prepareResultSummary(List<SearchResult> searchResults) {
        if (searchResults == null || searchResults.isEmpty()) {
            return "Nenhum resultado encontrado.";
        }

        // Limita a quantidade de resultados enviados para o prompt para evitar exceder limites
        int maxResults = Math.min(searchResults.size(), 5);
        List<SearchResult> limitedResults = searchResults.subList(0, maxResults);

        // Formata os resultados em texto
        return limitedResults.stream()
                .map(result -> "Título: " + result.getTitle() + "\n" +
                        "URL: " + result.getUrl() + "\n" +
                        "Descrição: " + result.getDescription() + "\n")
                .collect(Collectors.joining("\n"));
    }
}
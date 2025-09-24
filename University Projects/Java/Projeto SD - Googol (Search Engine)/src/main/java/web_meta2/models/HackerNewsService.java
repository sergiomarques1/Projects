package web_meta2.models;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Service
public class HackerNewsService {

    private final RestTemplate restTemplate = new RestTemplate();
    private final String HN_API_BASE_URL = "https://hacker-news.firebaseio.com/v0";
    private final ExecutorService executorService = Executors.newFixedThreadPool(10);

    @Autowired
    private GoogolService googolService;

    /**
     * Obtém o ID das top stories do Hacker News
     */
    public List<Long> getTopStoryIds() {
        Long[] topStoryIds = restTemplate.getForObject(
                HN_API_BASE_URL + "/topstories.json",
                Long[].class
        );

        List<Long> result = new ArrayList<>();
        if (topStoryIds != null) {
            // Limitando a 100 histórias para não sobrecarregar
            int limit = Math.min(topStoryIds.length, 100);
            for (int i = 0; i < limit; i++) {
                result.add(topStoryIds[i]);
            }
        }

        return result;
    }

    /**
     * Obtém os detalhes de uma história específica pelo ID
     */
    public HackerNewsStory getStoryDetails(Long storyId) {
        return restTemplate.getForObject(
                HN_API_BASE_URL + "/item/" + storyId + ".json",
                HackerNewsStory.class
        );
    }

    /**
     * Obtém histórias de um usuário específico do Hacker News
     */
    public List<Long> getUserStoryIds(String username) {
        HackerNewsUser user = restTemplate.getForObject(
                HN_API_BASE_URL + "/user/" + username + ".json",
                HackerNewsUser.class
        );

        if (user != null && user.getSubmitted() != null) {
            // Limitando a 100 histórias para não sobrecarregar
            int limit = Math.min(user.getSubmitted().size(), 100);
            return user.getSubmitted().subList(0, limit);
        }

        return new ArrayList<>();
    }

    /**
     * Filtra e indexa as top stories que contêm os termos de pesquisa
     * @return Um resumo da operação de indexação
     */
    public IndexationSummary indexTopStoriesWithTerms(String searchTerms) {
        List<Long> topStoryIds = getTopStoryIds();
        return processAndIndexStories(topStoryIds, searchTerms);
    }

    /**
     * Filtra e indexa as histórias de um usuário específico que contêm os termos de pesquisa
     */
    public IndexationSummary indexUserStoriesWithTerms(String username, String searchTerms) {
        List<Long> userStoryIds = getUserStoryIds(username);
        return processAndIndexStories(userStoryIds, searchTerms);
    }

    /**
     * Processa e indexa histórias que contêm os termos de pesquisa
     */
    private IndexationSummary processAndIndexStories(List<Long> storyIds, String searchTerms) {
        IndexationSummary summary = new IndexationSummary();
        summary.setSearchTerms(searchTerms);

        // Lista para armazenar as futuras operações assíncronas
        List<CompletableFuture<Boolean>> futures = new ArrayList<>();

        for (Long storyId : storyIds) {
            CompletableFuture<Boolean> future = CompletableFuture.supplyAsync(() -> {
                try {
                    HackerNewsStory story = getStoryDetails(storyId);
                    if (story != null && storyContainsTerms(story, searchTerms)) {
                        // Só indexamos histórias que contêm URL
                        if (story.getUrl() != null && !story.getUrl().isEmpty()) {
                            UrlSubmission submission = googolService.submitUrl(story.getUrl());
                            if ("success".equals(submission.getStatus())) {
                                summary.incrementIndexedCount();
                                summary.addIndexedUrl(story.getUrl(), story.getTitle());
                                return true;
                            }
                        }
                    }
                } catch (Exception e) {
                    System.out.println("Erro ao processar história do HackerNews ID " + storyId + ": " + e.getMessage());
                }
                return false;
            }, executorService);

            futures.add(future);
        }

        // Aguarda a conclusão de todas as operações
        CompletableFuture.allOf(futures.toArray(new CompletableFuture[0])).join();

        summary.setTotalProcessed(storyIds.size());
        return summary;
    }

    /**
     * Verifica se uma história contém os termos de pesquisa especificados
     */
    private boolean storyContainsTerms(HackerNewsStory story, String searchTerms) {
        if (searchTerms == null || searchTerms.trim().isEmpty()) {
            return false;
        }

        // Divide os termos de pesquisa por espaço para verificar cada termo individualmente
        String[] terms = searchTerms.toLowerCase().split("\\s+");

        // Campos a verificar
        String title = story.getTitle() != null ? story.getTitle().toLowerCase() : "";
        String text = story.getText() != null ? story.getText().toLowerCase() : "";

        // Verifica se todos os termos estão presentes em pelo menos um dos campos
        for (String term : terms) {
            if (!title.contains(term) && !text.contains(term)) {
                return false;
            }
        }

        return true;
    }
}

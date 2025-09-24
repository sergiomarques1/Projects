package web_meta2.models;

import java.util.List;
import java.util.Objects;

public class SearchResponse {
    private List<SearchResult> results;
    private int currentPage;
    private int totalPages;
    private int totalResults;
    private String query;

    // Construtor vazio (substitui @NoArgsConstructor)
    public SearchResponse() {
    }

    // Construtor com todos os campos (substitui @AllArgsConstructor)
    public SearchResponse(List<SearchResult> results, int currentPage, int totalPages, int totalResults, String query) {
        this.results = results;
        this.currentPage = currentPage;
        this.totalPages = totalPages;
        this.totalResults = totalResults;
        this.query = query;
    }

    // Builder pattern manual (substitui @Builder)
    public static SearchResponseBuilder builder() {
        return new SearchResponseBuilder();
    }

    public static class SearchResponseBuilder {
        private List<SearchResult> results;
        private int currentPage;
        private int totalPages;
        private int totalResults;
        private String query;

        public SearchResponseBuilder results(List<SearchResult> results) {
            this.results = results;
            return this;
        }

        public SearchResponseBuilder currentPage(int currentPage) {
            this.currentPage = currentPage;
            return this;
        }

        public SearchResponseBuilder totalPages(int totalPages) {
            this.totalPages = totalPages;
            return this;
        }

        public SearchResponseBuilder totalResults(int totalResults) {
            this.totalResults = totalResults;
            return this;
        }

        public SearchResponseBuilder query(String query) {
            this.query = query;
            return this;
        }

        public SearchResponse build() {
            return new SearchResponse(results, currentPage, totalPages, totalResults, query);
        }
    }

    // Getters e setters (substitui @Data)
    public List<SearchResult> getResults() {
        return results;
    }

    public void setResults(List<SearchResult> results) {
        this.results = results;
    }

    public int getCurrentPage() {
        return currentPage;
    }

    public void setCurrentPage(int currentPage) {
        this.currentPage = currentPage;
    }

    public int getTotalPages() {
        return totalPages;
    }

    public void setTotalPages(int totalPages) {
        this.totalPages = totalPages;
    }

    public int getTotalResults() {
        return totalResults;
    }

    public void setTotalResults(int totalResults) {
        this.totalResults = totalResults;
    }

    public String getQuery() {
        return query;
    }

    public void setQuery(String query) {
        this.query = query;
    }

    // Métodos equals e hashCode (parte de @Data)
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        SearchResponse that = (SearchResponse) o;
        return currentPage == that.currentPage &&
                totalPages == that.totalPages &&
                totalResults == that.totalResults &&
                Objects.equals(results, that.results) &&
                Objects.equals(query, that.query);
    }

    @Override
    public int hashCode() {
        return Objects.hash(results, currentPage, totalPages, totalResults, query);
    }

    // Método toString (parte de @Data)
    @Override
    public String toString() {
        return "SearchResponse{" +
                "results=" + results +
                ", currentPage=" + currentPage +
                ", totalPages=" + totalPages +
                ", totalResults=" + totalResults +
                ", query='" + query + '\'' +
                '}';
    }
}
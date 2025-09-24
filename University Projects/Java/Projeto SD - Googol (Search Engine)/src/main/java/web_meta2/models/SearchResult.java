package web_meta2.models;

import java.util.Objects;

public class SearchResult {
    private String url;
    private String title;
    private String description;

    // Construtor vazio (substitui @NoArgsConstructor)
    public SearchResult() {
    }

    // Construtor com todos os campos (substitui @AllArgsConstructor)
    public SearchResult(String url, String title, String description) {
        this.url = url;
        this.title = title;
        this.description = description;
    }

    // Builder pattern manual (substitui @Builder)
    public static SearchResultBuilder builder() {
        return new SearchResultBuilder();
    }

    public static class SearchResultBuilder {
        private String url;
        private String title;
        private String description;

        public SearchResultBuilder url(String url) {
            this.url = url;
            return this;
        }

        public SearchResultBuilder title(String title) {
            this.title = title;
            return this;
        }

        public SearchResultBuilder description(String description) {
            this.description = description;
            return this;
        }

        public SearchResult build() {
            return new SearchResult(url, title, description);
        }
    }

    // Factory method para criar a partir da string formatada recebida do Barrel
    public static SearchResult fromBarrelString(String barrelString) {
        String[] parts = barrelString.split("#");
        if (parts.length >= 3) {
            return SearchResult.builder()
                    .url(parts[0])
                    .title(parts[1])
                    .description(parts[2])
                    .build();
        }
        return new SearchResult(barrelString, "Sem título", "Sem descrição");
    }

    // Getters e setters (substitui @Data)
    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    // Métodos equals e hashCode (parte de @Data)
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        SearchResult that = (SearchResult) o;
        return Objects.equals(url, that.url) &&
                Objects.equals(title, that.title) &&
                Objects.equals(description, that.description);
    }

    @Override
    public int hashCode() {
        return Objects.hash(url, title, description);
    }

    // Método toString (parte de @Data)
    @Override
    public String toString() {
        return "SearchResult{" +
                "url='" + url + '\'' +
                ", title='" + title + '\'' +
                ", description='" + description + '\'' +
                '}';
    }
}
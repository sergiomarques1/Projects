package web_meta2.models;

import java.util.Objects;

public class UrlSubmission {
    private String url;
    private String status;
    private String message;

    // Construtor vazio (substitui @NoArgsConstructor)
    public UrlSubmission() {
    }

    // Construtor com todos os campos (substitui @AllArgsConstructor)
    public UrlSubmission(String url, String status, String message) {
        this.url = url;
        this.status = status;
        this.message = message;
    }

    // Builder pattern manual (substitui @Builder)
    public static UrlSubmissionBuilder builder() {
        return new UrlSubmissionBuilder();
    }

    public static class UrlSubmissionBuilder {
        private String url;
        private String status;
        private String message;

        public UrlSubmissionBuilder url(String url) {
            this.url = url;
            return this;
        }

        public UrlSubmissionBuilder status(String status) {
            this.status = status;
            return this;
        }

        public UrlSubmissionBuilder message(String message) {
            this.message = message;
            return this;
        }

        public UrlSubmission build() {
            return new UrlSubmission(url, status, message);
        }
    }

    // Getters e setters (substitui @Data)
    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    // Métodos equals e hashCode (parte de @Data)
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        UrlSubmission that = (UrlSubmission) o;
        return Objects.equals(url, that.url) &&
                Objects.equals(status, that.status) &&
                Objects.equals(message, that.message);
    }

    @Override
    public int hashCode() {
        return Objects.hash(url, status, message);
    }

    // Método toString (parte de @Data)
    @Override
    public String toString() {
        return "UrlSubmission{" +
                "url='" + url + '\'' +
                ", status='" + status + '\'' +
                ", message='" + message + '\'' +
                '}';
    }
}
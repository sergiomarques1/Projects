package web_meta2.models;

import java.util.List;
import java.util.Objects;

public class PageLinks {
    private String url;
    private List<String> links;

    public PageLinks() {
    }

    public PageLinks(String url, List<String> links) {
        this.url = url;
        this.links = links;
    }

    public static PageLinksBuilder builder() {
        return new PageLinksBuilder();
    }

    public static class PageLinksBuilder {
        private String url;
        private List<String> links;

        public PageLinksBuilder url(String url) {
            this.url = url;
            return this;
        }

        public PageLinksBuilder links(List<String> links) {
            this.links = links;
            return this;
        }

        public PageLinks build() {
            return new PageLinks(url, links);
        }
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public List<String> getLinks() {
        return links;
    }

    public void setLinks(List<String> links) {
        this.links = links;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        PageLinks pageLinks = (PageLinks) o;
        return Objects.equals(url, pageLinks.url) && Objects.equals(links, pageLinks.links);
    }

    @Override
    public int hashCode() {
        return Objects.hash(url, links);
    }

    @Override
    public String toString() {
        return "PageLinks{" +
                "url='" + url + '\'' +
                ", links=" + links +
                '}';
    }
}
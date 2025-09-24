package web_meta2.models;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class IndexationSummary {
    private String searchTerms;
    private int totalProcessed;
    private int indexedCount;
    private Map<String, String> indexedUrls; // URL -> título

    public IndexationSummary() {
        this.indexedCount = 0;
        this.totalProcessed = 0;
        this.indexedUrls = new HashMap<>();
    }

    public void incrementIndexedCount() {
        this.indexedCount++;
    }

    public void addIndexedUrl(String url, String title) {
        this.indexedUrls.put(url, title);
    }

    public String getSearchTerms() {
        return searchTerms;
    }

    public void setSearchTerms(String searchTerms) {
        this.searchTerms = searchTerms;
    }

    public int getTotalProcessed() {
        return totalProcessed;
    }

    public void setTotalProcessed(int totalProcessed) {
        this.totalProcessed = totalProcessed;
    }

    public int getIndexedCount() {
        return indexedCount;
    }

    public void setIndexedCount(int indexedCount) {
        this.indexedCount = indexedCount;
    }

    public Map<String, String> getIndexedUrls() {
        return indexedUrls;
    }

    public void setIndexedUrls(Map<String, String> indexedUrls) {
        this.indexedUrls = indexedUrls;
    }

    public List<Map<String, String>> getIndexedUrlsAsList() {
        List<Map<String, String>> result = new ArrayList<>();
        for (Map.Entry<String, String> entry : indexedUrls.entrySet()) {
            Map<String, String> item = new HashMap<>();
            item.put("url", entry.getKey());
            item.put("title", entry.getValue());
            result.add(item);
        }
        return result;
    }

    @Override
    public String toString() {
        return "IndexationSummary{" +
                "searchTerms='" + searchTerms + '\'' +
                ", totalProcessed=" + totalProcessed +
                ", indexedCount=" + indexedCount +
                ", indexedUrls=" + indexedUrls +
                '}';
    }
}
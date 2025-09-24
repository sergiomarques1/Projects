package web_meta2.models;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import java.util.List;

@JsonIgnoreProperties(ignoreUnknown = true)
public class HackerNewsUser {
    private String id;
    private Long created;
    private Integer karma;
    private String about;
    private List<Long> submitted;

    public HackerNewsUser() {
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public Long getCreated() {
        return created;
    }

    public void setCreated(Long created) {
        this.created = created;
    }

    public Integer getKarma() {
        return karma;
    }

    public void setKarma(Integer karma) {
        this.karma = karma;
    }

    public String getAbout() {
        return about;
    }

    public void setAbout(String about) {
        this.about = about;
    }

    public List<Long> getSubmitted() {
        return submitted;
    }

    public void setSubmitted(List<Long> submitted) {
        this.submitted = submitted;
    }

    @Override
    public String toString() {
        return "HackerNewsUser{" +
                "id='" + id + '\'' +
                ", karma=" + karma +
                ", submitted.size=" + (submitted != null ? submitted.size() : 0) +
                '}';
    }
}
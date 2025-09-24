abstract class Personagem {

    private String tipo;
    private String nome;
    private int nivelXP;
    private double inteligencia;
    private double forca;
    private double agilidade;
    Personagem(String tipo, String nome, int nivelXP, double inteligencia, double forca, double agilidade){
        this.tipo = tipo;
        this.nome = nome;
        this.nivelXP = nivelXP;
        this.inteligencia = inteligencia;
        this.forca = forca;
        this.agilidade = agilidade;
    }

    public String getTipo() {
        return tipo;
    }

    public void setTipo(String tipo) {
        this.tipo = tipo;
    }

    public String getNome() {
        return nome;
    }

    public void setNome(String nome) {
        this.nome = nome;
    }

    public int getNivelXP() {
        return nivelXP;
    }

    public void setNivelXP(int nivelXP) {
        this.nivelXP = nivelXP;
    }

    public double getInteligencia() {
        return inteligencia;
    }

    public void setInteligencia(double inteligencia) {
        this.inteligencia = inteligencia;
    }

    public double getForca() {
        return forca;
    }

    public void setForca(double forca) {
        this.forca = forca;
    }

    public double getAgilidade() {
        return agilidade;
    }

    public void setAgilidade(double agilidade) {
        this.agilidade = agilidade;
    }

    public abstract void personagemEspecial();      // método abstrato que será implementado em cada subclasse com o objetivo de mostrar todas as personagens com atributos especiais
    public void evoluirPersonagem(){        // método que aumenta o nível XP de uma personagem e que melhora, na subclasse que corresponde à personagem, as suas capacidades
        int nivelXP = getNivelXP() + 1;
        setNivelXP(nivelXP);
    }

    public String toString() {
        return ("\nNome: " +nome+" | Nivel XP: " + nivelXP);
    }
}
public class Mago extends Personagem {

    private String sementes;
    private String folhas;

    Mago(String tipo, String nome, int nivelXP, double inteligencia, double forca, double agilidade, String sementes, String folhas) {
        super(tipo, nome, nivelXP, inteligencia, forca, agilidade);
        this.sementes = sementes;
        this.folhas = folhas;
    }

    public String getSementes() {
        return sementes;
    }

    public void setSementes(String sementes) {
        this.sementes = sementes;
    }

    public String getFolhas() {
        return folhas;
    }

    public void setFolhas(String folhas) {
        this.folhas = folhas;
    }

    public String toString() {
        return super.toString();
    }

    public void personagemEspecial (){      // método que verifica se o mago tem sementes de abóbora na sua mochila, com o intuito de descobrir se é uma personagem com atributos especiais ou não
        if ((this.sementes).equals("abóbora")){
            System.out.print("\nNome: "+super.getNome()+" | Sementes de "+getSementes());
        }
    }
    public void evoluirPersonagem() {       // método que melhora as capacidades do mago, tendo em conta as devidas percentagens de melhoria
        super.evoluirPersonagem();
        double inteligencia = super.getInteligencia() + super.getInteligencia() * 0.2;
        double forca = super.getForca() + super.getForca() * 0.05;
        double agilidade = super.getAgilidade() + super.getAgilidade() * 0.1;
        super.setInteligencia(inteligencia);
        super.setForca(forca);
        super.setAgilidade(agilidade);
    }
}

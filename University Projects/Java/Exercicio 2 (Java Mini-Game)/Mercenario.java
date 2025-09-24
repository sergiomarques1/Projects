public class Mercenario extends Personagem {

    private String armaLongo;
    private int municoes;

    Mercenario(String tipo, String nome, int nivelXP, double inteligencia, double forca, double agilidade, String armaLongo, int municoes) {
        super(tipo, nome, nivelXP, inteligencia, forca, agilidade);
        this.armaLongo = armaLongo;
        this.municoes = municoes;
    }

    public String getArma() {
        return armaLongo;
    }

    public void setArma(String arma) {
        this.armaLongo = arma;
    }

    public int getMunicoes() {
        return municoes;
    }

    public void setMunicoes(int municoes) {
        this.municoes = municoes;
    }

    public String toString() {
        return super.toString();
    }

    public void personagemEspecial (){      // método que verifica se o mercenário possui um arco como a sua arma de longo alcance, com o intuito de descobrir se é uma personagem com atributos especiais ou não
        if ((this.armaLongo).equals("Arco")){
            System.out.print("\nNome: "+super.getNome()+" | Arma Longo Alcance: "+getArma()+" | Munições: "+getMunicoes());
        }
    }
    public void evoluirPersonagem() {       // método que melhora as capacidades do mercenário, tendo em conta as devidas percentagens de melhoria
        super.evoluirPersonagem();
        double inteligencia = super.getInteligencia() + super.getInteligencia() * 0.08;
        double forca = super.getForca() + super.getForca() * 0.08;
        double agilidade = super.getAgilidade() + super.getAgilidade() * 0.2;
        super.setInteligencia(inteligencia);
        super.setForca(forca);
        super.setAgilidade(agilidade);
    }
}

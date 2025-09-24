class Guerreiro extends Personagem {

    private boolean armadura;
    private String armaCurto;
    Guerreiro (String tipo, String nome, int nivelXP, double inteligencia, double forca, double agilidade, boolean armadura, String armaCurto) {
        super(tipo, nome, nivelXP, inteligencia, forca, agilidade);
        this.armadura = armadura;
        this.armaCurto = armaCurto;
    }
    public boolean getArmadura() {
        return armadura;
    }

    public void setArmadura(boolean armadura) {
        this.armadura = armadura;
    }

    public String getArma() {
        return armaCurto;
    }

    public void setArma(String arma) {
        this.armaCurto = arma;
    }

    @Override
    public String toString() {
        return super.toString();
    }

    public void personagemEspecial (){      // método que verifica se o guerreiro possui armadura, com o intuito de descobrir se é uma personagem com atributos especiais ou não
        if (this.getArmadura() == true){
            System.out.print("\nNome: "+super.getNome()+" | Armadura: Sim");
        }
    }

    public void evoluirPersonagem(){        // método que melhora as capacidades do guerreiro, tendo em conta as devidas percentagens de melhoria
        super.evoluirPersonagem();
        double inteligencia = super.getInteligencia() + super.getInteligencia() * 0.05;
        double forca = super.getForca() + super.getForca() * 0.2;
        double agilidade = super.getAgilidade() + super.getAgilidade() * 0.1;
        super.setInteligencia(inteligencia);
        super.setForca(forca);
        super.setAgilidade(agilidade);
    }


}

import java.util.ArrayList;

/**
 * Classe para gerir os dados da categoria Desporto
 */
public class Desporto extends Pergunta {

    /**
     * ArrayList das opções de verdadeiro e falso
     */
    private ArrayList<String> OpcoesVerdadeiroFalso;

    /**
     * Resposta correta
     */
    private String correctAnswer;

    // Construtor
    /**
     * Construtor da classe Desporto, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     * @param opcoesVerdadeiroFalso
     * @param correctAnswer
     */
    public Desporto(String enunciado, int pontosPergunta, ArrayList<String> opcoesVerdadeiroFalso,
                    String correctAnswer) {
        super(enunciado, pontosPergunta);
        OpcoesVerdadeiroFalso = opcoesVerdadeiroFalso;
        this.correctAnswer = correctAnswer;

    }

    //Setters and Getters

    /**
     * Método de acesso à ArrayList das opções de verdadeiro e falso
     * @return
     */
    protected ArrayList<String> getOpcoesVerdadeiroFalso() {
        return OpcoesVerdadeiroFalso;
    }

    /**
     * Método que atualiza a ArrayList das opções de verdadeiro e falso
     * @param opcoesVerdadeiroFalso
     */
    protected void setOpcoesVerdadeiroFalso(ArrayList<String> opcoesVerdadeiroFalso) {
        OpcoesVerdadeiroFalso = opcoesVerdadeiroFalso;
    }

    /**
     * Método de acesso à resposta correta
     * @return
     */
    protected String getCorrectAnswer() {
        return correctAnswer;
    }

    /**
     * Método que atualiza a resposta correta
     * @param correctAnswer
     */
    protected void setCorrectAnswer(String correctAnswer) {
        this.correctAnswer = correctAnswer;
    }

    /**
     * Método toString da classe Desporto
     * @return
     */
    @Override
    public String toString() {
        return "Tipo: Desporto," +getEnunicado()+ ", Opções de Resposta: " + OpcoesVerdadeiroFalso + " Solução: " + correctAnswer;
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    @Override
    public int majoracao(int pontos){
        return pontos;
    }

    /**
     * Método que cria a janela da pergunta
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    @Override
    public String janelaPergunta(int perguntasCounter, int respostasCertas){
        String letra = "Monospaced";
        GUI gui = new GUI();
        gui.perguntas(letra, getEnunicado(),getOpcoesVerdadeiroFalso());
        String userAnswer = gui.getOpcaoEscolhida();
        return userAnswer;
    }

    /**
     * Método que verifica se a opção escolhida pleo utilizador é, ou não, a correta
     * @param userAnswer
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    @Override
    public String verificaResposta(String userAnswer, int perguntasCounter, int respostasCertas){
        if(userAnswer.equalsIgnoreCase(getCorrectAnswer())){
            String acertou = "Sim";
            return acertou;
        }else{
            String acertou = "Nao";
            return acertou;
        }
    }
}
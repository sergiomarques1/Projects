import java.util.ArrayList;

/**
 * Classe para gerir os dados da categoria Ciencia
 */
public class Ciencia extends Pergunta {

    /**
     * ArrayList das opções fáceis
     */
    private ArrayList<String> OpcoesFacil;

    /**
     * ArrayList das opções difíceis
     */
    private ArrayList<String> OpcoesDificil;

    /**
     * Resposta correta
     */
    private String correctAnswerCiencias;

    //Contrutor
    /**
     * Construtor da classe Ciencia, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     * @param opcoesFacil
     * @param opcoesDificil
     * @param correctAnswerCiencias
     */
    public Ciencia(String enunciado, int pontosPergunta, ArrayList<String> opcoesFacil,
                   ArrayList<String> opcoesDificil, String correctAnswerCiencias) {
        super(enunciado, pontosPergunta);
        OpcoesFacil = opcoesFacil;
        OpcoesDificil = opcoesDificil;
        this.correctAnswerCiencias = correctAnswerCiencias;
    }

    //Setters and Getters
    /**
     * Método de acesso à ArrayList das opções fáceis
     * @return
     */
    protected ArrayList<String> getOpcoesFacil() {
        return OpcoesFacil;
    }

    /**
     * Método que atualiza a Arraylist de opções fáceis
     * @param opcoesRespostaFacil
     */
    protected void setOpcoesFacil(ArrayList<String> opcoesRespostaFacil) {
        OpcoesFacil = opcoesRespostaFacil;
    }

    /**
     * Método de acesso à ArrayList das opções difíceis
     * @return
     */
    protected ArrayList<String> getOpcoesDificil() {
        return OpcoesDificil;
    }


    /**
     * Método que atualiza a ArrayList das opções difíceis
     * @param opcoesRespostaDificil
     */
    protected void setOpcoesDificil(ArrayList<String> opcoesRespostaDificil) {
        OpcoesDificil = opcoesRespostaDificil;
    }

    /**
     * Método de acesso à opção correta
     * @return
     */
    protected String getCorrectAnswerCiencias() {
        return correctAnswerCiencias;
    }

    /**
     * Método que atualiza a resposta correta
     * @param correctAnswerCiencias
     */
    protected void setCorrectAnswerCiencias(String correctAnswerCiencias) {
        this.correctAnswerCiencias = correctAnswerCiencias;
    }

    /**
     * Método toString da classe Ciencia
     * @return
     */
    @Override
    public String toString() {
        return "Tipo: Ciencias, Opcões fáceis: " + OpcoesFacil + ", Opções Dificeis:" + OpcoesDificil + ", Solução: "+ correctAnswerCiencias;
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    @Override
    public int majoracao(int pontos){
        return pontos + 5;
    }

    /**
     * Método que cria a janela da pergunta e verifica se esta é fácil ou difícil
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    public String janelaPergunta(int perguntasCounter, int respostasCertas){
        GUI gui = new GUI();
        String letra = "Monospaced";
        //Supondo que
        if (respostasCertas > 2){
            String userAnswer = gui.perguntas(letra, getEnunicado(),getOpcoesDificil());
            return userAnswer;
        }else{
            String userAnswer = gui.perguntas(letra, getEnunicado(),getOpcoesFacil());
            return userAnswer;
        }
    }

    /**
     * Método que verifica se a opção escolhida pleo utilizador é, ou não, a correta
     * @param userAnswer
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    @Override
    public String verificaResposta(String userAnswer, int perguntasCounter, int respostasCertas) {
        if (userAnswer.equalsIgnoreCase(getCorrectAnswerCiencias())) {
            String acertou = "Sim";
            return acertou;
        } else {
            String acertou = "Nao";
            return acertou;
        }
    }
}
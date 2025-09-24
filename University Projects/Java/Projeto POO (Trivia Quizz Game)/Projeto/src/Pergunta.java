import java.io.Serializable;

/**
 * Classe para gerir os dados das perguntas
 */
public class Pergunta implements Serializable {

    //Texto da Pergunta

    /**
     * Enunciado da pergunta
     */
    private String enunciado;

    //Pontuacao que vamos atribuir à pergunta

    /**
     * Pontuação da pergunta
     */
    private int pontosPergunta;

    //Construtor
    /**
     * Construtor da classe Pergunta, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     */
    public Pergunta(String enunciado, int pontosPergunta) {
        this.enunciado = enunciado;
        this.pontosPergunta = pontosPergunta;

    }

    //Setters e Getters
    /**
     * Método de acesso ao enunciado
     * @return
     */
    protected String getEnunicado() {
        return enunciado;
    }

    /**
     * Método que atualiza o enunciado da pergunta
     * @param enunciado
     */
    protected void setEnunicado(String enunciado) {
        this.enunciado = enunciado;
    }


    /**
     * Método de acesso ao número de pontos da pergunta
     * @return
     */
    protected int getPontosPergunta() {
        return pontosPergunta;
    }

    /**
     * Método que atualiza o número de pontos da pergunta
     * @param pontosPergunta
     */
    protected void setPontosPergunta(int pontosPergunta) {
        this.pontosPergunta = pontosPergunta;
    }

    /**
     * Método toString da classe Pergunta
     * @return
     */
    @Override
    public String toString() {
        return "Enunciado:" + enunciado + ", cotação:" + pontosPergunta + " pontos";
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    public int majoracao(int pontos){
        return pontos;
    }

    /**
     * Método que cria a janela da pergunta
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    public String janelaPergunta(int perguntasCounter, int respostasCertas){
        String userAnswer = null;
        return userAnswer;
    }

    /**
     * Método que verifica se a opção escolhida pleo utilizador é, ou não, a correta
     * @param userAnswer
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    public String verificaResposta(String userAnswer, int perguntasCounter, int respostasCertas){
        String acertou = null;
        return acertou;
    }
}
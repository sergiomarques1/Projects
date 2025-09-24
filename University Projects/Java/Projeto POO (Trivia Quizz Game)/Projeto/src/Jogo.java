import java.io.Serializable;
import java.util.ArrayList;

/**
 * Classe para gerir os dados de um jogo
 */
public class Jogo implements Serializable, Comparable<Jogo> {

    /**
     * Nome do utilizador
     */
    private String UserName;

    /**
     * Data e hora do jogo
     */
    private String DataHora;

    /**
     * ArrayList com as perguntas corretas do jogo
     */
    private ArrayList<Pergunta> PerguntasCorretas;

    /**
     * ArrayList com as perguntas erradas do jogo
     */
    private ArrayList<Pergunta> PerguntasErradas;

    /**
     * Pontuação do jogo
     */
    private int pontuacaoJogo;

    //Construtor
    /**
     * Construtor da classe Jogo, que recebe os dados necessários incialização dos atributos
     * @param userName
     * @param dataHora
     * @param perguntasCorretas
     * @param perguntasErradas
     * @param pontuacaoJogo
     */
    public Jogo(String userName, String dataHora, ArrayList<Pergunta> perguntasCorretas, ArrayList<Pergunta> perguntasErradas, int pontuacaoJogo) {
        this.UserName = userName;
        this.DataHora = dataHora;
        this.PerguntasCorretas = perguntasCorretas;
        this.PerguntasErradas = perguntasErradas;
        this.pontuacaoJogo = pontuacaoJogo;
    }

    //Setters and Getters
    /**
     * Método de acesso ao nome de utilizador
     * @return
     */
    public String getUserName() {
        return UserName;
    }

    /**
     * Método que atualiza o nome de utilizador
     * @param userName
     */
    protected void setUserName(String userName) {
        UserName = userName;
    }

    /**
     * Método de acesso à data e hora do jogo
     * @return
     */
    public String getDataHora() {
        return DataHora;
    }

    /**
     * Método que atualiza a data e hora do jogo
     * @param dataHora
     */
    public void setDataHora(String dataHora) {
        DataHora = dataHora;
    }

    /**
     * Método de acesso à ArrayList que contém as perguntas corretas do jogo
     * @return
     */
    public ArrayList<Pergunta> getPerguntasCorretas() {
        return PerguntasCorretas;
    }

    /**
     * Método que atualiza a ArrayList de perguntas corretas do jogo
     * @param perguntasCorretas
     */
    public void setPerguntasCorretas(ArrayList<Pergunta> perguntasCorretas) {
        PerguntasCorretas = perguntasCorretas;
    }

    /**
     * Método de acesso à ArrayList que contém as perguntas erradas do jogo
     * @return
     */
    public ArrayList<Pergunta> getPerguntasErradas() {
        return PerguntasErradas;
    }

    /**
     * Método que atualiza a ArrayList das respostas erradas do jogo
     * @param perguntasErradas
     */
    public void setPerguntasErradas(ArrayList<Pergunta> perguntasErradas) {
        PerguntasErradas = perguntasErradas;
    }

    /**
     * Método de acesso à pontuação do jogo
     * @return
     */
    public int getPontuacaoJogo() {
        return pontuacaoJogo;
    }

    /**
     * Método que atualiza a pontuação do jogo
     * @param pontuacaoJogo
     */
    public void setPontuacaoJogo(int pontuacaoJogo) {
        this.pontuacaoJogo = pontuacaoJogo;
    }

    /**
     * Comparação de todos os jogos
     * @param proximoJogo the object to be compared.
     * @return
     */
    @Override
    public int compareTo(Jogo proximoJogo) {
        // Compara objetos Jogo com base na pontuacaoJogo
        return Integer.compare(proximoJogo.pontuacaoJogo, this.pontuacaoJogo);
    }
}
import java.util.ArrayList;

/**
 * Classe para gerir os dados da subcategoria Futebol
 */
public class Futebol extends Desporto {

    //Caractersiticas especificas de Futebol
    /**
     * ArrayList com as opções dos números das camisolas
     */
    private ArrayList<String> NumerosCamisolas;

    /**
     * Solução dos números das camisolas
     */
    private String NumerosCamisolaSolucao;

    /**
     * ArrayList com as opções dos nomes dos jogadores
     */
    private ArrayList<String> NomesJogadores;

    /**
     * Solução dos nomes dos jogadores
     */
    private String NomesJogadoresSolucao;

    /**
     * Construtor da classe Futebol, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     * @param opcoesVerdadeiroFalso
     * @param correctAnswer
     * @param numerosCamisolas
     * @param numerosCamisolaSolucao
     * @param nomesJogadores
     * @param nomesJogadoresSolucao
     */
    public Futebol(String enunciado, int pontosPergunta, ArrayList<String> opcoesVerdadeiroFalso, String correctAnswer, ArrayList<String> numerosCamisolas, String numerosCamisolaSolucao, ArrayList<String> nomesJogadores, String nomesJogadoresSolucao) {
        super(enunciado, pontosPergunta, null, null);
        NumerosCamisolas = numerosCamisolas;
        NumerosCamisolaSolucao = numerosCamisolaSolucao;
        NomesJogadores = nomesJogadores;
        NomesJogadoresSolucao = nomesJogadoresSolucao;
    }

    //Setters e Getters
    /**
     * Método de acesso à ArrayList que contem as opções com os números das camisolas
     * @return
     */
    protected ArrayList<String> getNumerosCamisolas() {
        return NumerosCamisolas;
    }

    /**
     * Método que atualiza a ArrayList das opções com os números das camisolas
     * @param numerosCamisolas
     */
    protected void setNumerosCamisolas(ArrayList<String> numerosCamisolas) {
        NumerosCamisolas = numerosCamisolas;
    }

    /**
     * Método de acesso à opção correta relativa aos números das camisolas
     * @return
     */
    protected String getNumerosCamisolaSolucao() {
        return NumerosCamisolaSolucao;
    }

    /**
     * Método que atualiza a opção correta para os números das camisolas
     * @param numerosCamisolaSolucao
     */
    protected void setNumerosCamisolaSolucao(String numerosCamisolaSolucao) {
        NumerosCamisolaSolucao = numerosCamisolaSolucao;
    }

    /**
     * Método de acesso à ArrayList que contem as opções com os nomes dos jogadores
     * @return
     */
    protected ArrayList<String> getNomesJogadores() {
        return NomesJogadores;
    }

    /**
     * Método que atualiza a ArrayList das opções dos nomes dos jogadores
     * @param nomesJogadores
     */
    protected void setNomesJogadores(ArrayList<String> nomesJogadores) {
        NomesJogadores = nomesJogadores;
    }

    /**
     * Método de acesso à opção correta relativa aos nomes dos jogadores
     * @return
     */
    protected String getNomesJogadoresSolucao() {
        return NomesJogadoresSolucao;
    }

    /**
     * Método que atualiza a solução dos nomes dos jogadores
     * @param nomesJogadoresSolucao
     */
    protected void setNomesJogadoresSolucao(String nomesJogadoresSolucao) {
        NomesJogadoresSolucao = nomesJogadoresSolucao;
    }

    /**
     * Método toString da classe Futebol
     * @return
     */
    @Override
    public String toString() {
        return "Tipo: Desporto, Categoria: Futebol Numeros das Camisolas:" + NumerosCamisolas + ", Solução do Número da Camisola: " + NumerosCamisolaSolucao
                + ", Nomes dos Jogadores:" + NomesJogadores + ", Solucao do Nome: " + NomesJogadoresSolucao + "]";
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    @Override
    public int majoracao(int pontos){
        //Desporto  + Futebol
        return pontos + 3 + 1;
    }

    /**
     * Método que cria a janela da pergunta e verifica se esta é fácil ou difícil
     * @param perguntasCounter
     * @param respostasCertas
     * @return
     */
    @Override
    public String janelaPergunta(int perguntasCounter, int respostasCertas){
        GUI gui = new GUI();
        String letra = "Monospaced";
        if (perguntasCounter < 3){
            String userAnswer = gui.perguntas(letra, getEnunicado(),getNomesJogadores());
            return userAnswer;
        }else{
            String userAnswer = gui.perguntas(letra, getEnunicado(),getNumerosCamisolas());
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
    public String verificaResposta(String userAnswer, int perguntasCounter, int respostasCertas){
        if (perguntasCounter < 3){ //Vamos Verificar usando os Nomes dos Jogadores
            if(userAnswer.equalsIgnoreCase(getNomesJogadoresSolucao())){
                String acertou = "Sim";
                return acertou;
            }else{
                String acertou = "Nao";
                return acertou;
            }
        }
        else{ //Vamos verificar usando os Numeros das Camisolas
            if(userAnswer.equalsIgnoreCase(getNumerosCamisolaSolucao())){
                String acertou = "Sim";
                return acertou;
            }else{
                String acertou = "Nao";
                return acertou;
            }
        }
    }
}
import java.util.ArrayList;
import java.util.Random;

/**
 * Classe para gerir os dados da categoria Arte
 */
public class Arte extends Pergunta {

    /**
     * ArrayList com as opções de resposta
     */
    private ArrayList<String> OpcoesRespostaArte;

    /**
     * Resposta correta
     */
    private String correctAnswerArte;

    //Contructor

    /**
     * Construtor da classe Arte, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     * @param opcoesRespostaArte
     * @param correctAnswerArte
     */
    public Arte(String enunciado, int pontosPergunta, ArrayList<String> opcoesRespostaArte, String correctAnswerArte) {
        super(enunciado, pontosPergunta);
        OpcoesRespostaArte = opcoesRespostaArte;
        this.correctAnswerArte = correctAnswerArte;
    }


    //Setters and Getters
    /**
     * Método de acesso à ArrayList de opções de resposta
     * @return
     */
    protected ArrayList<String> getOpcoesRespostaArte() {
        return OpcoesRespostaArte;
    }

    /**
     * Método que atualiza a ArrayList das opções de resposta
     * @param opcoesRespostaArte
     */
    protected void setOpcoesRespostaArte(ArrayList<String> opcoesRespostaArte) {
        OpcoesRespostaArte = opcoesRespostaArte;
    }

    /**
     * Método de acesso à resposta correta
     * @return
     */
    protected String getCorrectAnswer() {
        return correctAnswerArte;
    }


    /**
     * Método que atualiza a resposta correta
     * @param correctAnswer
     */
    protected void setCorrectAnswer(String correctAnswer) {
        this.correctAnswerArte = correctAnswer;
    }

    /**
     * Método to String da classe Arte
     * @return
     */
    @Override
    public String toString() {
        return "Tipo: Arte, Opcoes de Resposta:" + OpcoesRespostaArte + " Solução: " + correctAnswerArte;
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    @Override
    public int majoracao(int pontos){
        return pontos * 10;
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
        if (perguntasCounter < 3){ //Modificar isto
            ArrayList<String> copy = getOpcoesRespostaArte();
            ArrayList<String> tresRespostas = new ArrayList<>();
            for (int i = 0; i < copy.size(); i++){
                String opcao = copy.get(i);
                //Adicionar a arraylist de opcoes mais curta a resposta //TRANSFROMAR EM METODO
                if(opcao.equalsIgnoreCase(correctAnswerArte)){
                    tresRespostas.add(opcao);
                    copy.remove(i);
                }
            }
            //Adicionar a opcao nº2 à arraylsit de opcoes mais curta
            Random random = new Random();
            int indiceRandom  = random.nextInt(copy.size());
            tresRespostas.add(copy.get(indiceRandom));
            copy.remove(indiceRandom);

            //Adicionar a opcao nº3 à arraylsit de opcoes mais curta
            Random random2 = new Random();
            int indiceRandom2  = random2.nextInt(copy.size());
            tresRespostas.add(copy.get(indiceRandom2));
            copy.remove(indiceRandom2);

            gui.perguntas(letra, getEnunicado(),tresRespostas);
            String userAnswer = gui.getOpcaoEscolhida();
            return userAnswer;
        }else{
            gui.perguntas(letra, getEnunicado(),getOpcoesRespostaArte());
            String userAnswer = gui.getOpcaoEscolhida();
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
    public String verificaResposta(String userAnswer,int perguntasCounter, int respostasCertas){
        if(userAnswer.equalsIgnoreCase(getCorrectAnswer())){
            String acertou = "Sim";
            return acertou;
        }else{
            String acertou = "Nao";
            return acertou;
        }
    }
}
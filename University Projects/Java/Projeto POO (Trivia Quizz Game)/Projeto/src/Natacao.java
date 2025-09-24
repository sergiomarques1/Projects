import java.util.ArrayList;

/**
 * Classe para gerir os dados da subcategoria Natacao
 */
public class Natacao extends Desporto {

    // Construtor
    /**
     * Construtor da classe Natacao, que recebe os dados necessários incialização dos atributos
     * @param enunciado
     * @param pontosPergunta
     * @param opcoesVerdadeiroFalso
     * @param correctAnswer
     */
    Natacao(String enunciado, int pontosPergunta, ArrayList<String> opcoesVerdadeiroFalso, String correctAnswer) {
        super(enunciado, pontosPergunta, opcoesVerdadeiroFalso, correctAnswer);
    }

    /**
     * Método que calcula o número de pontos a serem atribuídos ao utilizador caso este acerte a pergunta
     * @param pontos
     * @return
     */
    @Override
    public int majoracao(int pontos){
        return pontos + 3 + 10;
    }
}
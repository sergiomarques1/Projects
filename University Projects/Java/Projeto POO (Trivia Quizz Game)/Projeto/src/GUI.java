import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.awt.GridLayout;
import javax.swing.JOptionPane;
import java.awt.*;

/**
 * Classe para gerir os a interface do nosso jogo
 */
public class GUI {

    /**
     * Opção escolhida pelo utilizador
     */
    private String opcaoEscolhida;

    /**
     * Método de acesso à opção escolhida pelo utilizador
     * @return
     */
    protected String getOpcaoEscolhida() {
        return opcaoEscolhida;
    }

    /**
     * Método que atualiza a opção escolhida pelo utilizador
     * @param opcaoEscolhida
     */
    protected void setOpcaoEscolhida(String opcaoEscolhida) {
        this.opcaoEscolhida = opcaoEscolhida;
    }

    /**
     * Método que mantém a janela aberta até haver uma interação por parte deo utilizador
     */
    private void esperar(JFrame frame){
        while (frame.isVisible()) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Método que cria a janela do menu inicial do jogo
     * @param letra
     * @param readyToGo
     */
    protected void menuInicial(String letra, boolean[] readyToGo){

        // Criação da janela referente ao menu inicial do jogo
        JFrame menuInicial = new JFrame();
        menuInicial.setTitle("POOTrivia");
        menuInicial.setSize(600, 800);
        menuInicial.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        menuInicial.setBackground(Color.lightGray);

        // Labels vazios que serão adicionados aos espaços do ecrã (às células do GridLayout) que pretendemos que fiquem em branco
        ArrayList<JLabel> espacos = new ArrayList<>();
        for(int i = 0; i < 39; i++){
            JLabel espaco = new JLabel("");
            espacos.add(espaco);
        }

        // Criação do painel onde serão adicionados todos os componentes do menu principal
        JPanel painel = new JPanel();
        painel.setLayout(new GridLayout(15, 3));

        // Label com o título do jogo ("POOTrivia")
        JLabel nome = new JLabel("POOTrivia", SwingConstants.CENTER);
        nome.setFont(new Font(letra, Font.BOLD, 35));
        nome.setForeground(Color.white);

        // Label com informação sobre a categoria das Artes
        JLabel artes = new JLabel("Artes      x10", SwingConstants.CENTER);
        artes.setFont(new Font(letra, Font.BOLD, 17));
        artes.setBackground(Color.yellow);
        artes.setOpaque(true);
        artes.setToolTipText("Ao responder corretamente a uma pergunta desta categoria, o número de pontos da pergunta será multiplicado por 10");

        // Label com informação sobre a categoria do Desporto
        JLabel desporto = new JLabel("Desporto    +3", SwingConstants.CENTER);
        desporto.setFont(new Font(letra, Font.BOLD, 17));
        desporto.setBackground(Color.orange);
        desporto.setOpaque(true);
        desporto.setToolTipText("Ao responder corretamente a uma pergunta desta categoria, o número de pontos da pergunta será aumentado em 3 pontos");

        // Label com informação sobre a categoria das Ciências
        JLabel ciencias = new JLabel("Ciências    +5", SwingConstants.CENTER);
        ciencias.setFont(new Font(letra, Font.BOLD, 17));
        ciencias.setBackground(Color.green);
        ciencias.setOpaque(true);
        ciencias.setToolTipText("Ao responder corretamente a uma pergunta desta categoria, o número de pontos da pergunta será aumentado em 5 pontos");

        // Botão de "jogar" que, quando clicado, deverá fechar a janela do menu principal e e colocar uma variável booleana com o valor true, de modo a ser iniciado o jogo
        JButton jogar = new JButton("Jogar");
        jogar.setFont(new Font(letra, Font.BOLD, 35));
        jogar.setBackground(Color.white);
        jogar.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                readyToGo[0] = true;
                menuInicial.dispose();
            }
        });

        // Botão para sair do jogo. Quando clicado, fecha a janela do menu inicial
        JButton sair = new JButton("Sair");
        sair.setFont(new Font(letra, Font.BOLD, 25));
        sair.setBackground(Color.red);
        sair.setForeground(Color.white);
        sair.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                menuInicial.dispose();
            }
        });

        // Colocação dos componentes (labels e botões) no espaço correto da grelha do painel
        for(int i = 0; i < 4; i++){
            painel.add(espacos.get(i));
        }
        painel.add(nome);
        for(int i = 4; i < 12; i++){
            painel.add(espacos.get(i));
        }
        painel.add(artes);
        for(int i = 12; i < 17; i++){
            painel.add(espacos.get(i));
        }
        painel.add(desporto);
        for(int i = 17; i < 22; i++){
            painel.add(espacos.get(i));
        }
        painel.add(ciencias);
        for(int i = 22; i < 30; i++){
            painel.add(espacos.get(i));
        }
        painel.add(jogar);
        for(int i = 30; i < 35; i++){
            painel.add(espacos.get(i));
        }
        painel.add(sair);
        for(int i = 35; i < 39; i++){
            painel.add(espacos.get(i));
        }

        // Adição do painel que contém os componentes à janela do menu inicial
        painel.setBackground(Color.lightGray);
        menuInicial.add(painel);
        menuInicial.setVisible(true);
        esperar(menuInicial);
    }

    /**
     * Método que  cria a janela das perguntas do jogo
     * @param letra
     * @param enunciado
     * @param opcoes
     * @return
     */
    protected String perguntas (String letra, String enunciado, ArrayList<String> opcoes){

        // Inicialização da janela destinada às perguntas
        JFrame perguntas = new JFrame();
        perguntas.setTitle("POOTrivia");
        perguntas.setSize(600, 800);
        perguntas.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        perguntas.setBackground(Color.lightGray);

        // Criação do painel onde serão adicionadas as informações da pergunta
        JPanel painel = new JPanel();
        painel.setLayout(new BorderLayout());

        // Label destinado ao enunciado da pergunta em questão
        JLabel pergunta = new JLabel(enunciado, SwingConstants.CENTER);
        pergunta.setFont(new Font(letra, Font.BOLD, 15));
        painel.add(pergunta, BorderLayout.NORTH);

        // Criação dos botões que irão conter as opções de resposta
        ArrayList<JButton> botoes = new ArrayList<>();
        int i = 0;
        for(String opcao : opcoes){
            JButton opcaoo = new JButton(opcao);
            opcaoo.setFont(new Font(letra, Font.BOLD, 15));
            opcaoo.setBackground(Color.white);
            botoes.add(opcaoo);
            i += 1;
        }

        // Inicialização do painel das opções de resposta com GridLayout cujo número de linhas terá em conta o número de opções de resposta
        JPanel painelOpcoes = new JPanel();
        painelOpcoes.setLayout(new GridLayout((i * 2) + 1, 3));

        // Labels vazios que serão adicionados aos espaços do ecrã (às células do GridLayout) que pretendemos que fiquem em branco
        ArrayList<JLabel> espacos = new ArrayList<>();
        for(int z = 0; z < 4; z++){
            JLabel espaco = new JLabel();
            espacos.add(espaco);
        }
        for(JLabel espaco : espacos){
            painelOpcoes.add(espaco);
        }

        // Colocação dos botões no local correto da grelha do painel das opções
        for(int x = 0; x < botoes.size(); x++) {
            JButton botao = botoes.get(x);
            int n;
            if(x == (botoes.size() - 1)){
                n = 4;
            }
            else{
                n = 5;
            }
            // Se um botão for clicado, a opção escolhida irá corresponder ao texto desse botão e, a janela das perguntas irá ser fechada
            botao.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    for (JButton button : botoes) {
                        button.setEnabled(false);
                    }
                    setOpcaoEscolhida(botao.getText());
                    perguntas.dispose();
                }
            });
            painelOpcoes.add(botao);
            for(int j = 0; j < n; j++) {
                JLabel espaco = new JLabel();
                painelOpcoes.add(espaco);
            }
        }

        // Colocação do painel das opções no painel principal e, de seguida, a colocação deste último na janela das perguntas
        painelOpcoes.setBackground(Color.lightGray);
        painel.add(painelOpcoes, BorderLayout.CENTER);
        painel.setBackground(Color.lightGray);
        perguntas.add(painel);
        perguntas.setVisible(true);
        esperar(perguntas);

        // será devolvida a opção escolhida pelo utilizador para verificar se esta é, ou não, a correta
        return getOpcaoEscolhida();
    }
    //Metodos

    /**
     * Método que apresenta ao utilizador uma mensagem com um campo de texto onde deverá inserir o seu nome
     * @return
     */
    protected String getUserInput() {
        return JOptionPane.showInputDialog(null, "Insira o nome de utilizador:", "POO Trivia", JOptionPane.QUESTION_MESSAGE);
    }

    /**
     * Método que apresenta ao utilizador uma mensagem com o número de pontos obtidos, caso a resposta à pergunta for a correta
     * @param pontosToShow
     */
    protected void showMessageCorreto(int pontosToShow){
        JOptionPane.showMessageDialog(null, "Parabéns acertou a pergunta e ganhou: "+pontosToShow+" pontos !","Informação", JOptionPane.PLAIN_MESSAGE);
    }

    /**
     * Método que apresenta ao utilizador uma mensagem que o irá informar que respondeu incorretamente à pergunta
     */
    protected void showMessageErrado(){
        JOptionPane.showMessageDialog(null, "A resposta está errada!","Informação", JOptionPane.PLAIN_MESSAGE);
    }

    /**
     * Método que apresenta ao utilizador a pontuação final do seu jogo e o top 3 de pontações de todos os jogos realizados
     * @param top3
     * @param pontuacaoJogo
     */
    protected void top3(ArrayList<String> top3, int pontuacaoJogo){
        if(top3.size() < 3){
            JOptionPane.showMessageDialog(null, "TOP 3\n\nAinda não foram efetuados 3 jogos!", "POOTrivia", JOptionPane.PLAIN_MESSAGE);
        }
        else {
            JOptionPane.showMessageDialog(null, "Pontuação do seu Jogo: "+pontuacaoJogo+"\n\nTOP 3\n\n"+top3.get(0)+"\n"+top3.get(1)+"\n"+top3.get(2), "POOTrivia", JOptionPane.PLAIN_MESSAGE);
        }
    }

    /**
     * O método ecraFinal é responsável por criar uma janela que apresente a: "Sim" ou "Não" ao utilizador. Caso a opção escolhida seja a de jogar outra
     * @param jogarOutraVez
     */
    protected void ecraFinal(boolean[] jogarOutraVez) {
        // Mostra as opções Jogar outra vez e Fim do Jogo
        int option = JOptionPane.showOptionDialog(
                null,
                "Deseja jogar outra vez?",
                "Fim do Jogo",
                JOptionPane.YES_NO_OPTION,
                JOptionPane.QUESTION_MESSAGE,
                null,
                new Object[]{"Jogar Outra Vez", "Sair"},
                "Jogar Outra Vez");

        // Atualiza o boolean com base na resposta do utilizador
        if(option == JOptionPane.YES_OPTION){
            jogarOutraVez[0] = true;
            System.out.println("A começar novo jogo");
        }

        // Sai do programa se o utilizador selecionar a opção Não
        else {
            System.out.println("Utilizador saiu do jogo");
            System.exit(0);
        }
    }
}
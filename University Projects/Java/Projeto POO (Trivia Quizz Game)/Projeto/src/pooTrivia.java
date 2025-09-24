import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.time.format.DateTimeFormatter;
import java.time.LocalDateTime;

/**
 * Classe onde vai ser executado o ciclo do jogo
 */

public class pooTrivia {
    public static void main(String[] args) {
        pooTrivia trivia = new pooTrivia();
        String filePath = "pooTrivia.txt";
        String folderPath = "jogosJogados";
        String letra = "Monospaced";
        ArrayList<Pergunta> TodasAsPerguntas = new ArrayList<>();
        ArrayList<Pergunta> PerguntasCorretas = new ArrayList<>();
        ArrayList<Pergunta> PerguntasErradas = new ArrayList<>();
        ArrayList<Jogo> top3ArrayList = new ArrayList<>();
        boolean[] jogarOutraVez = {false};
        do {
            jogarOutraVez[0] = false;
            trivia.cicloJogo(filePath, folderPath, TodasAsPerguntas, PerguntasCorretas, PerguntasErradas, top3ArrayList, trivia, jogarOutraVez ,letra);
        }while (jogarOutraVez[0]);
    }

    /**
     * O metódo cicloJogo vai receber todos os dados necessários para iniciar um ciclo de jogo. Vai ser chamado sempre que o utilizador queira voltar a jogar
     * @param filePath
     * @param folderPath
     * @param TodasAsPerguntas
     * @param PerguntasCorretas
     * @param PerguntasErradas
     * @param top3ArrayList
     * @param trivia
     * @param jogarOutraVez
     * @param letra
     */
    protected void cicloJogo(String filePath, String folderPath, ArrayList<Pergunta> TodasAsPerguntas, ArrayList<Pergunta> PerguntasCorretas, ArrayList<Pergunta> PerguntasErradas, ArrayList<Jogo> top3ArrayList, pooTrivia trivia, boolean[] jogarOutraVez, String letra ) {
        jogarOutraVez[0] = false;
        int perguntasCounter = 0;
        int respostasCertas = 0;
        PerguntasCorretas.clear();
        PerguntasErradas.clear();
        TodasAsPerguntas.clear();
        top3ArrayList.clear();

        //Carregar Perguntas
        trivia.perguntasReader(filePath,TodasAsPerguntas);

        //Escreve as Peguntas que leu
        for (int j = 0; j < TodasAsPerguntas.size(); j++){
            Pergunta pergunta = TodasAsPerguntas.get(j);
            System.out.println(pergunta);
        }

        //ÊCRA INICIAL
        GUI gui = new GUI();
        boolean[] ready = {false};
        //gui.ecraInicial(ready);
        gui.menuInicial(letra, ready);

        if(ready[0]){
            System.out.println("Jogo a começar");
            while(perguntasCounter < 5){
                perguntasCounter += 1;
                //Gerar um numero aleatorio cada vez que se vai buscar uma pergunta para o jogo
                Random random = new Random();
                int questionSelector  = random.nextInt(TodasAsPerguntas.size());
                //Buscar pergunta à ArrayList
                Pergunta pergunta = TodasAsPerguntas.get(questionSelector);

                //Funcao que recebe os dados da pergunta e cria janela GUI dá return na resposta do user
                String userAnswer = pergunta.janelaPergunta(perguntasCounter, respostasCertas);

                //Analisar Resposta Utilzador e Incrementar Variaveis Respostas Certas, Adicionar à respectiva lista(Respotas Certas ou Erradas)
                String correto = pergunta.verificaResposta(userAnswer, perguntasCounter, respostasCertas);
                if (correto.equalsIgnoreCase("Sim")){ //ACERTOU!!
                    respostasCertas += 1;
                    PerguntasCorretas.add(pergunta);
                    //Janela para mostrar os pontos ganhos
                    int pontosToShow = pergunta.majoracao(pergunta.getPontosPergunta());
                    gui.showMessageCorreto(pontosToShow);
                    System.out.println("Pontuação Obtida a ser mostrada");
                }else{//ERROU!
                    PerguntasErradas.add(pergunta);
                    gui.showMessageErrado();
                    System.out.println("Mensagem de resposta errada a ser mostrada");

                }
                //Remove a pergunta da ArrayList de forma a não voltar a aparecer
                TodasAsPerguntas.remove(questionSelector);
            }

            //->No final do ciclo while (FIM DO Jogo)
            System.out.println("Fim do Jogo");

            //-> Pedir ao user o Nome (Chamar GUI para input)
            String userName = gui.getUserInput();
            System.out.println("Nome inserido foi: " + userName);

            //->Guardar Data e Tempo
            LocalDateTime currDateTime = LocalDateTime.now();
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyyMMddHHmm");
            DateTimeFormatter formatter2 = DateTimeFormatter.ofPattern("HH:mm dd/MM/yyyy");
            String formattedDateTime = currDateTime.format(formatter); //Data e Tempo Formatado
            String formatedTop3 = currDateTime.format(formatter2);

            //Criar objeto jogo
            Jogo jogo = new Jogo(userName, formatedTop3, PerguntasCorretas, PerguntasErradas,0);
            System.out.println("Objeto Jogo criado!");

            //Colocar o nome do ficheiro no formato: pootrivia_jogo_2023111020855_LFC.dat
            String nomeObjetosFile = trivia.calculaNomeFicheiro(userName, formattedDateTime);

            trivia.escreverParaFicheiroObjetos(folderPath,nomeObjetosFile, jogo);
            System.out.println("Jogo escrito para o ficheiro de objetos!");

            int pontuacaoJogo = trivia.lerFicheiroObjetos(folderPath, top3ArrayList);
            trivia.sortJogosByTotalPontos(top3ArrayList);

            ArrayList<String> top3StringArrayList = new ArrayList<>();

            trivia.criaStringTop3(top3ArrayList, top3StringArrayList);

            gui.top3(top3StringArrayList, pontuacaoJogo);

            gui.ecraFinal(jogarOutraVez);

        }else{
            System.out.println("Utilizador saiu do jogo");
        }
    }

    /**
     * O metódo criaStringTop3 vai receber o ArrayList com os jogos já ordenados por pontuação, vai aceder às variáveis UserName, DataHora e PontuaçãoJogo de cada objeto jogo de forma a construir a string a ser exibida ao utilizador aquando da janela do Top 3.
     * A string construida é armazenada no ArrayList de Strings.
     * @param jogoArrayList
     * @param stringArrayList
     */
    protected void criaStringTop3(ArrayList<Jogo> jogoArrayList, ArrayList<String> stringArrayList) {
        for (int i = 0; i < jogoArrayList.size(); i++){
            Jogo jogo = jogoArrayList.get(i);
            String nome = jogo.getUserName();
            String horaData = jogo.getDataHora();
            String pontuacao = String.valueOf(jogo.getPontuacaoJogo());
            String top = "Nome do utilizador: "+nome +" | Data e Hora: "+ horaData +" | Pontuação: "+ pontuacao+" pontos";
            stringArrayList.add(top);
        }
    }

    /**
     * O metódo passarArrayParaArrayList vai receber um array e uma ArrayList e vai passar todos os elementos do array para dentro da ArrayList
     * @param array
     * @param arraylist
     */
    protected void passarArrayParaArrayList(String[] array, ArrayList<String> arraylist){
        //Passar o que está dentro do array para o ArrayList -> Passar a método
        for (int i  = 0; i < array.length; i++){
            arraylist.add(array[i]);
        }
    }

    /**
     * O metódo perguntasReader vai ser responsável por ler o ficheiro .txt, fazer o parsing de cada linha e detetar que tipo de objeto deve criar, uma vez criado o objeto de acordo com a respectiva classe, este vai ser armazenado na ArrayList que recebe.
     * @param filePath
     * @param TodasAsPerguntas
     */
    protected void perguntasReader(String filePath, ArrayList<Pergunta> TodasAsPerguntas) {

        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            String line;

            while ((line = br.readLine()) != null) {
                // Separar as difrentes parts da linha de forma a obterms os campos
                String[] parts = line.split(";");

                // Deteta o tipo de pergunta
                if (parts[0].equalsIgnoreCase("Arte")){
                    //Acede à informação e armazena-a em variaveis
                    String enunciado = parts[1];
                    int pontosPergunta = Integer.parseInt(parts[2]);
                    String correctAnswer = parts[3];

                    // Extrair e Armazenar as opcoes de Resposta em Array
                    String[] ans = parts[4].substring(1, parts[4].length() - 1).split("\\|");

                    //Criar arrayList que vai armazenar as opcoes de resposta
                    ArrayList<String> ArrayListArte = new ArrayList<>();

                    //Passar o que está dentro do array para o ArrayList -> Passar a método
                    for (int i  = 0; i < ans.length; i++){
                        ArrayListArte.add(ans[i]);
                    }
                    // Criar Pergunta do tipo classe e adicionar à ArrayList Todas as Perguntas
                    TodasAsPerguntas.add(new Arte(enunciado, pontosPergunta, ArrayListArte, correctAnswer));
                }

                if (parts[0].equalsIgnoreCase("Ciencia")){

                    //Acede à  informação e armazena-a em variaveis
                    String enunciado = parts[1];
                    int pontosPergunta = Integer.parseInt(parts[2]);
                    String correctAnswer = parts[3];

                    // Extrair e Armazenar as opcoes de Resposta em Arrays
                    String[] ansDificil = parts[4].substring(1, parts[4].length() - 1).split("\\|");
                    String[] ansFacil = parts[5].substring(1, parts[5].length() - 1).split("\\|");

                    ArrayList<String>  ArrayListCienciaFacil = new ArrayList<>();
                    ArrayList<String> ArrayListCienciaeDificl = new ArrayList<>();

                    passarArrayParaArrayList(ansFacil, ArrayListCienciaFacil);
                    passarArrayParaArrayList(ansDificil, ArrayListCienciaeDificl);

                    TodasAsPerguntas.add(new Ciencia(enunciado, pontosPergunta, ArrayListCienciaFacil, ArrayListCienciaeDificl, correctAnswer));
                }

                if (parts[0].equals("Desporto")){
                    //Desporto;Futebol;enunciado;ptsPergunta;ArrayListNCamisola;SolutionCamisolaNumero;ArrayListNomes;SolutionNomes
                    if (parts[1].equals("Futebol")){

                        String enunciado = parts[2];
                        int pontosPergunta = Integer.parseInt(parts[3]);

                        ArrayList<String> ArrayListNCamisola = new ArrayList<>();
                        String[] playersShirtNumber = parts[4].substring(1, parts[4].length() - 1).split("\\|");
                        passarArrayParaArrayList(playersShirtNumber, ArrayListNCamisola);

                        String SolutionCamisolaNumero = parts[5];

                        ArrayList<String> ArrayListNomes = new ArrayList<>();
                        String[] playersNames = parts[6].substring(1, parts[6].length() - 1).split("\\|");
                        passarArrayParaArrayList(playersNames, ArrayListNomes);

                        String SolutionNomes = parts[7];
                        TodasAsPerguntas.add(new Futebol(enunciado, pontosPergunta, null, null, ArrayListNCamisola, SolutionCamisolaNumero, ArrayListNomes, SolutionNomes));


                    }else{ //Caso para Ski e Natação
                        ArrayList<String> OpcoesVerdadeiroFalso = new ArrayList<>();
                        OpcoesVerdadeiroFalso.add("Verdadeiro");
                        OpcoesVerdadeiroFalso.add("Falso");

                        String enunciado = parts[2];
                        int pontosPergunta = Integer.parseInt(parts[3]);
                        String correctAnswer = parts[5];

                        TodasAsPerguntas.add(new Desporto(enunciado, pontosPergunta, OpcoesVerdadeiroFalso, correctAnswer));

                    }
                }
            }
        } catch (IOException | NumberFormatException e) {
            e.printStackTrace(); // Handle the exception appropriately
        }
    }

    /**
     * O metódo calculaNomeFicheiro vai ser responsável pela criação da String a colocar no nome do ficheiro de objetos respectivo ao jogo em questão. Recebe o nome do utilizador e calcula a data e hora.
     * Vai dar return à string a colocar no nome do ficheiro de objetos
     * @param userName
     * @param DataHora
     * @return
     */
    protected String calculaNomeFicheiro(String userName, String DataHora){

        //Parte do Nome
        String userFinal = "";
        String SplittedName[] = userName.split(" "); //["Luis","Miguel","Vaz"]
        for (int i = 0; i < SplittedName.length ;i++){
            String a[] = SplittedName[i].split("");
            userFinal = userFinal + a[0];
        }
        String nome_ficheiro = "pootrivia_jogo_"+DataHora+"_"+userFinal+".dat";
        return nome_ficheiro;

    }

    /**
     * O metódo escreverParaFicheiroObjetos vai ser responsável pela escrita do objeto da classe Jogo no ficheiro de objetos. O objeto da classe Jogo que contem as informações relativas à: data e hora do jogo, nome do utilizador, lista de perguntas que o utilizador acertou e a lista de perguntas que errou.
     * @param folderPath
     * @param fileName
     * @param jogo
     */
    protected void escreverParaFicheiroObjetos(String folderPath, String fileName, Jogo jogo) {
        try {
            File folder = new File(folderPath);
            if (!folder.exists()) {
                folder.mkdirs();
            }

            String filePath = folderPath + File.separator + fileName;

            try (FileOutputStream fos = new FileOutputStream(filePath);
                 ObjectOutputStream oos = new ObjectOutputStream(fos)) {
                oos.writeObject(jogo);
            }

        } catch (IOException ex) {
            System.out.println("Erro a escrever para o ficheiro:" + ex.getMessage());
        }
    }

    /**
     * O metódo lerFicheiroObjetos vai ser responsável pela leitura de todos os ficheiros de objetos presentes no path da pasta, vai aceder às perguntas que o utilizador respondeu corretamente e calcular a pontuação do jogo lido
     * Na ArrayList que recebe vão ficar armazenados todos os jogos já com a pontuação total respectiva de cada jogo.
     * @param folderPath
     * @param arrayList
     */
    protected int lerFicheiroObjetos(String folderPath, ArrayList<Jogo> arrayList) {
        File folder = new File(folderPath);

        // Verifica se a pasta existe
        if (!folder.exists() || !folder.isDirectory()) {
            System.out.println("Pasta não encontrada!");
            return 0;
        }

        // Listar todos os ficheiros da pasta para podermos iterar
        File[] files = folder.listFiles();

        // Verifica se existem ficheiros na pasta
        if (files == null || files.length == 0) {
            System.out.println("Não foram encontrados ficheiros na pasta!");
            return 0;
        }

        // Iterar sobre cada ficheiro existente na pasta
        int pontuacaoFinalJogo = 0;
        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            try (FileInputStream fis = new FileInputStream(file);
                 ObjectInputStream ois = new ObjectInputStream(fis)) {

                // Ler o objeto do ficheiro
                Jogo jogo = (Jogo) ois.readObject();

                // A pontuação começa a 0 para cada jogo
                pontuacaoFinalJogo = 0;

                // Iterar sobre as perguntas corretas do jogo
                for (Pergunta pergunta : jogo.getPerguntasCorretas()) {
                    pontuacaoFinalJogo += pergunta.majoracao(pergunta.getPontosPergunta());
                }
                // Set a pontuação do Jogo
                jogo.setPontuacaoJogo(pontuacaoFinalJogo);

                // Guardar o jogo em ArrayList
                arrayList.add(jogo);

            } catch (IOException | ClassNotFoundException ex) {
                System.out.println("Error reading file " + file.getName() + ": " + ex.getMessage());
            }
        }

        System.out.println("Leitura do ficheiro feita com sucesso!");
        return pontuacaoFinalJogo;
    }

    /**
     * O metódo sortJogosByTotalPontos vai ser responsável por ordenar a lista de objetos Jogo que recebe.
     * @param jogos
     */
    protected void sortJogosByTotalPontos(List<Jogo> jogos) {
        Collections.sort(jogos);
    }
}

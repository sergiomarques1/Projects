import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Random;

public class PoW {

    public static void imprimirPersonagens (ArrayList<Personagem> personagens ){        // método que mostra todas as personagens existentes no jogo
        System.out.print("\n\n== Guerreiros ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Guerreiro")) {       // verifica se a personagem se trata de um guerreiro
                System.out.print(personagem);
            }
        }
        System.out.print("\n\n== Magos ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Mago")) {        // verifica se a personagem se trata de um mago
                System.out.print(personagem);
            }
        }
        System.out.print("\n\n== Mercenários ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Mercenário")) {      // verifica se a personagem se trata de um mercenário
                System.out.print(personagem);
            }
        }
    }
    public static void xpMaiorQue (ArrayList<Personagem> personagens, int n){       // método que mostra todas as personagens com nível XP maior que um valor definido (neste caso, 10)
        boolean xpEncontrado = false;       // variável utilizada para perceber se existe alguma personagem com nível XP superior ao indicado
        for(Personagem personagem : personagens){
            if(personagem.getNivelXP() > n){
                xpEncontrado = true;        // se for encontrada pelo menos uma personagem com nível XP superior, a variável passa a ter valor true
                System.out.print(personagem);
            }
        }
        if(xpEncontrado == false){      // se a variável continuar com o valor false depois de ter sido percorrida a arrayList onde estão todas as personagens, é porque não existe nenhuma oersonagem com o nível XP superior ao indicado
            System.out.println("\nNão existe nenhuma personagem com nível XP superior a "+n);
        }
    }

    public static void atributosEspeciais(ArrayList<Personagem> personagens ){      // método que mostra todas as personagens com atributos especiais (guerreiros com armadura, magos com semntes de abóbora e mercenários com arcos)
        System.out.print("\n\n== Guerreiros com armadura ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Guerreiro")) {
                personagem.personagemEspecial();
            }
        }
        System.out.print("\n\n== Magos com sementes de abóbora ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Mago")) {
                personagem.personagemEspecial();
            }
        }
        System.out.print("\n\n== Mercenários com arco ==");
        for(Personagem personagem : personagens) {
            if ((personagem.getTipo()).equals("Mercenário")) {
                personagem.personagemEspecial();
            }
        }
    }

    public static void main(String[] args){

        Scanner scanner = new Scanner(System.in);

        ArrayList<Personagem> personagens = new ArrayList<>();      // criação da arrayList onde vão ser guardadas todas as personagens
        ArrayList<String> armasCurto = new ArrayList<>(Arrays.asList("Faca", "Machado", "Espada"));     // arrayList com as armas de curto alcance possíveis para os guerreiros
        ArrayList<String> sementesArray = new ArrayList<>(Arrays.asList("abóbora", "feijão", "batata"));        // arrayList com tipos de sementes que vão ser escolhidos aleatoriamente na criação dos primeiros 5 magos
        ArrayList<String> folhasArray = new ArrayList<>(Arrays.asList("eucalipto", "tília", "castanheiro"));        // arrayList com tipos de folhas que vão ser escolhidos aleatoriamente na criação dos primeiros 5 magos
        ArrayList<String> armasLongo = new ArrayList<>(Arrays.asList("Pedras", "Arco", "Pistola"));     // arrayList com as armas de longo alcance possíveis para os mercenários

        for(int i = 0; i < 5; i++){     // criação das primeiras 15 personagens (5 de cada tipo)
            personagens.add(new Guerreiro("Guerreiro", "Guerreiro"+(i + 1), new Random().nextInt(0, 25) + 1, 3, 10, 5, new Random().nextBoolean(), armasCurto.get(new Random().nextInt(0, 3))));
            personagens.add(new Mago("Mago", "Mago"+(i + 1), new Random().nextInt(0, 25) + 1, 9, 2, 4, sementesArray.get(new Random().nextInt(0, 3)), folhasArray.get(new Random().nextInt(0, 3))));
            personagens.add(new Mercenario("Mercenário", "Mercenário"+(i + 1), new Random().nextInt(0, 25) + 1, 4, 4, 10, armasLongo.get(new Random().nextInt(0, 3)), new Random().nextInt(10, 20) + 1));
        }

        int opcao;      // variável utilizada para a seleção da opção pretendida no menu principal
        do{
            System.out.println("\n\n=== POAO OF WARCRAFT ===");                 // menu principal do jogo
            System.out.println("     -->  PoW  <--      \n");
            System.out.println("1 - Personagens Disponíveis");
            System.out.println("2 - Criar Personagem");
            System.out.println("3 - Personagens com nivel XP superior a 10");
            System.out.println("4 - Personagens com Atributos Especiais");
            System.out.println("5 - Evoluir Personagem");
            System.out.println("6 - Sair do Jogo");
            System.out.print("\nOpção: ");
            opcao = scanner.nextInt();
            scanner.nextLine();
            switch(opcao){
                case 1:
                    imprimirPersonagens(personagens);       // chamada do método que imprime np ecrã todas as personagens
                    break;
                case 2:
                    boolean personagemEncontrada = false;       // variável utilizada para verificar se existe alguma personagem com o nome igual ao nome inserido
                    System.out.print("\nIntroduza o nome da nova personagem: ");
                    String nomePersonagem = scanner.nextLine();
                    for(Personagem personagem : personagens){
                        if(nomePersonagem.equals(personagem.getNome())){
                            personagemEncontrada = true;        // se existir alguma personagem com o nome que foi inserido, a variável passa a ter o valor true e não se procedrá à criação da personagem
                            System.out.println("\nJá existe uma personagem com este nome!");
                        }
                    }
                    if(personagemEncontrada == false) {     // se o valor da variável se mantiver em false, procede-se à criação da nova personagem
                        System.out.println("\nSelecione o tipo de personagem que pretende criar:\n");       // escolha do tipo de personagem
                        System.out.println("1 - Guerreiro");
                        System.out.println("2 - Mago");
                        System.out.println("3 - Mercenário");
                        System.out.print("\nOpção: ");
                        int tipoPersonagem = scanner.nextInt();
                        if (tipoPersonagem == 1) {
                            System.out.println("\nEscolha uma arma de curto alcance:\n");       // escolha da arma de curto alcance para o guerreiro
                            System.out.println("1 - Faca");
                            System.out.println("2 - Machado");
                            System.out.println("3 - Espada");
                            System.out.print("\nOpção: ");
                            int tipoArmaCurto = scanner.nextInt();
                            String armaCurto;
                            if (tipoArmaCurto == 1) {
                                armaCurto = "Faca";
                            } else if (tipoArmaCurto == 2) {
                                armaCurto = "Machado";
                            } else if (tipoArmaCurto == 3) {
                                armaCurto = "Espada";
                            } else {
                                System.out.println("\nSelecionou uma opção inválida!");
                                System.out.println("Tente outra vez");
                                break;
                            }
                            System.out.println("\nArmadura do Guerreiro:\n");       // possibilidade de se escolher se o guerreiro possui ou não uma armadura
                            System.out.println("1 - Sim");
                            System.out.println("2 - Não");
                            System.out.print("\nOpção: ");
                            int armaduraSimNao = scanner.nextInt();
                            boolean armadura;
                            if (armaduraSimNao == 1) {
                                armadura = true;
                            }
                            else if (armaduraSimNao == 2) {
                                armadura = false;
                            }
                            else {
                                System.out.println("\nSelecionou uma opção inválida!");
                                System.out.println("Tente outra vez");
                                break;
                            }
                            // criação de um novo guerreiro com os dados escolhidos pelo utilizador
                            personagens.add(new Guerreiro("Guerreiro", nomePersonagem, new Random().nextInt(0, 25) + 1, 3, 10, 5, armadura, armaCurto));
                            System.out.print("Personagem criada com sucesso!");
                        }
                        else if (tipoPersonagem == 2) {
                            System.out.println("\nConteúdo da mochila:\n");     // escolha do tipo de sementes e do tipo de folhas do mago
                            System.out.print("Sementes de: ");
                            String sementes = scanner.next();
                            System.out.print("Folhas de: ");
                            String folhas = scanner.next();
                            // criação de um novo mago com os dados escolhidos pelo utilizador
                            personagens.add(new Mago("Mago", nomePersonagem, new Random().nextInt(0, 25) + 1, 9, 2, 4, sementes, folhas));
                            System.out.print("\nPersonagem criada com sucesso!");
                        }
                        else if (tipoPersonagem == 3) {
                            System.out.println("\nEscolha uma arma de longo alcance:\n");       // escolha da arma de longo alcance do mercenário
                            System.out.println("1 - Pedras");
                            System.out.println("2 - Arco");
                            System.out.println("3 - Pistola");
                            System.out.print("\nOpção: ");
                            int tipoArmaLongo = scanner.nextInt();
                            String armaLongo;
                            if (tipoArmaLongo == 1) {
                                armaLongo = "Faca";
                            } else if (tipoArmaLongo == 2) {
                                armaLongo = "Machado";
                            } else if (tipoArmaLongo == 3) {
                                armaLongo = "Espada";
                            } else {
                                System.out.println("\nSelecionou uma opção inválida!");
                                System.out.println("Tente outra vez");
                                break;
                            }
                            System.out.print("\nNúmero de munições: ");     // escolha do número de munições
                            int municoes = scanner.nextInt();
                            // criação de um novo mercenário com os dados escolhidos pelo utilizador
                            personagens.add(new Mercenario("Mercenário", nomePersonagem, new Random().nextInt(0, 25) + 1, 4, 4, 10, armaLongo, municoes));
                            System.out.print("\nPersonagem criada com sucesso!");
                        }
                        else{
                            System.out.println("\nSelecionou uma opção inválida!");
                            System.out.println("Tente outra vez");
                            break;
                        }
                    }
                    break;
                case 3:
                    xpMaiorQue(personagens, 10);        // chamada do método que imprime no ecrã todas as personagens com nível XP supeior a 10
                    break;
                case 4:
                    atributosEspeciais(personagens);        // chamada do método que imprime no ecrã todas as personagens com atributos especiais
                    break;
                case 5:
                    boolean nomeEncontrado = false;     // variável que verifica se existe alguma personagem com o nome igual ao nome inserido
                    System.out.print("\nIntroduza o nome da personagem a evoluir: ");
                    String nome = scanner.nextLine();
                    for(Personagem personagem : personagens){
                        if(nome.equals(personagem.getNome())){
                            nomeEncontrado = true;      // se o nome inserido for igual ao nome de alguma personagem, o valor da variável passa a ser true
                            if(personagem.getNivelXP() < 25){       // verifica se o nível XP é menor que 25, uma vez que 25 é ovalor máximo para o nível XP
                                System.out.println("\nPersonagem evoluída com sucesso!");
                                System.out.println("\nEstatísticas antes da evolução:");
                                System.out.println("Nível XP: " +personagem.getNivelXP());
                                System.out.println("Força: " +personagem.getForca());
                                System.out.println("Agilidade: " +personagem.getAgilidade());
                                System.out.println("Inteligência: " +personagem.getInteligencia());
                                personagem.evoluirPersonagem();     // chamada do método que evolui a personagem que se pretende evoluir
                                System.out.println("\nEstatísticas depois da evolução:");
                                System.out.println("Nível XP: " +personagem.getNivelXP());
                                System.out.println("Força: " +personagem.getForca());
                                System.out.println("Agilidade: " +personagem.getAgilidade());
                                System.out.println("Inteligência: " +personagem.getInteligencia());
                            }
                            else{
                                System.out.println("\nA personagem que pretende evoluir já se encontra no seu nível máximo!");
                                System.out.println("Tente evoluir outra personagem");
                            }

                        }
                    }
                    if (nomeEncontrado == false){       // se a variável continuar com o valor false, significa que não existe nenhuma personagem com o nome inserido
                        System.out.println("\nNão foi encontrada nenhuma personagem com esse nome!");
                        System.out.println("Tente outra vez");
                    }
                    break;

                case 6:
                    System.out.println("\nA sair do jogo...\n");
                    break;

                default:
                    System.out.println("\nSelecionou uma opção inválida!");
                    break;
            }
        } while(opcao != 6);        // repetição do menu até à opção inserida no menu principal ser igual a 6 (ou seja, até que se pretenda sair do jogo)
    }
}
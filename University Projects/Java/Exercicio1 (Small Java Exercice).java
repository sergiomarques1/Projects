public class Exercicio1 {

    public static void main(String[] args) {

        String[] URLs = {
                "https://www.dei.uc.pt/poao/exames",
                "http://www.scs.org/index.html",
                "https://www.nato.int/events",
                "https://www.btu.de/",
                "https://www.dei.uc.pt/poao/exames",
                "http://www.eth.ch/index.html",
                "http://www.osu.edu/" };

        String[][] paises = {
                {"pt", "Portugal"},
                {"org", "EUA"},
                {"fr", "França"},
                {"uk", "Reino Unido"},
                {"de", "Alemanha"},
                {"edu", "EUA"} };

        String[] paisesNaoDuplicados = paisesNaoDuplicados(paises);
        int[] contador = contador(paises, URLs, paisesNaoDuplicados);
        String[][] contadorPorPais = contadorPorPais(paisesNaoDuplicados, contador);
        imprimirContadores(contadorPorPais);
    }
    public static String[] paisesNaoDuplicados(String[][] paises){

        int paisesDuplicados = 0;

            for (int i = 0; i < paises.length; i++) {   // bloco de código que calcula o número de repetições de países iguais da matriz "paises" (com o intuito de calcular o tamanho do array "paisesNaoDuplicados")
                for (int j = 0; j < paises.length; j++) {
                    if ((i < j) && (paises[i][1].equals(paises[j][1]))) {
                        paisesDuplicados++;
                        break;
                    }
                }
            }

        String[] paisesNaoDuplicados = new String[paises.length - paisesDuplicados];  // array que guarda o nome dos países apenas uma vez (sem repetições)

        int n = 0;
        for (int i = 0; i < paises.length; i++) {
            int duplicados = 0;   // variável que verifica se o nome do país já está inserido no array "paisesNaoDuplicados"
            for (int j = 0; j < paises.length; j++) {
                if ((i > j) && (paises[i][1].equals(paises[j][1]))) {
                    duplicados = 1;
                    break;
                }
            }
            if (duplicados == 0) {
                paisesNaoDuplicados[n] = paises[i][1];
                n++;
            }
        }
        return paisesNaoDuplicados;
    }
    public static int[] contador(String[][] paises, String[] URLs, String[] paisesNaoDuplicados) {

        int[] contador = new int[paisesNaoDuplicados.length + 1];

        for (String url : URLs) {
            int identificadorEncontrado = 0;  // variável usada para verificar se o identificador do país foi ou não encontrado
            String[] separar = url.split("//");  // separação de cada link nas duas barras consecutivas
            separar = separar[1].split("/");  // separa a parte da direita do link (pos 1) por barra
            int indiceID = (separar[0].lastIndexOf(".")) + 1;  // obtenção do indice do caracter inicial do identificador do pais
            String idPais = separar[0].substring(indiceID);  // string que armazena o identificador de país
            for (String[] pais : paises) {
                if (idPais.equals(pais[0])) {
                    identificadorEncontrado = 1;
                    for (int j = 0; j < paisesNaoDuplicados.length; j++) {
                        if (pais[1].equals(paisesNaoDuplicados[j])) {
                            contador[j]++;  // incremento do contador do país encontrado
                        }
                    }
                }
            }

            if (identificadorEncontrado == 0) {
                contador[contador.length - 1]++;  // caso não seja encontrado nenhum identificador de um país existente na matriz "paises", o contador (contador de outros países) que se encontra na ultima posição do array "contador" será incrementado
            }
        }
        return contador;
    }
    public static String[][] contadorPorPais(String[] paisesNaoDuplicados, int[] contador){

        String[][] contadorPorPais = new String[paisesNaoDuplicados.length + 1][2];  //matriz que armazena os nomes e contadores dos respetivos países
        for (int i = 0; i < paisesNaoDuplicados.length; i++) {
            contadorPorPais[i][0] = paisesNaoDuplicados[i];
            contadorPorPais[i][1] = String.valueOf(contador[i]);
        }
        contadorPorPais[contador.length - 1][0] = "Outro(s)";
        contadorPorPais[contador.length - 1][1] = String.valueOf(contador[contador.length - 1]);

        return contadorPorPais;
    }
    public static void imprimirContadores(String[][] contadorPorPais) {

        for (String[] contador : contadorPorPais) {
            if (!(contador[1].equals("0"))) {  // condição que nos permite imprimir no ecrã apenas os países que têm contadores diferentes de zero
                System.out.println(contador[0] + ": " + contador[1]);
            }
        }
    }
}
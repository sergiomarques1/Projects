package app_meta1;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.Properties;

public class Configuration {
    
    private static final Properties config = new Properties();
    private static boolean carregada = false;

    public static String HOST_RMI;
    public static int PORTA_RMI;
    public static int NUM_DOWN;
    public static int NUM_BARRELS;
    public static int TAM_PAGINA;

    static {
        carregarInfo();
        HOST_RMI = propriedade("host.rmi");
        PORTA_RMI = intPropriedade("porta.rmi");
        NUM_DOWN = intPropriedade("num.down");
        NUM_BARRELS = intPropriedade("num.barrels");
        TAM_PAGINA = intPropriedade("tam.pagina");

    }

    public static void carregarInfo() {
        if (!carregada) {
            try (FileInputStream ficheiro = new FileInputStream("config.properties")) {
                config.load(ficheiro);
                carregada = true;
            } catch (IOException e) {
                System.err.println("Erro ao carregar o ficheiro de configurações: " + e.getMessage());
            }
        }
    }

    public static String propriedade(String chave) {
        if (!carregada) {
            carregarInfo();
        }
        return config.getProperty(chave);
    }

    public static int intPropriedade(String chave) {
        String valor = propriedade(chave);
            return Integer.parseInt(valor);
    }
}
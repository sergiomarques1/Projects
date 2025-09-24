package app_meta1;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.ArrayList;

public interface BarrelRemote extends Remote{

    public void inserirDados(String url, String titulo, String descricao, ArrayList<String> palavras, ArrayList<String> ligacoes) throws RemoteException;

    public int numLigacoes(String url) throws RemoteException;

    public ArrayList<String> pesquisa(String pesquisa, int pagina, int tamanhoPagina) throws RemoteException;

    public int getTotalResultados(String pesquisa) throws RemoteException;

    public ArrayList<String> ligacoes(String url) throws RemoteException;

    // Método para retornar o tamanho do índice para estatísticas
    public int getTamanhoIndice() throws RemoteException;
}
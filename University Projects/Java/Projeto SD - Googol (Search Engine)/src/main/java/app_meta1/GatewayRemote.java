package app_meta1;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Map;

public interface GatewayRemote extends Remote{

    public void enviarURL(String url) throws RemoteException;

    public ArrayList<String> efetuarPesquisa(String pesquisa, int pagina) throws RemoteException;

    public int getTotalResultados(String pesquisa) throws RemoteException;

    public int getTotalPaginas(String pesquisa) throws RemoteException;

    public ArrayList<String> ligacoesPagina(String url) throws RemoteException;

    // Métodos para estatísticas
    public Map<String, Integer> getTop10Pesquisas() throws RemoteException;

    public Map<String, Integer> getBarrelsInfo() throws RemoteException;

    public Map<String, Double> getTempoMedioPorBarrel() throws RemoteException;
}
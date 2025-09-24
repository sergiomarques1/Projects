package app_meta1;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface URLQueueRemote extends Remote {

    public void adicionarURL(String url) throws RemoteException;
    public String retirarURL() throws RemoteException;

}
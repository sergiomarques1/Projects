package app_meta1;

import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.rmi.server.UnicastRemoteObject;

public class URLQueue extends UnicastRemoteObject implements URLQueueRemote {

    Queue<String> queue;

    ArrayList<String> existentes;

    protected URLQueue() throws RemoteException{
        super();
        queue = new ConcurrentLinkedQueue<>();
        existentes = new ArrayList<>();
        
    }

    public synchronized void adicionarURL(String url){

        System.out.println("Url "+url+" recebido ...");

        if(!existentes.contains(url)){
            queue.add(url);
            existentes.add(url);
            System.out.println("Url "+url+" adicionado à queue com sucesso!");
        }
        else{
            System.out.println("Url "+url+" já está na queue!");
        }
    }    

    public synchronized String retirarURL(){

        if(queue.isEmpty()){
            return null;
        }

        return queue.poll();
    }
}

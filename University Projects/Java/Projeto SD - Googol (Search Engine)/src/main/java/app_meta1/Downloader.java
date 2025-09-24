package app_meta1;

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class Downloader {

    private static URLQueueRemote queue;

    private static void conectarQueue(){
                
        try{
            queue = (URLQueueRemote) Naming.lookup("rmi://"+Configuration.HOST_RMI+":"+Configuration.PORTA_RMI+"/QUEUE");
        } catch(MalformedURLException e){
            System.err.println("\nEndereço da queue está errado!");
            System.exit(-1);
        } catch(NotBoundException | RemoteException e){
            conexaoFalhada();
        }

    }

    private static void conexaoFalhada(){

        System.err.println("Impossível conectar à Queue. Nova tentativa de conexão dentro de 5 segundos!");

        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
            System.out.println("Espera interrompida. A tentar conectar à queue!");
        }
        conectarQueue();
    }

    public static void main(String[] args) throws InterruptedException {
        
        conectarQueue();

        int num_threads = Configuration.NUM_DOWN;

        Thread[] threads = new Thread[num_threads];
        
        for (int i = 0; i < num_threads; i++) {

            threads[i] = new DownloaderThread(i, queue);
            System.out.println("Downloader "+(i+1)+" iniciado!");
            threads[i].start();

        }
        for (Thread thread : threads) {
            thread.join();
        }
    }   
}
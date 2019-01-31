import java.io.*;
import java.net.*;

//THREAD CLASS
class ServerThread extends Thread {

  // varialibili
  private Socket clientSocket = null;
  private Stanza[] listaStanze = null;

  // costruttore
  public ServerThread(Socket clientSocket, Stanza[] listaStanze) {
    this.clientSocket = clientSocket;
    this.listaStanze = listaStanze;
  }

  // THREAD IMPLEMENTATION
  public void run() {
    System.out.println("Attivazione figlio: " + Thread.currentThread().getName());
    String requestType = null;

    // Creazione input/output Streams della socket.
    DataInputStream inSock;
    DataOutputStream outSock;
    ObjectOutputStream objectOut = null;
    ObjectInputStream objectIn = null;
    try {
      inSock = new DataInputStream(clientSocket.getInputStream());
      outSock = new DataOutputStream(clientSocket.getOutputStream());
      objectOut = new ObjectOutputStream(outSock);
      objectIn = new ObjectInputStream(inSock);
    } catch (IOException ioe) {
      System.out.println("Problemi nella creazione degli stream di input/output su socket: ");
      ioe.printStackTrace();
      return;
    }

    try {
      try {
        // GESTIONE RICHIESTE
        while (true) {

          // lettura tipo richiesta
          try{
            requestType = inSock.readUTF();
          }catch(SocketTimeoutException ste) {
            System.out.println("Timeout scattato: ");
            ste.printStackTrace();
            clientSocket.close();
            System.exit(1);
          }

          // Visualizza stanza
          if (requestType.equals("VS")) {

            // lettura dei filtri
            String nome = null;
            nome = inSock.readUTF();
            boolean trovata = false;

            for (int i = 0; i < listaStanze.length; i++) {
              if (listaStanze[i].nome.equals(nome)) {
                //trovata stanza richiesta, invio ho finito.
                objectOut.writeObject(listaStanze[i]);
                trovata = true;
                break;
              }
            }

            if (!trovata) {
              // Stanza assente ritorna stanza vuota
              objectOut.writeObject(new Stanza());
            }

          } //First request type
          // Second Reuqest type
          else if (requestType.equals("RS")) {
            
            String nome = inSock.readUTF();
            boolean trovata = false;

            for (int i = 0; i < listaStanze.length; i++) {
              if (listaStanze[i].nome.equals(nome)) {
                if (listaStanze[i].stato.charAt(0) == 'S') {
                  listaStanze[i].stato = listaStanze[i].stato.replace("S", "");
                  System.out.println("Rimossa S, new stato: " + listaStanze[i].stato);
                  //Stanza riavviata con successo
                  outSock.writeInt(1);
                  
                  //WARNING: we just changed the our data structure, since serialized objects 
                  // are intelligently cached by ObjectOutputStream, we must 
                  // tell it to not send the previously serialized version as it may no longer be 
                  // up to date, If no change was made sending the cached version will improve
                  // performance incase of large number of requests.
                  // To ignore previous serialized object use:
                  objectOut.reset();
                  
                  trovata = true;
                  break;
                } else {
                  System.out.println("Stanza gia avviata: " + listaStanze[i].stato);
                  trovata = true;
                  //Stanza gia avviata, no operation required
                  outSock.writeInt(0);
                }
              }
            }

            if (!trovata) {
              //Stanza assente ritorna errore
              outSock.writeInt(-1);
            } 
            
          } // second request
        }//while (ciclo richieste)
      } catch (EOFException eof) {
        System.out.println("Raggiunta la fine delle ricezioni, chiudo...");
        clientSocket.close();
        System.out.println("Server: termino thread...");
        //System.exit(0);
      }
    } catch (IOException ioe) {
      System.out.println("Problemi nella chiusura della socket: ");
      ioe.printStackTrace();
      System.out.println("Chiudo ed esco...");
      System.exit(3);
    }
    
  }//run()

}// thread

public class ServerStream {

  public static void main(String[] args) throws IOException {
    int port = -1;

    try {
      if (args.length == 1) {
        port = Integer.parseInt(args[0]);
        // controllo che la porta sia nel range consentito 1024-65535
        if (port < 1024 || port > 65535) {
          System.out.println("# Usage: java ServerName [serverPort>1024]");
          System.exit(1);
        }
      } else {
        System.out.println("# Usage: java ServerName port");
        System.exit(1);
      }
    } catch (Exception e) {
      System.out.println("Problemi, i seguenti: ");
      e.printStackTrace();
      System.out.println("# Usage: java ServerName port");
      System.exit(1);
    }

    //CREAZIONE E INIZIALIZZAZIONE DI STRUTTURE DATI
    final int N = 10;
    Stanza[] listaStanze = new Stanza[N];
    for(int i=0; i<N; i++){
      listaStanze[i] = new Stanza();
    }
    int i = 0;
    listaStanze[i].nome = "Stanza 1";
    listaStanze[i].stato = "P";
    listaStanze[i].users[0] = "Pippo";
    listaStanze[i].users[1] = "Minnie";
    listaStanze[i].users[2] = "Orazio";
    i++;
    listaStanze[i].nome = "Stanza 2";
    listaStanze[i].stato = "SP";
    listaStanze[i].users[0] = "Pippo";
    listaStanze[i].users[1] = "Pluto";
    i++;
    listaStanze[i].nome = "Stanza 3";
    listaStanze[i].stato = "M";
    listaStanze[i].users[0] = "Pluto";
    listaStanze[i].users[1] = "Paperino";
    listaStanze[i].users[2] = "Quo";
    listaStanze[i].users[3] = "Qui";
    i = N-1;
    listaStanze[i].nome = "Stanza " + (N - 1);
    listaStanze[i].stato = "SM";    

    ServerSocket serverSocket = null;
    Socket clientSocket = null;

    try {
      serverSocket = new ServerSocket(port);
      serverSocket.setReuseAddress(true);
      System.out.println("Server: avviato ");
      System.out.println("Server: creata la server socket: " + serverSocket);
    } catch (Exception e) {
      System.err.println("Server: problemi nella creazione della server socket: " + e.getMessage());
      e.printStackTrace();
      serverSocket.close();
      System.exit(1);
    }

    try {
      while (true) {
        System.out.println("Server: in attesa di richieste...\n");

        try {
          clientSocket = serverSocket.accept(); // bloccante!!!
          System.out.println("Server: connessione accettata: " + clientSocket);
        } catch (Exception e) {
          System.err.println("Server: problemi nella accettazione della connessione: " + e.getMessage());
          e.printStackTrace();
          continue;
        }

        try {
          new ServerThread(clientSocket, listaStanze).start();
        } catch (Exception e) {
          System.err.println("Server: problemi nel server thread: " + e.getMessage());
          e.printStackTrace();
          continue;
        }
      } // while true
    } catch (Exception e) {
      e.printStackTrace();
      System.out.println("Server: termino...");
      System.exit(2);
    }
  }
}

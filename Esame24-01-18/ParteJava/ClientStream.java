import java.net.*;
import java.io.*;

public class ClientStream {

  public static void main(String[] args) throws IOException {

    // CONTROLLO E ASSEGNAMENTO DEGLI ARGOMENTI
    InetAddress addr = null;
    int port = -1;

    try {
      if (args.length == 2) {
        addr = InetAddress.getByName(args[0]);
        port = Integer.parseInt(args[1]);
        if (port < 1024 || port > 65535) {
          System.out.println("Usage: java ClientStream serverAddr serverPort");
          System.exit(1);
        }
      } else {
        System.out.println("Usage: java ClientStream serverAddr serverPort");
        System.exit(1);
      }
    } catch (Exception e) {
      System.out.println("Problemi, i seguenti: ");
      e.printStackTrace();
      System.out.println("Usage: java ClientStream serverAddr serverPort");
      System.exit(2);
    }

    // CREAZIONE DELLA SOCKET E DEGLI STREAM I/O
    Socket socket = null;
    DataInputStream inSock = null;
    DataOutputStream outSock = null;

    try {
      socket = new Socket(addr, port);
      socket.setSoTimeout(30000);
      System.out.println("Creata la socket: " + socket);
      inSock = new DataInputStream(socket.getInputStream());
      outSock = new DataOutputStream(socket.getOutputStream());
    } catch (IOException ioe) {
      System.out.println("Problemi nella creazione degli stream su socket: ");
      ioe.printStackTrace();
      System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
      System.exit(1);
    }

    // LETTURA DEI COMANDI DA STDIN
    BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
    String comando = null;
    String nomeFolder = null;
    long dim_min = 0;
    String nomeFile = null;
    String parolaDaEliminare = null;

    System.out.println("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il comando di una operazione.");
    System.out.print("# Comando (E=Eliminazione, T=Trasferimento): ");
    while ((comando = stdIn.readLine()) != null) {
      if (comando.equals("T")) {

        System.out.print("Inserire il nome del direttorio: ");
        nomeFolder = stdIn.readLine();
        System.out.print("Inserire la dimensione di soglia: ");
        dim_min = Long.parseLong(stdIn.readLine());

        // INVIO TIPO RICHIESTA AL SERVER
        outSock.writeUTF(comando);

        // INVIO DATI RICHIESTA
        outSock.writeUTF(nomeFolder);
        outSock.writeLong(dim_min);

        try {
          String nomeFileRicevuto;
          long numeroByte;
          FileOutputStream outFileCorr;

          while ((nomeFileRicevuto = inSock.readUTF()) != null) {
            if (nomeFileRicevuto.equals("#")) {
              break;
            }

            numeroByte = inSock.readLong();
            System.out.println("Scrivo il file " + nomeFileRicevuto + " di " + numeroByte + " byte");
            outFileCorr = new FileOutputStream(nomeFileRicevuto);
            FileUtility.trasferisci_N_byte_file_binario(inSock, new DataOutputStream(outFileCorr), numeroByte);
            outFileCorr.close();

          } // while
        } catch (SocketTimeoutException ste) {
          System.out.println("Timeout scattato: ");
          ste.printStackTrace();
          socket.close();
          System.exit(1);
        } catch (Exception e) {
          System.out.println("Problemi, i seguenti : ");
          e.printStackTrace();
          System.out.println("Chiudo ed esco...");
          socket.close();
          System.exit(2);
        }
        System.out.println("Trasferimento concluso");        
        //FINE CICLO DI TRASFERIMENTO
      } else if(comando.equals("E")){

        System.out.print("Inserire il nome del file: ");
        nomeFile = stdIn.readLine();
        System.out.print("Inserire la parola da eliminare: ");
        parolaDaEliminare = stdIn.readLine();

        // INVIO TIPO RICHIESTA AL SERVER
        outSock.writeUTF(comando);

        // INVIO DATI RICHIESTA
        outSock.writeUTF(nomeFile);
        outSock.writeUTF(parolaDaEliminare);
        
        //RICEZIONE ESITO DELLA RICHIESTA
        try {
          int numOccorrenzeEliminate = inSock.readInt();
          System.out.println("Numero di occorrenze di "+parolaDaEliminare+" eliminate dal file "+nomeFile+": "+numOccorrenzeEliminate);
        } catch (SocketTimeoutException ste) {
          System.out.println("Timeout scattato: ");
          ste.printStackTrace();
          socket.close();
          System.exit(1);
        } catch (Exception e) {
          System.out.println("Problemi, i seguenti : ");
          e.printStackTrace();
          System.out.println("Chiudo ed esco...");
          socket.close();
          System.exit(2);
        }
        System.out.println("Richiesta di eliminazione terminata");
      }//fine eliminazione

      System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il nome della cartella: ");
      continue;
    }//fine while richiesta di comandi

    System.out.println("Chiudo e termino...");
    socket.close();

  }
}

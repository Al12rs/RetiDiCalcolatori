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
    ObjectOutputStream objectOut = null;
    ObjectInputStream objectIn = null;

    try {
      socket = new Socket(addr, port);
      socket.setSoTimeout(30000);
      System.out.println("Creata la socket: " + socket);
      inSock = new DataInputStream(socket.getInputStream());
      outSock = new DataOutputStream(socket.getOutputStream());
      objectOut = new ObjectOutputStream(outSock);
      objectIn = new ObjectInputStream(inSock);
    } catch (IOException ioe) {
      System.out.println("Problemi nella creazione degli stream su socket: ");
      ioe.printStackTrace();
      System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, solo invio per continuare: ");
      System.exit(1);
    }

    // LETTURA DEI COMANDI DA STDIN
    BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
    String comando = null;
    String nomeStanza = null;

    System.out.println("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti il comando di una operazione.");
    System.out.print("# Comando (VS=Visualizza stanza, RS=Ripresa stanza): ");
    while ((comando = stdIn.readLine()) != null) {
      if (comando.equals("VS")) {

        System.out.print("Inserire nome stanza: ");
        nomeStanza = stdIn.readLine();

        // INVIO TIPO RICHIESTA AL SERVER
        outSock.writeUTF(comando);

        // INVIO DATI RICHIESTA
        outSock.writeUTF(nomeStanza);
        Stanza receivedRoom = null;

        try {
          // RICEZIONE EVENTI VALIDI
          receivedRoom = (Stanza) objectIn.readObject();
          if (!receivedRoom.nome.equals("L")) {
            System.out.println(receivedRoom.toString());
          } else {
            System.out.println("No such room");
          }
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
        // FINE COMANDO 1
      } else if (comando.equals("RS"))
      {

        System.out.print("Inserire nome stanza: ");
        nomeStanza = stdIn.readLine();

        // INVIO TIPO RICHIESTA AL SERVER
        outSock.writeUTF(comando);

        // INVIO DATI RICHIESTA
        outSock.writeUTF(nomeStanza);

        // RICEZIONE ESITO DELLA RICHIESTA
        int result;

        try {
          result = inSock.readInt();
          if (result == -1) {
            System.out.println("No such room");
          } else if (result == 0) {
            System.out.println("Stanza già avviata");
          } else {
            System.out.println("Operazione eseguita correttamente");
          }
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
      } // fine eliminazione

      System.out.print("\n^D(Unix)/^Z(Win)+invio per uscire, altrimenti immetti tipo operazione: ");
      System.out.print("# Comando (VET=Visualizza eventi per tipo e luogo, VEP=Visualizza eventi per prezzo): ");
    } // fine
      // while
      // richiesta
      // di
      // comandi

    System.out.println("Chiudo e termino...");
    socket.close();

  }
}

import java.io.*;
import java.net.*;

//THREAD CLASS
class ServerThread extends Thread {

  // varialibili
  private Socket clientSocket = null;

  // costruttore
  public ServerThread(Socket clientSocket) {
    this.clientSocket = clientSocket;
  }

  // THREAD IMPLEMENTATION
  public void run() {
    System.out.println("Attivazione figlio: " + Thread.currentThread().getName());
    String requestType = null;

    // Creazione input/output Streams della socket.
    DataInputStream inSock;
    DataOutputStream outSock;
    try {
      inSock = new DataInputStream(clientSocket.getInputStream());
      outSock = new DataOutputStream(clientSocket.getOutputStream());
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

          // Transfer Files Request
          if (requestType.equals("T")) {

            // lettura path della cartella source
            String folderName = null;
            folderName = inSock.readUTF();

            long minDim = inSock.readLong();

            File currentDir = new File(folderName);
            if (currentDir.exists() && currentDir.isDirectory()) {
              File[] files = currentDir.listFiles();

              // iterate on files in currentDir
              for (int i = 0; i < files.length; i++) {
                File currentFile = files[i];
                System.out.println("File con nome: " + currentFile.getName());
                if (currentFile.isFile() && minDim <= currentFile.length()) {

                  // Send fileName
                  outSock.writeUTF(currentFile.getName());

                  // Send fileSize
                  outSock.writeLong(currentFile.length());

                  // Send file
                  FileUtility.trasferisci_N_byte_file_binario(
                      new DataInputStream(new FileInputStream(currentFile.getAbsolutePath())), outSock,
                      currentFile.length());

                } else
                  System.out.println("File saltato");
              } // fine for

            } // if currentDir.exists

            // finiti i file da trasferire
            outSock.writeUTF("#");

          } // transfer files request
          // Eliminate Word Request
          else if (requestType.equals("E")) {
            System.out.println("Richiesta Elimina");
            String wordToDelete;
            String targetFileName;
            int removedCount = 0;
            char[] separators = { ' ', '\n' };

            targetFileName = inSock.readUTF();

            wordToDelete = inSock.readUTF();

            System.out.println("Elimina " + wordToDelete +" da "+ targetFileName);

            try {
              removedCount = FileUtility.removeWordFromFile(targetFileName, wordToDelete, separators);
            } catch (IOException e) {
              System.out.println("Errori durante la lettura/scrittura files.");
              removedCount = -1;
            }
            outSock.writeInt(removedCount);

          } // eliminate word request
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
          new ServerThread(clientSocket).start();
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


/** RMI_Server.java
 * 
 * 
 **/

import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.io.*;
import java.util.*;//ArrayList

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	public static final long serialVersionUID = 1;

	// Costruttore
  public RMI_Server() throws RemoteException {
    super();
  }

  //AUXILIAR FUNCTIONS   

  //FIRST RMI METHOD
  public String[] lista_nomi_file_contenenti_parola_in_linea(String directoryPath, String keyword)
      throws RemoteException 
  {
    ArrayList<String> result = new ArrayList<String>();
    BufferedReader fileReader = null;
    String buffer = null;
    File originalDirectory = new File(directoryPath);

    if (originalDirectory.isDirectory()) {
      for (File f : originalDirectory.listFiles()) {
        if (!f.isDirectory()) {
          try 
          {          
            fileReader = new BufferedReader(new FileReader(f));
            while((buffer = fileReader.readLine()) != null)
            {
              if (buffer.indexOf(keyword) != -1) {
                result.add(f.getName());
                break;
              }
            }
            fileReader.close();                        
          } 
          catch (IOException e) 
          {
            e.printStackTrace();
            continue;            
          }                              
        }
      }
    } else {
      throw new RemoteException("Not a directory:" + directoryPath);
    }
    return result.toArray(new String[0]);
  }

  //SECOND RMI METHOD
  public int conta_numero_linee(String fileName, String keyword) throws RemoteException {
    int lineCount = 0;
    String currentLine;
    boolean foundKeyword = false;
    File originalFile = new File(fileName);
    if (!originalFile.exists()) {
      System.out.println("# Server: Error: " + fileName + " not found.");
      return -1;
    }
    try {
      BufferedReader originalReader = new BufferedReader(new FileReader(originalFile));

      while ((currentLine = originalReader.readLine()) != null) {
        lineCount++;
        if (currentLine.contains(keyword)) {
          foundKeyword = true;
        }
      }
      originalReader.close();

    } catch (IOException e) {
      System.out.println("# Server: Error during read/write operation.");
      throw new RemoteException("Error during read operation.");
    }
    if (foundKeyword) {
      return lineCount;
    } else {
      return -1;
    }
  }

  //MAIN
	public static void main(String[] args) {

		int registryPort = 1099;
		String registryHost = "localhost";
		String serviceName = "Server";

		// Controllo parametri
		if (args.length != 0 && args.length != 1) {
			System.out.println("Sintassi: ServerImpl [registryPort]");
			System.exit(1);
		}
		if (args.length == 1) {
			try {
				registryPort = Integer.parseInt(args[0]);
			} catch (Exception e) {
				System.out.println("Sintassi: ServerImpl [registryPort], registryPort intero");
				System.exit(2);
			}
		}

		// Registrazione del servizio RMI
		String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
		try {
			RMI_Server serverRMI = new RMI_Server();
			Naming.rebind(completeName, serverRMI);
			System.out.println("Server RMI: Servizio \"" + serviceName + "\" registrato");
		} catch (Exception e) {
			System.err.println("Server RMI \"" + serviceName + "\": " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
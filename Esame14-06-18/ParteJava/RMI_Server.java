
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

  private static final int MAX_FILES = 30;      

	// Costruttore
  public RMI_Server() throws RemoteException {
    super();
  }

  //AUXILIAR FUNCTIONS
  /*
  private ArrayList<String> listMatchingFilesInDirRecursive(File currentDir, String pattern) {
    ArrayList<String> result = new ArrayList<String>();
    for (File f : currentDir.listFiles()) {
      if (f.isDirectory()) {
        //recursive call
        result.addAll(listMatchingFilesInDirRecursive(f, pattern));
      } else {
        if (f.getName().matches(pattern)) {
          result.add(f.getAbsolutePath());
        }
      }
    }    
    return result;
  }*/

  private int listMatchingFilesInDirRecursive(File currentDir, String pattern, String[] vector, int index) {
    for (File f : currentDir.listFiles()) {
      if (f.isDirectory()) {
        // recursive call
        index = listMatchingFilesInDirRecursive(f, pattern, vector, index);
      
      } else {
        if (index >= MAX_FILES) {
          return index;
        } else if (f.getName().matches(pattern)) {
          vector[index] = new String(f.getName());
          index++;
        }
      }
    }
    return index;    
  }      
   

  //FIRST RMI METHOD
  public String[] lista_file(String directoryPath) throws RemoteException {
    String[] vector = new String[MAX_FILES];
    File originalDirectory = new File(directoryPath);
    //'.' is: "any character", 'X*' is: "X, 0 or more times"
    String pattern = ".*[AEIOUaeiou][AEIOUaeiou].*";
    int index = 0;
    if (originalDirectory.isDirectory()) {
      index = listMatchingFilesInDirRecursive(originalDirectory, pattern, vector, index);
      System.out.println("Trovati " + index + "files");
    } else {
      throw new RemoteException("# Parameter directoryPath " + directoryPath + " is not a directory.");
    }
    if(index>0)
      return vector;
    else
      return new String[0];
    /*if (originalDirectory.isDirectory()) {
      result = listMatchingFilesInDirRecursive(originalDirectory, pattern);
    } else {
      throw new RemoteException("# Parameter directoryPath " + directoryPath + " is not a directory.");
    }
    return result.toArray(new String[0]);*/
  }

  //SECOND RMI METHOD
  public int numerazione_righe(String fileName) throws RemoteException {
    int lineCount = 0;

    String currentLine;
    File originalFile = new File(fileName);
    if (!originalFile.exists()) {
      System.out.println("# Server: Error: " + fileName + " not found.");
      return -1;
    }
    File editedFile = new File(fileName + ".edited");
    try {
      BufferedReader originalReader = new BufferedReader(new FileReader(originalFile));
      BufferedWriter editedWriter = new BufferedWriter(new FileWriter(editedFile));

      while ((currentLine = originalReader.readLine()) != null) {
        lineCount++;
        editedWriter.write(lineCount + " " + currentLine);
        editedWriter.newLine();
      }

      originalReader.close();
      editedWriter.close();

      editedFile.renameTo(originalFile);

    } catch (IOException e) {
      System.out.println("# Server: Error during read/write operation.");
      e.printStackTrace();
      return -1;
    }
    return lineCount;
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
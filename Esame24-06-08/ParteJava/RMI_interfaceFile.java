
/** RMI_interfaceFile.java
 * 
 * 
 **/

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote {

  /**
  * @param directoryPath = path of the directory to examine.
  * @throws RemoteException = IOException or no file found.
  * @return list of files containing keyword.
  **/
	public String[] lista_nomi_file_contenenti_parola_in_linea(String directoryPath, String keyword) throws RemoteException;

  /**
   * @param filePath = remote file di cui contare le linee
   * @throws RemoteException = = IOException.
   * @return: # of linees in case the file contains keyword, -1 in case filePath does not exist or keyword is missing
   **/
	public int conta_numero_linee(String filePath, String keyword) throws RemoteException;
}
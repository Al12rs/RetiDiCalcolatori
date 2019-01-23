
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
  * @return list of filesNames with two or more consecutive vocals in the name.
  **/
	public String[] lista_file(String directoryPath) throws RemoteException;

  /**
   * @param fileName = remote file where line numbers will be added.
   * @throws RemoteException = = IOException.
   * @return: # of numbered lines or -1 in case of error
   **/
	public int numerazione_righe(String fileName) throws RemoteException;
}
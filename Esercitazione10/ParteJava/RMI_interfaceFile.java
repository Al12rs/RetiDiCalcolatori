/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote {

	/**
	 * @param nomeCantante = nome del file remoto
	 * @throws RemoteException = IOException
	 * @return Oggetto cantante corrispondendte a nomeCantante
	 **/
	public Singer visualizza_cantante(String nomeCantante) throws RemoteException;

	/**
	 * @throws RemoteException = = IOException
	 * @return: Array of top 3 Singer by votes
	 **/
	public Singer[] visualizza_podio() throws RemoteException;
}
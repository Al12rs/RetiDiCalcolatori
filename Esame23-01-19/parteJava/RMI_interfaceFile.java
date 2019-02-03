/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote {

	/**
	 * @param plateToDelete = targa della prenotazione da eliminare
	 * @throws RemoteException
	 * @return 0 if successful, -1 if not found
	 **/
	public int elimina_prenotazione(String plateToDelete) throws RemoteException;

	/**
	 * @param tipo = tipo di veicolo
	 * @throws RemoteException
	 * @return: array di prenotazioni con veicoli >2011
	 **/
	public Prenotazione[] visualizza_prenotazioni(String tipo) throws RemoteException;
}
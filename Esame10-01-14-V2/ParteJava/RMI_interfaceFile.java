/**
 * Interfaccia remota di servizio
 */

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface RMI_interfaceFile extends Remote {

	/**
	 * @param insertEvent = evento da inserire
	 * @throws RemoteException 
	 * @return 0 if successful, -1 if not enough space
	 **/
	public int inserimento_evento(Evento insertEvent) throws RemoteException;

	/**
	 * @throws RemoteException
	 * @return: 0 if successful, -1 if no event matches parameters
	 **/
	public int acquista_biglietti(String description, int ticketsToBuy) throws RemoteException;
}
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	public static final long serialVersionUID = 1;

	private Evento[] listaEventi;
	private final int MAX_EVENTI = 4;

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
		listaEventi = new Evento[MAX_EVENTI];
		for (int i = 0; i < MAX_EVENTI; i++) {
			listaEventi[i] = new Evento();
		}
		int i = 0;
		listaEventi[i].descrizione = "String";
		listaEventi[i].tipo = "Concerto";
		listaEventi[i].data = "11/01/2014";
		listaEventi[i].luogo = "Verona";
		listaEventi[i].disponibilita = 40;
		listaEventi[i].prezzo = 40;
		i++;
		listaEventi[i].descrizione = "Junentus-Inger";
		listaEventi[i].tipo = "Calcio";
		listaEventi[i].data = "03/05/2014";
		listaEventi[i].luogo = "Torino";
		listaEventi[i].disponibilita = 21;
		listaEventi[i].prezzo = 150;
		i++;
		listaEventi[i].descrizione = "GP Bologna";
		listaEventi[i].tipo = "Formula1";
		listaEventi[i].data = "07/09/2014";
		listaEventi[i].luogo = "Bologna";
		listaEventi[i].disponibilita = 10;
		listaEventi[i].prezzo = 200;
	}

	public int inserimento_evento(Evento insertEvent) throws RemoteException {
		int result;
		result = -1;

		for (int i = 0; i < MAX_EVENTI; i++) {
			if (listaEventi[i].descrizione.equals("L")) {
				listaEventi[i] = insertEvent;
				result = 0;
				System.out.println("Evento inserito in posizione: " + i);
				break;
			}
		}

		return result;
	}

	public int acquista_biglietti(String description, int ticketsToBuy) throws RemoteException 
	{
		int result;
		result = -1;
		for (int i = 0; i < MAX_EVENTI; i++) {
			if (description.equals(listaEventi[i].descrizione) && listaEventi[i].disponibilita >= ticketsToBuy) {
				listaEventi[i].disponibilita -= ticketsToBuy;
				result = 0;
				break;								
			}
		}						
		return result;								
	}	

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
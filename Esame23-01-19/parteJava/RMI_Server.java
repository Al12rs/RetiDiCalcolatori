import java.io.File;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	public static final long serialVersionUID = 1;

	private Prenotazione[] table;
	private final int MAX_ELEMENTS = 10;

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
		table = new Prenotazione[MAX_ELEMENTS];
		for (int i = 0; i < MAX_ELEMENTS; i++) {
			table[i] = new Prenotazione();
		}
		int i = 0;
		table[i].targa = "AN745NL";
		table[i].patente = "00003";
		table[i].tipo = "auto";
		table[i].folder_img = "AN745NL_img/";
		i++;
		table[i].targa = "FE457GF";
		table[i].patente = "50006";
		table[i].tipo = "camper";
		table[i].folder_img = "FE457GF_img/";
		i++;
		table[i].targa = "NU547PL";
		table[i].patente = "40063";
		table[i].tipo = "auto";
		table[i].folder_img = "NU547PL_img/";
		i++;
		table[i].targa = "LR897AH";
		table[i].patente = "56832";
		table[i].tipo = "camper";
		table[i].folder_img = "LR897AH_img/";
		i++;
		table[i].targa = "MD506DW";
		table[i].patente = "00100";
		table[i].tipo = "camper";
		table[i].folder_img = "MD506DW_img/";
		i++;
	}
	
	public int elimina_prenotazione(String plateToDelete) throws RemoteException {
		int result;
		result = -1;

		for (int i = 0; i < MAX_ELEMENTS; i++) {
			if (table[i].targa.equals(plateToDelete)) {
				File imgFolder = new File(table[i].folder_img);
				if (imgFolder.exists()) {
					for (File file : imgFolder.listFiles()) {
					file.delete();
					}
					if (!imgFolder.delete()) {
						break;
					}
				}
				table[i] = new Prenotazione();
				result = 0;
				System.out.println("Evento eliminato in posizione: " + i);
				break;
			}
		}

		return result;
	}

	public Prenotazione[] visualizza_prenotazioni(String tipo) throws RemoteException 
	{
		Prenotazione[] result = new Prenotazione[MAX_ELEMENTS];
		int resultIndex = 0;
		for (int i = 0; i < MAX_ELEMENTS; i++) {
			if (tipo.equals(table[i].tipo) && table[i].targa.substring(0, 2).compareTo("ED") > 0) {
				result[resultIndex] = table[i];
				resultIndex++;
			}
		}
		
		for (int i = resultIndex; i < MAX_ELEMENTS; i++) {
			result[i] = new Prenotazione();
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
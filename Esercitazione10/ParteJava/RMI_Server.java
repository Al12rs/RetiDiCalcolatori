import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RMI_Server extends UnicastRemoteObject implements RMI_interfaceFile {

	public static final long serialVersionUID = 1;

	private Singer[] listaCantanti;
	private final int MAX_SINGERS = 16;

	// Costruttore
	public RMI_Server() throws RemoteException {
		super();
		listaCantanti = new Singer[MAX_SINGERS];
		for (int i = 0; i < MAX_SINGERS; i++) {
			listaCantanti[i].name = "L";
			listaCantanti[i].category = "L";
			listaCantanti[i].voto = -1;
			listaCantanti[i].fileName = "L";
		}
		int i = 0;
		// 1a riga
		listaCantanti[i].name = "Mandarino";
		listaCantanti[i].category = "Campioni";
		listaCantanti[i].voto = 1500;
		listaCantanti[i].fileName = "vicino.avi";
		// 2a riga
		i++;
		listaCantanti[i].name = "Amara Bianchi";
		listaCantanti[i].category = "Campioni";
		listaCantanti[i].voto = 2000;
		listaCantanti[i].fileName = "immobilismo.avi";
		// 3a riga
		i++;
		listaCantanti[i].name = "Zucchero";
		listaCantanti[i].category = "NuoveProposte";
		listaCantanti[i].voto = 550;
		listaCantanti[i].fileName = "ascolto.avi";
		// 4a riga
		i++;
		listaCantanti[i].name = "Amari";
		listaCantanti[i].category = "NuoveProposte";
		listaCantanti[i].voto = 800;
		listaCantanti[i].fileName = "cosaE.avi";
	}

	public Singer visualizza_cantante(String nomeCantante) throws RemoteException {
		for (int i = 0; i < listaCantanti.length; i++) {
			if (nomeCantante.equals(listaCantanti[i].name) || listaCantanti[i].name.equals("L")) {
				return listaCantanti[i];
			}
		}
		return new Singer();
	}

	public Singer[] visualizza_podio() throws RemoteException {
		Singer[] result = new Singer[3];
		int firstIndex = -1;
		int secondIndex = -1;
		int thirdIndex = -1;
		int firstValue = -1;
		int secondValue = -1;
		int thirdValue = -1;
		for (int i = 0; i < listaCantanti.length; i++) {
			if (!listaCantanti[i].name.equals("L")) {
				if (firstValue < listaCantanti[i].voto) {
					thirdIndex = secondIndex;
					thirdValue = secondValue;
					secondIndex = firstIndex;
					secondValue = firstValue;
					firstIndex = i;
					firstValue = listaCantanti[i].voto;
				} else if (secondValue < listaCantanti[i].voto) {
					thirdIndex = secondIndex;
					thirdValue = secondValue;
					secondIndex = i;
					secondValue = listaCantanti[i].voto;
				} else if (thirdValue < listaCantanti[i].voto) {
					thirdIndex = i;
					thirdValue = listaCantanti[i].voto;
				}
			} else {
				break;
			}
		}
		if (firstIndex > 0)
			result[1] = listaCantanti[firstIndex];
		else
			result[1] = new Singer();
		if (secondIndex > 0)
			result[2] = listaCantanti[secondIndex];
		else
			result[2] = new Singer();
		if (thirdIndex > 0)
			result[2] = listaCantanti[thirdIndex];
		else
			result[2] = new Singer();
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
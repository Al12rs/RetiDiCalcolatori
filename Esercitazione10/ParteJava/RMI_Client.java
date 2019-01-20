
/**
 * Client.java
 * */

import java.rmi.*;
import java.io.*;

public class RMI_Client {

	public static void main(String[] args) {
		int registryPort = 1099;
		String registryHost = null;
		String serviceName = "Server";
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

		// Controllo parametri
		if (args.length != 1 && args.length != 2) {
			System.out.println("Sintassi: ClientFile RegistryHost [registryPort]");
			System.exit(1);
		} else {
			registryHost = args[0];
			if (args.length == 2) {
				try {
					registryPort = Integer.parseInt(args[1]);
				} catch (Exception e) {
					System.out.println("Sintassi: ClientFile NomeHost [registryPort], registryPort intero");
					System.exit(1);
				}
			}
		}

		// Connessione al servizio RMI remoto
		try {
			String completeName = "//" + registryHost + ":" + registryPort + "/" + serviceName;
			RMI_interfaceFile serverRMI = (RMI_interfaceFile) Naming.lookup(completeName);
			System.out.println("Client RMI: Servizio \"" + serviceName + "\" connesso");

			System.out.println("\nRichieste di servizio fino a fine file");

			String service;
			System.out.print("# Servizio (VC=visualizza_cantante, VP=visualizza_podio): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("VC")) {

					String nomeCantante;
					System.out.print("# Nome cantante? ");
					nomeCantante = stdIn.readLine();

					// Invocazione remota
					try {
						Singer cantante = serverRMI.visualizza_cantante(nomeCantante);
						System.out.println("# Il cantante " + nomeCantante + " , della categoria " + cantante.category
								+ " , ha ottenuto " + cantante.voto + " voti, con il suo singolo " + cantante.fileName);
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // C
				else if (service.equals("VP")) {
					Singer[] res = new Singer[3];
					try {
						res = serverRMI.visualizza_podio();
						System.out.println("# Prima   posizione: " + res[0].name + " " + res[0].voto);
						System.out.println("# Seconda posizione: " + res[1].name + " " + res[1].voto);
						System.out.println("# Terza   posizione: " + res[2].name + " " + res[2].voto);
						
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // S
				else
					System.out.println("Servizio non disponibile");

				System.out.print("# Servizio (VC=visualizza_cantante, VP=visualizza_podio):  ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
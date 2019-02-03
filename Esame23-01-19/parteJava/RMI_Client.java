
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

			//CORPO DEL CLIENT
			System.out.println("\nRichieste di servizio fino a fine file");
			String service;
			System.out.print("# Servizio (EP=Elimina Prenotazione, VP=Visualizza Prenotazioni): ");

			while ((service = stdIn.readLine()) != null) {
				
				//First method
				if (service.equals("EP")) {					
					System.out.print("# Targa: ");
					String targa = stdIn.readLine();

					// Invocazione remota
					int res = -1;
					try {
						res = serverRMI.elimina_prenotazione(targa);
						if (res == 0)
							System.out.println("Operazione eseguita correttamente.");
						else
							System.out.println("Errore lato server");
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // First Method
				//Second Method
				else if (service.equals("VP")) {
					String tipo = null;
					System.out.print("Inserire tipo prenotazione: ");
					tipo = stdIn.readLine();
																														
					Prenotazione[] res =  null;
					//Invocazione Remota
					try {
						res = serverRMI.visualizza_prenotazioni(tipo);
						for (int i = 0; i < res.length; i++) {
							if (res[i].targa.equals("L")) {
								System.out.println("No more results.");
								break;
							} else {
								System.out.println(res[i].toString());								
							}
						}																	
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // Second Method
				else
					System.out.println("Servizio non disponibile");

				System.out.print("# Servizio (EP=Elimina Prenotazione, VP=Visualizza Prenotazioni): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
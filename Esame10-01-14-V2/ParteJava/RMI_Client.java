
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
			System.out.print("# Servizio (IE=Inserisci Evento, AB=Acquista Biglietti): ");

			while ((service = stdIn.readLine()) != null) {

				if (service.equals("IE")) {

					System.out.print("# Inserici dettaglie evento:");
					Evento newEvent = new Evento();
					System.out.print("# Descrizione: ");
					newEvent.descrizione = stdIn.readLine();
					System.out.print("# Tipo: ");
					newEvent.tipo = stdIn.readLine();
					System.out.print("# Data: ");
					newEvent.data = stdIn.readLine();
					System.out.print("# Luogo: ");
					newEvent.luogo = stdIn.readLine();
					System.out.print("# disponibilita (int): ");
					while (true) {
						try{
							newEvent.disponibilita = Integer.parseInt(stdIn.readLine());
							break;														
						} catch (NumberFormatException e) {
							System.out.print("Inserire un numero per la disponibilità: ");
							continue;
						}
					}
					System.out.print("# prezzo (int): ");
					while (true) {
						try{
							newEvent.prezzo = Integer.parseInt(stdIn.readLine());
							break;														
						} catch (NumberFormatException e) {
							System.out.print("Inserire un numero per prezzo: ");
							continue;
						}
					}


					// Invocazione remota
					int res = -1;
					try {
						res = serverRMI.inserimento_evento(newEvent);
						if (res == 0)
							System.out.println("Operazione eseguita correttamente.");
						else
							System.out.println("Errore lato server");
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // C
				else if (service.equals("AB")) {
					String descrizione = null;
					String bigliettiDaAcquistare = null;
					int ticketsToBuy;
					System.out.print("Inserire descrizione evento: ");
					descrizione = stdIn.readLine();
					System.out.print("Inserire numero biglietti da acquistare: ");
					while (true) {
						try{
							bigliettiDaAcquistare = stdIn.readLine();
							ticketsToBuy = Integer.parseInt(bigliettiDaAcquistare);
							break;														
						} catch (NumberFormatException e) {
							System.out.print("Inserire un numero valido di biglietti da acquistare: ");
							continue;
						}
					}
																														
					int res = -1;
					//Invocazione Remota
					try {
						res = serverRMI.acquista_biglietti(descrizione, ticketsToBuy);
						if(res == 0)
							System.out.println("Operazione eseguita correttamente.");
						else
							System.out.println("Errore lato server");																		
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // AB
				else
					System.out.println("Servizio non disponibile");

				System.out.print("# Servizio (IE=Inserisci Evento, AB=Acquista Biglietti): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}

/**
 * RMI_Client.java
 * 
 * 
 **/

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

      //CLIENT BODY
			String service;
			System.out.print("# Servizio (LF=lista_file, NR=numero_righe): ");

      while ((service = stdIn.readLine()) != null) {

				if (service.equals("LF")) {
          //RMI1
					String direttorio = null;
					String keyword = null;
					System.out.print("# Direttorio? ");
					direttorio = stdIn.readLine();
					System.out.print("# Parola? ");
					keyword = stdIn.readLine();					

					// Invocazione remota
					try {
						String[] lista_file = serverRMI.lista_nomi_file_contenenti_parola_in_linea(direttorio, keyword);
            int i=0;
            System.out.println("File trovati:");
            for (String s : lista_file) {
              i++;
              System.out.println("File "+i+": "+s);												
            }
            System.out.println("Trovati:" + i + " file");            
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} //RMI1
        else if (service.equals("NR")) {
					//RMI2
					String nomeFile;
					System.out.print("# Nome file? ");
					nomeFile = stdIn.readLine();
					String keyword;
					System.out.print("# Parola chiave? ");
					keyword = stdIn.readLine();
					try {
						int res = serverRMI.conta_numero_linee(nomeFile, keyword);
						if(res >= 0)
							System.out.println("Sono state contate "+ res +" righe");
						else
							System.out.println("Si è verificato un errore lato server");
						
					} catch (RemoteException re) {
						System.out.println("Errore remoto: " + re.toString());
					}

				} // RMI2
				else
					System.out.println("Servizio non disponibile");

				System.out.print("# Servizio (LF=lista_file, NR=numerazione_righe): ");
			} // !EOF

		} catch (Exception e) {
			System.err.println("ClientRMI: " + e.getMessage());
			e.printStackTrace();
			System.exit(1);
		}
	}
}
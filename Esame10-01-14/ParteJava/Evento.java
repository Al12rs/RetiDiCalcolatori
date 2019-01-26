import java.io.Serializable;    

public class Evento  implements Serializable{

  String descrizione;
  String tipo;
  String data;
  String luogo;
  int disponibilita;
  int prezzo;

  public Evento(){
    descrizione = "L";
    tipo = "L";
    data = "L";
    luogo = "L";
    disponibilita = -1;
    prezzo = -1;
  }
}
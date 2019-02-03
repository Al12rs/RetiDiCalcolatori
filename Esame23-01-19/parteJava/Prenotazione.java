import java.io.Serializable;

public class Prenotazione implements Serializable {

  public static final long serialVersionUID = 1;

  String targa;
  String patente;
  String tipo;
  String folder_img;

  public Prenotazione() {
    targa = "L";
    tipo = "L";
    patente = "0";
    folder_img = "L";
  }

  @Override
  public String toString() {
    return "Prenotazione:\n" +
      "\tTarga: " + targa + "\n" +
      "\tPatente: " + patente + "\n" +
      "\tTipo: " + tipo + "\n" +
      "\tFolder_img: " + folder_img + "\n";
  }

}
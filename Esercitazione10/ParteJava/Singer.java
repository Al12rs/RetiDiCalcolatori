
import java.io.Serializable;

public class Singer implements Serializable{
  public String name;
  public String category;
  public int voto;
  public String fileName;
  
  public static final long serialVersionUID = 1;
   
  public Singer() {
    this.name = "L";
    this.category = "L";
    this.voto = -1;
    this.fileName = "L";
  }

  public Singer(String name, String category, int voto, String fileName) {
    this.name = name;
    this.category = category;
    this.voto = voto;
    this.fileName = fileName;
  }

  
}
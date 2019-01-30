import java.io.Serializable;

public class Stanza implements Serializable {

  public static final long serialVersionUID = 1;

  private final int K = 10;

  String nome;
  String stato;
  String[] users;

  public Stanza() {
    nome = new String("L");
    stato = new String("L");
    users = new String[K];
    for (int i = 0; i < K; i++) {
      users[i] = new String("L");
    }
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append("Stanza:\n" + "\tNome: " + nome + "\n" + "\tstato: " + stato + "\n");
    for (int j = 0; j < K; j++) {
      if(!users[j].equals("L"))
        sb.append("\tUtente" + (j + 1) + ": " + users[j] + "\n");
    }
    return sb.toString();
  }

}

// FileUtility.java

import java.io.*;

public class FileUtility {

  /**
   * Nota: sorgente e destinazione devono essere correttamente aperti e chiusi da
   * chi invoca questa funzione.
   *
   */
  static protected void trasferisci_a_byte_file_binario(DataInputStream src, DataOutputStream dest) throws IOException {

    // ciclo di lettura da sorgente e scrittura su destinazione
    int buffer = 0;
    try {
      // esco dal ciclo all lettura di un valore negativo -> EOF
      while ((buffer = src.read()) >= 0) {
        dest.write(buffer);
      }
      dest.flush();
    } catch (EOFException e) {
      System.out.println("Problemi, i seguenti: ");
      e.printStackTrace();
    }
  } // trasferisci_a_byte_file_binario

  /**
   * Nota: sorgente e destinazione devono essere correttamente aperti e chiusi da
   * chi invoca questa funzione.
   *
   */
  static protected void trasferisci_N_byte_file_binario(DataInputStream src, DataOutputStream dest, long daTrasferire)
      throws IOException {

    int cont = 0;
    // ciclo di lettura da sorgente e scrittura su destinazione
    int buffer = 0;
    try {
      // esco dal ciclo quando ho letto il numero di byte da trasferire
      while (cont < daTrasferire) {
        buffer = src.read();
        dest.write(buffer);
        cont++;
      }
      dest.flush();
      System.out.println("Byte trasferiti: " + cont);
    }
    // l'eccezione dovrebbe scattare solo se chi chiama la funzione ha
    // impostato un numero sbagliato di byte da leggere
    catch (EOFException e) {
      System.out.println("Problemi, i seguenti: ");
      e.printStackTrace();
    }
  } // trasferisci_N_byte_file_binario

  static protected int removeWordFromFile(String fileName, String wordToBeRemoved, char wordSeparators[])
      throws IOException {
    int removedCount = 0;
    File originalFile = new File(fileName);
    File editedFile = new File(fileName + ".edited");

    BufferedReader targetFileReader = new BufferedReader(new FileReader(fileName));
    BufferedWriter editedFileWriter = new BufferedWriter(new FileWriter(fileName + ".edited"));
    int currentCharacter;
    StringBuilder charBuffer = new StringBuilder();

    while ((currentCharacter = targetFileReader.read()) != -1) {
      //System.out.println("Dentro al ciclo, currentChar is " + (char)currentCharacter );
      boolean isSeparator = false;
      for (int i = 0; i < wordSeparators.length; i++) {
        if ((char) currentCharacter == wordSeparators[i]) {
          isSeparator = true;
          break;
        }
      }
      if (isSeparator) {
        //System.out.println("Compare " + charBuffer.toString() + " " + wordToBeRemoved);
        
        if (!charBuffer.toString().equals(wordToBeRemoved)) {
          charBuffer.append((char)currentCharacter);
          editedFileWriter.write(charBuffer.toString());
        } else {
          removedCount++;
        }
        charBuffer = new StringBuilder();
      } else {
        charBuffer.append((char)currentCharacter);
      }
    } //finita lettura target
    targetFileReader.close();
    editedFileWriter.close();

    //sovrascrivo originale con edited
    editedFile.renameTo(originalFile);

    return removedCount;
  }
}
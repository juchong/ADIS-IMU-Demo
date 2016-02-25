/**
 * Serial Read & Write to LogFile
 *
 * Read data from Arduino through serial port and appends the data to a text file provided.
 * Code by: Naveen Karuthedath, Barry Chris & Aradhan
 * Modified for use with the ADIS-Demo by Juan Chong
*/
 
import processing.serial.*;
import java.util.List;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import static javax.swing.JOptionPane.*;

Serial sPort;  
String sData;
byte serialBuffer[] = new byte[4];

final boolean debug = true;
 
String file1 = "C:/Users/jchong/Desktop/log.txt";
int syncWord = 0xFF;
 
void setup()
{
  size(800,600); // Window size
  background(#090A40);
  serialSetup();   
}

// Read and sort serial data, the assign to global variables rollDegrees, pitchScaled, and yawScaled
void serialEvent(Serial sPort) {
  serialBuffer = sPort.readBytesUntil(syncWord); // Loads buffer until syncWord is detected (syncWord is last byte)
  if (serialBuffer != null && serialBuffer.length == 4) { // Don't assign null data
    for(int i=0; i<3; i++) {
      logData(file1,(str(serialBuffer[i])) + ",", true);
    }
    logData(file1,"\n", true);
  }
}

// Main loop
void draw()
{
  serialEvent(sPort);
}

// This function creates a file object and handles the file I/O needed to write to it
void logData( String fileName, String newData, boolean appendData)
{
  BufferedWriter bw=null;
  try { //try to open the file
    FileWriter fw = new FileWriter(fileName, appendData);
    bw = new BufferedWriter(fw);
    bw.write(newData);// + System.getProperty("line.separator"));
  } 
  catch (IOException e) {
  } 
  finally {
    if (bw != null) { //if file was opened try to close
        try {
        bw.close();
        }
      catch (IOException e) {
      }
    }
  }
}

// This function draws a dialog box which allows the user to select a serial port to read from
void serialSetup() {
  String COMx, COMlist = "";
  if(debug) printArray(Serial.list());
    int i = Serial.list().length;
  if (i != 0) {
    if (i >= 2) {
      // need to check which port the inst uses -
      // for now we'll just let the user decide
      for (int j = 0; j < i;) {
        COMlist += char(j+'a') + " = " + Serial.list()[j];
        if (++j < i) COMlist += ",  ";
      }
      COMx = showInputDialog("Which port is the demo connected to? (a,b,..):\n"+"Check the Device Manager if unsure!\n"+COMlist);
      if ((COMx == null) || (COMx.isEmpty())) {
        showMessageDialog(frame,"Port is not available!\nIt may be in use by another program or nothing was selected.");
        exit();
      }
      i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
    try{
      String portName = Serial.list()[i-1];
      if(debug) println(portName);
      sPort = new Serial(this, portName, 9600);
      sPort.bufferUntil(syncWord); // Loads buffer stopping at the syncWord from the Arduino
    }
      catch (Exception e) {
        showMessageDialog(frame,"Device is not connected to the PC or does not exist.");
        if (debug) println("Error: ", e);
        exit();
      }
    }
  }
}
 
String getDateTime()
{
      SimpleDateFormat df = new SimpleDateFormat("[ dd/MM/yyyy HH:mm:ss ] ");
      Calendar date = Calendar.getInstance(); // the current date and time
      return(df.format(date.getTime()));
}
 
//Arduino Sample Code:
/*void setup() {
  Serial.begin(9600);
}
 
void loop() {
  Serial.write(VAR1);
  Serial.write(VAR2);
  Serial.write(VAR3);
  Serial.write(0xFF);
  delay(200);
}
*/


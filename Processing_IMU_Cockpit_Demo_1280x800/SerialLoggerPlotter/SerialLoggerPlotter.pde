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
import org.gwoptics.graphics.graph2D.Graph2D;
import org.gwoptics.graphics.graph2D.traces.ILine2DEquation;
import org.gwoptics.graphics.graph2D.traces.RollingLine2DTrace;

class eq implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return sData[0];
  }    
}

class eq2 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return sData[1];
  }    
}

class eq3 implements ILine2DEquation{
  public double computePoint(double x,int pos) {
    return sData[2];
  }    
}

RollingLine2DTrace r,r2,r3;
Graph2D g;

Serial sPort;  
int sData[] = new int[4];
byte serialBuffer[] = new byte[4];

final boolean debug = true;
 
String file1 = "C:/Users/jchong/Desktop/log.txt";
int syncWord = 0xFF;
 
void setup()
{
  size(600,300); // Window size
  serialSetup();

  // Configure plot properties
  r  = new RollingLine2DTrace(new eq() ,10,0.1f);
  r.setTraceColour(0, 255, 0);
  
  r2 = new RollingLine2DTrace(new eq2(),10,0.1f);
  r2.setTraceColour(255, 0, 0);
  
  r3 = new RollingLine2DTrace(new eq3(),10,0.1f);
  r3.setTraceColour(0, 0, 255);
   
  g = new Graph2D(this, 400, 200, false);
  g.setYAxisMax(600);
  g.addTrace(r);
  g.addTrace(r2);
  g.addTrace(r3);
  g.position.y = 50;
  g.position.x = 100;
  g.setYAxisTickSpacing(100);
  g.setXAxisMax(5f);
    
}

// Read and sort serial data, the assign to global variables rollDegrees, pitchScaled, and yawScaled
void serialEvent(Serial sPort) {
  serialBuffer = sPort.readBytesUntil(syncWord); // Loads buffer until syncWord is detected (syncWord is last byte)
  if (serialBuffer != null && serialBuffer.length == 4) { // Don't assign null data
    for(int i=0; i<3; i++) {
      logData(file1,(str(serialBuffer[i])) + ",", true);
      sData[i] = int(serialBuffer[i]);
    }
    logData(file1,"\n", true);
    background(#FFFFFF);
    g.draw();
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



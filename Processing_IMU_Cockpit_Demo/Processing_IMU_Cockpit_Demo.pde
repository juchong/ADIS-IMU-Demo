// Libraries to import
import processing.serial.*; // Import the serial library for Processing
import static javax.swing.JOptionPane.*;

// Variable declarations
int width = 1600; // Width of GUI window on monitor in pixels
int height = 900; // Height of GUI window on monitor in pixels
byte serialBuffer[] = new byte[4];
int roll, pitch, yaw = 0; // Raw roll data from serial port
int rollDegrees = 0; // Roll data in degrees
int pitchScaled = 0; // Raw roll data from serial port
int pitchDegrees = 0; // Pitch data in degrees
int yawScaled = 0; // To store data from serial port, used to color background
int yawDegrees = 0; // Yaw data in degrees
float ArtificialHoizonMagnificationFactor=0.25; // scales artificial horizon
float CompassMagnificationFactor=0.85; // scales compass
float SpanAngle=120; // to draw circular scales
int NumberOfScaleMajorDivisions; // to draw circular scales 
int NumberOfScaleMinorDivisions; // to draw circular scales
int syncWord = 0xFF; // This must be the same sync word as the Arduino and never appear in the data (roll, pitch, yaw)
int frameCount = 0;
int samples = 2;
final boolean debug = true;

// Object instantiations
Serial port; // The serial port object
PImage cockpit; // Cockpit object in foreground
PImage background; // Scenery object in background
PImage map; // Map object on dash

LowPass lowPassRoll, lowPassPitch, lowPassYaw;

// Initial setup
void setup() {
  lowPassRoll = new LowPass(samples);
  lowPassPitch = new LowPass(samples);
  lowPassYaw = new LowPass(samples);
  size(width, height); // Size of GUI window on monitor in pixels
  smooth(); // Draws all geometry with smooth (anti-aliased) edges
  frameRate(30); // Frame rate to render
  // Using the first available port (might be different on your computer)
  //port = new Serial(this, Serial.list()[2], 9600); // Make sure this part agrees with the port listed in the PC and settings in Arduino Code
  background = loadImage("boundless-horizon-2.jpg"); // Load background image
  cockpit = loadImage("Cockpit.png"); // Load cockpit image
  map = loadImage("Map.png"); // Load map image
  noLoop();
  rectMode(CENTER); // for instrumentation
  serialSetup();
}

// Draw loop
void draw() {
  // Moves the background scenery with IMU motion
  pushMatrix(); // Built-in function that saves the current position of the coordinate system
    translate(width/2, height/2); // Centers the background scenery
    rotate(radians(rollDegrees)); // Rolls the coordinate system
    translate(yawScaled, pitchScaled); // Pans coordinate system up/down (pitch) and left/right (yaw)
    image(background, -width, -height*2, width*2, height*4); // Draws background scenery in shifted coordinate system
  popMatrix(); // Restores the coordinate system to the way it was before the translate

  // Dash
  pushMatrix(); // Built-in function that saves the current position of the coordinate system
    translate(0, height/3.5);
    scale((float)width/(float)cockpit.width);
    //Map
    pushMatrix();
      translate(425, 11);
      //tint(255, 127); // helps to center the map
      pushMatrix(); // Built-in function that saves the current position of the coordinate system
        translate(width/2, height/2);
        pushMatrix(); // Built-in function that saves the current position of the coordinate system
          rotate(radians(yawDegrees));
          image(map, -map.width/2, -map.height/2);
        popMatrix(); // Restores the coordinate system to the way it was before the translate
      popMatrix(); // Restores the coordinate system to the way it was before the translate
    popMatrix(); // Restores the coordinate system to the way it was before the translate
    // Artificial Horizon
    pushMatrix();
      translate(width/5.7, height/2.0); 
      Horizon();
      pushMatrix();
        rotate(radians(rollDegrees));
        pitchScale();
        Axis();
      popMatrix();
      //rotate(-radians(rollDegrees));
      pushMatrix();
        Borders();
        Plane();
        showAngles();
        Compass();
        showYaw();
      popMatrix();
    popMatrix();
    image(cockpit, 0, 0); // Draws cockpit on top of background
  popMatrix(); // Restores the coordinate system to the way it was before the translate
    
}

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
      port = new Serial(this, portName, 9600);
      port.bufferUntil(syncWord); // Loads buffer stopping at the syncWord from the Arduino
    }
      catch (Exception e) {
        showMessageDialog(frame,"Device is not connected to the PC or does not exist.");
        if (debug) println("Error: ", e);
        exit();
      }
    }
  }
}

// Read and sort serial data, the assign to global variables rollDegrees, pitchScaled, and yawScaled
void serialEvent(Serial port) {
  serialBuffer = port.readBytesUntil(syncWord); // Loads buffer until syncWord is detected (syncWord is last byte)
  if (serialBuffer != null && serialBuffer.length == 4) { // Don't assign null data
    roll = serialBuffer[0]; // Raw roll data from serial port
    pitch = serialBuffer[1]; // Raw pitch data from serial port
    yaw = serialBuffer[2]; // Raw yaw data from serial port
  }
  
  lowPassRoll.input((float)roll);
  roll = (int)lowPassRoll.output;
  
  lowPassPitch.input((float)pitch);
  pitch = (int)lowPassPitch.output;
  
  lowPassYaw.input((float)yaw);
  yaw = (int)lowPassYaw.output;
  
  rollDegrees = (int)( ( (float)roll )/255*360 + 180 ); // Scale roll data in degrees
  pitchScaled = -(int)( ( (float)pitch )/255*height*4 ); // Scale pitch data w.r.t. image size
  pitchDegrees = -(int)( ( (float)pitch )/255*360 ); // Scale pitch data in degrees
  yawScaled = (int)( ( (float)yaw )/255*width ); // Scale yaw data w.r.t. image size
  yawDegrees = -(int)( ( ( (float)yaw )/255*360-90+180 ) ); // Scale yaw data in degrees
  redraw();
  // For debugging
  //println(Serial.list());
  //println( "Raw Input: " + roll + " " + pitch + " " +yaw); // Uncomment for debugging
  //println( "Rotation Degrees: " + rollDegrees + " " + pitchScaled + " " +yawScaled); // Uncomment for debugging
  //println( "Frame Count: " + frameCount++); // Uncomment for debugging
}

void Horizon() {
  scale(ArtificialHoizonMagnificationFactor);
  noStroke();
  fill(0, 180, 255);
  rect(0, -100, 900, 1000);
  fill(95, 55, 40);
  rotate(radians(rollDegrees));
  rect(0, 400+(float)pitchDegrees/90*400, 900, 800);
  rotate(-radians(rollDegrees));
  rotate(-PI-PI/6);
  SpanAngle=120;
  NumberOfScaleMajorDivisions=12;
  NumberOfScaleMinorDivisions=24; 
  CircularScale();
  rotate(PI+PI/6);
  rotate(-PI/6); 
  CircularScale();
  rotate(PI/6);
}

void showYaw() {
  fill(50);
  noStroke();
  rect(20, 470, 440, 50);
  int yaw1=round(yawDegrees);
  textAlign(CORNER);
  textSize(35);
  fill(255);
  text("yaw:  "+yaw1+" Deg", 80, 477, 500, 60);
}

void Compass() {
  translate(2*width/3, 0);
  scale(CompassMagnificationFactor);
  noFill();
  stroke(100);
  strokeWeight(80);
  ellipse(0, 0, 750, 750);
  strokeWeight(50);
  stroke(50);
  fill(0, 0, 40);
  ellipse(0, 0, 610, 610);
  for (int k=255;k>0;k=k-5) {
    noStroke();
    fill(0, 0, 255-k);
    ellipse(0, 0, 2*k, 2*k);
  }
  strokeWeight(20);
  NumberOfScaleMajorDivisions=18;
  NumberOfScaleMinorDivisions=36; 
  SpanAngle=180;
  CircularScale();
  rotate(PI);
  SpanAngle=180;
  CircularScale();
  rotate(-PI);
  fill(255);
  textSize(60);
  textAlign(CENTER);
  text("W", -375, 0, 100, 80);
  text("E", 370, 0, 100, 80);
  text("N", 0, -365, 100, 80);
  text("S", 0, 375, 100, 80);
  textSize(30);
  text("COMPASS-01", 0, -130, 500, 80);
  rotate(PI/4);
  textSize(40);
  text("NW", -370, 0, 100, 50);
  text("SE", 365, 0, 100, 50);
  text("NE", 0, -355, 100, 50);
  text("SW", 0, 365, 100, 50);
  rotate(-PI/4);
  CompassPointer();
}

void CompassPointer() {
  rotate(PI+radians(yawDegrees)); 
  stroke(0);
  strokeWeight(4);
  fill(100, 255, 100);
  triangle(-20, -210, 20, -210, 0, 270);
  triangle(-15, 210, 15, 210, 0, 270);
  ellipse(0, 0, 45, 45);  
  fill(0, 0, 50);
  noStroke();
  ellipse(0, 0, 10, 10);
  triangle(-20, -213, 20, -213, 0, -190);
  triangle(-15, -215, 15, -215, 0, -200);
  rotate(-PI-radians(yawDegrees));
}

void Plane() {
  fill(0);
  strokeWeight(1);
  stroke(0, 255, 0);
  triangle(-20, 0, 20, 0, 0, 25);
  rect(110, 0, 140, 20);
  rect(-110, 0, 140, 20);
}

void CircularScale() {
  float GaugeWidth=800; 
  textSize(GaugeWidth/30);
  float StrokeWidth=1;
  float an;
  float DivxPhasorCloser;
  float DivxPhasorDistal;
  float DivyPhasorCloser;
  float DivyPhasorDistal;
  strokeWeight(2*StrokeWidth);
  stroke(255);
  float DivCloserPhasorLenght=GaugeWidth/2-GaugeWidth/9-StrokeWidth;
  float DivDistalPhasorLenght=GaugeWidth/2-GaugeWidth/7.5-StrokeWidth;
  for (int Division=0;Division<NumberOfScaleMinorDivisions+1;Division++) {
    an=SpanAngle/2+Division*SpanAngle/NumberOfScaleMinorDivisions; 
    DivxPhasorCloser=DivCloserPhasorLenght*cos(radians(an));
    DivxPhasorDistal=DivDistalPhasorLenght*cos(radians(an));
    DivyPhasorCloser=DivCloserPhasorLenght*sin(radians(an));
    DivyPhasorDistal=DivDistalPhasorLenght*sin(radians(an));  
    line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal);
  }
  DivCloserPhasorLenght=GaugeWidth/2-GaugeWidth/10-StrokeWidth;
  DivDistalPhasorLenght=GaugeWidth/2-GaugeWidth/7.4-StrokeWidth;
  for (int Division=0;Division<NumberOfScaleMajorDivisions+1;Division++) {
    an=SpanAngle/2+Division*SpanAngle/NumberOfScaleMajorDivisions; 
    DivxPhasorCloser=DivCloserPhasorLenght*cos(radians(an));
    DivxPhasorDistal=DivDistalPhasorLenght*cos(radians(an));
    DivyPhasorCloser=DivCloserPhasorLenght*sin(radians(an));
    DivyPhasorDistal=DivDistalPhasorLenght*sin(radians(an));
    if (Division==NumberOfScaleMajorDivisions/2|Division==0|Division==NumberOfScaleMajorDivisions) {
      strokeWeight(15);
      stroke(0);
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal);
      strokeWeight(8);
      stroke(100, 255, 100);
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal);
    } else {
      strokeWeight(3);
      stroke(255);
      line(DivxPhasorCloser, DivyPhasorCloser, DivxPhasorDistal, DivyPhasorDistal);
    }
  }
}

void Axis() {
  stroke(255, 0, 0);
  strokeWeight(3);
  line(-115, 0, 115, 0);
  line(0, 280, 0, -280);
  fill(100, 255, 100);
  stroke(0);
  triangle(0, -285, -10, -255, 10, -255);
  triangle(0, 285, -10, 255, 10, 255);
}

void showAngles() {
  textSize(30);
  fill(50);
  noStroke();
  rect(-150, 400, 280, 40);
  rect(150, 400, 280, 40);
  fill(255);
  //pitch=pitch/5;
  //int pitch1=round(pitchDegrees);
  //roll=roll*180/PI;
  //int roll1=round(rollDegrees);
  text("pitch:  "+pitchDegrees+" Deg", -20, 411, 500, 60);
  text("roll:  "+rollDegrees+" Deg", 280, 411, 500, 60);
}

void Borders() {
  noFill();
  stroke(0);
  strokeWeight(400);
  rect(0, 0, 1100, 1100);
  strokeWeight(200);
  ellipse(0, 0, 1000, 1000);
  fill(0);
  noStroke();
  rect(4*width/5, 0, width, 2*height);
  rect(-4*width/5, 0, width, 2*height);
}

void pitchScale() { 
  stroke(255);
  fill(255);
  strokeWeight(3);
  textSize(24);
  textAlign(CENTER);
  for (int i=-4;i<5;i++) { 
    if ((i==0)==false) {
      line(110, 50*i, -110, 50*i);
    } 
    text(""+i*10, 140, 50*i, 100, 30);
    text(""+i*10, -140, 50*i, 100, 30);
  }
  textAlign(CORNER);
  strokeWeight(2);
  for (int i=-9;i<10;i++) { 
    if ((i==0)==false) {   
      line(25, 25*i, -25, 25*i);
    }
  }
}

class LowPass {
    ArrayList buffer;
    int len;
    float output;

    LowPass(int len) {
        this.len = len;
        buffer = new ArrayList(len);
        for(int i = 0; i < len; i++) {
            buffer.add(new Float(0.0));
        }
    }

    void input(float v) {
        buffer.add(new Float(v));
        buffer.remove(0);

        float sum = 0;
        for(int i=0; i<buffer.size(); i++) {
                Float fv = (Float)buffer.get(i);
                sum += fv.floatValue();
        }
        output = sum / buffer.size();
    }
}

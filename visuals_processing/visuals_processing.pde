//connection with the arduino
import processing.serial.*;

Serial myPort;

String data="";
float roll, pitch, value;

// =========================
// INPUT MODES
// =========================

final int ARDUINO_ROLL = 0;
final int ARDUINO_PITCH = 1;
final int ARDUINO_VALUE = 2;

int pitchMode = ARDUINO_ROLL;
int reverbMode = ARDUINO_PITCH;
int heightMode = ARDUINO_VALUE;

// =========================
// SLIDERS
// =========================

Slider slider1, slider2, slider3;

void setup() {
  size(1200, 800);
  textFont(createFont("Arial", 16));

  myPort = new Serial(this, "COM6", 9600); // starts the serial communication
  myPort.bufferUntil('\n');

  slider1 = new Slider(650, 150, 400);
  slider2 = new Slider(650, 280, 400);
  slider3 = new Slider(650, 410, 400);
}

void draw() {
  background(20, 0, 40);

  drawTitle();
  drawLeftPanel();
  drawRightPanel();
  drawBottom();

  // APPLY INPUT MODES
  slider1.value = getInputValue(pitchMode);
  slider2.value = getInputValue(reverbMode);
  slider3.value = getInputValue(heightMode);
}

// =========================
// INPUT LOGIC
// =========================

float getInputValue(int mode) {
  if (mode == ARDUINO_ROLL) {
    return map(roll, -90, 90, -100, 100);
  } else if (mode == ARDUINO_PITCH) {
    return map(pitch, -90, 90, -100, 100);
  } else if (mode == ARDUINO_VALUE) {
    return constrain(value, -100, 100);
  }
  return 0;
}

// =========================
// DRAW UI
// =========================

void drawTitle() {
  fill(255);
  textSize(28);
  text("handTheSound", 160, 60);
}

void drawLeftPanel() {
  drawModeBox(160, 120, "Pitch", pitchMode);
  drawModeBox(160, 200, "Reverb", reverbMode);
  drawModeBox(160, 280, "Bass", heightMode);
}

void drawModeBox(int x, int y, String label, int mode) {
  stroke(255);
  noFill();
  rect(x, y, 200, 60, 0, 60, 60, 0);
  rect(x-120, y, 120, 60, 60, 0, 0, 60);

  fill(255);
  text(label, x - 60, y + 35);
  text(getModeName(mode), x + 100, y + 35);
}

String getModeName(int mode) {
  if (mode == ARDUINO_ROLL) return "ROLL";
  if (mode == ARDUINO_PITCH) return "PITCH";
  if (mode == ARDUINO_VALUE) return "BUTTON";
  return "NONE";
}

void drawRightPanel() {
  fill(255);

  text("Pitch", 650, 130);
  slider1.display();

  text("Reverb", 650, 260);
  slider2.display();

  text("Height", 650, 390);
  slider3.display();
}

void drawBottom() {
  drawWaveform(300, 550, 400, 60);

  fill(255);
  text("glove is connected", 750, 580);
}

// =========================
// SLIDER
// =========================

class Slider {
  float x, y, w;
  float value;

  Slider(float x, float y, float w) {
    this.x = x;
    this.y = y;
    this.w = w;
  }

  void display() {
    stroke(255);
    line(x, y, x + w, y);

    float pos = map(value, -100, 100, x, x + w);

    fill(255);
    ellipse(pos, y, 14, 14);

    textAlign(CENTER);
    text(int(value), pos, y + 25);
  }
}

// =========================
// WAVEFORM
// =========================

void drawWaveform(int x, int y, int w, int h) {
  stroke(255);
  noFill();
  rect(x, y, w, h, 30);

  for (int i = 0; i < w; i += 5) {
    float n = noise(i * 0.05, frameCount * 0.05);
    float bar = map(n, 0, 1, 5, h - 5);
    line(x + i, y + h/2, x + i, y + h/2 - bar/2);
  }
}

// =========================
// INTERACTION
// =========================

void mousePressed() {
  // cycle modes on click

  if (overBox(50, 120)) pitchMode = (pitchMode + 1) % 3;
  if (overBox(50, 200)) reverbMode = (reverbMode + 1) % 3;
  if (overBox(50, 280)) heightMode = (heightMode + 1) % 3;
}

boolean overBox(int x, int y) {
  return mouseX > x && mouseX < x + 300 &&
    mouseY > y && mouseY < y + 60;
}

// Read data from the Serial Port
void serialEvent (Serial myPort) {
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');

  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {

      //--- Roll,Pitch in degrees
      roll = float(items[0]);
      pitch = float(items[1]);
      value = float(items[2]);
    }
  }
}

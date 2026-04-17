/*
    Arduino and ADXL345 Accelerometer - 3D Visualization Example 
     by Dejan, https://howtomechatronics.com
*/
#include <Wire.h>  // Wire library - used for I2C communication

int ADXL345 = 0x53;  // The ADXL345 sensor I2C address

float X_out, Y_out, Z_out;  // Outputs
float roll, pitch, rollF, pitchF = 0;

//botões
const int BT_PIN1 = 2;
const int BT_PIN2 = 3;

int btState1;  // variable for reading the pushbutton status
int btState2;
int lastReading1 = HIGH;
int lastReading2 = HIGH;

unsigned long lastDebounceTime1 = 0;  // the last time the button input changed
const unsigned long debounceDelay1 = 50;

unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay2 = 50;

unsigned long lastRepeatTime1 = 0;
unsigned long lastRepeatTime2 = 0;
const unsigned long repeatInterval = 80;

int value = 0;

void setup() {
  Serial.begin(9600);  // Initiate serial communication for printing the results on the Serial monitor

  Wire.begin();  // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345);  // Start communicating with the device
  Wire.write(0x2D);                 // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8);  // Bit D3 High for measuring enable (8dec -> 0000 1000 binary)
  Wire.endTransmission();
  delay(10);

  //Off-set Calibration
  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);
  Wire.write(1);
  Wire.endTransmission();
  delay(10);
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F);
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);

  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20);
  Wire.write(-9);
  Wire.endTransmission();
  delay(10);

  //botoes
  pinMode(BT_PIN1, INPUT_PULLUP);
  pinMode(BT_PIN2, INPUT_PULLUP);

  // Initialize debouncing state with current readings
  btState1 = digitalRead(BT_PIN1);
  btState2 = digitalRead(BT_PIN2);
  lastReading1 = btState1;
  lastReading2 = btState2;
}

void loop() {
  // Read raw button inputs
  int reading1 = digitalRead(BT_PIN1);
  int reading2 = digitalRead(BT_PIN2);

  if (reading1 != lastReading1) {
    lastDebounceTime1 = millis();
    lastReading1 = reading1;
  }

  if (reading2 != lastReading2) {
    lastDebounceTime2 = millis();
    lastReading2 = reading2;
  }

  // Update stable debounced states
  if ((millis() - lastDebounceTime1) > debounceDelay1 && reading1 != btState1) {
    btState1 = reading1;
    if (btState1 == LOW) {
      value++;
      lastRepeatTime1 = millis();
    }
  }

  if ((millis() - lastDebounceTime2) > debounceDelay2 && reading2 != btState2) {
    btState2 = reading2;
    if (btState2 == LOW) {
      value--;
      lastRepeatTime2 = millis();
    }
  }

  // Continuous repeat while the button remains pressed
  if (btState1 == LOW && (millis() - lastRepeatTime1) >= repeatInterval) {
    value++;
    lastRepeatTime1 = millis();
  }

  if (btState2 == LOW && (millis() - lastRepeatTime2) >= repeatInterval) {
    value--;
    lastRepeatTime2 = millis();
  }

  value = constrain(value, -100, 100);

  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);  // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);        // Read 6 registers total, each axis value is stored in 2 registers
  X_out = (Wire.read() | Wire.read() << 8);  // X-axis value
  X_out = X_out / 256;                       //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = (Wire.read() | Wire.read() << 8);  // Y-axis value
  Y_out = Y_out / 256;
  Z_out = (Wire.read() | Wire.read() << 8);  // Z-axis value
  Z_out = Z_out / 256;

  // Calculate Roll and Pitch (rotation around X-axis, rotation around Y-axis)
  roll = atan(Y_out / sqrt(pow(X_out, 2) + pow(Z_out, 2))) * 180 / PI;
  pitch = atan(X_out / sqrt(pow(Y_out, 2) + pow(Z_out, 2))) * 180 / PI;

  // Low-pass filter
  rollF = 0.94 * rollF + 0.06 * roll;
  pitchF = 0.94 * pitchF + 0.06 * pitch;

  Serial.print(rollF);
  Serial.print("/");
  Serial.print(pitchF);
  Serial.print("/");
  Serial.println(value);
}

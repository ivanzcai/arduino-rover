#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

// --- Motor Pins ---
const int enablePin = 2;    // PWM pin for speed control
const int in1Pin = 22;      // Direction control pin 1
const int in2Pin = 24;      // Direction control pin 2

// --- Radio & Servo ---
Servo myServo;              // Create a servo object
RF24 radio(9, 10);          // CE, CSN (Mega pins: 9, 10)

const byte address[6] = "00001";
char receivedMsg[32];

// Variables to store parsed values
int xVal = 0;
int yVal = 0;
int buttonVal = 0;

void setup() {
  // Set motor pins as outputs
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

  myServo.attach(8);  // Tell the Arduino the servo is on pin 8
  
  Serial.begin(9600);
  Serial.println("Mega Receiver & Motor Control Ready");

  if (!radio.begin()) {
    Serial.println("Radio init failed!");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();  // Must enable on both sides

  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    // Read the incoming message
    radio.read(&receivedMsg, sizeof(receivedMsg));
    
    // Parse the incoming CSV string
    int parsed = sscanf(receivedMsg, "%d,%d,%d", &xVal, &yVal, &buttonVal);
    
    if (parsed == 3) {
      
      // --- 1. Control Servo (X-Axis) ---
      int servoAngle;
 
      servoAngle = map(xVal, 0, 1023, 140, 40); 

      myServo.write(servoAngle);

      // --- 2. Control Motor (Y-Axis) ---
      // Using a deadzone (520-540) around the neutral 531 to prevent jitter
      if (yVal > 540) {
        // Pushing forward
        int speed = map(yVal, 541, 1023, 0, 255);
        motorForward(speed);
      } 
      else if (yVal < 520) {
        // Pulling backward
        int speed = map(yVal, 519, 0, 0, 255);
        motorReverse(speed);
      } 
      else {
        // Neutral position
        motorStop();
      }

    } else {
      Serial.println("Parse failed");
    }

    // Load ACK payload for NEXT transmission
    const char reply[] = "Hi Nano!";
    radio.writeAckPayload(0, &reply, sizeof(reply));
    delay(50);
  }
}

// --- Motor Control Functions ---

void motorForward(int speed) {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  analogWrite(enablePin, speed);
}

void motorReverse(int speed) {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  analogWrite(enablePin, speed);
}

void motorStop() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  analogWrite(enablePin, 0);
}

void motorBrake() {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, HIGH);
  analogWrite(enablePin, 255);
}
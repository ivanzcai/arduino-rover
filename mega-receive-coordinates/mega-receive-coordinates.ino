

#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

Servo myServo;  // Create a servo object

RF24 radio(9, 10);  // CE, CSN (Mega pins: 9, 10)

const byte address[6] = "00001";
char receivedMsg[32];

// Variables to store parsed values
int xVal = 0;
int yVal = 0;
int buttonVal = 0;

void setup() {
  myServo.attach(8);  // Tell the Arduino the servo is on P
  Serial.begin(9600);

  if (!radio.begin()) {
    Serial.println("Radio init failed!");
    while (1)
      ;
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();  // Must enable on both sides

  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("Mega Receiver Ready");
}

void loop() {

  if (radio.available()) {
    // Read the incoming message
    radio.read(&receivedMsg, sizeof(receivedMsg));
    Serial.print("Received: ");
    Serial.println(receivedMsg);

    char textIn[32] = "";
    // int x, y, b;
    int parsed = sscanf(receivedMsg, "%d,%d,%d", &xVal, &yVal, &buttonVal);
    if (parsed == 3) {
      Serial.print(" -> Parsed X: ");
      Serial.print(xVal);
      Serial.print(" Y: ");
      Serial.print(yVal);
      Serial.print(" Button: ");
      Serial.println(buttonVal);
      int servoAngle = map(xVal, 0, 1023, 40, 140);
      myServo.write(servoAngle);

    } else {
      Serial.print(" -> Parse failed, got ");
      Serial.print(parsed);
      Serial.println(" values");
    }
    // Load ACK payload for NEXT transmission
    // This gets sent automatically when the sender transmits again
    const char reply[] = "Hi Nano!";
    radio.writeAckPayload(0, &reply, sizeof(reply));
    Serial.println("ACK payload loaded for next reply");
    delay(100);
  }
}
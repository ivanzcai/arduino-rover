

// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>

// RF24 radio(7, 8); 
// const byte address[6] = "00001";

// void setup() {
//   Serial.begin(9600);
//   radio.begin();
//   radio.openWritingPipe(address);
//   radio.setPALevel(RF24_PA_MIN);
//   radio.setDataRate(RF24_250KBPS); 
//   radio.stopListening();
  
//   pinMode(2, INPUT_PULLUP);
// }

// void loop() {
//   int xVal = analogRead(A0);
//   int yVal = analogRead(A1);
//   byte btn = digitalRead(2);

//   // 1. Create a character buffer (max 32 bytes for NRF24)
//   char textOut[32];

//   // 2. Format the string: "X,Y,B" (e.g., "512,510,1")
//   sprintf(textOut, "%d,%d,%d", xVal, yVal, btn);

//   // 3. Send the string
//   bool success = radio.write(&textOut, sizeof(textOut));

//   if (success) {
//     Serial.print("Sent String: ");
//     Serial.println(textOut);
//   } else {
//     Serial.println("Delivery Failed");
//   }
  
//   delay(500); 
// }

#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";
char replyBuffer[32];

void setup() {
  Serial.begin(9600);
  
  if (!radio.begin()) {
    Serial.println("Radio init failed!");
    while(1);
  }
  
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.enableAckPayload();  // Enable automatic acknowledgement with payload
  
  radio.openWritingPipe(address);
  radio.stopListening();
  
  // Pre-load an ACK payload (optional - can be updated anytime)
  const char ackMsg[] = "ACK from Nano";
  radio.writeAckPayload(1, &ackMsg, sizeof(ackMsg));
  
  Serial.println("Nano Transmitter Ready");
}

void loop() {

    int xVal = analogRead(A0);
  int yVal = analogRead(A1);
  byte btn = digitalRead(2);

  // 1. Create a character buffer (max 32 bytes for NRF24)
  char textOut[32];

  // 2. Format the string: "X,Y,B" (e.g., "512,510,1")
  sprintf(textOut, "%d,%d,%d", xVal, yVal, btn);
  // const char message[] = "Hello Mega!";
  
  // // Send and wait for ACK payload automatically
  // bool success = radio.write(&message, sizeof(message));
  bool success = radio.write(&textOut, sizeof(textOut));

  if (success) {
    Serial.print("Sent: ");
    Serial.println(textOut);
    
    // Check if we got an ACK payload
    if (radio.isAckPayloadAvailable()) {
      radio.read(&replyBuffer, sizeof(replyBuffer));
      Serial.print("Got ACK reply: ");
      Serial.println(replyBuffer);
    } else {
      Serial.println("Sent OK (no ACK payload)");
    }
  } else {
    Serial.println("Send failed - no ACK received");
  }
  
  delay(0);
}

// #include <SPI.h>
// #include <RF24.h>

// RF24 radio(9, 10); 
// const byte address[6] = "00001";

// void setup() {
//   Serial.begin(9600);
//   pinMode(53, OUTPUT); // Essential for Mega

//   radio.begin();
//   radio.setDataRate(RF24_250KBPS);
//   radio.openReadingPipe(1, address);

//   radio.startListening();
  
//   Serial.println("Mega Receiver (String Mode) Ready");
// }

// void loop() {
//   if (radio.available()) {
//     char textIn[32] = "";
//     radio.read(&textIn, sizeof(textIn));

//     // 1. Print the raw string received
//     Serial.print("Raw: ");
//     Serial.print(textIn);

//     // 2. Parse the string back into integers
//     int x, y, b;
//     // sscanf looks for the commas we added in the transmitter
//     sscanf(textIn, "%d,%d,%d", &x, &y, &b);

//     // 3. Print the parsed values to prove it worked
//     Serial.print(" -> Parsed X: ");
//     Serial.println(x);
//   }
// }

// // #include <SPI.h>
// // #include <RF24.h>

// // RF24 radio(9, 10); 

// // void setup() {
// //   Serial.begin(9600);
// //   pinMode(53, OUTPUT); // MUST be output for Mega SPI to wake up
  
// //   Serial.println("Starting SPI Hardware Scan...");
  
// //   if (radio.begin()) {
// //     Serial.println("SUCCESS: Mega found the Radio Chip!");
// //   } else {
// //     Serial.println("FAIL: Check Pins 50, 51, 52.");
// //   }
// // }

// // void loop() {}

#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN (Mega pins: 9, 10)

const byte address[6] = "00001";
char receivedMsg[32];

// Variables to store parsed values
int xVal = 0;
int yVal = 0;
int buttonVal = 0;

void setup() {
  Serial.begin(9600);
  
  if (!radio.begin()) {
    Serial.println("Radio init failed!");
    while(1);
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
  }
}
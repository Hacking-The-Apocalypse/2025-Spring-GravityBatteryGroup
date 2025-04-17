//Bluetooth Code 
#include <SoftwareSerial.h>


// HC-05 connection pins
const int hc05_rx_pin = 10;  // Nano pin 10 -> HC-05 TX
const int hc05_tx_pin = 11;  // Nano pin 11 -> HC-05 RX


SoftwareSerial BTSerial(hc05_rx_pin, hc05_tx_pin);


void setup() {
  // Start serial communication with computer
  Serial.begin(9600);
  delay(1000);  // Give serial monitor time to open


  Serial.println("\n----- HC-05 AT Command Interface -----");
  Serial.println("Make sure HC-05 is in AT mode (LED blinking slowly)");
  Serial.println("Enter AT commands in the Serial Monitor");
  Serial.println("For example: AT, AT+VERSION, AT+NAME, etc.");


  // Start communication with HC-05 at 38400 baud
  BTSerial.begin(38400);
}


void loop() {
  // Check for commands from Serial Monitor
  if (Serial.available()) {
    // Read the command
    String command = Serial.readStringUntil('\n');


    // Trim any whitespace
    command.trim();


    // Show what we're sending
    Serial.print("Sending: ");
    Serial.println(command);


    // Send command to HC-05 with proper line endings
    BTSerial.print(command);
    BTSerial.print("\r\n");


    // Wait for response
    delay(1000);


    // Display response
    Serial.println("Response:");
    int responseCount = 0;


    // Check for response for 2 seconds
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
      if (BTSerial.available()) {
        char c = BTSerial.read();
        Serial.write(c);
        responseCount++;
      }
    }


    if (responseCount == 0) {
      Serial.println("No response received");
    }


    Serial.println("\n-----");
  }
}

//Master
#include <SoftwareSerial.h>


// HC-05 connection pins
const int hc05_rx_pin = 10;  // Nano pin 10 -> HC-05 TX
const int hc05_tx_pin = 11;  // Nano pin 11 -> HC-05 RX


// Relay control pin
const int relayPin = 4;  // Connect relay to pin 4


SoftwareSerial BTSerial(hc05_rx_pin, hc05_tx_pin);


// Variables for relay operation
unsigned long previousMillis = 0;
const long relayInterval = 5000;  // 2 seconds on, 2 seconds off
boolean relayState = false;


void setup() {
  // Start serial communication with computer
  Serial.begin(9600);
  delay(1000);  // Give serial monitor time to open
 
  Serial.println("\n----- Master Controller -----");
  Serial.println("Commands:");
  Serial.println("O - Open the remote solenoid");
  Serial.println("C - Close the remote solenoid");
  Serial.println("AT - Enter AT command mode");
 
  // Initialize relay pin as output
  pinMode(relayPin, OUTPUT);
 
  // Ensure relay starts in OFF state
  digitalWrite(relayPin, LOW);
  Serial.println("Local relay is OFF (default state)");
 
  // Start communication with HC-05 at 9600 baud for normal operation
  BTSerial.begin(9600);
}


void loop() {
  // Operate local relay in the background
  unsigned long currentMillis = millis();
 
  if (currentMillis - previousMillis >= relayInterval) {
    previousMillis = currentMillis;
   
    // Toggle relay state
    relayState = !relayState;
   
    if (relayState) {
      // Turn relay ON
      digitalWrite(relayPin, HIGH);
      Serial.println("Local relay is ON");
    } else {
      // Turn relay OFF
      digitalWrite(relayPin, LOW);
      Serial.println("Local relay is OFF");
    }
  }
 
  // Check for commands from Serial Monitor
  if (Serial.available()) {
    // Read the command
    char command = Serial.read();
   
    // Handle AT command mode
    if (command == 'A' && Serial.peek() == 'T') {
      Serial.read(); // Clear the 'T'
     
      // Switch to AT command mode
      Serial.println("\n----- Switching to AT Command Mode -----");
      BTSerial.end();
      BTSerial.begin(38400);
     
      // Handle AT commands until 'EXIT' is received
      handleATCommands();
     
      // Return to normal mode
      BTSerial.end();
      BTSerial.begin(9600);
      Serial.println("\n----- Returning to Normal Mode -----");
      return;
    }
   
    // Handle remote solenoid commands
    if (command == 'O' || command == 'C') {
      // Show what we're sending
      Serial.print("Sending command to remote solenoid: ");
      Serial.println(command);
     
      // Send command to slave
      BTSerial.print(command);
     
      // Wait for response
      Serial.println("Waiting for response...");
      unsigned long startTime = millis();
      boolean responseReceived = false;
     
      // Check for response for 5 seconds
      while (millis() - startTime < 5000 && !responseReceived) {
        if (BTSerial.available()) {
          String response = BTSerial.readString();
          Serial.print("Response from slave: ");
          Serial.println(response);
          responseReceived = true;
        }
      }
     
      if (!responseReceived) {
        Serial.println("No response received from slave");
      }
    }
  }
 
  // Check for any responses from slave
  if (BTSerial.available()) {
    String response = BTSerial.readString();
    Serial.print("Unsolicited message from slave: ");
    Serial.println(response);
  }
}


void handleATCommands() {
  Serial.println("Enter AT commands (type 'EXIT' to return to normal mode)");
 
  while (true) {
    if (Serial.available()) {
      // Read the command
      String command = Serial.readStringUntil('\n');
     
      // Trim any whitespace
      command.trim();
     
      // Check for exit command
      if (command == "EXIT") {
        return;
      }
     
      // Show what we're sending
      Serial.print("Sending AT command: ");
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
}

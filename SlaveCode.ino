//Slave
#include <SoftwareSerial.h>

// HC-05 connection pins
const int hc05_rx_pin = 10;  // Nano pin 10 -> HC-05 TX
const int hc05_tx_pin = 11;  // Nano pin 11 -> HC-05 RX

// Solenoid control pin
const int solenoidPin = 5;  // Connect TIP120 to pin D5

SoftwareSerial BTSerial(hc05_rx_pin, hc05_tx_pin);

void setup() {
  // Start serial communication with computer
  Serial.begin(9600);
  delay(1000);  // Give serial monitor time to open
  
  Serial.println("\n----- Solenoid Control - SLAVE -----");
  Serial.println("Waiting for commands from master");
  
  // Initialize solenoid pin as output
  pinMode(solenoidPin, OUTPUT);
  
  // Ensure solenoid starts in CLOSED state
  digitalWrite(solenoidPin, LOW);
  Serial.println("Solenoid is CLOSED (default state)");
  
  // Start communication with HC-05 at 9600 baud for normal operation
  BTSerial.begin(9600);
}

void loop() {
  // Check for commands from master
  if (BTSerial.available()) {
    char command = BTSerial.read();
    
    Serial.print("Command received from master: ");
    Serial.println(command);
    
    // Process commands
    if (command == 'O') {
      // Open solenoid
      Serial.println("Opening solenoid");
      digitalWrite(solenoidPin, HIGH);  // Turn transistor ON
      
      // Send confirmation to master
      BTSerial.println("Solenoid OPENED successfully");
      Serial.println("Sent response: Solenoid OPENED successfully");
    } 
    else if (command == 'C') {
      // Close solenoid
      Serial.println("Closing solenoid");
      digitalWrite(solenoidPin, LOW);  // Turn transistor OFF
      
      // Send confirmation to master
      BTSerial.println("Solenoid CLOSED successfully");
      Serial.println("Sent response: Solenoid CLOSED successfully");
    }
  }
  
  // Check for local console commands (for testing)
  if (Serial.available()) {
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
    
    // For local testing of solenoid
    if (command == 'O') {
      Serial.println("LOCAL: Opening solenoid");
      digitalWrite(solenoidPin, HIGH);
    } 
    else if (command == 'C') {
      Serial.println("LOCAL: Closing solenoid");
      digitalWrite(solenoidPin, LOW);
    }
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
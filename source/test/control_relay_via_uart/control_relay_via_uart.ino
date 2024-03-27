#define RELAY_PIN 23

void setup() {
  // Initialize UART communication
  Serial.begin(9600);
  
  // Set relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  
  // By default, turn off the relay
  digitalWrite(RELAY_PIN, HIGH);
}

void loop() {
  // Check if data is available to read
  if (Serial.available() > 0) {
    // Read the incoming byte
    char command = Serial.read();
    
    // Process the command
    switch (command) {
      case '0':
        // Turn off the relay
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Relay turned off");
        break;
      case '1':
        // Turn on the relay
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("Relay turned on");
        break;
      default:
        // Invalid command
        Serial.println("Invalid command");
        break;
    }
  }
}

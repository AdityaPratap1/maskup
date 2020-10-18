const int LED_BUILTIN = 13;


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever

void loop() {
  if (Serial.available() > 0) {
    int val = char(Serial.read()) - '0';
    if (val == 2) {
      Serial.write("ON\n");
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      Serial.write("ON\n");
      digitalWrite(LED_BUILTIN, LOW);
      }
  }
}
  
  
  
  // wait for a second
}

const byte numJacks = 4;
const byte jackPins[numJacks] = {12, 11, 10, 9}; // Detection pins
const byte ledPins[numJacks]  = {7, 6, 5, 4};    // LEDs

bool connected[numJacks][numJacks] = {false}; 

void setup() {
  Serial.begin(9600);
  for (byte i = 0; i < numJacks; i++) {
    pinMode(jackPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

void loop() {
  bool newConnected[numJacks][numJacks] = {false};

  // Scan all jacks
  for (byte i = 0; i < numJacks; i++) {
    pinMode(jackPins[i], OUTPUT);
    digitalWrite(jackPins[i], LOW);

    for (byte j = 0; j < numJacks; j++) {
      if (j != i) {
        pinMode(jackPins[j], INPUT_PULLUP);
        if (digitalRead(jackPins[j]) == LOW) {
          newConnected[i][j] = true;
          newConnected[j][i] = true;  // Set symmetric
        }
      }
    }
    pinMode(jackPins[i], INPUT_PULLUP);
  }

  // Print changes
  for (byte i = 0; i < numJacks; i++) {
    for (byte j = i + 1; j < numJacks; j++) {
      if (newConnected[i][j] && !connected[i][j]) {
        Serial.print("Connected: Jack ");
        Serial.print(i + 1);
        Serial.print(" ↔ Jack ");
        Serial.println(j + 1);
      }
      if (!newConnected[i][j] && connected[i][j]) {
        Serial.print("Disconnected: Jack ");
        Serial.print(i + 1);
        Serial.print(" ↔ Jack ");
        Serial.println(j + 1);
      }
    }
  }

  // Update LEDs after full scan
  for (byte i = 0; i < numJacks; i++) {
    bool anyConnection = false;
    for (byte j = 0; j < numJacks; j++) {
      if (newConnected[i][j]) {
        anyConnection = true;
        break;
      }
    }
    digitalWrite(ledPins[i], anyConnection ? HIGH : LOW);
  }

  // Save state
  for (byte i = 0; i < numJacks; i++) {
    for (byte j = 0; j < numJacks; j++) {
      connected[i][j] = newConnected[i][j];
    }
  }

  delay(50);
}

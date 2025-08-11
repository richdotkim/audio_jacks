const byte numJacks = 4;
const byte jackPins[numJacks] = {12, 11, 10, 9}; // 12=idx0, 11=idx1, 10=idx2, 9=idx3
const byte ledPins[numJacks]  = {7, 6, 5, 4};

const byte lockPin = A0; // Magnetic lock control pin

bool connections[numJacks][numJacks];
bool puzzleSolved = false; // latch flag

bool extraConnections(); // <-- forward declaration

void setup() {
  for (byte i = 0; i < numJacks; i++) {
    pinMode(jackPins[i], INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(lockPin, OUTPUT);

  digitalWrite(lockPin, HIGH); // Relay ON → Maglock locked at startup
  Serial.begin(9600);
}

void loop() {
  // Scan all jack-to-jack connections
  for (byte i = 0; i < numJacks; i++) {
    for (byte j = 0; j < numJacks; j++) {
      if (i != j) {
        pinMode(jackPins[i], OUTPUT);
        digitalWrite(jackPins[i], LOW);
        delay(2);
        connections[i][j] = (digitalRead(jackPins[j]) == LOW);
        pinMode(jackPins[i], INPUT_PULLUP);
      }
    }
  }

  // Light LEDs for any jack that has at least one connection
  for (byte i = 0; i < numJacks; i++) {
    bool anyConnection = false;
    for (byte j = 0; j < numJacks; j++) {
      if (connections[i][j]) {
        anyConnection = true;
        break;
      }
    }
    digitalWrite(ledPins[i], anyConnection ? HIGH : LOW);
  }

  // Only check puzzle solution if not already solved
  if (!puzzleSolved) {
    bool solved =
      connections[0][3] && connections[3][0] &&   // 12 ↔ 9
      connections[2][1] && connections[1][2] &&   // 10 ↔ 11
      !extraConnections();

    if (solved) {
      puzzleSolved = true;
      digitalWrite(lockPin, LOW); // Relay OFF → Maglock unlocked
      Serial.println("Puzzle Solved! Maglock Released");
    }
  }

  delay(200);
}

bool extraConnections() {
  // True if any wrong connection exists
  for (byte i = 0; i < numJacks; i++) {
    for (byte j = i + 1; j < numJacks; j++) {
      bool correctPair =
        ( (i == 0 && j == 3) || (i == 3 && j == 0) ||
          (i == 2 && j == 1) || (i == 1 && j == 2) );
      if (!correctPair && connections[i][j]) {
        return true;
      }
    }
  }
  return false;
}

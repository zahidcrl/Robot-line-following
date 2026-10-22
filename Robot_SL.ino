// Robot Line Following Code - v1
// https://github.com/zahidcrl/Robot-line-following

// Left Motor
const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;

// Right Motor
const int ENB = 10;
const int IN3 = 8;
const int IN4 = 9;

// Sensors
const int SENS_PIN[7] = {A0, A1, A2, A3, A4, A5, A6}; // FIX
int thresholdSensor[7];

// Debounce
int stableReading = 0;
int previewReading = 0;
int filterCounter = 0;
const int FILTER_THRESHOLD = 3;

// Flip-Flop
int memory = 0;

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  for(int i = 0; i < 7; i++) pinMode(SENS_PIN[i], INPUT);

  // Calibration routine - 5 seconds
  int maxSensor[7] = {0, 0, 0, 0, 0, 0, 0};                      // Dark
  int minSensor[7] = {1023, 1023, 1023, 1023, 1023, 1023, 1023}; // Clear

  unsigned long initTime = millis();
  while (millis() - initTime < 5000) {
    for (int i = 0; i < 7; i++) {
      int lightReading = analogRead(SENS_PIN[i]);
      if (lightReading > maxSensor[i]) maxSensor[i] = lightReading; // Max peaks
      if (lightReading < minSensor[i]) minSensor[i] = lightReading; // Min peaks
    }
  }

  for (int i = 0; i < 7; i++) thresholdSensor[i] = (maxSensor[i] + minSensor[i]) / 2;
}

// Left Motor
void motorLeft(int speed, bool forward) {
  if (speed == 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, 255);
  } else {
    digitalWrite(IN1, forward ? HIGH : LOW);
    digitalWrite(IN2, forward ? LOW : HIGH);
    analogWrite(ENA, speed);
  }
}

// Right Motor
void motorRight(int speed, bool forward) {
  if (speed == 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, 255);
  } else {
    digitalWrite(IN3, forward ? HIGH : LOW);
    digitalWrite(IN4, forward ? LOW : HIGH);
    analogWrite(ENB, speed);
  }
}

void loop() {
  int currentReading = 0;
  
  // Convert analog to binary
  for(int i = 0; i < 7; i++) {
    int analogValue = analogRead(SENS_PIN[i]);
    int val = 0;
    
    if (analogValue < thresholdSensor[i]) val = 1; // FIX
    else val = 0;
    currentReading |= (val << (6 - i)); // FIX
  }

  // Debounce
  if (currentReading == previewReading) {
    filterCounter++;
    if (filterCounter >= FILTER_THRESHOLD) stableReading = currentReading;
  } else {
    filterCounter = 0;
    previewReading = currentReading;
  }
  previewReading = currentReading;

  switch (stableReading) {
    
    case 0b0001000:
    case 0b0011100:
      memory = 0;
      motorLeft(255, true);
      motorRight(255, true);
      break;

    case 0b0000100:
    case 0b0000110:
      memory = 1;
      motorLeft(255, true);
      motorRight(140, true);
      break;

    case 0b0010000:
    case 0b0110000:
      memory = 2;
      motorLeft(140, true);
      motorRight(255, true);
      break;

    case 0b0000010:
    case 0b0000011:
      memory = 1;
      motorLeft(255, true);
      motorRight(0, true);
      break;

    case 0b0100000:
    case 0b1100000:
      memory = 2;
      motorLeft(0, true);
      motorRight(255, true);
      break;

    case 0b1000000:
      memory = 2;
      motorLeft(150, false);
      motorRight(255, true);
      break;

    case 0b0000001:
      memory = 1;
      motorLeft(255, true);
      motorRight(150, false);
      break;

    case 0b0000000: 
      if (memory == 1) {
        motorLeft(200, true);
        motorRight(150, false);
      } 
      else if (memory == 2) {
        motorLeft(150, false);
        motorRight(200, true);
      } 
      else {
        motorLeft(100, true);
        motorRight(100, true);
      }
      break;

    default:
      motorLeft(150, true);
      motorRight(150, true);
      break;
  }
}

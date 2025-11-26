#include <AFMotor.h>

class MelodyPlayer {
  private:
    int pin;

  public:
    MelodyPlayer(int buzzerPin) {
      pin = buzzerPin;
    }

    void init() {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }

    void playNote(int period, int duration) {
      long cycles = (long)duration * 1000L / (period * 2);
      for (long i = 0; i < cycles; i++) {
        digitalWrite(pin, LOW);
        delayMicroseconds(period);
        digitalWrite(pin, HIGH);
        delayMicroseconds(period);
      }
    }

    void playStartupTune() {
      playNote(1915, 200); // A
      playNote(1700, 200); // B
      playNote(1519, 200); // C#
      playNote(1432, 400); // D
    }

    void honk() {
      digitalWrite(pin, LOW);
      delay(160);
      digitalWrite(pin, HIGH);
    }
    
    void enable() { 
      digitalWrite(pin, LOW); 
    }
    void disable() { 
      digitalWrite(pin, HIGH); 
    }
};

class RobotCar {
  private:
    AF_DCMotor motorFL;
    AF_DCMotor motorFR;
    AF_DCMotor motorBL;
    AF_DCMotor motorBR;

    int headingLedPin;
    int backLedPin;
    int lowBattLedPin;
    int frontSensorPin;
    int backSensorPin;

    int currentSpeed;
    MelodyPlayer* buzzer; 

    bool isBlocked(int sensorPin) {
      return digitalRead(sensorPin) == LOW;
    }

  public:
    RobotCar(MelodyPlayer* bz, int hLed, int bLed, int lbLed, int fSens, int bSens) 
      : motorFL(1), motorBL(2), motorBR(3), motorFR(4), buzzer(bz) {
      
      headingLedPin = hLed;
      backLedPin = bLed;
      lowBattLedPin = lbLed;
      frontSensorPin = fSens;
      backSensorPin = bSens;
      currentSpeed = 255; 
    }

    void init() {
      pinMode(backLedPin, OUTPUT);
      pinMode(headingLedPin, OUTPUT);
      pinMode(lowBattLedPin, OUTPUT);
      pinMode(frontSensorPin, INPUT);
      pinMode(backSensorPin, INPUT);

      setSpeed(currentSpeed);
      stop(); 
    }

    void setSpeed(int speedVal) {
      currentSpeed = speedVal;
      motorFL.setSpeed(currentSpeed);
      motorFR.setSpeed(currentSpeed);
      motorBL.setSpeed(currentSpeed);
      motorBR.setSpeed(currentSpeed);
    }

    void stop() {
      motorFL.run(RELEASE);
      motorFR.run(RELEASE);
      motorBL.run(RELEASE);
      motorBR.run(RELEASE);
      digitalWrite(backLedPin, HIGH);
    }

    void lightsOn() { 
      digitalWrite(headingLedPin, HIGH); 
    }
    void lightsOff() { 
      digitalWrite(headingLedPin, LOW); 
    }

    void moveForward() {
      buzzer->disable(); 
      
      if (isBlocked(frontSensorPin)) {
        stop();
        return;
      }
      
      // Reset to full speed in case diagonal mode changed it previously
      setSpeed(currentSpeed); 
      
      motorFR.run(FORWARD);
      motorFL.run(FORWARD);
      motorBL.run(FORWARD);
      motorBR.run(FORWARD);
      digitalWrite(backLedPin, LOW);
    }

    void moveBackward() {
      buzzer->disable(); 

      if (isBlocked(backSensorPin)) {
        stop();
        return;
      }

      setSpeed(currentSpeed); 

      motorFR.run(BACKWARD);
      motorFL.run(BACKWARD);
      motorBL.run(BACKWARD);
      motorBR.run(BACKWARD);
      digitalWrite(backLedPin, HIGH);
    }

    void turnRight() {
      setSpeed(currentSpeed);
      motorFR.run(BACKWARD);
      motorBR.run(BACKWARD);
      motorFL.run(FORWARD);
      motorBL.run(FORWARD);
      digitalWrite(backLedPin, LOW);
    }

    void turnLeft() {
      setSpeed(currentSpeed);
      motorFR.run(FORWARD);
      motorBR.run(FORWARD);
      motorFL.run(BACKWARD);
      motorBL.run(BACKWARD);
      digitalWrite(backLedPin, LOW);
    }

    void moveForwardLeft() {
      buzzer->disable();
      if (isBlocked(frontSensorPin)) { 
        stop(); 
        return; 
      }

      motorFR.run(FORWARD);
      motorFL.run(FORWARD);
      motorBL.run(FORWARD);
      motorBR.run(FORWARD);

      // Diagonal Logic: Reduce speed of left motors
      motorFL.setSpeed(currentSpeed / 4);
      motorFR.setSpeed(currentSpeed);
      motorBL.setSpeed(currentSpeed / 4);
      motorBR.setSpeed(currentSpeed);
      
      digitalWrite(backLedPin, LOW);
    }

    void moveForwardRight() {
      buzzer->disable();
      if (isBlocked(frontSensorPin)) { 
        stop(); 
        return; 
      }

      motorFR.run(FORWARD);
      motorFL.run(FORWARD);
      motorBL.run(FORWARD);
      motorBR.run(FORWARD);

      // Diagonal Logic: Reduce speed of right motors
      motorFL.setSpeed(currentSpeed);
      motorFR.setSpeed(currentSpeed / 4);
      motorBL.setSpeed(currentSpeed);
      motorBR.setSpeed(currentSpeed / 4);
      
      digitalWrite(backLedPin, LOW);
    }

    void moveBackwardLeft() {
      buzzer->disable();
      if (isBlocked(backSensorPin)) { 
        stop(); 
        return; 
      }

      motorFR.run(BACKWARD);
      motorFL.run(BACKWARD);
      motorBL.run(BACKWARD);
      motorBR.run(BACKWARD);

      motorFL.setSpeed(currentSpeed / 4);
      motorFR.setSpeed(currentSpeed);
      motorBL.setSpeed(currentSpeed / 4);
      motorBR.setSpeed(currentSpeed);
      
      digitalWrite(backLedPin, HIGH);
    }

    void moveBackwardRight() {
      buzzer->disable();
      if (isBlocked(backSensorPin)) { 
        stop(); 
        return; 
      }

      motorFR.run(BACKWARD);
      motorFL.run(BACKWARD);
      motorBL.run(BACKWARD);
      motorBR.run(BACKWARD);

      motorFL.setSpeed(currentSpeed);
      motorFR.setSpeed(currentSpeed / 4);
      motorBL.setSpeed(currentSpeed);
      motorBR.setSpeed(currentSpeed / 4);
      
      digitalWrite(backLedPin, HIGH);
    }
};

// ==========================================
// MAIN SKETCH
// ==========================================

// Pin Definitions
const int BUZ_PIN = 2;
const int HEADING_LED = A5;
const int BACK_LED = A4;
const int LOW_BAT_LED = A3;
const int FRONT_OBSTACLE = A0;
const int BACK_OBSTACLE = A1;

// Instantiate Objects
MelodyPlayer speaker(BUZ_PIN);
RobotCar robot(&speaker, HEADING_LED, BACK_LED, LOW_BAT_LED, FRONT_OBSTACLE, BACK_OBSTACLE);

void setup() {
  Serial.begin(9600);
  
  // Initialize hardware
  speaker.init();
  robot.init(); 
  
  // Play startup sound
  speaker.playStartupTune();
}

void loop() {
  while (Serial.available() > 0) {
    char command = Serial.read();
    handleCommand(command);
  }
}

void handleCommand(char command) {
  switch (command) {
    case 'F': 
      robot.moveForward(); 
      break;

    case 'B': 
      robot.moveBackward(); 
      break;

    case 'R': 
      robot.turnRight(); 
      break;

    case 'L': 
      robot.turnLeft(); 
      break;

    case 'G': 
      robot.moveForwardLeft(); 
      break;

    case 'H': 
      robot.moveForwardRight(); 
      break;

    case 'I': 
      robot.moveBackwardLeft(); 
      break;

    case 'J': 
      robot.moveBackwardRight(); 
      break;

    case 'S': 
      robot.stop(); 
      break;

    case 'Y': 
      speaker.honk(); 
      break;

    case 'U': 
      robot.lightsOn(); 
      break;

    case 'u': 
      robot.lightsOff(); 
      break;

    case '1': 
      robot.setSpeed(65); 
      break;

    case '2': 
      robot.setSpeed(130); 
      break;

    case '3': 
      robot.setSpeed(195); 
      break;

    case '4': 
      robot.setSpeed(255); 
      break;
  }
}
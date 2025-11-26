#include <AFMotor.h>

const int movement_pin = 2;
const int blocked_pin = A0;
AF_DCMotor motors[4] = {AF_DCMotor(1), AF_DCMotor(2), AF_DCMotor(3), AF_DCMotor(4)};

const int wheelSpeed = 120;
const int wheelSideSpeed = 200;
const int wheelStop = 0;
const int motors_numb = 4;

const int rampStep = 40;       
const int rampInterval = 30;    
unsigned long lastRampTime = 0;
int currentRampSpeed = 0;

const unsigned long blockThreshold = 3000;   
const unsigned long sidewayDuration = 650;  

unsigned long blockedStartTime = 0;
unsigned long sidewayStartTime = 0;
unsigned long currenttime = 0;

bool isSidewayActive = false;

enum State {
  MOVE_FORWARD,
  MOVE_BACKWARD,
  BLOCKED,
  SIDEWAY,
  RAMP_DOWN 
};

State currentState = MOVE_FORWARD;
State nextState = MOVE_FORWARD; //напрямок який буде після плавної зупинки

void setup() {
  pinMode(movement_pin, INPUT);
  pinMode(blocked_pin, INPUT);
}

void loop() {
  bool blocked = digitalRead(blocked_pin);
  bool movement = digitalRead(movement_pin);
  currenttime = millis();
  State desiredState = currentState; 
  if (blocked) {
    if (blockedStartTime == 0) blockedStartTime = currenttime;

    if (currenttime - blockedStartTime >= blockThreshold && !isSidewayActive) { //якщо ми в забороненому стані 3 або більше секунди то повертаємо в бік
      desiredState = SIDEWAY;
      if (!isSidewayActive) {
        sidewayStartTime = currenttime;
        isSidewayActive = true;
      }
    } 
    else if (!isSidewayActive) {
      desiredState = BLOCKED;
    }
  } 
  else {
    blockedStartTime = 0;
    if (isSidewayActive) { //перевіряємо чи завершили ми поворот
      if (currenttime - sidewayStartTime >= sidewayDuration) {
        isSidewayActive = false;
        desiredState = (movement ? MOVE_FORWARD : MOVE_BACKWARD);
      } 
      else {
        desiredState = SIDEWAY;
      }
    } 
    else {
      desiredState = (movement ? MOVE_FORWARD : MOVE_BACKWARD);
    }
  }

  if (currentState != RAMP_DOWN) { 
    bool isSwitchingDirection = (currentState == MOVE_FORWARD && desiredState == MOVE_BACKWARD) || (currentState == MOVE_BACKWARD && desiredState == MOVE_FORWARD); //перевіряємо чи є зміна стану

    if (isSwitchingDirection) {
      currentState = RAMP_DOWN;
      nextState = desiredState;     //записужмо куди ми хочемо поїхати
      currentRampSpeed = wheelSpeed;
      lastRampTime = currenttime;
    } 
    else {
      currentState = desiredState;
    }
  }

  switch (currentState) {
    case BLOCKED:
      Blocked();
      break;

    case MOVE_FORWARD:
      Forward();
      break;

    case MOVE_BACKWARD:
      Backward();
      break;

    case SIDEWAY:
      Sideways();
      break;

    case RAMP_DOWN:
      RampDownLogic();
      break;
  }
}

void RampDownLogic() {
  if (currenttime - lastRampTime >= rampInterval) {
    lastRampTime = currenttime; 
    currentRampSpeed -= rampStep; //зменшуємо швидкість

    if (currentRampSpeed > 0) { //поки швидкість більше 0 зменшуємо її
      for (int i = 0; i < motors_numb; i++) {
        motors[i].setSpeed(currentRampSpeed);
      }
    } 
    else {
       for (int i = 0; i < motors_numb; i++) {
         motors[i].run(RELEASE);
         motors[i].setSpeed(wheelSpeed); //повертажмось до необхідної швидкості
       }
       currentState = nextState; 
    }
  }
}

void Sideways() {
  for (int i = 0; i < motors_numb; i++){
    motors[i].setSpeed(wheelSideSpeed);
  }
  motors[0].run(FORWARD);
  motors[1].run(FORWARD);
  motors[2].run(BACKWARD);
  motors[3].run(BACKWARD);
}

void Blocked() {
  for (int i = 0; i < motors_numb; i++) {
    motors[i].setSpeed(wheelStop);
    motors[i].run(RELEASE);
  }
}

void Forward() {
  for (int i = 0; i < motors_numb; i++) {
    motors[i].setSpeed(wheelSpeed);
    motors[i].run(FORWARD);
  }
}

void Backward() {
  for (int i = 0; i < motors_numb; i++) {
    motors[i].setSpeed(wheelSpeed);
    motors[i].run(BACKWARD);
  }
}

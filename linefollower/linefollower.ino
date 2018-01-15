#include "stack.c"


// wired connections
#define HG7881_B_IA 10 // D10 --> Motor B Input A --> MOTOR B +
#define HG7881_B_IB 9 // D11 --> Motor B Input B --> MOTOR B -

#define HG7881_A_IA 5 //D6 - Motor A input A
#define HG7881_A_IB 6 //D5 - Motor A input B

// functional connections
#define MOTOR_B_PWM HG7881_B_IA // Motor B PWM Speed
#define MOTOR_B_DIR HG7881_B_IB // Motor B Direction

#define MOTOR_A_PWM HG7881_A_IA // Motor B PWM Speed
#define MOTOR_A_DIR HG7881_A_IB // Motor B Direction



#define LSENSOR 8
#define RSENSOR 7


void forwardMotorA(int speed){
	digitalWrite(MOTOR_A_DIR, HIGH);
	analogWrite(MOTOR_A_PWM, 255-speed);
}

void reverseMotorA(int speed){
	digitalWrite(MOTOR_A_DIR, LOW);
	analogWrite(MOTOR_A_PWM, speed);
}

void forwardMotorB(int speed){
	digitalWrite(MOTOR_B_DIR, HIGH);
	analogWrite(MOTOR_B_PWM, 255-speed);
}

void reverseMotorB(int speed){
	digitalWrite(MOTOR_B_DIR, LOW);
	analogWrite(MOTOR_B_PWM, speed);
}

void stopMotorA(){
	digitalWrite(MOTOR_A_DIR, LOW);
	digitalWrite(MOTOR_A_PWM, LOW);
}

void stopMotorB(){
	digitalWrite(MOTOR_B_DIR, LOW);
	digitalWrite(MOTOR_B_PWM, LOW);
}

//Speeds and motor assignments should be checked empirically
void startTurnLeft(){
	reverseMotorB(120);
	forwardMotorA(120);
}

//same as above
void startTurnRight(){
	reverseMotorA(120);
	forwardMotorB(120);
}

//self explanatory
void stopAllMotors(){
	stopMotorA();
	stopMotorB();
}

/*
Returns an angle in degrees of the current direction measured by the onboard compass
*/
int readMagnetometer(){

}

/*
CONTROVERSY GOES HERE: a decision will always involve a turn. I believe this is a reasonable assumption and the 
function declaration matches the required protocol (void FUNCTION|(int arg)) in order to
be stackable. For a decision that involves going straight, an angle of 0 can be specified
*/
/*
DecisionTurn is just a wrapper for a turn that's special. This allows the processor to
pick it up (any pointer to this function is the same as any other pointer to this function).
The OTHER option will be pushed onto the stack when a decision is made.
*/
void decisionTurn(int angle){ 
	turn(angle);
}


/*
Angles will be clockwise, for any angle greater than 180, robot will turn to the left instead of the right.
*/
void turn(int angle){
	int originalAngle = readMagnetometer();
	int desiredNewAngle = originalAngle + angle; //calculate the angle we want to rotate until
	if (desiredNewAngle > 360) desiredNewAngle -= 360; //ie - 270 current, request rotation of 270 (90 degrees counter clockwise). (270 + 270) - 360 = 180. The new angle is 180 degrees, which is 90 degrees counter-clockwise of the original angle

	if(angle > 180){
		startTurnLeft();
	} else{
		startTurnRight();
	}

	while(readMagnetometer() != desiredNewAngle); //stick in this while loop and keep reading the magnetometer. We may want to add tolerance into this or stop a little early (the motors wind down and add a little bit extra)
	stopAllMotors();
}


/*
This should also deal with hitting walls/tape on the side. It may take some approximation for distance since bounding off walls may add a bit of distance
*/
void goStraight(int distance){

}



void setup() {
	//5,6,9.10 are outputs to the half bridge
	pinMode(MOTOR_A_DIR, OUTPUT);
	pinMode(MOTOR_A_PWM, OUTPUT);
	pinMode(MOTOR_B_DIR, OUTPUT);
	pinMode(MOTOR_B_PWM, OUTPUT);
	//7 and 8 are the IR inputs
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	Serial.begin(9600);
}

void loop() {

	//This is all old code to follow a line
	bool rightSensor = digitalRead(RSENSOR);
	bool leftSensor = digitalRead(LSENSOR);
	forwardMotorA(120);
	forwardMotorB(120);
	if (rightSensor == HIGH){
		stopMotorA();
	}
	if (leftSensor == HIGH){
		stopMotorB();
	}
	delay(10);
}

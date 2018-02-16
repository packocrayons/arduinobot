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


#define RIGHT_LFOLLOW_SENSOR_MASK 0b01000000
#define RIGHT_SENSOR_PIN 7
#define RIGHT_LFOLLOW_SENSOR_SHIFT 6
#define LEFT_LFOLLOW_SENSOR_MASK 0b10000000
#define LEFT_SENSOR_PIN 8
#define LEFT_LFOLLOW_SENSOR_SHIFT 7
#define LTURN_SENSOR_MASK 0b00100000
#define LTURN_SENSOR_SHIFT 5
#define FORWARD_SENSOR_MASK 0b00010000
#define FORWARD_SENSOR_SHIFT 4




char readSensors(){
//natey boy
}

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



void turnLeft(){
	//natey
}


/*
This should also deal with hitting walls/tape on the side. It may take some approximation for distance since bounding off walls may add a bit of distance
*/

void makeDecision(){
	if (sensors & LTURN_SENSOR_MASK){ //left turn sensor
		turnLeft()
	}


}


void goStraight(char sensors){
	//This is all old code to follow a line

	bool rightSensor = sensors & 0b01000000;
	bool leftSensor = sensors & 0b1000000;
	forwardMotorA(120);
	forwardMotorB(120);
	if (rightSensor == HIGH){
		stopMotorA();
	}
	if (leftSensor == HIGH){
		stopMotorB();
	}


}



void setup() {
	//5,6,9.10 are outputs to the half bridge
	pinMode(MOTOR_A_DIR, OUTPUT);
	pinMode(MOTOR_A_PWM, OUTPUT);
	pinMode(MOTOR_B_DIR, OUTPUT);
	pinMode(MOTOR_B_PWM, OUTPUT);
	//7 and 8 are the IR inputs
	pinMode(RIGHT_SENSOR_PIN, INPUT);
	pinMode(LEFT_SENSOR_PIN, INPUT);
	Serial.begin(9600);
}

void loop() {

	char sensors = readSensors();	
	makeDecision(sensors);

}

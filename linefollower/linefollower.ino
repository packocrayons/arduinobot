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

void forwardMotorB(int speed){
	digitalWrite(MOTOR_B_DIR, HIGH);
	analogWrite(MOTOR_B_PWM, 255-speed);
}

void stopMotorA(){
	digitalWrite(MOTOR_A_DIR, LOW);
	digitalWrite(MOTOR_A_PWM, LOW);
}

void stopMotorB(){
	digitalWrite(MOTOR_B_DIR, LOW);
	digitalWrite(MOTOR_B_PWM, LOW);
}



void setup() {
	// put your setup code here, to run once:
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

// wired connections
#define HG7881_B_IA 10 // D10 --> Motor B Input A --> MOTOR B +
#define HG7881_B_IB 9 // D11 --> Motor B Input B --> MOTOR B -

#define HG7881_A_IA 5 //D6 - Motor A input A
#define HG7881_A_IB 6 //D5 - Motor A input B

// functional connections
#define Motor_L_PWM HG7881_B_IA // Motor B PWM Speed
#define Motor_L_DIR HG7881_B_IB // Motor B Direction

#define Motor_R_PWM HG7881_A_IA // Motor A PWM Speed
#define Motor_R_DIR HG7881_A_IB // Motor A Direction


// #define RIGHT_LFOLLOW_SENSOR_MASK 0b01000000
// #define RIGHT_SENSOR_PIN 7
// #define RIGHT_LFOLLOW_SENSOR_SHIFT 6

// #define LEFT_LFOLLOW_SENSOR_MASK 0b10000000
// #define LEFT_SENSOR_PIN 8
// #define LEFT_LFOLLOW_SENSOR_SHIFT 7

// #define LTURN_SENSOR_MASK 0b00100000
// #define LTURN_SENSOR_PIN 11 //TODO Wiring dependant pin
// #define LTURN_SENSOR_SHIFT 5

// #define FORWARD_SENSOR_MASK 0b00010000
// #define FORWARD_SENSOR_PIN 12 //TODO Wiring dependant pin
// #define FORWARD_SENSOR_SHIFT 4

//this is increased for ultrasonic since we need to be able to slow down.
#define DEFAULT_SPEED 150 

//find this experimentally
#define ULTRASONIC_COLLISION 100 

//also should probbly be experimented with
#define FRONT_LEFT_TURN_HYSTERESIS 10

#define ONLY_RIGHT_HYSTERESIS 250


/*
	Sensor Layout

		    (3)

		(1)		(2)
		-----------
		|         |
Motor L |   Bot   | Motor R
		|         |
		-----------
*/


typedef struct ultraReading{
	int leftSensor;
	int rightSensor;
	int frontSensor;
} ultraReading;


ultraReading readSensors(){
	//ASSUMPTIONS 
	//	- there is no weighting (weighting will have to be found experimentally)
	//	- Closer to the wall means a higher number
//Sarah this is where the driver goes
}

void forwardMotorR(int speed){
	digitalWrite(Motor_R_DIR, HIGH);
	analogWrite(Motor_R_PWM, 255-speed);
}

void reverseMotorR(int speed){
	digitalWrite(Motor_R_DIR, LOW);
	analogWrite(Motor_R_PWM, speed);
}

void forwardMotorL(int speed){
	digitalWrite(Motor_L_DIR, HIGH);
	analogWrite(Motor_L_PWM, 255-speed);
}

void reverseMotorL(int speed){
	digitalWrite(Motor_L_DIR, LOW);
	analogWrite(Motor_L_PWM, speed);
}

void stopMotorR(){
	digitalWrite(Motor_R_DIR, LOW);
	digitalWrite(Motor_R_PWM, LOW);
}

void stopMotorL(){
	digitalWrite(Motor_L_DIR, LOW);
	digitalWrite(Motor_L_PWM, LOW);
}

//Speeds and motor assignments should be checked empirically
void startTurnLeft(){
	reverseMotorL(DEFAULT_SPEED);
	forwardMotorR(DEFAULT_SPEED);
}

//same as above
void startTurnRight(){
	reverseMotorR(DEFAULT_SPEED);
	forwardMotorL(DEFAULT_SPEED);
}

//self explanatory
void stopAllMotors(){
	stopMotorR();
	stopMotorL();
}



void turn(ultraReading sensors, bool left){
	stopAllMotors();
	delay(100);
	int oldFrontSensor = readSensors().forwardSensor;
	bool exit = 0;
	if (left){
		reverseMotorL(DEFAULT_SPEED);
		forwardMotorR(DEFAULT_SPEED);
		while(1) {
			int sensorsRight = readSensors().rightSensor;
			if (abs(sensorsRight - oldFrontSensor) < FRONT_LEFT_TURN_HYSTERESIS) break;
		}
	} else {
		reverseMotorR(DEFAULT_SPEED);
		forwardMotorL(DEFAULT_SPEED);
		while(1) {
			int sensorsLeft = readSensors().leftSensor;
			if (abs(sensorsLeft - oldFrontSensor) < FRONT_LEFT_TURN_HYSTERESIS) break;
		}
	}
	stopAllMotors();
}


/*
This should also deal with hitting walls/tape on the side. It may take some approximation for distance since bounding off walls may add a bit of distance
*/

void makeDecision(ultraReading sensors){
	if (sensors.frontSensor > ULTRASONIC_COLLISION){
		if ((leftSensor - rightSensor) > ONLY_RIGHT_HYSTERESIS){
			turn(sensors, false);
		}
		turn(sensors, true);
	} else{
		followLine(sensors);
	}


}


void followLine(char sensors){
	
	leftBias = sensors.leftSensor - sensors.rightSensor; //leftbias is how much closer to the left than we are to the right
	forwardMotorR(DERAULT_SPEED - leftBias); //if we're closer to the left, we want to slow down the right - remember leftBias can be negative if we're closer to the right
	forwardMotorL(DEFUALT_SPEED + leftBias); //opposite for the right

}



void setup() {
	//5,6,9.10 are outputs to the half bridge
	pinMode(Motor_R_DIR, OUTPUT);
	pinMode(Motor_R_PWM, OUTPUT);
	pinMode(Motor_L_DIR, OUTPUT);
	pinMode(Motor_L_PWM, OUTPUT);
	//7 and 8 are the IR inputs
	pinMode(RIGHT_SENSOR_PIN, INPUT);
	pinMode(LEFT_SENSOR_PIN, INPUT);
	Serial.begin(9600);
}

void loop() {

	

	makeDecision(sensors);


}
	
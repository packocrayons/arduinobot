// wired connections
#define HG7881_B_IA 10 // D10 --> Motor B Input A --> MOTOR B +
#define HG7881_B_IB 9 // D11 --> Motor B Input B --> MOTOR B -

//TODO Change pins to 3 and 4 to avoid arduino delay() interaction
#define HG7881_A_IA 5 //D6 - Motor A input A
#define HG7881_A_IB 6 //D5 - Motor A input B

// functional connections
#define Motor_L_PWM HG7881_B_IA // Motor B PWM Speed
#define Motor_L_DIR HG7881_B_IB // Motor B Direction

#define Motor_R_PWM HG7881_A_IA // Motor A PWM Speed
#define Motor_R_DIR HG7881_A_IB // Motor A Direction

// Constant to correct the H-Bridge power disparity
#define R_POWER 25

#define MOTOR_REDUCTION 50//45

/*
Written and tuning by Nathaniel Charlebois and Brydon Gibson

	NB: Proper indentation is corrupted by the atrocious Arduino IDE
*/


//Using bit fields would be a heavier but more readable implementation
/*


Alternate workflow
typedef enum {
LEFT = 0,
RIGHT = 1,
TURN = 3,
FORWARD = 4

} sensorMask;

//Beware system dependant endian-ness
union sensorField {
	struct {
	bool left:1;
	bool right:1;
	bool forward:1;
	bool leftTurn:1;
};
	char sensorChar;
};
*/

#define ANALOG_BATT_PIN 0
//the analog reading at which the battery is dead - this is a cell at ~3.4 volts
//#define BATTERY_DEAD 700
#define BATTERY_DEAD 0


#define RIGHT_LFOLLOW_SENSOR_MASK B00000001
#define RIGHT_SENSOR_PIN 4


#define LEFT_LFOLLOW_SENSOR_MASK B00000010
#define LEFT_SENSOR_PIN 3


#define LTURN_SENSOR_MASK B00000100
#define LTURN_SENSOR_PIN 8 //TODO Wiring dependant pin


#define RTURN_SENSOR_MASK B00010000
#define RTURN_SENSOR_PIN 7


#define FORWARD_SENSOR_MASK B00001000
#define FORWARD_SENSOR_PIN 11 //TODO Wiring dependant pin


#define WIN_CONDITION_MASK B00000111

#define DEFAULT_SPEED 130
#define START_SPEED 100

// TODO: THIS CONSTANT NEEDS TO BE TUNED
#define TURNAROUND_TIME 80
#define SAMPLE_HYSTERESIS 10



/*
Sensor Layout

(4)

(3)

(1)		(2)
-----------
|         |
Motor L |   Bot   | Motor R
|         |
-----------
*/


bool motorRRunning = false;
bool motorLRunning = true;
unsigned long forwardSamples = 0;

//Global that sets which motor primary motor last engaged in
// false = left
// true  = right
bool prevL = false;
//left == 1
bool path[] = {1,0,1,0,0};
int nextPathIndex = 0;

char readSensors(bool useHysteresis){
	/*
	Bitfield alternative workflow
	union sensorField sensors;
	sensors.left = digitalRead(LEFT_SENSOR_PIN);
	sensors.right = digitalRead(LEFT_SENSOR_PIN);
	*/
	char sensors = 0x00;


	if (digitalRead(LEFT_SENSOR_PIN)){
		sensors += LEFT_LFOLLOW_SENSOR_MASK;
	}
	if (digitalRead(RIGHT_SENSOR_PIN)){
		sensors += RIGHT_LFOLLOW_SENSOR_MASK;
	}
	if (digitalRead(LTURN_SENSOR_PIN)){
		sensors += LTURN_SENSOR_MASK;
	}
	if (digitalRead(RTURN_SENSOR_PIN)){
		sensors += RTURN_SENSOR_MASK;
	}
	if (useHysteresis){
		forwardSamples = forwardSamples << 1;
		unsigned long temp = digitalRead(FORWARD_SENSOR_PIN);
		forwardSamples += temp;
		//Serial.print("digitalRead: ");
		//Serial.println(temp);

		long samplesHigh = 0;
		for(int i = 0; i < sizeof(forwardSamples) * 8; i++){
			samplesHigh += ((1 << i) & forwardSamples) > 0 ? 1 : 0;
		}
		if(samplesHigh > SAMPLE_HYSTERESIS){
			sensors += FORWARD_SENSOR_MASK;
			digitalWrite(13, HIGH);
		} else {
			digitalWrite(13, LOW);
		}
	} else {
		if (digitalRead(FORWARD_SENSOR_PIN)){
			sensors += FORWARD_SENSOR_MASK;
		}
	}

	return sensors;
}

void checkBatteryVoltage(){
	Serial.print("Battery Voltage: ");
	Serial.println(analogRead(ANALOG_BATT_PIN));
	if (analogRead(ANALOG_BATT_PIN) < BATTERY_DEAD){
		stopAllMotors();
		while(1){
			digitalWrite(13, HIGH);
			delay(50);
			digitalWrite(13, LOW);
			delay(50);
		}
	}
}


void forwardMotorR(int speed){
  digitalWrite(Motor_R_DIR, HIGH);
  analogWrite(Motor_R_PWM, 255 - speed - R_POWER);
}

void reverseMotorR(int speed){
  digitalWrite(Motor_R_DIR, LOW);
  analogWrite(Motor_R_PWM, speed);
}

void stopMotorR(){
	digitalWrite(Motor_R_DIR, LOW);
	digitalWrite(Motor_R_PWM, LOW);
	motorRRunning = false;

}

void forwardMotorL(int speed){
  digitalWrite(Motor_L_DIR, HIGH);
  analogWrite(Motor_L_PWM, 255-speed);
}

void reverseMotorL(int speed){
  digitalWrite(Motor_L_DIR, LOW);
  analogWrite(Motor_L_PWM, speed);
}

void stopMotorL(){
	digitalWrite(Motor_L_DIR, LOW);
	digitalWrite(Motor_L_PWM, LOW);
	motorLRunning = false;
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


void win(){
	stopAllMotors();
	delay(5000);
	forwardMotorR(200);
	reverseMotorL(200);
	while(1){
		digitalWrite(13, HIGH); //blink the light so we know it's won
		delay(100);
		digitalWrite(13, LOW);
		delay(100);
	}
}


void turnAround(){
	forwardMotorL(DEFAULT_SPEED);
	reverseMotorR(DEFAULT_SPEED);
	delay(TURNAROUND_TIME);
	while(!(readSensors(false) & FORWARD_SENSOR_MASK)){//poll the front sensor until it hits the line
		delay(1);
	}
	stopAllMotors();
}

// TODO: Refactor turn
void turn(bool left){

	stopAllMotors();
  delay(2000);
  char mask;
  if(left){
    mask = RIGHT_LFOLLOW_SENSOR_MASK;
  } else {
    mask = LEFT_LFOLLOW_SENSOR_MASK;
  }

  char sensors = readSensors(false);

	if(sensors & FORWARD_SENSOR_MASK){
		//Turn until forward sensor is off the line
		do{
			sensors = readSensors(true);
			if (left){
				forwardMotorR(DEFAULT_SPEED);
        reverseMotorL(DEFAULT_SPEED);

			} else {
				forwardMotorL(DEFAULT_SPEED);
        reverseMotorR(DEFAULT_SPEED);

			}
		} while (sensors & FORWARD_SENSOR_MASK);
    //stopAllMotors();
    //delay(1000);
    Serial.println("STATE: F off the line");
	}


	do {
		sensors = readSensors(true);
		if (left){
			forwardMotorR(DEFAULT_SPEED);
      reverseMotorL(DEFAULT_SPEED);
		} else {
			forwardMotorL(DEFAULT_SPEED);
      reverseMotorR(DEFAULT_SPEED);
		}
	} while (!(sensors & FORWARD_SENSOR_MASK));
  //stopAllMotors();
  Serial.println("STATE: F found the line");
  //delay(1000);
  do {
    sensors = readSensors(false);
    if (left){
      forwardMotorR(DEFAULT_SPEED);
      reverseMotorL(DEFAULT_SPEED);
    } else {
      forwardMotorL(DEFAULT_SPEED);
      reverseMotorR(DEFAULT_SPEED);
    }
  } while (!(sensors & mask));
  //stopAllMotors();
  Serial.println("STATE: side found the line");
  //delay(1000);

}


/*
This should also deal with hitting walls/tape on the side. It may take some approximation for distance since bounding off walls may add a bit of distance
*/

void makeDecision(char sensors){
	/*  Reference
	*  Type B000TCTFF
	*  Loc  B000RCLLR
	*/
	if (sensors & LTURN_SENSOR_MASK){ //left turn sensor
		Serial.println("Reaching turnLeft");
    if(path[nextPathIndex]){
      nextPathIndex++;

		  turn(true);
    }
		return;
	} else if (sensors & RTURN_SENSOR_MASK){
		Serial.println("Reaching turnRight");
    if(!path[nextPathIndex]){
      nextPathIndex++;
		  turn(false);
    }
		return;
	} else if {
		win();
		return;
	} else {
	Serial.println("Reaching followLine");
	followLine(sensors);
	return;
	}
}

// Line Following
void followLine(char sensors){

  Serial.println("Following Line");
  bool rightSensor = sensors & RIGHT_LFOLLOW_SENSOR_MASK;
  bool leftSensor = sensors & LEFT_LFOLLOW_SENSOR_MASK;
  if (sensors & FORWARD_SENSOR_MASK){
    forwardMotorR(DEFAULT_SPEED);
    forwardMotorL(DEFAULT_SPEED);
  } else {
    do {
      sensors = readSensors(false);
      if (sensors & LEFT_LFOLLOW_SENSOR_MASK){
        forwardMotorR(DEFAULT_SPEED);
        reverseMotorL(DEFAULT_SPEED - 20);
      } else {
        forwardMotorL(DEFAULT_SPEED);
        reverseMotorR(DEFAULT_SPEED - 20);
      }
    } while (!(sensors & FORWARD_SENSOR_MASK));
  }
}



void setup() {
	//3,4,9,10 are outputs to the half bridge
	pinMode(Motor_R_DIR, OUTPUT);
	pinMode(Motor_R_PWM, OUTPUT);
	pinMode(Motor_L_DIR, OUTPUT);
	pinMode(Motor_L_PWM, OUTPUT);
	pinMode(13, OUTPUT);
	//7 and 8 are the IR inputs
	pinMode(RIGHT_SENSOR_PIN, INPUT);
	pinMode(LEFT_SENSOR_PIN, INPUT);
	pinMode(FORWARD_SENSOR_PIN, INPUT);
	pinMode(LTURN_SENSOR_PIN, INPUT);
	//Populates the hysteresis int so that the BOT starts in followLine()
	for(int i = 0; i < 30; i++){
		readSensors(true);
	}
	//Define Baud rate
	Serial.begin(9600);
  	forwardMotorL(DEFAULT_SPEED);
  	forwardMotorR(DEFAULT_SPEED);
  	delay(250);
}

void loop() {

	char sensors = readSensors(false);
	Serial.println(sensors,BIN);
  	Serial.print("nextTurn: ");
  	Serial.println(path[nextPathIndex]);
	makeDecision(sensors);
	//checkBatteryVoltage();

}

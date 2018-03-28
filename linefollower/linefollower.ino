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


//Using bit fields would be a heavier but more readable implementation
/*


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


#define RIGHT_LFOLLOW_SENSOR_MASK B00000001
#define RIGHT_SENSOR_PIN 7
#define RIGHT_LFOLLOW_SENSOR_SHIFT 6

#define LEFT_LFOLLOW_SENSOR_MASK B00000010
#define LEFT_SENSOR_PIN 8
#define LEFT_LFOLLOW_SENSOR_SHIFT 7

#define LTURN_SENSOR_MASK B00000100
#define LTURN_SENSOR_PIN 4 //TODO Wiring dependant pin
#define LTURN_SENSOR_SHIFT 5

#define FORWARD_SENSOR_MASK B00001000
#define FORWARD_SENSOR_PIN 3 //TODO Wiring dependant pin
#define FORWARD_SENSOR_SHIFT 4

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


char readSensors(){
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

  forwardSamples = forwardSamples << 1;
  unsigned long temp = digitalRead(FORWARD_SENSOR_PIN);
  forwardSamples += temp;
  Serial.print("digitalRead: ");
  Serial.println(temp);
  
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

  

  
	return sensors;
}

void forwardMotorR(int speed){
  /*
	if (!motorRRunning){
		digitalWrite(Motor_R_DIR, HIGH);
		analogWrite(Motor_R_PWM, 255-START_SPEED);	
		delay(1);
		motorRRunning = true;
	}
 */
	digitalWrite(Motor_R_DIR, HIGH);
	analogWrite(Motor_R_PWM, 255-speed);
}

void reverseMotorR(int speed){
  /*
	if (!motorRRunning){
		digitalWrite(Motor_R_DIR, LOW);
		analogWrite(Motor_R_PWM, START_SPEED);	
		delay(1);
		motorRRunning = true;
	}
 */
	digitalWrite(Motor_R_DIR, LOW);
	analogWrite(Motor_R_PWM, speed);
}

void stopMotorR(){
	digitalWrite(Motor_R_DIR, LOW);
	digitalWrite(Motor_R_PWM, LOW);
	motorRRunning = false;

}

void forwardMotorL(int speed){
  /*
	if (!motorLRunning){
		digitalWrite(Motor_L_DIR, HIGH);
		analogWrite(Motor_L_PWM, 255-START_SPEED);	
		delay(1);
		motorLRunning = true;
	}
 */
 
	digitalWrite(Motor_L_DIR, HIGH);
	analogWrite(Motor_L_PWM, 255-speed);
}

void reverseMotorL(int speed){
  /*
	if (!motorLRunning){
		digitalWrite(Motor_L_DIR, LOW);
		analogWrite(Motor_L_PWM, START_SPEED);
		delay(1);
		motorLRunning = true;
	}
 */
	digitalWrite(Motor_L_DIR, LOW);
	analogWrite(Motor_L_PWM, speed);
}

void stopMotorL(){
	digitalWrite(Motor_L_DIR, LOW);
	digitalWrite(Motor_L_PWM, LOW);
	motorLRunning=false;
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

}


void turnAround(){
	forwardMotorL(DEFAULT_SPEED);
	reverseMotorR(DEFAULT_SPEED);
	delay(TURNAROUND_TIME);
	while(!(readSensors() & FORWARD_SENSOR_MASK)){//poll the front sensor until it hits the line
		//delay(1);
	}
	stopAllMotors();
}

void turn(bool left){
	//forwardMotorR(DEFAULT_SPEED); //take a jump forward
	//forwardMotorL(DEFAULT_SPEED);
	//delay(10);
	stopAllMotors();
  char sensors = readSensors();
	bool exit = 0;
  delay(2000);
	//char sensors = readSensors();
	if(sensors & FORWARD_SENSOR_MASK){
		//Turn until forward sensor is off the line
		do{
			sensors = readSensors();
			if (left){
				forwardMotorR(DEFAULT_SPEED);
			} else {
				forwardMotorL(DEFAULT_SPEED);
			}
		} while (sensors & FORWARD_SENSOR_MASK);
	}

	do {
		sensors = readSensors();
		if (left){
			forwardMotorR(DEFAULT_SPEED);
		} else {
			forwardMotorL(DEFAULT_SPEED);
		}
	} while (!(sensors & FORWARD_SENSOR_MASK));

}


/*
This should also deal with hitting walls/tape on the side. It may take some approximation for distance since bounding off walls may add a bit of distance
*/

void makeDecision(char sensors){

	if (sensors & LTURN_SENSOR_MASK){ //left turn sensor    
	  Serial.println("Reaching turnLeft");
		turn(true);
    return;
	} else if (!(sensors & FORWARD_SENSOR_MASK)){
	  
    stopAllMotors();
    delay(10); //making an assumption that we'll coast to the line
    if (sensors & RIGHT_LFOLLOW_SENSOR_MASK){
      Serial.println("Reaching Turn Right");
      turn(false);
      
      return;
    } else {
      Serial.println("Reaching Turn Around");
      turnAround();
      return;
    }
	} else {
    Serial.println("Reaching followLine");
		followLine(sensors);
    return;
	}

 
	if (sensors & WIN_CONDITION_MASK){
		win();
    return;		
	}
}


void followLine(char sensors){
	//This is all old code to follow a line
  	Serial.println("Following Line");
	bool rightSensor = sensors & RIGHT_LFOLLOW_SENSOR_MASK;
	bool leftSensor = sensors & LEFT_LFOLLOW_SENSOR_MASK;
	forwardMotorR(DEFAULT_SPEED);
	forwardMotorL(DEFAULT_SPEED);
	if (rightSensor == HIGH){
		stopMotorR();
   delay(5);
	}
	if (leftSensor == HIGH){
		stopMotorL();
    delay(5);
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
	//Define Baud rate
	Serial.begin(9600);
}

void loop() {

	char sensors = readSensors();
	makeDecision(sensors);

}

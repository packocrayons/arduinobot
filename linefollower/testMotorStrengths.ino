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

//25 addition speed required to even out motor drivers
#define R_POWER 25

#define MOTOR_REDUCTION 40//45


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

#define DEFAULT_SPEED 120
#define START_SPEED 100

// TODO: THIS CONSTANT NEEDS TO BE TUNED
#define TURNAROUND_TIME 80
#define SAMPLE_HYSTERESIS 10

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
    //readSensors(true);
  }
  //Define Baud rate
  Serial.begin(9600);
  forwardMotorL(DEFAULT_SPEED);
  forwardMotorR(DEFAULT_SPEED + R_POWER);
  delay(250);

}

void loop() {
  forwardMotorL(DEFAULT_SPEED);
  forwardMotorR(DEFAULT_SPEED + R_POWER);

}

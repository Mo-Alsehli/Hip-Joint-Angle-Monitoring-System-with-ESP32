#include <Wire.h>
#include <ezBuzzer.h> // ezBuzzer library

/*
===================================================================
* Macros
===================================================================
*/

#define EXCEED_LIMIT_GYORA          (AngleRoll_gyroA <= rollLimit_gyroA || AnglePitch_gyroA >= pitchLimit_gyroA || AngleYaw_gyroA >= yawLimit_gyroA)
#define EXCEED_LIMIT_GYORB          (AngleRoll_gyroB <= rollLimit_gyroB || AnglePitch_gyroB >= pitchLimit_gyroB || AngleYaw_gyroB >= yawLimit_gyroB)

//#define BUZZER_WARNNING_GYROA       (AngleRoll_gyroA > (rollLimit_gyroA - 10) || AnglePitch_gyroA > (pitchLimit_gyroA - 5) || AngleYaw_gyroA > (yawLimit_gyroA - 3))
//#define BUZZER_WARNNING_GYROB       (AngleRoll_gyroA > (rollLimit_gyroB - 10) || AnglePitch_gyroB > (pitchLimit_gyroB - 5) || AngleYaw_gyroB > (yawLimit_gyroB - 3))


/*
===================================================================
* Global Variables
===================================================================
*/

// First Gyro Variables.
float RateRoll_gyroA, RatePitch_gyroA, RateYaw_gyroA;
float AccX_gyroA=0, AccY_gyroA=0, AccZ_gyroA=0;
float AngleRoll_gyroA, AnglePitch_gyroA, AngleYaw_gyroA;
float AngleYawCalibValue_gyroA = 0;

// Second Gyro Variables.
float RateRoll_gyroB, RatePitch_gyroB, RateYaw_gyroB;
float AccX_gyroB=0, AccY_gyroB=0, AccZ_gyroB=0;
float AngleRoll_gyroB, AnglePitch_gyroB, AngleYaw_gyroB;
float LoopTimer;
float AngleYawCalibValue_gyroB = 0;

// Variables to get yaw angle.
unsigned long currentTime_gyroA, prevTime_gyroA = 0;
float dt_gyroA;
unsigned long currentTime_gyroB, prevTime_gyroB = 0;
float dt_gyroB;

// Angles Limits:
const int rollLimit_gyroA = 5, pitchLimit_gyroA = -37, yawLimit_gyroA = 10;
const int rollLimit_gyroB = -10, pitchLimit_gyroB= -10, yawLimit_gyroB = 35;

// Buzzer Variables.
const int Buzzer_PIN = 4; // digital pin.

ezBuzzer buzzer(Buzzer_PIN); // create ezBuzzer object that attach to a pin;

// notes in the melody:
int melody[] = {
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_E5,  
};

// note durations: 4 = quarter note, 8 = eighth note, etc, also called tempo:
int noteDurations[] = {
  8, 8, 4,
  8, 8, 4,
    8, 8, 4,
  8, 8, 4,
};

/* int noteDurationsWarnning[] = {
  4, 4, 2,
  4, 4, 2,
  4, 4, 4,
  4, 4, 2,
  4, 4, 2,
  2, 4, 4
}; */

// Define states of the state machine for alarm.
enum State {
  NORMAL,
  ALARM_ACTIVE
};

// Initialize state variable
State currentState = NORMAL;



/*
===================================================================
* APIs
===================================================================
*/

// Function to Calibrate Gyro Angles.
void calibrateYawAngle(){
     for(int i = 0; i < 10000; i++){
       Second_gyro_signals();

       AngleYawCalibValue_gyroB += RateYaw_gyroB;        
     }

     AngleYawCalibValue_gyroB /= 10000;
    Serial.print("Calibration Value: ");
    Serial.print(AngleYawCalibValue_gyroB);
    delay(2000);
}


// Configure First Gyro and Initialize it.
void First_gyro_signals() {
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  // Configure Gyro Output
  Wire.beginTransmission(0x68);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  //Pull the accelerometer measurements from the sensor
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68,6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();  

  // Configure  the  gyroscope output and pull rotation rate measurements from the sensor
  Wire.beginTransmission(0x68);
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();  
  Wire.beginTransmission(0x68);
  Wire.write(0x43);
  Wire.endTransmission();

  // Extract the rotation Rates:
  Wire.requestFrom(0x68,6);
  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();

  RateRoll_gyroA=(float)GyroX/65.5;
  RatePitch_gyroA=(float)GyroY/65.5;
  RateYaw_gyroA=(float)GyroZ/65.5 + 0.9;  


  // Convert the Acc Measurments From LSB to G (Physical Values):
  AccX_gyroA=(float)AccXLSB/4096 - 0.03;
  AccY_gyroA=(float)AccYLSB/4096 + 0.01;
  AccZ_gyroA=(float)AccZLSB/4096 + 0.04;

  // Calculate the Absolute Angles.
  AngleRoll_gyroA=atan(AccY_gyroA/sqrt(AccX_gyroA*AccX_gyroA+AccZ_gyroA*AccZ_gyroA))*1/(3.142/180);
  AnglePitch_gyroA=-atan(AccX_gyroA/sqrt(AccY_gyroA*AccY_gyroA+AccZ_gyroA*AccZ_gyroA))*1/(3.142/180);

  // Calculate Yaw Angle:
  currentTime_gyroA = millis();
  dt_gyroA = (currentTime_gyroA - prevTime_gyroA) * 1e-3;
  prevTime_gyroA = currentTime_gyroA;

  AngleYaw_gyroA += RateYaw_gyroA * dt_gyroA;

}


// Configure Second Gyro and Initialize it.
void Second_gyro_signals() {
  Wire.beginTransmission(0x69);
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();
  // Configure Gyro Output
  Wire.beginTransmission(0x69);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  //Pull the accelerometer measurements from the sensor
  Wire.beginTransmission(0x69);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x69,6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read();
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();  

  // Configure  the  gyroscope output and pull rotation rate measurements from the sensor
  Wire.beginTransmission(0x69);
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();  
  Wire.beginTransmission(0x69);
  Wire.write(0x43);
  Wire.endTransmission();

  // Extract the rotation Rates:
  Wire.requestFrom(0x69,6);
  int16_t GyroX=Wire.read()<<8 | Wire.read();
  int16_t GyroY=Wire.read()<<8 | Wire.read();
  int16_t GyroZ=Wire.read()<<8 | Wire.read();

  RateRoll_gyroB=(float)GyroX/65.5;
  RatePitch_gyroB=(float)GyroY/65.5;
  RateYaw_gyroB=(float)GyroZ/65.5 + 0.06;  

  // Convert the Acc Measurments From LSB to G (Physical Values):
  AccX_gyroB=(float)AccXLSB/4096- 0.15;
  AccY_gyroB=(float)AccYLSB/4096- 0.04;
  AccZ_gyroB=(float)AccZLSB/4096+ 0.01;

  // Calculate the Absolute Angles.
  AngleRoll_gyroB  =  atan(AccY_gyroB/sqrt(AccX_gyroB *AccX_gyroB+AccZ_gyroB*AccZ_gyroB))*1/(3.142/180);
  AnglePitch_gyroB =  atan(AccX_gyroB/sqrt(AccY_gyroB*AccY_gyroB+AccZ_gyroB*AccZ_gyroB))*1/(3.142/180);

 // Calculate Yaw Angle:
  currentTime_gyroB = millis();
  dt_gyroB = (currentTime_gyroB - prevTime_gyroB) * 1e-3;
  prevTime_gyroB = currentTime_gyroB;

  AngleYaw_gyroB += RateYaw_gyroB * dt_gyroB;
}

void activateAlarm(){
   if (buzzer.getState() == BUZZER_IDLE) {
      int length = sizeof(noteDurations) / sizeof(int);
      buzzer.playMelody(melody, noteDurations, length); // playing
    }
}

void deactivateAlarm(){
      buzzer.stop() ; // stop
}

/* void activateWarningAlarm(){
      int length = sizeof(noteDurationsWarnning) / sizeof(int);
      buzzer.playMelody(melody, noteDurationsWarnning, length); // playing
}
 */
void waitGyroCalibrate(){
    digitalWrite(4, 1);
     delay(100);
    digitalWrite(4, 0);
     delay(600);
    digitalWrite(4, 1);
     delay(100);
     digitalWrite(4, 0);
     delay(600);
}



void setup(){
  Serial.begin(9600);
  Wire.setClock(400000);
  
  // For the ESP32, Wire.begin() defaults to:
  // GPIO 21 for SDA
  // GPIO 22 for SCL
  Wire.begin();
  delay(250);
  // Gyro-1
  Wire.beginTransmission(0x69);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
  // Gyro-2   
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  waitGyroCalibrate();

  prevTime_gyroA = millis(); 
  prevTime_gyroB = millis(); 
  //calibrateYawAngle();   
}


void loop() {
  buzzer.loop();
  First_gyro_signals();
  Second_gyro_signals();
  if(EXCEED_LIMIT_GYORA || EXCEED_LIMIT_GYORB){
    activateAlarm();    
  }else{
    deactivateAlarm();
  }

/*     case WARNNING:
      // Check if angles return to normal range
      if (!BUZZER_WARNNING_GYROA && !BUZZER_WARNNING_GYROB) {
        // Transition to NORMAL state
        currentState = NORMAL;
        // Deactivate alarm
        deactivateAlarm();
      } else if(EXCEED_LIMIT_GYORA || EXCEED_LIMIT_GYORB){
        // Transition to ALARM_ACTIVE state
        currentState = ALARM_ACTIVE;
         // Activate alarm
        activateAlarm();        
      } 
      break;*/

  // Add a delay to avoid rapid state transitions
  //delay(100);  // Adjust delay as needed
  
  Serial.print("\nRoll Angle: ");
  Serial.print(AngleRoll_gyroB); 
  Serial.print("    Pitch Angle: ");
  Serial.print(AnglePitch_gyroB);
  Serial.print("  Yaw Angle: ");
  Serial.print(AngleYaw_gyroB);  
  Serial.print("  Rate Yaw: ");
  Serial.print(RateYaw_gyroB);

  /* Serial.print("\nRoll Angle: ");
  Serial.print(AngleRoll_gyroA); 
  Serial.print("    Pitch Angle: ");
  Serial.print(AnglePitch_gyroA);
  Serial.print("  Yaw Angle: ");
  Serial.print(AngleYaw_gyroA);  
  Serial.print("  Rate Yaw: ");
  Serial.print(RateYaw_gyroA); */

  //delay(250);

}

















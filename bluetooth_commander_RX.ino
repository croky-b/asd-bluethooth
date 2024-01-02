
#include <SBUS.h>  
#include <Servo_MCPWM.h>   
#include <Filters.h>


#include <Filters/MedianFilter.hpp>        // MedianFilter

#define DEBUG 
#define COMMAND_RX  39

#define ANGLE_OFFSET 169
#define SWITCH_OFFSET 169
#define BOUTONS_OFFSET 1800

SBUS sBus(Serial2); // SBUS object on Serial 2 port
uint16_t SBUSchannels[16];
bool     SBUSfailSafe;
bool     SBUSlostFrame;
bool     sbusInit;


const uint8_t encoder_CH1_CH2 = 13; // encoder pin
const uint8_t encoder_CH3_CH4 = 12; // encoder pin
/*
float CH1X=0;
float CH2Y=0;

float CH3X=0;
float CH4Y=0;


float hyp_CH1_CH2=0;
float powhyp_CH1_CH2=0;
*/
float angle_L =0;


uint16_t powerESC_L;
uint8_t switch_Boutons;
uint8_t boutons;




float angle2_CH1_CH2 =0;
int16_t powerESC_CH1_CH2;

float hyp_CH3_CH4=0;
float powhyp_CH3_CH4=0;

float angle_CH3_CH4 =0;
float angle2_CH3_CH4 =0;

int16_t power_CH3_CH4;
int16_t powerESC_CH3_CH4;

Servo SERVO1;
Servo SERVO2;
Servo ESC1;
Servo ESC2;


MedianFilter<10, uint16_t> medfilt = {512};
MedianFilter<10, uint16_t> medfilt2 = {512};

void setup() {
disableCore0WDT();
disableCore1WDT();


  
 pinMode(COMMAND_RX, INPUT_PULLDOWN);
 
 SERVO1.attach(19, 1, 1, 0);
 SERVO2.attach(18, 1, 1, 1);
 ESC1.attach(32, 1, 2, 0);
 ESC2.attach(33, 1, 2, 1);

 // Task 1 setup (running on core 0)
  TaskHandle_t Task1;
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code,   /* Task function. */
    "Task1",     /* name of task. */
    100000,      /* Stack size of task (10000) */
    NULL,        /* parameter of the task */
    1,           /* priority of the task (1 = low, 3 = medium, 5 = highest)*/
    &Task1,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */

   

 sBus.begin(COMMAND_RX, false); 
 readSbusCommands();

Serial.begin(115200);
 
}


void readSbusCommands()
{
 
  if (sBus.read(&SBUSchannels[0], &SBUSfailSafe, &SBUSlostFrame))
  {
    sbusInit= true;
   }
}


//
// =======================================================================================================
// CORE 1
// =======================================================================================================
//
void loop() {

 readSbusCommands(); 
}

//
// =======================================================================================================
// CORE 2
// =======================================================================================================
//
void Task1code(void *pvParameters)
{
  for (;;)
  {
   
angle_L= map (SBUSchannels[9],172,ANGLE_OFFSET,0,359);
powerESC_L = map(SBUSchannels[8],172,1811,1500,2000);


switch_Boutons = ((SBUSchannels[11]- SWITCH_OFFSET)/6);
boutons= ((SBUSchannels[10]- BOUTONS_OFFSET)/6);


ESC1.writeMicroseconds(powerESC_CH1_CH2);

#ifdef DEBUG 
Debug();
#endif   
    }
  
}


//
// =======================================================================================================
// DEBUG
// =======================================================================================================
//

void Debug()
{

static unsigned long debugTime;

  if (millis() - debugTime > 300)     // Print the data every 300ms
  {
    debugTime = millis();

    Serial.println( "angle_L" );
    Serial.println( angle_L );
    Serial.println( "powerESC_L" );
    Serial.println( powerESC_L );
    Serial.println( "switch_Boutons" );
    Serial.println( switch_Boutons,BIN );
    Serial.println( "boutons" );
    Serial.println( boutons,BIN );

 
  }
}


//
// =======================================================================================================
//SERVO 360 code from aaedmusa
// =======================================================================================================
//

float GoTo_CH1_CH2(float setpoint){ // moves the servo to an input position
  float error_CH1_CH2=0;
  float out_CH1_CH2=0;
  

 
  float CW_error_CH1_CH2 = setpoint - getPos_CH1_CH2(); //calculates the error if moving CW
  if(CW_error_CH1_CH2 < 0){ //if CW_error is less than 0
    CW_error_CH1_CH2 = CW_error_CH1_CH2+360;
  }
  float CCW_error_CH1_CH2 = getPos_CH1_CH2()-setpoint; //calculates the error if moving CCW
  if(CCW_error_CH1_CH2 < 0){ //if CCW_error is less than 0
    CCW_error_CH1_CH2 = CCW_error_CH1_CH2+360;
  }

  // if CW_error is smaller than CCW_error (or both are equal) then error should be CW_error
  if(CW_error_CH1_CH2 < CCW_error_CH1_CH2 || CW_error_CH1_CH2 == CCW_error_CH1_CH2){ 
    error_CH1_CH2 = CW_error_CH1_CH2;
  }
  
  // if CCW_error is smaller then CW_error then make error CCW_error 
  else if(CCW_error_CH1_CH2 < CW_error_CH1_CH2 +5){ 
    error_CH1_CH2 = -1*CCW_error_CH1_CH2; //makes error negative
  }
  out_CH1_CH2 = 3*(error_CH1_CH2);
  out_CH1_CH2 = constrain(out_CH1_CH2, -255, 255); //constrains output to have maximum magnitude of 255 
  
  if(abs(out_CH1_CH2) <= 30){
    out_CH1_CH2 = 0;
  }
 
  if(out_CH1_CH2 > 0){ //if output is positive move CW
    SERVO1.writeMicroseconds(1350);
   
  }
  else if(out_CH1_CH2 < 0){ //if output is negative move CCW
    SERVO1.writeMicroseconds(1650);
   
  }
  else if(out_CH1_CH2 == 0){
    SERVO1.writeMicroseconds(1500);
   
  }

 


}
  


float getPos_CH1_CH2(){ // gets and returns encoder position
  // gets raw 10 bit position from the encoder and maps it to angle value
  // adds offset value

   
 float p_encoder_CH1_CH2= (medfilt(analogRead(encoder_CH1_CH2))*0.087890625);
 
 p_encoder_CH1_CH2 =map(p_encoder_CH1_CH2,360,0,0,360);
 
 uint16_t pos_CH1_CH2 = round (p_encoder_CH1_CH2);
   //corrects position if needed
  
  if (pos_CH1_CH2 < 0) {
    pos_CH1_CH2 = 359 + pos_CH1_CH2;
  }

  else if (pos_CH1_CH2 > 359) {
    pos_CH1_CH2 = pos_CH1_CH2 - 359;
  }
  return pos_CH1_CH2;
  
 
}



float GoTo_CH3_CH4(float setpoint){ // moves the servo to an input position
  float error_CH3_CH4=0;
  float out_CH3_CH4=0;
  

 
  float CW_error_CH3_CH4 = setpoint - getPos_CH3_CH4(); //calculates the error if moving CW
  if(CW_error_CH3_CH4 < 0){ //if CW_error is less than 0
    CW_error_CH3_CH4 = CW_error_CH3_CH4+360;
  }
  float CCW_error_CH3_CH4 = getPos_CH3_CH4()-setpoint; //calculates the error if moving CCW
  if(CCW_error_CH3_CH4 < 0){ //if CCW_error is less than 0
    CCW_error_CH3_CH4 = CCW_error_CH3_CH4+360;
  }

  // if CW_error is smaller than CCW_error (or both are equal) then error should be CW_error
  if(CW_error_CH3_CH4 < CCW_error_CH3_CH4 || CW_error_CH3_CH4 == CCW_error_CH3_CH4){ 
    error_CH3_CH4 = CW_error_CH3_CH4;
  }
  
  // if CCW_error is smaller then CW_error then make error CCW_error 
  else if(CCW_error_CH3_CH4 < CW_error_CH3_CH4 +5){ 
    error_CH3_CH4 = -1*CCW_error_CH3_CH4; //makes error negative
  }
  out_CH3_CH4 = 3*(error_CH3_CH4);
  out_CH3_CH4 = constrain(out_CH3_CH4, -255, 255); //constrains output to have maximum magnitude of 255 
  
  if(abs(out_CH3_CH4) <= 30){
    out_CH3_CH4 = 0;
  }
 
  if(out_CH3_CH4 > 0){ //if output is positive move CW
    SERVO2.writeMicroseconds(1350);
   
  }
  else if(out_CH3_CH4 < 0){ //if output is negative move CCW
    SERVO2.writeMicroseconds(1650);
   
  }
  else if(out_CH3_CH4 == 0){
    SERVO2.writeMicroseconds(1500);
   
  }
}

float getPos_CH3_CH4(){ // gets and returns encoder position
  // gets raw 10 bit position from the encoder and maps it to angle value
  // adds offset value

   
 float p_encoder_CH3_CH4= (medfilt2(analogRead(12))*0.087890625);
 
 p_encoder_CH3_CH4 =map(p_encoder_CH3_CH4,360,0,0,360);
 
 uint16_t pos_CH3_CH4 = round (p_encoder_CH3_CH4);
   //corrects position if needed
  
  if (pos_CH3_CH4 < 0) {
    pos_CH3_CH4 = 359 + pos_CH3_CH4;
  }

  else if (pos_CH3_CH4 > 359) {
    pos_CH3_CH4 = pos_CH3_CH4 - 359;
  }
  return pos_CH3_CH4;
  
 
}

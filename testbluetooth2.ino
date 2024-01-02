#include <SBusTx.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;


char caractere;
String mot;
String power;
String angle;
String power2;
String angle2;
String bouton;
String Switch;
String pwswitch;

int intPower;
int mapPower;
int intAngle;
int mapAngle;
int intPower2;
int intAngle2;
int intBouton;
int intSwitch;
int mapSwitch;
int intPwSwitch;

int separator;
int separator2;
int separator3;
int separator4;
int separator5;
int separator6;

void setup() {
  Serial.begin(115200);
  Serial2.begin(100000, SERIAL_8E2,23,23,false); /* Initialization with the right SBUS settings of the Serial UART used for the SBUS generator */
  SBusTx.serialAttach(&Serial2, SBUS_TX_NORMAL_TRAME_RATE_MS); /* Attach the SBUS generator to the Serial with SBUS_TX_NORMAL_TRAME_RATE_MS or SBUS_TX_HIGH_SPEED_TRAME_RATE_MS */
  SerialBT.begin("ESP32");
}

void loop() {


 static unsigned long SbusTime;

  if (millis() - SbusTime > 10)     // Print the data every 300ms
  {
    SbusTime = millis();

    mapPower = map (intPower,0,100,172,1800);
    mapAngle = map ( intAngle,0,360,172,1800);
    mapSwitch = map (intSwitch,0,255,202,1223);

    SBusTx.rawData(1, mapSwitch); /* Set the value to all the 16 SBUS channels */
    SBusTx.rawData(5, mapAngle); /* Set the value to all the 16 SBUS channels */
   
    SBusTx.sendChannels();               /* Send the SBUS frame  */

  
  }




  
  if(SerialBT.available())
  {
    caractere = SerialBT.read();  
    mot = mot + caractere; 

if (caractere == '*') {
      
      mot = mot.substring(0, mot.length() - 1); // Delete last char *
      separator = mot.indexOf(',');
      power = mot.substring(0, separator);
      separator2 = mot.indexOf(',', separator+1 );
      angle = mot.substring(separator+1, separator2);
      separator3 = mot.indexOf(',', separator2+1 );
      power2 = mot.substring(separator2+1, separator3);
      separator4 = mot.indexOf(',', separator3+1 );
      angle2 = mot.substring(separator3+1, separator4);
      separator5 = mot.indexOf(',', separator4+1 );
      bouton = mot.substring(separator4+1, separator5);
      separator6 = mot.indexOf(',', separator5+1 );
      Switch = mot.substring(separator5+1, separator6);
      pwswitch = mot.substring(separator6+1, -1);

      
       

      intPower = power.toInt();
      intAngle = angle.toInt();
      intPower2 = power2.toInt();
      intAngle2 = angle2.toInt();
      intBouton = bouton.toInt();
      intSwitch = Switch.toInt();
      intPwSwitch = pwswitch.toInt();
      mot = "";   

      if (intAngle<0){
        intAngle=(360+intAngle);
      }

       if (intAngle2<0){
        intAngle2=(360+intAngle2);
      }

      Serial.println("power");
      Serial.print(power);Serial.print(",");Serial.print(power2);
      Serial.print(mapPower);
      Serial.println("");
      Serial.println("angle");
      Serial.print(intAngle);Serial.print(",");Serial.print(intAngle2); Serial.println("");
      Serial.println("bouton");
      Serial.println(intBouton,BIN);
      Serial.println("Switch");
      Serial.println(intSwitch,BIN);
      Serial.println("PwSwitch");
      Serial.println(intPwSwitch,BIN);
  }
      }
}

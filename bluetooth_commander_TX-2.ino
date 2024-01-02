#include <SBusTx.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#define CODE_VERSION 1.0

#define DEBUG 


char caractere;
String mot;
String power_L;
String angle_L;
String power_R;
String angle_R;
String bouton;
String Switch;
String pwswitch;

int pwSwitchRawData;
int boutonx6;
int switchx6;
int intPower_L;
int mapPower_L;
int intAngle_L;
int mapAngle_L;
int intPower_R;
int mapPower_R;
int intAngle_R;
int mapAngle_R;
int intBouton;
int intSwitch;
int intPwSwitch;

int separator;
int separator2;
int separator3;
int separator4;
int separator5;
int separator6;

void setup() {
  
  Serial.begin(115200);
  Serial2.begin(100000, SERIAL_8E2,23,23,false); /*TX sur le 23 esp32 connecter au RX radiomaster  */
  SBusTx.serialAttach(&Serial2, SBUS_TX_NORMAL_TRAME_RATE_MS); /* Attach the SBUS generator to the Serial with SBUS_TX_NORMAL_TRAME_RATE_MS or SBUS_TX_HIGH_SPEED_TRAME_RATE_MS */
  SerialBT.begin("ESP32");
  Serial.print("bluetooth_commander_TX");Serial.print(",");Serial.print(CODE_VERSION);
    
}

void loop() {
  
#ifdef DEBUG 
Debug();
#endif 

sendSbus();
decodeBluetooth();


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

      Serial.println("power_L_R");
      Serial.print(power_L);Serial.print(",");Serial.print(power_R);Serial.print(",");
      Serial.print(mapPower_L);
      Serial.println("");
      Serial.println("angle_L_R");
      Serial.print(intAngle_L);Serial.print(",");Serial.print(intAngle_R); Serial.println("");
      Serial.println("bouton");
      Serial.println(boutonx6);
      Serial.println("Switch");
      Serial.println(intSwitch,BIN);
      Serial.println("PwSwitch");
      Serial.println(intPwSwitch,BIN);
  }
}

//
// =======================================================================================================
// DECODE BLUETOOTH
// =======================================================================================================
//

void decodeBluetooth()
{
if(SerialBT.available())
  {
    caractere = SerialBT.read();  
    mot = mot + caractere; 

  if (caractere == '*') {
      
      mot = mot.substring(0, mot.length() - 1); // Delete last char *
      separator = mot.indexOf(',');
      power_L = mot.substring(0, separator);
      separator2 = mot.indexOf(',', separator+1 );
      angle_L = mot.substring(separator+1, separator2);
      separator3 = mot.indexOf(',', separator2+1 );
      power_R = mot.substring(separator2+1, separator3);
      separator4 = mot.indexOf(',', separator3+1 );
      angle_R = mot.substring(separator3+1, separator4);
      separator5 = mot.indexOf(',', separator4+1 );
      bouton = mot.substring(separator4+1, separator5);
      separator6 = mot.indexOf(',', separator5+1 );
      Switch = mot.substring(separator5+1, separator6);
      pwswitch = mot.substring(separator6+1, -1);

      
       

      intPower_L = power_L.toInt();
      intAngle_L = angle_L.toInt();
      intPower_R = power_R.toInt();
      intAngle_R = angle_R.toInt();
      intBouton = bouton.toInt();
      intSwitch = Switch.toInt();
      intPwSwitch = pwswitch.toInt();
      mot = "";   

      if (intAngle_L<0){
        intAngle_L=(360+intAngle_L);
      }

      if (intAngle_R<0){
        intAngle_R=(360+intAngle_R);
      }
   }
 }
}
//
// =======================================================================================================
// SEND SBUS 
// =======================================================================================================
//

void sendSbus()
{
 static unsigned long sbusTime;

  if (millis() - sbusTime > 10)     
  {
    sbusTime = millis();

    mapPower_L =map(intPower_L,0,100,172,1811);
    mapAngle_L = map (intAngle_L,0,360,172,1811);
    
    mapPower_R =map(intPower_R,0,100,172,1811);
    mapAngle_R = map ( intAngle_R,0,360,172,1811);
    
    boutonx6=(172+(intBouton*6));
    switchx6=(172+(intSwitch*6));

    if(intPwSwitch){
      pwSwitchRawData=1811;
    }
    else{
      pwSwitchRawData=172;
    }


    SBusTx.rawData(1, mapPower_L); /* envois la valeur sur la voie ecolage 1*/
    SBusTx.rawData(2, mapAngle_L); /* envois la valeur sur la voie ecolage 2 */
    SBusTx.rawData(3, mapPower_R); /* envois la valeur sur la voie ecolage 3*/
    SBusTx.rawData(4, mapAngle_R); /* envois la valeur sur la voie ecolage 4 */
    SBusTx.rawData(5, boutonx6); /* envois la valeur sur la voie ecolage 5 */
    SBusTx.rawData(6, switchx6); /* envois la valeur sur la voie ecolage 6 */
    SBusTx.rawData(7, pwSwitchRawData); /* envois la valeur sur la voie ecolage 7 */
   
    SBusTx.sendChannels();               /* envois trame SBUS  */

  
  }
}

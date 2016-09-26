#include <SoftwareSerial.h>
 SoftwareSerial BTSerial(10, 11);//RX, TX
 int val;
 void setup()
 {
   Serial.begin(9600);
   BTSerial.begin(9600);
 }
 void loop()
 {
   val = Serial.read(); 
    if(val == '1') 
    {
            Serial.println("Play");
            BTSerial.print("pla,$");
    }
    else if(val == '2')
    {
            Serial.println("Stop");
            BTSerial.print("stop,$");
    }
    else if(val == '3')
    {
            Serial.println("pap,$");
            BTSerial.print("pap,$");
    }
    else if(val == 'R') 
    {
            Serial.println("Reset");
            BTSerial.print("reset,$");
    }
    else if(val == '4')
    {
            Serial.println("Next");
            BTSerial.print("next,$");
    }
    else if(val == '5')
    {
            Serial.println("Previous");
            BTSerial.print("previous,$");
    }
    else if(val == '6') 
    {
            Serial.println("Vol UP");
            BTSerial.print("vol+,$");
    }
    else if(val == '7') 
    {
            Serial.println("Vol DOWN");
            BTSerial.print("vol-,$");
    }
 }

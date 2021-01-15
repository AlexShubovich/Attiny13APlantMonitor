// **** INCLUDES *****
#include "LowPower.h"
#include <EEPROM.h>

// Pins
int led = 13;
int soilTrig = 10;
int lightTrig = 11;
int button = 2; //12

int soilListen = 0;
int lightListen = 1;

int lowerSoilValue = 0;
int lightValue = 0;
int soilValue = 0;
int time2 = 0;
int time3 = 0;
long voltage = 0;

void setup()
{
    pinMode(led, OUTPUT);
    pinMode(soilTrig, OUTPUT);
    pinMode(lightTrig, OUTPUT);
    pinMode(button, INPUT);
   Serial.begin(115200);
    lowerSoilValue = EEPROM.read(0) * 4 + EEPROM.read(1);

  //  Serial.print( "Lower Soil Value: " );
  //  Serial.println( lowerSoilValue );

}

void loop() 
{  
  time2 = micros();
   if (digitalRead(button) == HIGH){ 
    // Serial.println( "IN PROGRAM MODE" );
    while(digitalRead(button) == HIGH){
        digitalWrite(13,HIGH);
        delay(50);
        digitalWrite(13,LOW); 
       delay(50); 
      }
      while(digitalRead(button) == LOW){
        digitalWrite(13,HIGH);
        delay(50);
        digitalWrite(13,LOW); 
       delay(50); 
      }
      digitalWrite(soilTrig, HIGH);
      delay(1);
      int lowerSoilValue = analogRead(A1);
      digitalWrite(soilTrig, LOW); 
      int firstByte = lowerSoilValue / 4;
      int secondByte = lowerSoilValue % 4;
      EEPROM.write(0, firstByte);
      EEPROM.write(1, secondByte);
    //  Serial.println( "POGRAMMED OK" );
    } 
  
    voltage = readVcc();
  //  Serial.print( "Voltage: " );
  //  Serial.print( voltage );
  //  Serial.println( " MILIVOLTS" );
    
    digitalWrite(lightTrig, HIGH);
    delay(1);
    lightValue = analogRead(A0);
  //  Serial.print( "Light: " );
  //  Serial.println( lightValue );
    digitalWrite(lightTrig, LOW);
    if (lightValue < 500){
      digitalWrite(soilTrig, HIGH);
      delay(1);
      soilValue = analogRead(A1);
     // Serial.print( "Soil: " );
     // Serial.println( soilValue );
      digitalWrite(soilTrig, LOW);  
      if (soilValue < lowerSoilValue){
        
        digitalWrite(13,HIGH);
        delay(50);
        digitalWrite(13,LOW);  
        if(voltage < 2000){
          delay(50);
          digitalWrite(13,HIGH);
          delay(50);
          digitalWrite(13,LOW); 

        }
      }
      
    } else {
   // Serial.println( "zzzzz" );  
    }
    time3 = micros() - time2 ;
    Serial.println( "START" );
Serial.println( time3 ); 
Serial.println( "END");
    // Enter power down state for 8 s with ADC and BOD module disabled
   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  

    // Do something here
    // Example: Read sensor, data logging, data transmission.
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

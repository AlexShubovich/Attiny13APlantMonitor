/*
           _      ________   __ _____ _    _ _    _  _____ ____  __  __ 
     /\   | |    |  ____\ \ / // ____| |  | | |  | |/ ____/ __ \|  \/  |
    /  \  | |    | |__   \ V /| (___ | |__| | |  | | |   | |  | | \  / |
   / /\ \ | |    |  __|   > <  \___ \|  __  | |  | | |   | |  | | |\/| |
  / ____ \| |____| |____ / . \ ____) | |  | | |__| | |___| |__| | |  | |
 /_/    \_\______|______/_/ \_\_____/|_|  |_|\____(_)_____\____/|_|  |_|
                                                                               
        Minimal Plant Monitor by Alex Shu
        Watch Video at https://www.youtube.com/watch?v=KoVC7HvFjIk
*/

#include "LowPower.h"
#include <EEPROM.h>
#include "DigitalIO.h"

// Pins
const uint8_t led = 13;
const uint8_t soilTrig = 10;
const uint8_t lightTrig = 11;
const uint8_t button = 2;

// Variables
int lowerSoilValue = 0;
int lightValue = 0;
int soilValue = 0;
long voltage = 0;


void setup()
{
    // Configure pins
    fastPinMode(led, OUTPUT);
    fastPinMode(soilTrig, OUTPUT);
    fastPinMode(lightTrig, OUTPUT);
    fastPinMode(button, INPUT);

    // Get soil hydration value from EEPROM
    lowerSoilValue = EEPROM.read(0) * 4 + EEPROM.read(1);
}

void loop() 
{  
    // PROGRAM MODE
    if (fastDigitalRead(button) == HIGH){ 
    while(fastDigitalRead(button) == HIGH){
        fastDigitalWrite(13,HIGH);
        delay(50);
        fastDigitalWrite(13,LOW); 
        delay(50); 
      }
      while(fastDigitalRead(button) == LOW){
        fastDigitalWrite(13,HIGH);
        delay(50);
        fastDigitalWrite(13,LOW); 
        delay(50); 
      }
      // Get soil hydration reading
      fastDigitalWrite(soilTrig, HIGH);
      lowerSoilValue = analogRead(A1);
      lowerSoilValue = analogRead(A1);
      fastDigitalWrite(soilTrig, LOW); 
      
      // Write reading to EEPROM
      int firstByte = lowerSoilValue / 4;
      int secondByte = lowerSoilValue % 4;
      EEPROM.write(0, firstByte);
      EEPROM.write(1, secondByte);
    } // END OF PROGRAM MODE
      
    // Check if its day or night - double reading for stability
    fastDigitalWrite(lightTrig, HIGH);
    lightValue = analogRead(A0);
    lightValue = analogRead(A0);
    fastDigitalWrite(lightTrig, LOW);
    
    // Continue if its daytime
    if (lightValue < 500){
      // Check Soil hydration
      fastDigitalWrite(soilTrig, HIGH);
      soilValue = analogRead(A1);
      soilValue = analogRead(A1);
      fastDigitalWrite(soilTrig, LOW);  
      
      // Compare with the programmed value
      if (soilValue < lowerSoilValue){
        // If low -> blink once
        fastDigitalWrite(13,HIGH);
        delay(50);
        fastDigitalWrite(13,LOW);  
        
        // Get battery voltage
        voltage = readVcc(); 
        
        // Check if battery low
        if(voltage < 2000){
          // If battery low -> second blink
          delay(50);
          fastDigitalWrite(13,HIGH);
          delay(50);
          fastDigitalWrite(13,LOW); 
        }
      }
    }
   // Go to sleep for 8 seconds 
   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
}

// Function returns vcc rail voltage - aka secret voltmeter
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

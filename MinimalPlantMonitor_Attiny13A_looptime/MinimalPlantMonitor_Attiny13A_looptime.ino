// **** INCLUDES *****
#include <EEPROM.h>
#include <avr/sleep.h> //Needed for sleep_mode
#include <avr/wdt.h> //Needed to enable/disable watch dog timer
// Pins


int lowerSoilValue = 0;
//int soilValue = 0;
int voltage = 0;

void setup()
{
    pinMode(0, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(1, INPUT);
    //analogReference(INTERNAL);
    lowerSoilValue = EEPROM.read(0) * 4 + EEPROM.read(1);

  //  Serial.print( "Lower Soil Value: " );
  //  Serial.println( lowerSoilValue );
      set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Power down everything, wake up from WDT
  sleep_enable();

}

void loop() 
{  
  PORTB |= _BV(PB0); // on
     ADCSRA |= (1<<ADEN); //Enable ADC
     analogReference(INTERNAL);
    analogRead(A1);
    analogRead(A1);
    analogRead(A1);
    analogRead(A1);
    analogRead(A1);
  voltage = analogRead(A1) * 3;
  analogReference(DEFAULT);
      analogRead(A2);
      analogRead(A2);
      analogRead(A2);
     analogRead(A2);
     analogRead(A2);


   if (digitalRead(1) == HIGH){ 
    // Serial.println( "IN PROGRAM MODE" );
    while(digitalRead(1) == HIGH){
        PORTB |= _BV(PB3);
        delay(50);
        PORTB &= ~_BV(PB3); 
       delay(50); 
      }
      while(digitalRead(1) == LOW){
        PORTB |= _BV(PB3);
        delay(50);
        PORTB &= ~_BV(PB3); 
       delay(50); 
      }
      PORTB |= _BV(PB3);
      lowerSoilValue = analogRead(A2);
      lowerSoilValue = analogRead(A2);
        PORTB &= ~_BV(PB3);
     EEPROM.write(0, lowerSoilValue / 4);
      EEPROM.write(1, lowerSoilValue % 4);
    //  Serial.println( "POGRAMMED OK" );
    } 
  
  //  voltage = analogRead(A0) * (1100 / 1023.0) * (3/1.08);
  //  Serial.print( "Voltage: " );
  //  Serial.print( voltage );
  //  Serial.println( " MILIVOLTS" );
 
      PORTB |= _BV(PB3);
     // soilValue = analogRead(A2);
     // soilValue = analogRead(A2);
     // Serial.print( "Soil: " );
     // Serial.println( soilValue );
      
      if (analogRead(A2) < lowerSoilValue){
        
        PORTB |= _BV(PB3);
        delay(50);
        PORTB &= ~_BV(PB3);  
      if(voltage < 2000){
          delay(50);
          PORTB |= _BV(PB3);
          delay(50);
          PORTB &= ~_BV(PB3); 

        }
      }
      PORTB &= ~_BV(PB3);  
        ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
  setup_watchdog(9); //Setup watchdog to go off after 1sec
  sleep_mode(); //Go to sleep! Wake up 1sec later and check water

       wdt_disable(); //Turn off the WDT!!
    
  PORTB &= ~_BV(PB0); //off

}

void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Limit incoming amount to legal settings

  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDTIE); //Set the interrupt enable, this will keep unit from resetting after each int
}

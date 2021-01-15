#include <EEPROM.h>
#include <avr/sleep.h> //Needed for sleep_mode
#include <avr/wdt.h> //Needed to enable/disable watch dog timer

int lowerSoilValue = 0;
int voltage = 0;

void setup()
{
    // Setup Pins    
    DDRB = (1<<PB0);    //pinMode(0, OUTPUT);  
    DDRB = (1<<PB3);    // pinMode(3, OUTPUT);    
    DDRB = (0<<PB1);    //pinMode(1, INPUT);
    
    // Get Value from EEPROM
    lowerSoilValue = EEPROM.read(0) * 4 + EEPROM.read(1);

    // Sleep Setup
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Power down everything, wake up from WDT
    sleep_enable();

}

void loop() 
{  
     wdt_disable(); //Turn off the WDT!!
     ADCSRA |= (1<<ADEN); //Enable ADC
     
     // Battery Voltage Read
     // Switch Analog Reference to 1.1V Internal , take 5 readings that may be innacurate due to switching
    analogReference(INTERNAL);
    for(int i = 0; i<5; i++){ analogRead(A1); }
    voltage = analogRead(A1) * 3; // Final Reading
  
    // Switch back to default and take 5 readings that may be innacurate due to switching
    analogReference(DEFAULT);
    for(int i = 0; i<5; i++){ analogRead(A2); }

   // Programm Mode
    if (PINB & (1<<PB1)){ 
    while(PINB & (1<<PB1)){   // Blink until button released
        PORTB = (1<<PB0);
        delay(50);
        PORTB = (0<<PB0);
       delay(50); 
      }
      while(PINB & (1<<PB1)){  // Continue blinking until button pressed again to programm
        PORTB = (1<<PB0);
        delay(50);
        PORTB = (0<<PB0);
       delay(50); 
      }
      // Set pin 3 HIGH and take a double reading for accuracy, then set pin 3 LOW
      PORTB = (1<<PB3);
      lowerSoilValue = analogRead(A2);
      lowerSoilValue = analogRead(A2);
      PORTB = (0<<PB3);
      // Write values to EEPROM
      EEPROM.write(0, lowerSoilValue / 4);
      EEPROM.write(1, lowerSoilValue % 4);
    } // END of programm mode
  
       // Set pin 3 HIGH and take a double reading for accuracy
      PORTB = (1<<PB3);
      analogRead(A2);
      if (analogRead(A2) < lowerSoilValue){ // if reading is lower than progremmed value
        // Blink once
        PORTB = (1<<PB0);
        delay(50);
        PORTB = (0<<PB0); 
        
        // if voltage is low blink second time
        if(voltage < 2000){
            delay(50);
            PORTB = (1<<PB0);
            delay(50);
            PORTB = (0<<PB0);
          }
        }
      // Set pin 3 LOW  
      PORTB = (0<<PB3);  
      
      //Disable ADC, saves ~230uA
      ADCSRA &= ~(1<<ADEN); 
      
      // Sleep 
      setup_watchdog(9); //Setup watchdog to go off after 8sec
      sleep_mode(); //Go to sleep! 
}


//Sets the watchdog timer to wake us up, but not reset
//0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
//6=1sec, 7=2sec, 8=4sec, 9=8sec
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

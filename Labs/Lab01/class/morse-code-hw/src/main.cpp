#define LONG_DELAY 250
#define SHORT_DELAY 100
#include <avr/io.h>     // AVR device-specific IO definitions
#include <util/delay.h> // Functions for busy-wait delay loops
#include "Arduino.h"
#define Blick_led 8



int main(void)
{
  uint8_t led_value = HIGH; 
 // Set pin where on-board LED is connected as output
    pinMode(Blick_led, OUTPUT);

    // Infinite loop
    while (1)
    {
        // Generate a lettre `A` Morse code

    
      digitalWrite(Blick_led, LOW);
      _delay_ms(SHORT_DELAY);
      
     
      digitalWrite(Blick_led, HIGH);
      _delay_ms(LONG_DELAY);


   
      digitalWrite(Blick_led, LOW);
      _delay_ms(1000);

      digitalWrite(Blick_led, HIGH);
      _delay_ms(2000);




    }

    // Will never reach this
    return 0;
}



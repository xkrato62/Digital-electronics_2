/***********************************************************************
 * 
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * 
 **********************************************************************/


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions
#include <gpio.h>           // Use our gpio library
/*Define -------------------------------------------------------------*/
#define Js_button PD2       //PD2 is AVR pin where joystick button is connected
#define enc_SW_2  PB2       //second encoder SW (button) connect to pin PB2
#define enc_DT_2  PB3       //second encoder DT connect to pin PB3
#define enc_clk_2 PB4       //second encoder CLK connect to pin PB4
/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 100 ms.
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/

/*Global Para.--------------------------------------------------------*/
uint8_t LastState,current_state;          //global variable for encoder 2
int8_t counter = 0;                       // invalid value for counter = 0;

uint8_t Last_state_JS, current_state_JS;
int8_t rememb=0;

uint8_t Last_state_en2, current_state_en2;
int8_t rememb_enc2 = 0;

int main(void)
{
  


    // Initialize display
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(0, 0); lcd_puts("x:");
    lcd_gotoxy(0, 1); lcd_puts("y:");
    lcd_gotoxy(11, 1); lcd_puts("e:");       // na pozici counteru  = 13,1 asi na 10,1
    lcd_gotoxy(13, 1); lcd_puts("0");
    lcd_gotoxy(12, 0); lcd_puts("0");
    lcd_gotoxy(8, 1); lcd_puts("0");       // půjde na 8,1
    
    GPIO_mode_input_pullup(&DDRD,Js_button);    //declaration our Js_button port as input
    GPIO_mode_input_pullup(&DDRB,enc_SW_2);     //declaration second encoder's button as input pullup

    GPIO_mode_input_nopull(&DDRB,enc_DT_2);     //declaration second encoder's DT pin as input nopull
    GPIO_mode_input_nopull(&DDRB,enc_clk_2);    //declaration second encoder's CLK pin as input nopull
    LastState = GPIO_read(&PINB,enc_clk_2);     //put value on PB4 into variable LastState
    Last_state_JS = GPIO_read(&PIND,Js_button);
    Last_state_en2 =GPIO_read(&PINB,enc_SW_2);

      uint8_t customChar [8]= 
    {
        0b00000,
        0b11000,
        0b00111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00000,
    };

    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < 8; i++)  // Copy new character patterns line by line to CGRAM
    lcd_data(customChar[i]);
    lcd_command(1<<LCD_DDRAM);       // Set addressing back to DDRAM (Display Data RAM)
                                     // ie to character codes

    // Display symbol with Character code 0
    lcd_gotoxy(15, 0);
    lcd_putc(0x00);

    // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX = ADMUX | (1<<REFS0);
    // Select input channel ADC0 (voltage divider pin)
    ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
    // Enable ADC module
    ADCSRA = ADCSRA | (1<<ADEN);
    // Enable conversion complete interrupt
    ADCSRA = ADCSRA | (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA = ADCSRA | (1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0);

    // Configure 16-bit Timer/Counter1 to start ADC conversion
    // Set prescaler to 33 ms and enable overflow interrupt
    TIM1_overflow_33ms();
    TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();



    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Use Single Conversion mode and start conversion every 100 ms.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{

char string[4];
char letters[26] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };



    // Start ADC conversion
    ADCSRA = ADCSRA | (1<<ADSC);


/*Joystick Button---------------------------------------------------------------*/
//current_Js_state=GPIO_read(&PIND,Js_button);

current_state_JS = GPIO_read(&PIND,Js_button);

    if (current_state_JS!=Last_state_JS)
        {
            if (GPIO_read(&PIND,Js_button)==current_state_JS && current_state_JS==1)
            {
                        
                if (rememb==0)
                {
                    rememb++;
                }

                else 
                {
                    rememb--;
                }
            }

    itoa(rememb, string, 10);
    lcd_gotoxy(12, 0);
    lcd_puts("  ");    
    lcd_gotoxy(12, 0);
    lcd_puts(string);
        }

Last_state_JS=current_state_JS;


/*Encoder 2 button and putting value into display-----------------------------------*/

current_state_en2 = GPIO_read(&PINB,enc_SW_2);

    if (current_state_en2!=Last_state_en2)
        {
            if (GPIO_read(&PINB,enc_SW_2)==current_state_en2 && current_state_en2==1)
                {
                if (rememb_enc2<24)        
                    {
                            
                       rememb_enc2 ++;
                           
                    }
                else 
                {
                    rememb_enc2=0;
                }
                }
    itoa(rememb_enc2, string, 10);
    lcd_gotoxy(8, 1);
    lcd_puts("   ");    
    lcd_gotoxy(8, 1);
    lcd_puts(string);
        }
    Last_state_en2=current_state_en2;

    
    lcd_gotoxy(8, 0);
    lcd_putc(letters[rememb_enc2]); 
    


    //Encoder 2 putting value into our display
    current_state = GPIO_read(&PINB,enc_clk_2);

    if (current_state != LastState && current_state==1)
    {
        if(GPIO_read(&PINB,enc_DT_2) != current_state)
        {
            counter ++;
        }
        else
        {
            counter --;
        }

    itoa(counter, string, 10);
    lcd_gotoxy(13, 1);
    lcd_puts("    ");    
    lcd_gotoxy(13, 1);
    lcd_puts(string);
    }
    LastState=current_state;
}


/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display converted value on LCD screen.
 **********************************************************************/
ISR(ADC_vect)
{
    static uint8_t channel = 0;
    uint16_t x, y;
    char string[4];  // String for converted numbers by itoa()


    // Read converted value
    // Note that, register pair ADCH and ADCL can be read as a 16-bit value ADC
    if (channel == 0)
    {
        ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
        ADCSRA = ADCSRA | (1<<ADSC);
        x = ADC;
        // Convert "value" to "string" and display it
        itoa(x, string, 10);
        lcd_gotoxy(2, 0);
        lcd_puts("    ");
        lcd_gotoxy(2, 0);
        lcd_puts(string);
        channel++;
    }
    
    if (channel == 1)
    {
        ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1); ADMUX = ADMUX | (1<<MUX0);
        ADCSRA = ADCSRA | (1<<ADSC);
        y = ADC;
        // Convert "value" to "string" and display it
        itoa(y, string, 10);
        lcd_gotoxy(2, 1);
        lcd_puts("    ");
        lcd_gotoxy(2, 1);
        lcd_puts(string);
        channel =0;
    }
}
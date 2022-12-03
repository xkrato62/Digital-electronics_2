/***********************************************************************
 * Project1: Communication with multiple modules
 * 
 * Team members: Tomáš Kašpar, Dušan Kratochvíl
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
#define Js_button PD2       //PD2 is pin where joystick button is connected (SW)

#define enc_SW_2  PB2       //rotary-encoder SW (button) connect to pin PB2
#define enc_DT_2  PB3       //rotary encoder DT connect to pin PB3
#define enc_clk_2 PB4       //rotary encoder CLK connect to pin PB4

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: We want use every component including their buttons
 *           Joystick - display value of x,y axis
 *           Joystick button - display 0 and change 0 to 1 and on the contrary
 *           rotary encoder - spin value display e: ... (increse/decrease value) 
 *           rotary encoder's button - when press - increase from 0 to 24 and according to this value alphabet char is changing
 * 
 * Purpose:  Use Timer/Counter1 and start ADC conversion every 33 ms.
 *           Display value of our's components
 *           When AD conversion ends, send converted value to LCD screen.
 * Returns:  none
 **********************************************************************/

/*Global Parameters--------------------------------------------------------*/
uint8_t LastState,current_state;          //global variable for rotary encoder for saving it's state
int8_t counter = 0;                       // invalid value for counter = 0; Variable for counting

uint8_t Last_state_JS, current_state_JS;    //global variable for joystick button, so we can see last state
int8_t rememb=0;                            //invalid value for rememb =0; Varible for counting

uint8_t Last_state_en2, current_state_en2;  //global variable for rotary encoder value button; Save last state
int8_t rememb_enc2 = 0;                     //invalid value for rememb_enc2 =0 Variable for counting 

int main(void)
{
  
    // Initialize display
    lcd_init(LCD_DISP_ON);
    lcd_gotoxy(0, 0); lcd_puts("x:");       //put "x" char into position [0,0] on display
    lcd_gotoxy(0, 1); lcd_puts("y:");       //put "y" char into position [0,1] on display
    lcd_gotoxy(11, 1); lcd_puts("e:");      //put "e:" char into position [11,1] on display
    lcd_gotoxy(13, 1); lcd_puts("0");       //put "0" char into position [13,1] on display
    lcd_gotoxy(12, 0); lcd_puts("0");       //put "0" char into position [12,0] on display
    lcd_gotoxy(8, 1); lcd_puts("0");        //put "0" char into position [8,1] on display
    
    GPIO_mode_input_pullup(&DDRD,Js_button);    //set our Js_button port as input_pullup
    GPIO_mode_input_pullup(&DDRB,enc_SW_2);     //set out rotary encoder's button as input_pullup

    GPIO_mode_input_nopull(&DDRB,enc_DT_2);     //set rotary encoder's DT pin as input nopull
    GPIO_mode_input_nopull(&DDRB,enc_clk_2);    //set rotary encoder's CLK pin as input nopull
    LastState = GPIO_read(&PINB,enc_clk_2);     //read value of enc_clk and save into variable LastState
    Last_state_JS = GPIO_read(&PIND,Js_button); //save value of Js_button (joystick button) into variable Last_State_JS
    Last_state_en2 =GPIO_read(&PINB,enc_SW_2);  //save value of enc_SW_2 (button of rotary encoder) into variable Last_state_en2

/*Set our custom Char - VUT logo----------------------------------------*/
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
/*---------------------------------------------------------------------*/

    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < 8; i++)  // Copy new character patterns line by line to CGRAM
    lcd_data(customChar[i]);
    lcd_command(1<<LCD_DDRAM);       // Set addressing back to DDRAM (Display Data RAM)
                                     // ie to character codes

    // Display symbol with Character code 0 on position [15,0]
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

char string[4];     //we use it for save int value into string

// variable letters in which we save letters of alphabet, in next step, we index each of the letters
char letters[26] = {    
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };

    // Start ADC conversion
    ADCSRA = ADCSRA | (1<<ADSC);


/*Joystick Button------------------------------------------------------------*/
//When button is pressed, 0 change to 1 or 1 change to 0

current_state_JS = GPIO_read(&PIND,Js_button);      //save value of Js_button into variable current_state_JS

    if (current_state_JS!=Last_state_JS)
        {
            if (GPIO_read(&PIND,Js_button)==current_state_JS && current_state_JS==1)        //current_state_JS ==1 for change value +-1 (if not, there are bigger steps)
            {
                        
                if (rememb==0)  //if rememb save value 0, we incresed by 1
                {
                    rememb++;
                }

                else        //else we decreased by 1 [we assume there is 1]
                {
                    rememb--;
                }
            }

    //transform variable rememb into string and display it
    itoa(rememb, string, 10);
    lcd_gotoxy(12, 0);
    lcd_puts("  ");    
    lcd_gotoxy(12, 0);
    lcd_puts(string);
        }

Last_state_JS=current_state_JS;     //synchronize our states so we can detect another changes 


/*Rotary Encoder button and putting value into display-----------------------------------*/

current_state_en2 = GPIO_read(&PINB,enc_SW_2);      //save value of enc_SW_2 (button) into variable current_state_en

    if (current_state_en2!=Last_state_en2)          //if there is change..
        {
            if (GPIO_read(&PINB,enc_SW_2)==current_state_en2 && current_state_en2==1)   //if current value on enc_SW_2 is same as current_state_en2 than we decrease or increase rememb_enc2
                {
                if (rememb_enc2<24)        //24 becouse we are control by numbers of alphabet 
                    {
                            
                       rememb_enc2 ++;              //rememb_enc increased
                           
                    }
                else 
                {
                    rememb_enc2=0;                  //if there is value bigger than 24, we go back to value 0
                }
                }
    itoa(rememb_enc2, string, 10);              //put value of rememb_enc into string and display on our display
    lcd_gotoxy(8, 1);
    lcd_puts("   ");    
    lcd_gotoxy(8, 1);
    lcd_puts(string);
        }
    Last_state_en2=current_state_en2;       //synchronize states

    
    lcd_gotoxy(8, 0);                       
    lcd_putc(letters[rememb_enc2]);         //indexing char "letters" and according of value rememb_enc2 we change our output letter
    


    //Rotary Encoder getting value and putting value into our display
    current_state = GPIO_read(&PINB,enc_clk_2); //read value of enc_clk_2 and save it into current_state

    if (current_state != LastState && current_state==1)     //condition for change, if we detect change then if (true), current_state == 1 for change our value (increased/decreased) by 1 per cycle
    {
        if(GPIO_read(&PINB,enc_DT_2) != current_state)      //condition if there is value of enc_DT_2 different than current_state than if (true)
        {
            counter ++;
        }
        else
        {
            counter --;
        }
    //put value into string and display
    itoa(counter, string, 10);
    lcd_gotoxy(13, 1);
    lcd_puts("    ");    
    lcd_gotoxy(13, 1);
    lcd_puts(string);
    }
    LastState=current_state; //synchronize our's states
}


/**********************************************************************
 * Function: ADC complete interrupt, change ADMUX for (A0,A1)
 *           static uint8_t channel used for alternating A0,A1
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
        //set admux to basic value (A0)
        ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
       //anable ADC module
        ADCSRA = ADCSRA | (1<<ADSC);
        x = ADC;    //value from axis x
        // Convert "value" to "string" and display it
        itoa(x, string, 10);
        lcd_gotoxy(2, 0);
        lcd_puts("    ");
        lcd_gotoxy(2, 0);
        lcd_puts(string);
        channel++;      //set for read value of A1
    }
    
    if (channel == 1)
    {
        //set admux to read value from pin A1
        ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1); ADMUX = ADMUX | (1<<MUX0);
        //Anable ADC module
        ADCSRA = ADCSRA | (1<<ADSC);
        y = ADC;    //value from axis y
        // Convert "value" to "string" and display it
        itoa(y, string, 10);
        lcd_gotoxy(2, 1);
        lcd_puts("    ");   //for delete text which is on this position
        lcd_gotoxy(2, 1);
        lcd_puts(string);
        channel =0;     //for set position back to A0
    }
}
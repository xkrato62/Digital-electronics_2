/***********************************************************************
 *                             Project2
 * 
 * Precise generation of several PWM channels.
 * Application of two (or more) Servo motors SG90.
 * Author: Dušan Kratochvíl, Tomáš Kašpar
 * 
 *
***************************************************************************/

/* Includes libraries ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC 
#include <gpio.h>           // Use our gpio library
#include "timer.h"          // Timer library for AVR-GCC 
/*-------------------------------------------------------------------------------*/

/*------------------------define used pins--------------------------------------*/
#define servo1 PB1            // First servo, pin PB1 (pin D9 on arduino board)
#define servo2 PB2            // second servo, pin PB2 (pin D10 on arduino board)
#define MySwitch PB4          // Switch button, pin PB0 (pin D12 on arduino)
/*-------------------------------------------------------------------------------*/

/*----------------------------global parameters----------------------------------*/
uint8_t Last_state_SW, current_state_SW;    // global variable for switch button, so we can see last state
int8_t rememb=0;                            // invalid value for rememb =0; Varible for counting
/*--------------------------------------------------------------------------------*/

/* Define parameters for servo ---------------------------------------------------*/
# define zero 600                           // zero angle
# define max 1800                           // maximum angle

volatile uint32_t servo_position = zero;    // variable servo_position of servo 1,2 for scanning position


int main(void)
{
    /* ----------------------------- Timer for SERVO 1 ---------------------------------*/
    // Configure 8-bit Timer/Counter0 to control servo motors
    // Set prescaler to 1 ms and enable overflow interrupt
    TIM0_overflow_1ms();
    TIM0_overflow_interrupt_enable();

    /* ------------------------- Set pins with gpio library ----------------------------*/
    GPIO_mode_output(&DDRB, servo1);            // set servo1 pin as output
    GPIO_mode_output(&DDRB, servo2);            // set servo2 pin as output
    GPIO_mode_input_pullup(&DDRB,MySwitch);     // set MySwitch as input_pullup
    
    Last_state_SW = GPIO_read(&PINB,MySwitch); // save value of SW (switch button) into variable Last_State_SW

/*--------------------set PWM (instructions in datasheet) -------------------------------*/
    TCCR1A |= (1 << WGM11) | (1<<COM1A1) | (1<<COM1B1);                  
    TCCR1B |= (1 << WGM13);

    // Compare output
    TCCR1A |= (1 << COM0A1) | (1 << COM0B1); 

     //Set ICR1 (TOP) value
    ICR1 = 20000;
                         
 
    OCR1A = servo_position;
    OCR1B = servo_position;
    
    //Prescaler  64
    TCCR1B |= (1 << CS11); 
/*----------------------------------------------------------------------------------------*/

    PCICR |= (1<<PCIE0);        // Any change of any enable PCINT[7:0] pins will cause an interrupt                  
    PCMSK0|= (1<<PCINT4);       // Enable PCINT4 change interrupt

    // Enables interrupts by setting the global interrupt mask
    sei();
    
    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;

}

/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/1 overflow interrupt
 * Purpose:  Use single conversion mode and start conversion every 33 ms.
 **********************************************************************/

ISR(TIMER0_OVF_vect)
{
   
     /* ------------- get into default position, servo is not moving regularly------------------------------*/
   if (rememb == 0)                            // default value of button
    {
        servo_position = zero;                 // Set servo motor 1,2 into default position (zero angle)
        OCR1A = servo_position;                // Put value from variable into register OCR1A
        OCR1B = servo_position;                // Put value from variable into register OCR1B
    }

    /* ------------- First speed of servo motor  ------------------------------*/
   if (rememb == 1)                            // when switch button pressed once
    {
        if (servo_position<=max)               // until servo_position will not reach max angle, adds servo_position + 1
        {
            ++servo_position;
        } 

        if (servo_position==max)               // When servo possition reach max
        {
            for (servo_position;servo_position>=zero;servo_position--)  //for loop, decrease servo position until it reach value of zero
            {
                --servo_position;
            }
        }

        OCR1A = servo_position;                // Put value from variable into register OCR1A
        OCR1B = servo_position;                // Put value from variable into register OCR1B
    }
                    
/*--------------------faster servo-----------------------------------------*/
 if (rememb == 2)                              //When switch button is pressed for the second time
    {
         if (servo_position<=max)              // until servo_position will not reach max angle, adds servo_position + 2
        {
            servo_position +=2;
        } 

        if (servo_position==max || servo_position > max )  // When servo possition reach max, or when servo position is bigger than max - becouse sometimes servo possition get bigger than max, then simulation stop
        {
            for (servo_position;servo_position>=(zero-400);servo_position-=2)    //for loop, decrease servo position by 2 until it reach value of zero
            {
                servo_position-=2;
            }
        }

        OCR1A = servo_position;                // Put value from variable into register OCR1A
        OCR1B = servo_position;                // Put value from variable into register OCR1B
        }
}

/*************************************************************************************************************
 * Function: Change value on pin coused interrupt
 * Purpose:  For every push of button, there is incresed of rememb value until it resets into default 0 value
 *************************************************************************************************************/
ISR(PCINT0_vect)
{
    current_state_SW = GPIO_read(&PINB,MySwitch);           //save value of MySwtich into variable current_state_SW
    if (current_state_SW!=Last_state_SW)
        {
            if (GPIO_read(&PINB,MySwitch)==current_state_SW && current_state_SW ==1)    //current_state_SW ==1 for change value (we want increased by 1)
                {
                    if (rememb<2)       //if rememb value is lower than 2, there is incresed by one for every push
                        {
                            rememb++;
                        }
                    else                //if rememb value gets bigger, rememb value reset into value zero
                        {
                            rememb=0;
                        }  
                }
        }
    Last_state_SW = current_state_SW;   //synchronize our states so we can detect another changes
}

/* 
 * File:   main-button_leds.c
 * Author: jmorgan
 *
 * Created on July 6, 2014, 5:24 PM
 */


#include <nesi.h>

/*
 * This program starts with Red LEDs On at half power and Blue LEDs OFF.
 * When button is pressed, the condition changes to Red OFF and Blue ON at half power.
 */

int main(void)
{
       nesi.init();          // initialize all modules


    /*  Simple test program to make sure Software Development
     *  Environment is set up properly
     *  and code can be compiled, downloaded and executed on NESI+ Board
    */


    while(1)
    {
        if(button.isPressed())          // Turns red LED on halfway if the button is held down.
        {
            ledB.dutycycle(0);
            ledR.dutycycle(50);
        }

        else
        {
            ledB.dutycycle(50);      // If the button is not being held down, blue LED is halfway on.
            ledR.dutycycle(0);
        }

    }
    return 0;
}


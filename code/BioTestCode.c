/*
 * File:   BioTestCode.c
 * Author: Sam Cuthbertson
 *
 * Will test if samples  of bacteria are growing, as well as starting a new test on each reboot
 *  and storing all tests on the SD card.
 * (To test bateria this will need to get the light sensor value(super bright maybe) and store to SD)
 *
 *
 * Created on January 14, 2015, 8:14 AM
 */
#include <nesi.h>
#include <math.h>
#include "dateTime.h"
#define IDEAL_MSPR (83)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

//#define LIGHT_VALUE   (130)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (130)  // Value at which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (110) // Value at which the vial is refracting light onto the photoresistor

static int debug = 1; // Are we in debug mode?
static int dutycycle = 100; // Current dutycycle of the motor
int x = 0;
int record = 0;
static volatile Uint64 milisecondCount = 0; // Number of milliseconds since last vial

float getTemp(int);
int maintainSpeed(void);
int interval(void);
void logToScreen(String, int);


int main(void) {
    // Initialize all modules
    nesi.init();

    // Connect the USB COM interface
    usb.connect();

    logToScreen("*************************************************************",0); // Will help identify new tests

    T5CON = 0x0030;  // disabled, prescaled, internal oscillator
    TMR5 = 0;        // clear the timer

    PR5 = (Uint16)((CLOCKS_PER_SEC/256)/1000 );  // interrupt 1/2 second later

    _T5IE = ON;   // enable Timer5 to interrupt
    _T5IF = OFF;  // clear the interrupt flag
    _T5IP = 3;
    T5CONbits.TON = 1;// set to sub normal priority

    DateAndTime timeTemp;

    // time = 00:00:00
    timeTemp.hour   = 0;
    timeTemp.minute = 0;
    timeTemp.second = 0;

    dateTime.set(timeTemp); // Start dateTime
    
    while (1) { // Main Loop
        ledB.dutycycle(100);
        ledR.dutycycle(100); // Turn on the Amber LED

        /*
        * UNCOMMENT THIS LINE TO ENABLE CONSTANT RECORDING
        * record = 1;
        *
        */


        maintainSpeed(); // Hold centrifuge at speed

        timeTemp = dateTime.get(); // Get Current Time

        if(!record) { // We're not recording

            if(((timeTemp.second % 2) == 0)){ //&& (timeTemp.second < 3)){ // Is our time divisible by 5?

                //Start recording
                record = 1;
                logToScreen("New", 0);

            }
        } else { // We're recording

           if((timeTemp.second % 2)==1) { //We're out of that three second span

               //Stop recording
               record = 0;
               logToScreen("End", 0);

           }

       }

    }

    return 0;

}


int interval(void) {

    int recording = 0;

    while (1) {

        int PR = resistiveSensors.readQ2();
        delay(2);

        if (recording) {

            if (PR > DARK_VALUE || PR < SUPERLIGHT_VALUE) //Is there anything in the way?
            { //Yes - something in the way.

                logToScreen(" \r\n Light: %d", PR); //Log Light Level

            } else { //No - nothing in the way

                int inter = x - milisecondCount;
                milisecondCount = x;
                return inter; //Return the interval

            }

        } else { //Not recording

            if (PR > DARK_VALUE || PR < SUPERLIGHT_VALUE) { //Is there anything in the way?

                recording = 1;

            }

        }

    }

}

int maintainSpeed() {

    int inter = interval();

    //Speed up - Motor too slow.
    if (inter > (IDEAL_MSPR - SLUSH_MSPR)) {
        dutycycle = dutycycle + 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too slow interval:%d \r\n", dutycycle);
        return 0;
    }
    //Slow down - Motor too fast.
    if (inter < (IDEAL_MSPR + SLUSH_MSPR)) {
        dutycycle = dutycycle - 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too fast interval:%d \r\n", dutycycle);
        return 0;
    } else {
        //logToScreen("Maintaining speed. dc:%d \r\n", dutycycle);
        return 1;
    }
}

_ISR_ _T5Interrupt() {

    _T5IF = OFF;        //clear overflow flag
    x++;
}

float getTemp(int i) { // Forumla found by Nikil

    return 1.1;

}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (record) { //If we're recording

        dataLog.add(str, i);

    }

}

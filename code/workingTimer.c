*
 * File:   BioTestCode.c
 * Author: Mikayla Whiteaker
 *
 * test code to start a clock so that we can choose when to record data to teh SD card and when not to
 * once the expirment starts. Using the time we will record data for 3 seconds every two minutes
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

static int debug = 0; // Are we in debug mode?
static int dutycycle = 100; // Current dutycycle of the motor
int x = 0;
int record = 0;
static volatile Uint64 milisecondCount = 0; // Number of milliseconds since last vial

void logToScreen(String, int);


int main(void) {
    // Initialize all modules
    nesi.init();
    int record = 0;

    T5CON = 0x0030;  // disabled, prescaled, internal oscillator
    TMR5 = 0;        // clear the timer

    PR5 = (Uint16)((CLOCKS_PER_SEC/256)/1000 );  // interrupt 1/2 second later

    _T5IE = ON;   // enable Timer5 to interrupt
    _T5IF = OFF;  // clear the interrupt flag
    _T5IP = 3;
    T5CONbits.TON = 1;// set to sub normal priority


    DateAndTime timeTemp;

    // time = 00:00:00
    timeTemp.hour   = 0; //start a clock at time zero or the begining of the experiment
    timeTemp.minute = 0;
    timeTemp.second = 0;

    dateTime.set(timeTemp); // Start dateTime

    usb.connect();

    while (1) { // Main Loop




        timeTemp = dateTime.get(); // Get Current Time

        if(!record) { // We're not recording

            if(((timeTemp.minute % 5) == 0) && (timeTemp.second <= 3)){ // Is our time divisible by 5?

                //Start recording
                usb.print("logging == yay!\r\n");
                ledR.dutycycle(0); //tun the LED on to simulate recording data
                record = 1;
                logToScreen("New", 0);


            }
        }
        else { // We're recording

           if((timeTemp.second > 3 ) ) { //We're out of that three second span

               //Stop recording
             usb.print("not logging == cool!\r\n");
               ledR.dutycycle(100); //turn LED off to simulate not recording data
               record = 0;
               logToScreen("End", 0);
           }



           }




       

    }


    return 0;

}

_ISR_ _T5Interrupt() {

    _T5IF = OFF;        //clear overflow flag
    //x++;
}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (record) { //If we're recording

        dataLog.add(str, i);

    }
}








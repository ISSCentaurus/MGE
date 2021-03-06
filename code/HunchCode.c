/*
 * File:   HunchCode.c
 * Author: Sam Cuthbertson and Mikayla Whiteaker
 *
 * Will test efficiency of the motor through measuring dutycycle at a fixed speed, as well as starting a new test on each reboot
 *  and storing all test data on the SD card.
 *
 * Created on January 14, 2015, 8:14 AM
 */
#include <nesi.h>
#include <math.h>
#include "dateTime.h"
#include "lightLog.h"
#define IDEAL_MSPR (83)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

#define DARK_VALUE   (100)  // Value above which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (80) // Value below which the vial is refracting light onto the photoresistor
#define THAWVALUE (550) // Value from the thermistor at which we need to begin recording !(485)!

static int debug = 0; // Are we in debug mode? 1-Log to Serial 0-Log to SD
static int dutycycle = 100; // Current dutycycle of the motor
int x = 0; // Counter variable for intterupt 5
int record = 0; // Are we recording?
static volatile Uint64 milisecondCount = 0; // Number of milliseconds since last vial

double getTemp(void);
float tempInDegrees(double tempval);
int maintainSpeed(void);
int interval(void);
void logToScreen(String, int);
void logLight(String, int);

int main(void) {
    // Initialize all modules
    nesi.init();
    lightLog.init();
    
    // Connect the USB COM interface
    if (debug) {
        usb.connect();
    }

    logToScreen("*************************************************************", 0); // Will help identify new tests

    T5CON = 0x0030; // disabled, prescaled, internal oscillator
    TMR5 = 0; // clear the timer

    PR5 = (Uint16) ((CLOCKS_PER_SEC / 256) / 1000); // interrupt 1/2 second later

    _T5IE = ON; // enable Timer5 to interrupt
    _T5IF = OFF; // clear the interrupt flag
    _T5IP = 3;
    T5CONbits.TON = 1; // set to sub normal priority

    DateAndTime timeTemp;

    // time = 00:00:00
    timeTemp.hour = 0;
    timeTemp.minute = 0;
    timeTemp.second = 0;

    dateTime.set(timeTemp); // Start dateTime

    ledB.dutycycle(100); // Start the motor
    ledR.dutycycle(100); // Turn on the Amber LED
    
    int frozen = 0;  // Change to 1 to enable temperature check on startup
    
    while (frozen) {

        int tempval = getTemp();
        logToScreen("Temp: %d \r\n", tempval);
        if(tempval < THAWVALUE) { //We're going to need to start recording
            frozen = 0; //Get out of this loop, hop into the recording loop
            logToScreen(dateTime.getStamp(), 0); //Log the date
        }

    }

    while (1) { // Main Loop

        /*
         * UNCOMMENT THIS LINE TO ENABLE CONSTANT RECORDING
         * record = 1;
         *
         */


        maintainSpeed(); // Hold centrifuge at speed

        timeTemp = dateTime.get(); // Get Current Time

        if (!record) { // We're not recording

            if(timeTemp.second > 25) { //Safe to close lightLog
                
                lightLog.close();
                usb.connect();
                while(1){
                    ledB.dutycycle(0); //Turn off the motor as an audible cue for the fliers.
                }
                
            }
            
            if ((timeTemp.second < 20)) { //We just got plugged in!

                //Start recording
                record = 1;
                lightLog.open();
                logToScreen("NewSet *************************************************************************** \r\n", 0);

            }
        } else { // We're recording

            if (timeTemp.second > 20) { //We're out of that twenty second span

                //Stop recording
                record = 0;
                logToScreen("EndSet \r\n", 0);
                
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

                if (record) {
                    logToScreen("\r\n Light: %d", PR); //Log Light Level
                }

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
    if (inter > (IDEAL_MSPR + SLUSH_MSPR)) {
        dutycycle = dutycycle + 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too slow dc:%d \r\n", dutycycle);
        return 0;
    }
    //Slow down - Motor too fast.
    if (inter < (IDEAL_MSPR - SLUSH_MSPR)) {
        dutycycle = dutycycle - 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too fast dc:%d \r\n", dutycycle);
        return 0;
    } else {
        logToScreen("Maintaining speed. dc:%d \r\n", dutycycle);
        return 1;
    }
}

_ISR_ _T5Interrupt() {

    _T5IF = OFF; //clear overflow flag
    x++;
}

float tempInDegrees(double tempval) {
    
    return pow(28.41, (-0.05451*(3.3 - ((3.3 / 1024) * tempval)))/(3.3/10000)) + 3.120;
    
}

double getTemp() { 

    int vialFound = 0;
    
    while (!vialFound) {
        int PR = resistiveSensors.readQ2();
        delay(2);
        if (PR > DARK_VALUE || PR < SUPERLIGHT_VALUE) //Is there anything in the way?
        {

            ledB.dutycycle(0);
            vialFound = 1;
            return resistiveSensors.getQ1(10, 50);
            
        } else { // Nothing in the way

            ledB.dutycycle(100); // Spin slowly

        }
    }
    
    return 0.0;
}

void logLight(String str, int i) {
    
    if (debug) {

        usb.printf(str, i);

    } else if (record) { //If we're recording

        lightLog.add(str, i);

    }
    
}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (record) { //If we're recording

        lightLog.add(str, i);

    }

}

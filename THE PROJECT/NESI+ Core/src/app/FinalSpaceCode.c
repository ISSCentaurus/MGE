/*
 * File:   FinalSpaceCode.c
 * Authors: Sam Cuthbertson and Mikayla Whiteaker
 *
 * Will test if samples  of bacteria are growing, as well as starting a new test on each reboot
 *  and storing all tests on the SD card.
 *
 * Created on January 14, 2015, 8:14 AM
 */
#include <nesi.h>
#include <math.h>
#include "dateTime.h"
#include "lightLog.h"

#define IDEAL_MSPR (83)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds   

#define DARK_VALUE (120)  // Value above which the photoresitor is obstucted
//#define THAWVALUE (750) // Value from the thermistor at which we need to begin recording !(485)!

static int debug = 0; // Are we in debug mode? 1-Log to Serial 0-Log to SD
static int dutycycle = 100; // Current dutycycle of the motor
int x = 0; // Counter variable for interrupt 5
int record = 0; // Are we recording?
int ControlSample = 0; // Is it a control sample?

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

    record = 1;
    lightLog.open();
    logToScreen("*************************************************************", 0); // Will help identify new tests
    lightLog.close();
    record = 0;

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

    while (1) { // Main Loop

        /*
         * UNCOMMENT THIS LINE TO ENABLE CONSTANT RECORDING
         * record = 1;
         *
         */


        if (!ControlSample || record) {

            maintainSpeed(); // Hold centrifuge at speed

        }

        timeTemp = dateTime.get(); // Get Current Time

        if (!record) { // We're not recording

            if (((timeTemp.minute % 1) == 0) && (timeTemp.second <= 3)) {


                usb.disconnect();
                lightLog.open();

                //Start recording
                record = 1;
                logToScreen("NewSet ##### Temp:", resistiveSensors.readQ1());
                if (ControlSample) {
                    dutycycle = 100;
                    ledB.dutycycle(dutycycle);
                    logToScreen("Control Sample", 0);
                }
                ControlSample = 0;


            }

            /*if (((timeTemp.minute % 1) == 0) && (timeTemp.second > 6)) {

            }*/

        } else { // We're recording


            if (timeTemp.second > 3) { //We're out of that three second span

                //Stop recording
                logToScreen("EndSet ##### Temp:", resistiveSensors.readQ1());
                record = 0;
                if (ControlSample) {
                    ledB.dutycycle(0);
                }
                lightLog.close();
                usb.connect();
            }
        }

    }

    return 0;

}

int interval(void) {

    int recording = 0;
    int inter = 0;
    int vialLength = 0;
    while (1) {


        int PR = resistiveSensors.readQ2();
        delay(2);

        if (recording) {

            vialLength++;
            if (vialLength < 20) //Is there anything in the way?
            { //Yes - something in the way.

                if (PR > 512) { //Magic blacked out vial value (Frozen was ~290 max)
                    ControlSample = 1;
                }
                if (record) {
                    logToScreen("Light:", PR); //Log Light Level
                }

            } else {
                recording = 0;
                if (PR > DARK_VALUE) {
                    dutycycle = 100;
                }
                return inter;
            }

        } else { //Not recording

            if (PR > DARK_VALUE) { //Is there anything in the way?

                recording = 1;
                inter = x - milisecondCount;
                milisecondCount = x;

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
        logToScreen("Maintaining speed - was too slow dc:", dutycycle);
        logToScreen("Interval:", inter);
        return 0;
    }
    //Slow down - Motor too fast.
    if (inter < (IDEAL_MSPR - SLUSH_MSPR)) {
        dutycycle = dutycycle - 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too fast dc:", dutycycle);
        logToScreen("Interval:", inter);
        return 0;
    } else {
        logToScreen("Maintaining speed - was right speed dc:", dutycycle);
        logToScreen("Interval:", inter);
        return 1;
    }
}

_ISR_ _T5Interrupt() {

    _T5IF = OFF; //clear overflow flag
    x++;

    if (x > 100000000) {
        x = 0;
    }

}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (record) { //If we're recording

        lightLog.add(str, i);
    }
}

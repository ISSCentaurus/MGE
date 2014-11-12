/*
 * File:   testFinal.h
 * Authors: Sam Cuthbertson, Mikayla Whiteaker, Joe Sagrillo and Nate Weston
 *
 * Debug features activate when the button is pressed on boot - these will be
 *  removed in the final code.
 *
 * Created on November 9, 2014, 8:36 PM
 */

#include <nesi.h>
#include <math.h>

#define LIGHT_VALUE   (150)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (100)  // Value at which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (170)  // Value at which the vial is refracting light onto the photoresistor

#define IDEAL_MSPR (245)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

static int debug = 0; // Are we in debug mode?
static int dutycycle = 0; // Current dutycycle of the motor

float convertToC(int);
int checkSystems(void);
int maintainSpeed(void);
int waitUntilTemp(void);
int interval(void);
void logToScreen(String, int);

int main(void) {

    // Initialize all modules
    nesi.init();

    if (button.isPressed()) {
        debug = 1;
    }

    // Connect the USB COM interface - but only if we're in debug mode
    if (debug) {
        usb.connect();
    }

    while (1) {

        checkSystems();

        waitUntilTemp();

        while (1) {
            maintainSpeed();
        }

    }

    return 0;

}

int checkSystems() {
    ledB.dutycycle(0);
    ledR.dutycycle(100);

    //check photoresistor
    if (resistiveSensors.readQ2() >= LIGHT_VALUE && resistiveSensors.readQ2() < SUPERLIGHT_VALUE) {
        //Turn motor slowly to obstruct the photoresistor
        while (1) {
            ledB.dutycycle(100);
            wait(.1);
            ledB.dutycycle(0);
            if (resistiveSensors.readQ2() >= DARK_VALUE && resistiveSensors.readQ2() <= SUPERLIGHT_VALUE) {

                logToScreen("Had to spin motor to find middle of vial", 0);
                return 1;

            }
        }
    }

    //check if temp sensor is outputting data
    if (resistiveSensors.readQ1() > 1015) {
        logToScreen("Temperature sensor read error", 1);
        return 0;
    }
    logToScreen("Systems checked", 0);
    return (1);
}

int waitUntilTemp() {
    while (1) {
        //need to test for output a 4 degrees celsius
        if (resistiveSensors.getQ1(10, 5) >= 307) {
            logToScreen(sprintf("Temperature checked, was %d", resistiveSensors.readQ1()), 0);
            return (1);
        }

        logToScreen("Waiting to reach temp", 0);

    }

}

float convertToC(int i) // Forumla found by Brooks McDonald and Tray Guess
{
    //Convert to resistance
    return 70.815 - 20.33 * log(.33796 * pow(1.005989, i)); // Convert resistance value into temperature

}

int interval(void) {
    DateAndTime past = dateTime.get();
    int recording = 0;

    while (1) {
        if (recording) {

            if (resistiveSensors.readQ2() < LIGHT_VALUE) //Is there anything in the way?
            { //Yes - something in the way.

                logToScreen(sprintf("Light: %d at %s", resistiveSensors.readQ2(), dateTime.getStamp()), 3); //Log Light Level

            } else { //No - nothing in the way

                return (dateTime.sub(dateTime.get(), past).second)/1000; //Return the interval

            }

        } else { //Not recording

            if (resistiveSensors.readQ2() < LIGHT_VALUE) { //Is there anything in the way?

                recording = 1;
                past = dateTime.get();

            }

        }

    }

}

int maintainSpeed() {

    //speed up motor too slow
    if (interval() < (IDEAL_MSPR - SLUSH_MSPR)) {
        dutycycle = dutycycle + 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too fast", 0);
        return 0;
    }
    //slow down motor too fast
    if (interval() > (IDEAL_MSPR + SLUSH_MSPR)) {
        dutycycle = dutycycle - 1;
        ledB.dutycycle(dutycycle);
        logToScreen("Maintaining speed - was too slow", 0);
        return 0;
    } else {
        logToScreen("Maintaining speed.", 0);
        return 1;
    }
}

void logToScreen(String str, int i) {

    if (debug) {

        usb.print(str);
        usb.print("\r\n");

    } else if (i > 1) {

        dataLog.add(str, i);

    }

}
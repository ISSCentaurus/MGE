/*
 * File:   testFinal.h
 * Authors: Sam Cuthbertson, Mikayla Whiteaker, Joe Sagrillo and Nate Weston
 *
 * No debug features, just runs and stores data to the SD card.
 *
 * Created on November 9, 2014, 8:36 PM
 */

#include <nesi.h>
#include <math.h>

#define LIGHT_VALUE   (150)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (100)  // Value at which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (170)  // Value at which the vial is refracting light onto the photoresistor

#define DUTYCYCLE (0)   // Current dutycycle of the motor
#define IDEAL_MSPR (245)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

#define PAST DateAndTime    // Used for measuring interval
#define RECORDING (0)   // 1 or 0 for whether we're recording data or not.


float convertToC(int);
int checkSystems(void);
int maintainSpeed(void);
int temp(void);
int interval(void);

int main(void) {
    // Initialize all modules
    nesi.init();

    // Connect the USB COM interface
    usb.connect();

    while (1) {

        int val = resistiveSensors.getQ1(10, 50); // Read the tempurature sensor value
        int val2 = resistiveSensors.readQ2(); // Read the photoresitor value

        ledB.dutycycle(100); // Power the motor at full
        ledR.dutycycle(100); // Power the LED at full

        usb.printf("TempValue: %d Temp: %f Photo: %d \r\n", val, convertToC(val), val2); // Log all the data over USB

    }

    return 0;

}

int checkSystems(void) {
    ledB.dutycycle(0);
    ledR.dutycycle(100);

    //check photoresistor
    if (resistiveSensors.readQ2() >= LIGHT_VALUE && resistiveSensors.readQ2() < SUPERLIGHT_VALUE) {
        //Turn motor slowly to obstruct the photoresistor
        while (1) {
            ledB.dutycycle(100);
            wait(.01);
            ledB.dutycycle(0);
            if (resistiveSensors.readQ2() <= DARK_VALUE && resistiveSensors.readQ2() >= SUPERLIGHT_VALUE) {



            }
        }
    }

    //check if temp sensor is outputting data
    if (resistiveSensors.readQ1() > 1015) {
        dataLog.add(0x333, "Tempature sensor read error");
        return 0;
    }

}

int temp(void) {
    while (1) {
        //need to test for output a 4 degrees celsius
        if (resistiveSensors.getQ1(10, 5) = 37) {
            return 1;
        }
        else {
            return 0;
        }

    }

}

float convertToC(int i) // Forumla found by Brooks McDonald and Tray Guess
{
    //Convert to resistance
    return 70.815 - 20.33 * log(.33796 * pow(1.005989, i)); // Convert resistance value into temperature
}

int maintainSpeed() {

    //speed up motor too slow
    if (interval() < (IDEAL_MSPR - SLUSH_MSPR)) {
        DUTYCYCLE++;
        ledB.dutycycle(DUTYCYCLE);
        return 0;
    }
    //slow down motor too fast
    if (interval() > (IDEAL_MSPR + SLUSH_MSPR)) {
        DUTYCYCLE--;
        ledB.dutycycle(DUTYCYCLE);
        return 0;
    } else {
        return 1;
    }
}

int interval(void) {

    PAST = dateTime.get();

    while (1) {
        if (RECORDING) {

            if (resistiveSensors.readQ2() < LIGHT_VALUE) //Is there anything in the way?
            { //Yes - something in the way.

                DataLog.add(resistiveSensors.readQ2(), "light" + dateTime.getStamp); //Log Light Level
                delay(100);
            } else { //No - nothing in the way

                return (dateTime.sub(dateTime, PAST)); //Return the interval

            }

        } else { //Not recording

            if (resistiveSensors.readQ2() < LIGHT_VALUE) { //Is there anything in the way?

                RECORDING = 1;
                PAST = dateTime.get();

            } else {

                delay(1);

            }

        }

    }

}
/*
 * File:   usbDebug.c
 * Author: Sam Cuthbertson
 *
 * Created on November 6, 2014, 10:55 AM
 */

#include <nesi.h>
#include <math.h>

#define IDEAL_MSPR (83)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

#define LIGHT_VALUE   (200)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (500)  // Value at which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (70)  // Value at which the vial is refracting light onto the photoresistor

static int debug = 1; // Are we in debug mode?
static int dutycycle = 100; // Current dutycycle of the motor
int x = 0;
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

    /**
     * Overview
     *
     * Timer 5 will be used as the keystroke timeout timer. It will generate
     * an interrupt X milliseconds after it is started (@setTimeout(X)). The
     * timer will be started when a change in the button's state is detected,
     * and stopped automatically at the end of the timeout period. The keystroke
     * is determined by the number of times the button was pressed during the
     * timeout period.
     */

    /**
     * Timer5 Configuration Setup
     *
     * Timer5 will start off and be started once the button has been pressed
     * The prescaler will be set to 1:256 so that the period is of a reasonable
     * value and resolution. The internal system oscillator will be used.
     * Gating mode and synchronous operation will not be used.
     */
    T5CON = 0x0030;  // disabled, prescaled, internal oscillator
    TMR5 = 0;        // clear the timer

    /**
     * Timer5 Period Register Setup
     *
     * The period register contains the number of counts before Timer5 resets.
     * For 0.5s the count will be half that of the prescaled clocks per second.
     *
     * Note: The count begins at 0 (rather than 1), so 1 must be subtracted
     *       from the desired value.
     */
   PR5 = (Uint16)((CLOCKS_PER_SEC/256)/1000 );  // interrupt 1/2 second later

    /**
     * Timer5 Interrupt Setup
     *
     * Interrupts are required, so the interrupt enable bit must be set, and
     * the interrupt flag must be cleared. Also, the process has normal priority.
     */
    _T5IE = ON;   // enable Timer5 to interrupt
    _T5IF = OFF;  // clear the interrupt flag
    _T5IP = 3;
    T5CONbits.TON = 1;// set to sub normal priority

    while (1) {

        
        ledR.dutycycle(100);

        maintainSpeed();

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

                logToScreen("Light %d \r\n", PR); //Log Light Level

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
        logToScreen("Maintaining speed. dc:%d \r\n", dutycycle);
        return 1;
    }
}

_ISR_ _T5Interrupt() {

    _T5IF = OFF;        //clear overflow flag
    x++;
}

float getTemp(int i) { // Forumla found by Brooks Macdonald and Tray Guess

    return 70.815 - 20.33 * log(.33796 * pow(1.005989, i)); // Convert resistance value into temperature

}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (i > 1) {

        dataLog.add(str, i);

    }

}

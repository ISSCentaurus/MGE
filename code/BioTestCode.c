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

#define IDEAL_MSPR (83)   // Ideal Milliseconds per Revolution
#define SLUSH_MSPR (10) // Slush zone for Milliseconds per Revolution in Milliseconds

#define LIGHT_VALUE   (130)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (130)  // Value at which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (110)// Value at which the vial is refracting light onto the photoresistor
                                // super light will decrease as bacteria grows....


static int debug = 0; // Are we in debug mode?
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

    T5CON = 0x0030;  // disabled, prescaled, internal oscillator
    TMR5 = 0;        // clear the timer

   PR5 = (Uint16)((CLOCKS_PER_SEC/256)/1000 );  // interrupt 1/2 second later

    _T5IE = ON;   // enable Timer5 to interrupt
    _T5IF = OFF;  // clear the interrupt flag
    _T5IP = 3;
    T5CONbits.TON = 1;// set to sub normal priority

    // time = 12:52:50
    timeTemp.hour   = 12;
    timeTemp.minute = 52;
    timeTemp.second = 50;
    
    dateTime.set(timeTemp);
    
    
     
   




    while (1) {

       //if(getTemp(int i) = ?){
        ledR.dutycycle(100);

        maintainSpeed();

        timeTemp = dateTime.get()

        if((timeTemp.hour % 15) == 0){
            
        }
       //}


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

                logToScreen("Light: %d \r\n", PR); //Log Light Level

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
        //logToScreen("Maintaining speed - was too slow interval:%d \r\n", dutycycle);
        return 0;
    }
    //Slow down - Motor too fast.
    if (inter < (IDEAL_MSPR + SLUSH_MSPR)) {
        dutycycle = dutycycle - 1;
        ledB.dutycycle(dutycycle);
        //logToScreen("Maintaining speed - was too fast interval:%d \r\n", dutycycle);
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



}

void logToScreen(String str, int i) {

    if (debug) {

        usb.printf(str, i);

    } else if (i > 1) {

       //char buffer [33];
       //itoa (i,buffer,10);
       //file1.open("data.txt");
       //file1.write(str, i);
       //file1.close();''
       
        
        dataLog.add(str, i);
    }

}
/*
 * File:   usbDebug.c
 * Author: Sam Cuthbertson
 *
 * Created on November 6, 2014, 10:55 AM
 */

#include <nesi.h>
#include <math.h>

float getTemp(int);

int main(void)
{
    // Initialize all modules
    nesi.init();

    // Connect the USB COM interface
    usb.connect();

    while(1)
    {
     
        int val = resistiveSensors.getQ1(10,50);    // Read the tempurature sensor value
        int val2 = resistiveSensors.readQ2();   // Read the photoresitor value

        ledB.dutycycle(100);    // Power the motor at full
        ledR.dutycycle(100);    // Power the LED at full

        usb.printf("TempValue: %d Temp: %f Photo: %d \r\n", val, getTemp(val), val2);   // Log all the data over USB

    }

    return 0;
    
}

float getTemp(int i) {  // Forumla found by Brooks McDonald and Tray Guess

    return 70.815-20.33*log(.33796*pow(1.005989,i));    // Convert resistance value into temperature 

}
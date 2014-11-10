/*
 * File:   testFinal.h
 * Authors: Sam Cuthbertson, Mikayla Whiteaker, Joe Sagrillo and Nate Weston
 *
 * Created on November 9, 2014, 8:36 PM
 */

#include <nesi.h>
#include <math.h>

#define LIGHT_VALUE   (150)   // Value at which the photoresitor is unobstucted
#define DARK_VALUE   (100)  // Value at which the photoresitor is obstucted 

#define DUTYCYCLE (0) // Current dutycycle of the motor

float getTemp(int);
bool checkSystems(void);
bool maintainSpeed(void);
int interval(void);


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


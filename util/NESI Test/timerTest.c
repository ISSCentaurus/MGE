/*
 * File:   tempTest.c
 * Author: Sam Cuthbertson
 *
 * Created on November 6, 2014, 10:55 AM
 */

#include <nesi.h>
#include <math.h>

#define DARK_VALUE   (185)  // Value above which the photoresitor is obstucted
#define SUPERLIGHT_VALUE (80) // Value below which the vial is refracting light onto the photoresistor
#define THAWVALUE (550) // Value from the thermistor at which we need to begin recording !(485)!

float getTemp(int);

int main(void)
{
    // Initialize all modules
    nesi.init();

    // Connect the USB COM interface
    usb.connect();

    int frozen = 1;  // Change to 1 to enable temperature check on startup
    
    while (frozen) {

        int tempval = getTemp();
        usb.printf("Temp: %d \r\n", tempval);
        if(tempval < THAWVALUE) { //We're going to need to start recording
            frozen = 0; //Get out of this loop, hop into the recording loop
            usb.print(dateTime.getStamp()); //Log the date
        }

    }
    
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

            //ledB.dutycycle(100); // Spin slowly

        }
    }
    
    return 0.0;

}

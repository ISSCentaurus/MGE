/* 
 * File:   main-my_project.c
 * Author: 
 * Created on Month Day, Year, H:MM xM
 */

#include <nesi.h>

/*
 * This programs reads an analog value from RSQ1 Port (PhotoResistive sensor installed at JP3)
 * If the analog value is higher than a certain value (less light) the motor attached to PDA  will
 * begin rotating.  Increassing the ligght intensity will turn the motor off.
 */

int main(void)
{
 
    nesi.init();    // initialize all NESI modules
    Uint valQ1;     // declare varable to hold anaolg value

    while(1)
    {
        valQ1 = resistiveSensors.getQ1(5,100);      // take an average of 5 observations that are space 100ms apart
        if (valQ1 > 0x1ff)
        {
            powerDriverA.off();
        }
         else
        {
            powerDriverA.on();
        }
    }
     return(0);
}

 

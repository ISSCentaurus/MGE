/*
 * File:   main-my_project.c
 * Author:
 * Created on Month Day, Year, H:MM xM
 */

#include <nesi.h>
#include <uart2.h>
/*
 * This program reads the analog voltage output from the TMP36 temperature sensor
 * and outputs its value over UART to a PC when button is pressed
 */


int main(void)
{

    nesi.init();                // initialize all NESI modules
    uart2.init();               // UART 2 is ready to be used  Comm Port Pin 3 (TX), 4 (RX)
    uart2.baudrate(9600);       // Set baudrate to 9600
    
    double valQ4;               // Voltage read from RSQ4 Port
    double temperature;         // Holds Temperature in Celcius
    char message[80] = {0};    // Stores the message to be sent over UART
    
    while(1)
    {
        if(button.isPressed())
        {
            valQ4 = resistiveSensors.getQ4(5,20);        // Read 10-bit value representing voltage at RSQ4
            valQ4 = 3.3*valQ4/(double)1024;              // Converts 10 bit ADC value to a voltage
            temperature = (100*valQ4 - 50);              // Linear equation converts input voltage to degrees celcius
            sprintf(message,"Degrees Celcius: %.2f\r\n",temperature);   // Stores string into "message" and rounds temperature to 2 decimal places
            uart2.send(message, 30);    // If "bytes to send" is too high then it gets stuck in an infinate loop
            wait(300);                  //Pause .3 seconds before repeaating process
        }
    }
    return(0);
}




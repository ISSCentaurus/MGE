
/*  This program takes a pix and stores it when the button is pushed.
 * REMEMBER  -- jumper JP15 pins 1 and 2 for software control of camera
 * power and pins 2 and 3 for always on
*/

#include <nesi.h>


int main(void)
{

    nesi.init();            // Initializes NESI modules
    

    //prepare the usb connectivity
    usb.connect();
    wait(1000);

    while(1)
    {

    if (button.getStroke())
        {                
        // disable button keystroke detection
        button.disableStroke();
        
        // disconnect USB SD card (in case routine writes to SD memory)
        usb.eject();
        
        // take a picture 
        camera.getPix("my_picture.jpg");

        // reconnect USB SD card (to read files)
        usb.connect();

        // enable button keystroke detection
        button.enableStroke();

        //flash LED once
        // illuminate Red LED
        ledR.dutycycle(50);

        wait(1000);

        // lights off
        ledR.dutycycle(0);
        }

    }

    return 0;
}


 

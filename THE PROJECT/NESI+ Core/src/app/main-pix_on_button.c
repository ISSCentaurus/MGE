#include <nesi.h>
#include <string.h>

// stores timestamped picture
Boolean takePicture(void)
{
    char filename[32] = {0};
    sprintf(filename, "%s.jpg", dateTime.getStamp());

    // the ':' character is an invalid character, so it needs to be changed
    String temp = filename;
    while(*temp)
    {
        if(*temp == ':')
            *temp = '.';
        ++temp;
    }
    return camera.getPix(filename);
}

int main(void)
{
    // Initializes NESI modules
    nesi.init();

    // Set military time and Date
    DateAndTime timeTemp;

    // time = 12:52:50
    timeTemp.hour   = 16;
    timeTemp.minute = 0;
    timeTemp.second = 0;

    // date = 10/29/13
    timeTemp.month = NOVEMBER;
    timeTemp.day   = 25;
    timeTemp.year  = 13;

    dateTime.set(timeTemp);

    usb.connect();
    wait(1000);

    Keystroke key = KEYSTROKE_NONE;

    while(1)
    {
        // get keystroke
        key = button.getStroke();

        if(key == KEYSTROKE_SINGLE) // if button is tapped once
        {
            // disable button keystroke detection
            button.disableStroke();

            // disconnect USB SD card (in case routine writes to SD memory)
            usb.eject();

            // illuminate chamber
            ledB.dutycycle(50);
            ledR.dutycycle(50);

            char temp[128] = {0};
            if(takePicture()) // log if error occurred while attempting to take picture
                sprintf(temp,"Error taking picture at time: %s ", dateTime.getStamp());
            else
                sprintf(temp,"Successfully took picture at time: %s ", dateTime.getStamp());

            dataLog.add(temp, 0x1010);

            // lights off
            ledB.dutycycle(0);
            ledR.dutycycle(0);

            // reconnect USB SD card (to read files)
            usb.connect();

            // enable button keystroke detection
            button.enableStroke();
        }

    }

    return 0;
}

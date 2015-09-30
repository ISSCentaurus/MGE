/*************************** NESI Clear Spring Code for ISS ***************************
 *                                                                                    *
 * After initializing the NESI system the main will set the time (military) and date, *
 * power the camera, and connect the USB communication. Active USB communication      *
 * allows the on-board SD card to be read.                                            *
 *                                                                                    *
 * Once the above has completed a routine will start a 4-hour and a 12-hour process.  *
 * At every 4-hour mark the module will disconnect from the USB to allow writing the  *
 * the SD card. The camera will take a picture and save it to the SD card. A data log *
 * will occur and save to a text file on the SD card as well. The USB will reconnect. *
 *                                                                                    *
 * At each 12-hour mark the blue and red LEDs will alternate simulating day (red) and *
 * night (blue). Through each iteration of the while(1) loop, the checkWater function *
 * is called. The checkWater function keeps a constant moisture level and records the *
 * moisture inside the environment after each watering.                               *
 *                                                                                    *
 * Written by: Mickie Byrd and Colton Schimank                                        *
 *                                                                                    *
 **************************************************************************************/

#include <nesi.h>
#include <string.h>

void print(String message)
{
    dataLog.add(message, 0);
}

// returns FALSE if all systems are ok
Boolean checkSystems(void)
{
    // check solenoid
    if(!powerDriverA.isOk())
    {
        print("Error with Solenoid.");
        powerDriverA.init();
        if(!powerDriverA.isOk())
            return TRUE;
    }

    // check SD card
    if(!sdcard.isOk())
    {
        print("Error with SD card.");
        return TRUE;
    }

    // check camera
    if(!camera.isOk())
    {
        print("Error with camera.");
        return TRUE;
    }

    return FALSE;
}

void openSolenoid(void)
{
    powerDriverA.on();
}

void closeSolenoid(void)
{
    powerDriverA.off();
}

#define THRESHOLD_VOLTAGE  2.8
const Uint MIN_MOISTURE_VALUE = (1024/3.3) * THRESHOLD_VOLTAGE;

// Samples moisture in environment, adds appropriate water, takes data readings
void checkWater(void)
{
    int time = 0;  // watchdog counter
    while(1)
    {
        // take measurements
        unsigned valQ1 = resistiveSensors.getQ1(10,100); // 10 samples, 100ms between samples
        unsigned valQ2 = resistiveSensors.getQ2(10,100); // 10 samples, 100ms between samples
        unsigned valQ3 = resistiveSensors.getQ3(10,100); // 10 samples, 100ms between samples
        unsigned valQ4 = resistiveSensors.getQ4(10,100); // 10 samples, 100ms between samples

        // check if chambers have adequate moisture
        if((valQ1+valQ2+valQ3+valQ4)/4 < MIN_MOISTURE_VALUE)
            return;

        openSolenoid();
        wait(3000);
        closeSolenoid();

        usb.disconnect();
        wait(500);
        char message[64] = {0};
        sprintf(message, "Watered at %s ", dateTime.getStamp());
        dataLog.add(message, 0x1234);
        usb.connect();

        // prevents loop from running indefinitely
        if(time > 8){
            return;}
        time++;
    }
}

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

void logData(void)
{
    // take measurements
    unsigned valQ1 = resistiveSensors.getQ1(10,100); // get 10 samples, 100ms between samples
    unsigned valQ2 = resistiveSensors.getQ2(10,100); // get 10 samples, 100ms between samples
    unsigned valQ3 = resistiveSensors.getQ3(10,100); // get 10 samples, 100ms between samples
    unsigned valQ4 = resistiveSensors.getQ4(10,100); // get 10 samples, 100ms between samples
    unsigned valCO2 = co2.get(10,100);               // get 10 samples, 100ms between samples

    // log measurements
    char time[128] = {0};
    sprintf(time,"Time: %s ", dateTime.getStamp());
    dataLog.add(time, 0x4321);
    dataLog.add("Quadrant 1 = ", valQ1);
    dataLog.add("Quadrant 2 = ", valQ2);
    dataLog.add("Quadrant 3 = ", valQ3);
    dataLog.add("Quadrant 4 = ", valQ4);
    dataLog.add("CO2 Content = ", valCO2);
}

int main(void)
{
    // Initializes NESI modules
    nesi.init();

    // Set military time and Date
    DateAndTime timeTemp;

    // time = 12:52:50
    timeTemp.hour   = 12;
    timeTemp.minute = 52;
    timeTemp.second = 50;

    // date = 10/29/13
    timeTemp.month = NOVEMBER;
    timeTemp.day   = 17;
    timeTemp.year  = 13;

    dateTime.set(timeTemp);

    Boolean taken = NO;

    //checkSystems();

    usb.connect();
    wait(1000);

    while(1)
    {
        timeTemp = dateTime.get();

        // take picture every 4 hours
        if((timeTemp.hour % 4) == 0)
        {
            if(taken == NO) // only take picture once during the 4 hour block
            {
               taken = YES;

                usb.disconnect();
                wait(500);
                if(takePicture()) // log if error occurred while attempting to take picture
                {
                    char temp[128] = {0};
                    sprintf(temp,"Error taking picture at time: %s ", dateTime.getStamp());
                    dataLog.add(temp, 0x1010);
                }
                wait(2000);
                logData();
                wait(500);
                usb.connect();
            }
        }
        else
            taken = NO; // reset image flag after 4 hour period has passed

        // cycle lights every 12 hours
        if(timeTemp.hour < 12)
        {
            ledB.dutycycle(0);
            ledR.dutycycle(100);
        }
        else
        {
            ledB.dutycycle(100);
            ledR.dutycycle(0);
        }

        //  monitor the water continually
        checkWater();
    }

    return 0;
}

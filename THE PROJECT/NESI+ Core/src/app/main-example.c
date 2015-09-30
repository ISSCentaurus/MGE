/*
 * File:   example_main.c
 * Author: MISLmen
 *
 * Created on November 27, 2013, 9:33 AM
 */

#include <nesi.h>
#include <string.h>

void init(void)
{
    nesi.init();
}

void check_systems(void)
{
    if(!powerDriverA.isOk())
    {
        dataLog.add("Error with Solenoid", 79);
    }
    if(!camera.isOk())
    {
        dataLog.add("Error with Camera", 52);
    }
}

void light_cycle(Boolean redOn)
{
    if(redOn == TRUE)
    {
        ledR.dutycycle(50);
        ledB.dutycycle(0);
    }
    else
    {
        ledR.dutycycle(0);
        ledB.dutycycle(50);
    }
}

void openSolenoid(void)
{
    powerDriverA.on();
}

void closeSolenoid(void)
{
    powerDriverA.off();
}

void water_plant(void)
{
    Uint MIN_MOISTURE_VALUE = 0x1234;

    while(1)
    {
        // take measurements
        Uint valQ1 = resistiveSensors.readQ1();
        Uint valQ2 = resistiveSensors.readQ2();
        Uint valQ3 = resistiveSensors.readQ3();
        Uint valQ4 = resistiveSensors.readQ4();

        // check water level
        Uint average = (valQ1 + valQ2 + valQ3 + valQ4) / 4;

        // if ok, return
        if(average < MIN_MOISTURE_VALUE)
        {
            return;
        }

        // else add water
        else
        {
            openSolenoid();
            wait(3000);
            closeSolenoid();
        }

        usb.disconnect();
        wait(500);
        char message[32] = {0};
        sprintf(message, "Watered the plants at: %s", dateTime.getStamp());
        dataLog.add(message, 48);
        usb.connect();

    }
}

void store_values(void)
{
    // take measurements
    Uint valQ1 = resistiveSensors.readQ1();
    Uint valQ2 = resistiveSensors.readQ2();
    Uint valQ3 = resistiveSensors.readQ3();
    Uint valQ4 = resistiveSensors.readQ4();
    Uint valCO2 = co2.read();

    //log measurments
    char time[128] = {""};
    sprintf(time, "Time: %s", dateTime.getStamp());
    dataLog.add(time, 0);
    dataLog.add("Quadrant 1 = ", valQ1);
    dataLog.add("Quadrant 2 = ", valQ2);
    dataLog.add("Quadrant 3 = ", valQ3);
    dataLog.add("Quadrant 4 = ", valQ4);
    dataLog.add("CO2 Content = ", valCO2);
}

void take_picture(void)
{
    char filename[32] = {""};
    sprintf(filename, "%s.jpg", dateTime.getStamp());
    camera.getPix(filename);
}

/*
 * Controls NESI Experiment
 */
int main(void)
{
    init();

    DateAndTime timeTemp;

    // time = 11:33:25
    timeTemp.hour = 11;
    timeTemp.minute = 33;
    timeTemp.second = 25;

    // date = 11/27/2013
    timeTemp.month = NOVEMBER;
    timeTemp.day = 27;
    timeTemp.year = 13;

    dateTime.set(timeTemp);

    check_systems();

    usb.connect();

    while(1)
    {
        timeTemp = dateTime.get();

        if((timeTemp.hour % 4) == 0)
        {
            usb.disconnect();
            store_values();
            take_picture();
            usb.connect();
        }

        if(timeTemp.hour < 12)
        {
            light_cycle(TRUE); // red lights on
        }
        else
        {
            light_cycle(FALSE); // blue lights on
        }

        water_plant();
    }

    return 0;
}


/**
 * NESI MG811 (CO2 Gas Sensor) Driver License
 * Copyright (C) 2013 Mickie Byrd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Change Log
 *
 * 09/25/2013 - Dakotah Karrer
 *   Modified turnOn() and turnOff() and initialize() since 6volt control was
 *   moved to the system module.
 * 09/26/2013 - Dakotah Karrer
 *   Added readTest() to determine if a hardware failure has occurred with the
 *   heater.
 * 12/02/13 - Dakotah Karrer
 *   Modified initialize() to turn on heater power instead of being turned off.
 */

#include "system.h"
#include "co2.h"

/**
 * readValue() Information
 *
 * This function samples the sensor once and returns the raw 10-bit value.
 */
static Uint16 readValue(void)
{
    AD1CON1bits.ADON = ON;     // Turn the A/D conversion module on

    ADC1BUF0 = 0;              // clear the result buffer
    AD1CHS0 = 4;               // set channel to sample
    AD1CON1bits.DONE = NO;     // clear the done bit before starting

    AD1CON1bits.SAMP = ON;     // start the sampling sequence
    while (!AD1CON1bits.DONE); // wait for conversion to finish
    AD1CON1bits.ADON = OFF;    // turn the A/D converter off

    return ADC1BUF0;           // return the converted result
}

/**
 * setHeaterOutput() Information
 *
 * This function is used to control the heater switch (power driver).
 */
static void setHeaterOutput(Boolean desiredOutputState)
{
    _TRISE3 = 0;               // configure port as output
    _RE3 = desiredOutputState; // set the output
}

/**
 * turnOff() Information
 *
 * This function turns the heater on and powers it.
 */
static void turnOff(void)
{
    system.off6volt();     // turn 6volt enable off
    setHeaterOutput(OFF);  // turn heating element off
}

/**
 * turnOn() Information
 *
 * This function turns the heater on and powers it.
 */
static void turnOn(void)
{
    system.on6volt();     // turn 6volt enable on
    setHeaterOutput(ON);  // turn heating element on
}

/**
 * getReading() Information
 *
 * This function samples the sensor and returns the average raw 10-bit value.
 */
static Uint16 getReading(int samplesToTake, int interval)
{
    // sample the sensor multiple times for validation
    Uint32 readings = 0;  // accumulator
    int i;                // iterator

    for(i = 0; i < samplesToTake; ++i)
    {
        readings += readValue();
        wait(interval);
    }

    // average the readings and return it
    return (Uint16)(readings / samplesToTake);

}

/**
 * readTest() Information
 *
 * This function checks if a hardware failure has occurred.
 */
static Boolean readTest(void)
{
    _TRISE2 = 1; // configure port as input
    return _RE2; // return the value on the pin
}

/**
 * initialize() Information
 *
 * This function initializes the CO2 module.
 */
static void initialize(void)
{
    /**
     * Pin Configurations
     *
     * The pin connected to the sensor's output needs to be configured as an
     * analog input. The status line is configured as a digital input, and the
     * heater power driver is configured as a digital output (initialized low so
     * power driver is off).
     */
    _TRISB4 = 1; // configure port as input
    _PCFG4  = 0; // pin is in analog mode (disable digital mode)

    _TRISE2 = 1; // configure port as input

    _TRISE3 = 0; // configure port as output
    _RE3 = 0;    // set the output low (turn off power driver)

    /**
     * Enable Power - DK
     *
     * Previously the power was disabled by default for power saving purposes.
     * This has been changed since most users intend to use the power when
     * initializing. SInce the sensor is on it will be able to warm up and take
     * acurate readings.
     */

    turnOn(); // enable 6V power - DK

}

/**
 * co2 Information
 *
 * co2 is the software wrapper for the NESI gas sensor interface.
 */
const Co2 co2 = {
    status:readTest,
    read:readValue,
    get:getReading,
    on:turnOn,
    off:turnOff,
    setHeater:setHeaterOutput,
    init:initialize
};

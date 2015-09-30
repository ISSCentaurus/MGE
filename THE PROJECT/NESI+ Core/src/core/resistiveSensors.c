/**
 * NESI Resistive Sensor Driver License
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
 * 09/24/2013 - Dakotah Karrer
 *   Renamed and refactored from "moisture" to "resistiveSensors".
 */

#include "system.h"
#include "resistiveSensors.h"

/**
 * readQ1() Information
 *
 * This function samples the sensor connected to quadrant 1 (Q1) once and
 * returns the raw 10-bit value.
 */
static Uint16 readQ1(void) {
    AD1CON1bits.ADON = ON;     // Turn the A/D conversion module on

    ADC1BUF0 = 0;              // clear the result buffer
    AD1CHS0 = 0;               // set channel to sample
    AD1CON1bits.DONE = NO;     // clear the done bit before starting

    AD1CON1bits.SAMP = ON;     // start the sampling sequence
    while (!AD1CON1bits.DONE); // wait for conversion to finish
    AD1CON1bits.ADON = OFF;    // turn the A/D converter off

    return ADC1BUF0;           // return the converted result
}

/**
 * readQ2() Information
 *
 * This function samples the sensor connected to quadrant 2 (Q2) once and
 * returns the raw 10-bit value.
 */
static Uint16 readQ2(void) {
    AD1CON1bits.ADON = ON;     // Turn the A/D conversion module on

    ADC1BUF0 = 0;              // clear the result buffer
    AD1CHS0 = 1;               // set channel to sample
    AD1CON1bits.DONE = NO;     // clear the done bit before starting

    AD1CON1bits.SAMP = ON;     // start the sampling sequence
    while (!AD1CON1bits.DONE); // wait for conversion to finish
    AD1CON1bits.ADON = OFF;    // turn the A/D converter off

    return ADC1BUF0;           // return the converted result
}

/**
 * readQ3() Information
 *
 * This function samples the sensor connected to quadrant 3 (Q3) once and
 * returns the raw 10-bit value.
 */
static Uint16 readQ3(void) {
    AD1CON1bits.ADON = ON;     // Turn the A/D conversion module on

    ADC1BUF0 = 0;              // clear the result buffer
    AD1CHS0 = 2;               // set channel to sample
    AD1CON1bits.DONE = NO;     // clear the done bit before starting

    AD1CON1bits.SAMP = ON;     // start the sampling sequence
    while (!AD1CON1bits.DONE); // wait for conversion to finish
    AD1CON1bits.ADON = OFF;    // turn the A/D converter off

    return ADC1BUF0;           // return the converted result
}

/**
 * readQ4() Information
 *
 * This function samples the sensor connected to quadrant 4 (Q4) once and
 * returns the raw 10-bit value.
 */
static Uint16 readQ4(void) {
    AD1CON1bits.ADON = ON;     // Turn the A/D conversion module on

    ADC1BUF0 = 0;              // clear the result buffer
    AD1CHS0 = 3;               // set channel to sample
    AD1CON1bits.DONE = NO;     // clear the done bit before starting

    AD1CON1bits.SAMP = ON;     // start the sampling sequence
    while (!AD1CON1bits.DONE); // wait for conversion to finish
    AD1CON1bits.ADON = OFF;    // turn the A/D converter off

    return ADC1BUF0;           // return the converted result
}

/**
 * sampleQ1() Information
 *
 * This function samples the sensor connected to quadrant 1 (Q1) and returns
 * the average raw 10-bit value.
 */
static Uint16 sampleQ1(int samplesToTake, int interval) {
    // sample the sensor multiple times for validation
    Uint32 readings = 0; // accumulator
    int i; // iterator

    for (i = 0; i < samplesToTake; ++i) {
        readings += readQ1();
        wait(interval);
    }

    // average the readings and return it
    return (Uint16) (readings / samplesToTake);
}

/**
 * sampleQ2() Information
 *
 * This function samples the sensor connected to quadrant 2 (Q2) and returns
 * the average raw 10-bit value.
 */
static Uint16 sampleQ2(int samplesToTake, int interval) {
    // sample the sensor multiple times for validation
    Uint32 readings = 0; // accumulator
    int i; // iterator

    for (i = 0; i < samplesToTake; ++i) {
        readings += readQ2();
        wait(interval);
    }

    // average the readings and return it
    return (Uint16) (readings / samplesToTake);
}

/**
 * sampleQ3() Information
 *
 * This function samples the sensor connected to quadrant 3 (Q3) and returns
 * the average raw 10-bit value.
 */
static Uint16 sampleQ3(int samplesToTake, int interval) {
    // sample the sensor multiple times for validation
    Uint32 readings = 0; // accumulator
    int i; // iterator

    for (i = 0; i < samplesToTake; ++i) {
        readings += readQ3();
        wait(interval);
    }

    // average the readings and return it
    return (Uint16) (readings / samplesToTake);
}

/**
 * sampleQ4() Information
 *
 * This function samples the sensor connected to quadrant 4 (Q4) and returns
 * the average raw 10-bit value.
 */
static Uint16 sampleQ4(int samplesToTake, int interval) {
    // sample the sensor multiple times for validation
    Uint32 readings = 0; // accumulator
    int i; // iterator

    for (i = 0; i < samplesToTake; ++i) {
        readings += readQ4();
        wait(interval);
    }

    // average the readings and return it
    return (Uint16) (readings / samplesToTake);
}

/**
 * initialize() Information
 *
 * This function initializes hardware needed by the module.
 */
static void initialize(void) {
    /**
     * Channel Pin Configuration
     *
     * The pins need to be configured as analog inputs.
     */
    _TRISB0 = 1; // Q1 - configure port as input
    _PCFG0  = 0; // Q1 - pin is in analog mode (disable digital mode)
    _TRISB1 = 1; // Q2
    _PCFG1  = 0; // Q2
    _TRISB2 = 1; // Q3
    _PCFG2  = 0; // Q3
    _TRISB3 = 1; // Q4
    _PCFG3  = 0; // Q4
}

/**
 * resistiveSensors Information
 *
 * resistiveSensors is the software wrapper for the NESI Resistive Sensor
 * interfaced.
 */
const ResistiveSensors resistiveSensors = {
    readQ1 : readQ1,
    readQ2 : readQ2,
    readQ3 : readQ3,
    readQ4 : readQ4,
    getQ1 : sampleQ1,
    getQ2 : sampleQ2,
    getQ3 : sampleQ3,
    getQ4 : sampleQ4,
    init : initialize
};

/**
 * NESI System Driver License
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
 *   Moved 6volt control to system module.
 */

#include "system.h"

/**
 * Configuration Word 1 Usage and Setup
 *
 * The use of the watchdog timer is not necessary at this point, so the
 * Watchdog Timer Enable bit (CW1<7>) is cleared. Also, the microcontroller
 * will be programmed via ICSP so the JTAG will be disabled (CW1<14>).
 * Emulation functions will be shared with PGEDC2 (CW1<9:8>).
 */
_CONFIG1(0x3E7F);

/**
 * Configuration Word 2 Usage and Setup
 *
 * For this application, the system's clock frequency needs to be as fast as
 * possible. The maximum operational clock frequency of this microcontroller
 * is 32MHz. In order to reach that clock frequency without the use of an
 * external oscillator, the internal 8MHz Fast RC oscillator will be used in
 * conjunction with the 4xPLL block in order to generate the 32MHz signal. In
 * addition, the remappable pins will need to be reconfigured multiple times.
 * The IOLOCK One-Way must be disabled in order to allow the pins to be
 * changed once they have been initially mapped.
 *
 * The USB 96 MHz PLL Prescaler Select bits (CW2<14:12>) are cleared
 * (no prescaling of the clock in order to retain maximum frequency), the USB
 * 96 MHz PLL Disable bit (CW2<15>) is cleared (so that the PLL block will be
 * enabled), IOLOCK One-Way Set Enable bit (CW2<4>)is cleared (so that
 * multiple remappings can occur). The OSCO pin will be configured to output
 * the instruction clock (CW2<5>). USB will be handled by the microcontroller
 * so the regulator will need to be enabled (CW2<3>).
 */
 _CONFIG2(0x01E7); // enable maximum clock frequency and disable remapping lock

/**
 * initClock() Information
 *
 * This function configures and initializes the microcontroller's clock.
 */
static void initClock(void)
{
    /* >>>>>>>>>>>>>>>>>>>>>> Initialize System Clock <<<<<<<<<<<<<<<<<<<<<< */

    /**
     * System Clock Usage and Setup
     *
     * For this application, the system's clock frequency needs to be as fast
     * as possible. The maximum operational clock frequency of this
     * microprocessor is 32MHz. In order to reach that clock frequency without
     * the use of an external oscillator, the internal 8MHz Fast RC oscillator
     * will be used in conjunction with the 4xPLL block in order to generate
     * the 32MHz signal. Also to be noted is that the PLL block accepts
     * frequencies up to 4MHz. Signals with frequencies higher than 4MHz must
     * first be prescaled (divided down) to 4MHz or less before they can be
     * fed into the PLL block.
     */
    _RCDIV = 0b001;     // go from 8MHz down to 4MHz
    _CPDIV = 0b000;     // do not scale down the CPU clock

    /**
     * System Clock Usage and Setup (continued)
     *
     * The microcontroller will always use the Fast RC oscillator (postscaled
     * to 4MHz) in conjunction with the 4xPLL block as the system's clock
     * source. This means that the CPU's clock source never changes. Therefore
     * the current CPU oscillator and the new CPU oscillator are the same.
     */
    _COSC = 0b001;
    _NOSC = 0b001;

    /**
     *  System Clock Usage and Setup (continued)
     *
     * Since the Recover on Interrupt and CPU Peripheral Clock Ratio cause
     * undesirable side effects to the system clock, they will be disabled.
     */
    _ROI = 0;
    _DOZEN = 0;

}

/**
 * initAnalog() Information
 *
 * This function initializes microcontroller's analog module.
 */
static void initAnalog(void)
{
    /**
     * Disable Module
     *
     * Before configuring the module, it will be disabled. This will avoid
     * sporadic and undefined behavior.
     */
    AD1CON1bits.ADON = 0; // Turn the A/D converter off while configuring

    /**
     * General Analog to Digital Conversion Module Configuration
     *
     * The ADC module will be configured to sample the chosen channel when
     * triggered.
     */
    AD1CON1bits.ADSIDL = 0;   // do not stop ADC in Idle mode
    AD1CON1bits.FORM = 0b00;  // data formated as an unsigned integer
    AD1CON3bits.ADRC = 0;     // ADC drives clock from the system clock
    AD1CON1bits.SSRC = 0b111; // autoconvert samples
    AD1CON1bits.ASAM = 0;     // manual sample start
    AD1CON3bits.SAMC = 4;     // ADC samples for 4 Tad
    AD1CON3bits.ADCS = 1;     // ADC clock is 2 Tcy

    /**
     * ADC Input Configuration
     *
     * Inputs will be read as needed. Scanning will be disabled by the ADC module.
     */
    AD1CHS0bits.CH0NA = 0;    // MUXA negative input is Vref-
    AD1CON2bits.VCFG = 0b000; // ADC Vref+ reference is AVdd and Vref- is AVss
    AD1CON2bits.ALTS = 0;     // always sample from input A (mux A)
    AD1CON2bits.CSCNA = 0;    // do not scan ADC inputs on CH0+ during sample A
    AD1CSSL = 0;              // disable all channel scans

    /**
     * Interrupt Configuration
     *
     * Interrupts will not be used for conversion and storage.
     */
    IFS0bits.AD1IF = 0;  // clear the A/D interrupt flag
    IEC0bits.AD1IE = 0;  // disable A/D interrupt

}

/**
 * initialize() Information
 *
 * This function configures and initializes the microcontroller (PIC24FJ256GB106)
 * for use as the NESI central intelligence.
 */
static void initialize(void)
{
    /* Start by turning on the system clock */
    initClock();

    /* Next configure and initialize the necessary modules */
    initAnalog();
}

/**
 * setPowerOutput() Information
 *
 * This function turns the 6V power off.
 */
static void setPowerOutput(Boolean desiredOutputState)
{
    _TRISE6 = 0;               // configure port as output
    _RE6 = desiredOutputState; // set the output (active high)
}

/**
 * disable6volt() Information
 *
 * This function turns the 6V power off.
 */
static void disable6volt(void)
{
    setPowerOutput(OFF);   // turn 6volt enable off
}

/**
 * enable6volt() Information
 *
 * This function turns the 6V power on.
 */
static void enable6volt(void)
{
    setPowerOutput(ON);   // turn 6volt enable on
}

/**
 * system Information
 *
 * system is the wrapper for the core NESI utilities.
 */
const System system = {init:initialize, on6volt:enable6volt, off6volt:disable6volt};

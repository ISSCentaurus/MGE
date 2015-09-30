/**
 * NESI LED Driver License
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

#include "system.h"
#include "led.h"

/**
 * Mxx_DUTYCYCLE Information
 *
 * MIN_DUTYCYCLE and MAX_DUTYCYCLE are used as the global duty cycle limits.
 */
#define MIN_DUTYCYCLE   (0)
#define MAX_DUTYCYCLE   (100)

/**
 * initPwm1() Information
 *
 * This function initializes output compare module 1 and configures it to output
 * a pulse width modulated signal.
 */
 static void initPwm1(void)
{
    /* disable module before configuring */
    OC1CON1 &= 0xfff8;

    /* OC config */
    OC1CON1 = 0x1C08;     // clock source Fcy, trigger mode 1, disabled
    OC1CON2 = 0x001F;     // self sync, non inverted signal (active high)
    OC1RS = FCY/5000 - 1; // set the PWM period to a frequency of 5kHz
    OC1R  = 0;            // set the duty cycle to 0%

    /* configure pins */
    _PCFG8 = 1;  // PWM 1 is digital
    _TRISB8 = 0; // PWM 1 is an output
    _LATB8 = 0;  // drive output low

    __builtin_write_OSCCONL(OSCCON & 0xBF); // unlock Peripheral Pin Select Registers
    _RP8R = 17 + 1;                         // PWM offset + channel
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock Peripheral Pin Select Registers

    /* turn module on */
    OC1CON1 |= 0x6;
}

/**
 * setPwm1DutyCycle() Information
 *
 * This function sets the duty cycle of PWM 1 to that of the passed value (as
 * a positive integer percent). Values that are larger than MAX_DUTYCYCLE are
 * coerced to the max limit.
 */
 static void setPwm1DutyCycle(Uint16 newDutyCycle)
{
    if(newDutyCycle > MAX_DUTYCYCLE)  // if the passed value is too large
        newDutyCycle = MAX_DUTYCYCLE; // set value to max allowable duty cycle

    /* now set the duty cycle register to the new duty cycle value scaled to the current frequency */
    OC1R = round(((Uint32)(OC1RS + 1) * newDutyCycle) / (float)MAX_DUTYCYCLE);
}

/**
 * initPwm2() Information
 *
 * This function initializes output compare module 2 and configures it to output
 * a pulse width modulated signal.
 */
static void initPwm2(void)
{
    /* disable module before configuring */
    OC2CON1 &= 0xfff8;

    /* OC config */
    OC2CON1 = 0x1C08;     // clock source Fcy, trigger mode 1, disabled
    OC2CON2 = 0x001F;     // self sync, non inverted signal (active high)
    OC2RS = FCY/5000 - 1; // set the PWM period to a frequency of 5kHz
    OC2R  = 0;            // set the duty cycle to 0%

    /* configure pins */
    _PCFG9 = 1;  // PWM 2 is digital
    _TRISB9 = 0; // PWM 2 is an output
    _LATB9 = 0;  // drive output low

    __builtin_write_OSCCONL(OSCCON & 0xBF); // unlock Peripheral Pin Select Registers
    _RP9R = 17 + 2;                         // PWM offset + channel
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock Peripheral Pin Select Registers

    /* turn module on */
    OC2CON1 |= 0x6;
}

/**
 * setPwm2DutyCycle() Information
 *
 * This function sets the duty cycle of PWM 2 to that of the passed value (as
 * a positive integer percent). Values that are larger than MAX_DUTYCYCLE are
 * coerced to the max limit.
 */
static void setPwm2DutyCycle(Uint16 newDutyCycle)
{
    if(newDutyCycle > MAX_DUTYCYCLE)  // if the passed value is too large
        newDutyCycle = MAX_DUTYCYCLE; // set value to max allowable duty cycle

    /* now set the duty cycle register to the new duty cycle value scaled to the current frequency */
    OC2R = round(((Uint32)(OC2RS + 1) * newDutyCycle) / (float)MAX_DUTYCYCLE);
}

/**
 * ledX Information
 *
 * ledX are the software wrappers for the NESI LEDs. They use a pulse width
 * modulated signal to control the LED banks. ledR controls the R bank
 * and ledB controls the B bank.
 */
const Led ledB = {dutycycle:setPwm1DutyCycle, init:initPwm1},
          ledR  = {dutycycle:setPwm2DutyCycle, init:initPwm2};

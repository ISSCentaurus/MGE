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

#ifndef LED_H
#define LED_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: B8 (RP8), B9 (RP9)
 *   - Peripherals: Output Compare 1 and 2
 */

#include "system.h"

/**
 * ledX Information
 *
 * ledX are the software wrappers for the NESI LEDs. They use a pulse width
 * modulated signal to control the LED banks. ledR controls the R bank
 * and ledB controls the B bank.
 */
typedef struct {
    /**
     * Configures the duty cycle of the PWM output signal
     * @param newDutyCycle - the desired duty cycle of the output signal as a positive integer percentage
     *                       (range: <code>MIN_DUTYCYCLE</code> to <code>MAX_DUTYCYCLE</code>)
     */
    void (*dutycycle)(Uint16 newDutyCycle);
    /**
     * Initializes the LED software module
     */
    void (*init)(void);
} Led;

extern const Led ledR, ledB;

#endif /* LED_H */

/**
 * NESI PowerDriver Driver License
 * Copyright (C) 2013 Dakotah Karrer
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
 *   Migrated from solenoid software module.
 * 11/27/2013 - Mickie Byrd
 *   Added isOk() to make consistent with other NESI modules.
 */

#ifndef POWERDRIVER_H
#define POWERDRIVER_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: E6 (DO - 6V enable)
 *   - Peripherals: ADC, Clocks
 */

#include "system.h"

/**
* Wrapper for PowerDriver
*/
typedef struct {
    /**
     * Check if a hardware failure has occurred with the powerDriver
     * @return whether or not a hardware failure has occurred
     */
    Boolean (*isFailure)(void);
    /**
     * Checks if the powerDriver hardware is ok.
     * @return whether or not a hardware failure has occurred
     */
    Boolean (*isOk)(void);
    /**
     * Checks status level of powerDriver
     * @return tells you voltage level on low side of Power A's load
     */
    Boolean (*status)(void);
    /**
     * Sets the logic value present at the output
     * @param desiredOutputState the digital state to output on the pin (<code>OPEN</code> or <code>CLOSE</code>)
     */
    void (*set)(Boolean desiredOutputState);
    /**
     * Turns the powerDriver on
     */
    void (*on)(void);
    /**
     * Turns the powerDriver off
     */
    void (*off)(void);
    /**
     * Initializes the powerDriver software module
     */
    void (*init)(void);

} PowerDriver;


extern const PowerDriver powerDriverA, powerDriverB;

#endif /* POWERDRIVER_H */

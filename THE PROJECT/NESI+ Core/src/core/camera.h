/**
 * NESI C329-UART Camera Driver License
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

#ifndef CAMERA_H
#define CAMERA_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: G8 (RP19 : TX), G7 (RP26 : RX), E1 (power)
 *   - Peripherals: UART 1, Timer 2 (watchdog)
 */

/**
 * TODO
 *
 * Things left to do:
 *   state machine - needs to be written for reliability
 *   initialization - needs initialization routine
 *   error checking - need to implement file errors and comm errors
 *   comments - functions and code
 */

#include "system.h"

/**
 * Wrapper for the C329-UART camera
 */
typedef struct {
    /**
     * Takes a picture and saves it to the SD card using the passed filename
     * @param filename - as a <code>String</code> to store the image as
     */
    int (*getPix)(String filename);
    //int (*getPix)(void);
    /**
     * Initializes the Camera software module
     */
    void (*init)(void);
    /**
     * Turns on the camera
     */
    void (*on)(void);
    /**
     * Turns off the camera
     */
    void (*off)(void);
    /**
     * Checks status of camera
     */
    Boolean (*isOk)(void);

} Camera;

extern const Camera camera;

#endif /* CAMERA_H */

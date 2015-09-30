/**
 * NESI Button Driver License
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

#ifndef BUTTON_H
#define BUTTON_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: E4 (CN62)
 *   - Peripherals: Timer 5
 */

#include "system.h"

typedef enum {
    KEYSTROKE_NONE,
    KEYSTROKE_SINGLE,
    KEYSTROKE_DOUBLE
} Keystroke;

/**
 * button Information
 *
 * button is the software wrapper for the NESI push button.
 */
typedef struct {
    /**
     * Check if the button is pressed down
     * @return digital state of the button
     */
    Boolean (*isPressed)(void);
    /**
     * Initializes the button software module
     */
    void (*init)(void);
    /**
     * Disables the detection of button keystrokes
     */
    void (*disableStroke)(void);
    /**
     * Enables the detection of button keystrokes
     */
    void (*enableStroke)(void);
    /**
     * Returns the most recent keystroke detected (and clears the buffer)
     * @return most recent keystroke detected
     */
    Keystroke (*getStroke)(void);

} Button;

extern const Button button;

#endif /* BUTTON_H */

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

#ifndef RESISTIVESENSORS_H
#define RESISTIVESENSORS_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: B0 (AN0 - Q1), B1 (AN1 - Q2), B2 (AN2 - Q3), B3 (AN3 - Q4)
 *   - Peripherals: Analog to Digital Converter
 */

/**
 * resistiveSensors Information
 *
 * resistiveSensors is the software wrapper for the NESI Resistive Sensor
 * interface.
 */
typedef struct {
    /**
     * Read resistive sensors content of quadrant 1
     * @return resistive sensors value of Q1
     */
    Uint16 (*readQ1)(void);
    /**
     * Read resistive sensors content of quadrant 2
     * @return resistive sensors value of Q2
     */
    Uint16 (*readQ2)(void);
    /**
     * Read resistive sensors content of quadrant 3
     * @return resistive sensors value of Q3
     */
    Uint16 (*readQ3)(void);
    /**
     * Read resistive sensors content of quadrant 4
     * @return resistive sensors value of Q4
     */
    Uint16 (*readQ4)(void);
    /**
     * Take an average sampling of the resistive sensors content of quadrant 1
     * @param samplesToTake number of samples to take
     * @param interval milliseconds to wait in between samples
     * @return average of the samples taken
     */
    Uint16 (*getQ1)(int samplesToTake, int interval);
    /**
     * Take an average sampling of the resistive sensors content of quadrant 2
     * @param samplesToTake number of samples to take
     * @param interval milliseconds to wait in between samples
     * @return average of the samples taken
     */
    Uint16 (*getQ2)(int samplesToTake, int interval);
    /**
     * Take an average sampling of the resistive sensors content of quadrant 3
     * @param samplesToTake number of samples to take
     * @param interval milliseconds to wait in between samples
     * @return average of the samples taken
     */
    Uint16 (*getQ3)(int samplesToTake, int interval);
    /**
     * Take an average sampling of the resistive sensors content of quadrant 4
     * @param samplesToTake number of samples to take
     * @param interval milliseconds to wait in between samples
     * @return average of the samples taken
     */
    Uint16 (*getQ4)(int samplesToTake, int interval);
    /**
     * Initializes the solenoid software module
     */
    void (*init)(void);

} ResistiveSensors;

extern const ResistiveSensors resistiveSensors;

#endif /* RESISTIVESENSORS_H */

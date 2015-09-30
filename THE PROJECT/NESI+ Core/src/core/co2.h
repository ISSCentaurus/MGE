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

#ifndef CO2_H
#define CO2_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: B4 (AN4), E3 (DO - FET), E2 (DI - status)
 *   - Peripherals: Analog to Digital Converter
 */

/**
 * Change Log
 *
 * 09/26/2013 - Dakotah Karrer
 *   Added .status() to determine if a hardware failure has occurred with the
 *   heater control.
 */
#include "system.h"

/**
 * co2 Information
 *
 * co2 is the software wrapper for the NESI gas sensor interface.
 */
typedef struct {
    /**
     * Check if a hardware failure has occurred with the CO2 power driver
     * @return whether or not a hardware failure has occurred
     */
    Boolean (*status)(void);
    /**
     * Read CO2 sensor
     * @return unconverted CO2 voltage
     */
    Uint16 (*read)(void);
    /**
     * Take an average sampling of the CO2
     * @param samplesToTake number of samples to take
     * @param interval milliseconds to wait in between samples
     * @return average of the samples taken
     */
    Uint16 (*get)(int samplesToTake, int interval);
    /**
     * Controls the operation of the CO2 sensor heating element
     * @param desiredState <code>ON</code>/<code>OFF</code> (enable/disable)
     */
    void (*setHeater)(Boolean desiredState);
    /**
     * Initializes the CO2 sensor software module
     */
    void (*init)(void);
    /**
     * Turns on the CO2 sub-system and warms the sensor
     */
    void (*on)(void);
    /**
     * Turns the CO2 sub-system off
     */
    void (*off)(void);

} Co2;

extern const Co2 co2;

#endif /* CO2_H */

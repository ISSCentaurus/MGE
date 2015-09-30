/**
 * NESI SD Card Driver License
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

#ifndef SDCARD_H
#define SDCARD_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: D2 (DO - Primary Card Select),
 *           D9 (DO - Secondary Card Select),
 *           D7 (DO - Primary Card Detect),
 *           D10 (RP3 - SPI Clock),
 *           D11 (RP12 - MISO),
 *           D0 (RP11 - MOSI)
 *   - Peripherals: SPI 1
 *
 * Note: This module uses the Microchip Memory Disk Drive (MDD) file system for
 *       file I/O.
 */

#include "system.h"
#include "../mdd/FSIO.h"
#include "../mdd/HardwareProfile.h"

/**
 * sdcard Information
 *
 * sdcard is the software wrapper for the NESI SD card memory.
 */
typedef struct Sdcard Sdcard;

struct Sdcard {
    /**
     * Configures the microcontroller and initializes the file system.
     */
    void (*init)(void);
    /**
     * Checks SD card for issues.
     * @return whether or not the SD card was successfully initialized
     */
    Boolean (*isOk)(void);
};

extern const Sdcard sdcard;

#endif /* SDCARD_H */

/**
 * NESI DataLog Driver License
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

#ifndef DATALOG_H
#define DATALOG_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: none
 *   - Peripherals: none
 *
 * Note: This module uses dataLog.txt located on the NESI board's SD card.
 */

#include "system.h"
#include "file.h"

/**
 * dataLog Information
 *
 * dataLog is the software wrapper for the NESI data logger module.
 */
typedef struct {
    /**
     * Add a value to the data log
     * @param label tag for the data to add. This <code>String</code> precedes the value
     * @param value to write to the data log
     */
    void (*add)(String label, Uint16 value);
    /**
     * Initializes the data log software module
     */
    void (*init)(void);
    /**
     * Checks data log for issues.
     * @return whether or not the data log file is ok.
     */
    Boolean (*isOk)(void);

    FileStream* file;

} DataLog;

extern const DataLog dataLog;

#endif /* DATALOG_H */

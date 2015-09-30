/**
 * NESI UART 2 Module License
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

#ifndef UART2_H
#define UART2_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: D5 (RP20 : TX), D4 (RP25 : RX)
 *   - Peripherals: UART 2
 */

#include "system.h"
#include "uart.h"

/**
 * uart2 Information
 *
 * uart2 is the software wrapper for a UART module mapped to the by the NESI
 * COMM Port. uart2 allows the COMM Port to be used for serial I/O.
 */
extern const Uart uart2;

#endif /* UART2_H */

/**
 * NESI UART 3 Module License
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

#ifndef UART3_H
#define UART3_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: D3 (RP22 : TX), D1 (RP24 : RX)
 *   - Peripherals: UART 3
 */

#include "system.h"
#include "uart.h"

/**
 * uart3 Information
 *
 * uart3 is the software wrapper for the first UART module available on the
 * NESI board.
 */
extern const Uart uart3;

#endif /* UART3_H */

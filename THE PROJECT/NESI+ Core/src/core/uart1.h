/**
 * NESI UART 1 Module License
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

#ifndef UART1_H
#define UART1_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: G8 (RP19 : TX), G7 (RP26 : RX)
 *   - Peripherals: UART 1
 */

#include "system.h"
#include "uart.h"

/**
 * uart1 Information
 *
 * uart1 is the software wrapper for the first UART module available on the
 * NESI board.
 */
extern const Uart uart1;

#endif /* UART1_H */

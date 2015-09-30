/**
 * NESI UART Module License
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

#ifndef UART_H
#define UART_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: None
 *   - Peripherals: None
 */

#include "system.h"

/**
 * Wrapper that provides serial I/O using UART
 */
typedef struct {
    /**
     * Configures the baud rate
     * @param baudrate - the desired baud rate (in bits/sec) as a positive integer
     */
    void (*baudrate)(Uint64 baudrate);
    /**
     * Reads the specified number of bytes from the UART port's receive buffer
     *      and stores them at the pointed to memory location.
     * @param storageBuffer - pointer to the byte array to store the data copied
     *                        from the UART port
     * @param maxBytesToRead - maximum number of bytes to copy from the UART port
     *                         and store in the storageBuffer
     * @return number of bytes copied from the UART port receive buffer
     * @Note This function does not remove data from the UART port buffer.
     *       Bytes copied from the buffer remain until dropped.
     */
    int (*copy)(Byte* storageBuffer, int maxBytesToRead);
    /**
     * Removes bytes from the UART port's receive buffer
     * @param bytesToDrop - the desired number of bytes to drop from the receive buffer
     */
    void (*drop)(int bytesToDrop);
    /**
     * Removes all bytes from the UART port's receive buffer
     */
    void (*flush)(void);
    /**
     * Counts the number of bytes UART port's receive buffer
     * @return the number of bytes in the receive buffer
     */
    Uint16 (*size)(void);
    /**
     * Sends the specified number of bytes from the pointed to string to the
     *      connected peripheral.
     * @param startOfData - pointer to string of bytes to send
     * @param bytesToSend - number of bytes to send from the byte array
     * @return <code>Boolean</code> indicating weather or not bytes were sent to
     *       the connected peripheral.
     */
    Boolean (*send)(const Byte* startOfData, Uint16 bytesToSend);
    /**
     * Pulls the specified number of bytes from the UART port's receive buffer
     *      and stores them at the pointed to memory location.
     * @param storageBuffer - pointer to the byte array to store the data read
     *                        from the UART port
     * @param maxBytesToRead - maximum number of bytes to pull from the UART port
     *                         and store in the storageBuffer
     * @return number of bytes read from the UART port receive buffer
     * @Note This function removes data from the UART port buffer.
     */
    int (*receive)(Byte* storageBuffer, int maxBytesToRead);
    /**
     * Initializes the camera UART software module
     */
    void (*init)(void);

} Uart;

#endif /* UART_H */

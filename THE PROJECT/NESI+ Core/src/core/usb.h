/**
 * NESI USB Driver License
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
 * 12/31/2013 - Mickie Byrd
 *   Added several members to usb module:
 *     .state() - returns current state of USB stack state machine
 *     .read() - read data from USB communications port
 *     .write() - write data to USB communications port
 *     .print() - print a string to USB communications port
 *     .debug() - print a string to USB communications port
 *   Added .printf() to print formatted string to USB com port
 *   Updated comments with return information
 */

#ifndef USB_H
#define USB_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: D+, D-, Vusb, Vbus
 *   - Peripherals: USB
 *
 * Note: This module uses the Microchip USB stack to implement a Mass Storage
 *       Device (MSD class).
 */

#include "../dev/GenericTypeDefs.h"
#include "../dev/Compiler.h"
#include "../usb/usb_config.h"
#include "../usb/usb_common.h"
#include "../usb/usb_ch9.h"
#include "../usb/usb_device.h"
#include "../usb/usb_hal.h"

#include "system.h"

typedef struct {
    /**
     * Initialize the USB stack.
     */
    void (*init)(void);
    /**
     * Connect to the USB host.
     */
    void (*connect)(void);
    /**
     * Connect to the USB host.
     */
    void (*attach)(void);
    /**
     * Disconnect from the USB host.
     */
    void (*disconnect)(void);
    /**
     * Disconnect from the USB host.
     */
    void (*eject)(void);
    /**
     * Disconnect from the USB host.
     */
    void (*detach)(void);
    /**
     * Process pending requests and data transfers.
     */
    void (*process)(void);
    /**
     * Gets current state of USB state machine.
     * @return USB state
     */
    Byte (*state)(void);
    /**
     * Read data from the USB com port.
     * @return number of bytes read
     */
    Uint8 (*read)(String dataBuffer, Uint8 maxBytesToRead);
    /**
     * Write data to the USB com port.
     * @return whether or not write was successful
     */
    Boolean (*write)(String dataBuffer, Uint8 bytesToWrite);
    /**
     * Print a string to USB com port.
     * @return whether or not print was successful
     */
    Boolean (*print)(String data);
    /**
     * Print a formatted string to the USB com port.
     * @return number of characters printed
     */
    int (*printf)(const String format, ...);
    /**
     * Enable or Disable the printing of debug messages (controls @debugPrint)
     */
    void (*setDebug)(Boolean state);
    /**
     * Print a formatted string to the USB com port when debugging is enabled.
     * @return number of characters printed
     */
    int (*debugPrint)(const String format, ...);
} Usb;

extern const Usb usb;

#endif /* USB_H */

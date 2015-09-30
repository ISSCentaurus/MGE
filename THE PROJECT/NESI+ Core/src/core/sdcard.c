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

#include "sdcard.h"

/**
 * initialized Information
 *
 * initialized is a flag used to keep track of whether or not the SD card module
 * has been initialized.
 */
static Boolean initialized = FALSE;

/**
 * initialize() Information
 *
 * This function configures the SD card hardware and initializes the file system.
 */
static void initialize(void)
{
    // configure SD-SPI Chip Selects
    _TRISD2 = 0;  // configure primary chip select as output
    _RD2 = 1;     // deselect primary SD card
    _TRISD9 = 0;  // configure secondary chip select as output
    _RD9 = 1;     // deselect secondary SD card

    /* do not reinitialize if already configured */
    if(initialized)
        return;

    /* initialize the SPI - SD card used in MMC mode */
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    _SDI1R = 12; // data in (MISO)
    _RP11R = 7;  // data out (MOSI)
    _RP3R = 8;   // clock out (SCLK)
    __builtin_write_OSCCONL(OSCCON | 0x40);

    /*  Detecting the SD Card
     *
     *  The SD card detect switch is active low (reads 0 when the card is inserted).
     *  To prevent false detects, the detect line will be pulled high using the
     *  internal pull-up resistor. When the card is not present, the floating line
     *  will be pulled high. Once the card is inserted, the line will be driven low.
     */
    _CN53PUE = 1;
    _CN49PUE = 1;
    _CN56PUE = 1;
    _CN55PUE = 1;
    _CN54PUE = 1;
    _CN16PUE = 1;
    _CN61PUE = 1;

    /* FSInit initializes the MDD File System used to access files on the SD card. */
    initialized = FSInit();
}

/**
 * isInitialized() description
 *
 * This function checks if the SD card is initialized.
 */
static Boolean isInitialized(void)
{
    return initialized;
}

/**
 * sdcard Information
 *
 * sdcard is the software wrapper for the NESI SD card memory.
 */
const Sdcard sdcard = {init:initialize, isOk:isInitialized};

/**
 * NESI Initialization Driver License
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

#include "nesi.h"

/**
 * initialize() Information
 *
 * This function initializes the NESI board's modules.
 */
static void initialize(void)
{
    system.init();
    ledR.init();
    ledB.init();
    powerDriverA.init();
    powerDriverB.init();
    //button.init();
    co2.init();
    resistiveSensors.init();
    sdcard.init();
    usb.init();
    lightLog.init();
    camera.init();
    dateTime.init();
}

/**
 * nesi Information
 *
 * nesi is the software wrapper for the NESI board.
 */
const Nesi nesi = {.init = initialize};

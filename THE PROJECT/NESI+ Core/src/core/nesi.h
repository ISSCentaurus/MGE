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

#include "system.h"
#include "led.h"
#include "resistiveSensors.h"
#include "powerDriver.h"
#include "co2.h"
//#include "button.h"
#include "usb.h"
#include "sdcard.h"
#include "lightLog.h"
#include "camera.h"
#include "dateTime.h"

/**
 * nesi Information
 *
 * nesi is the software wrapper for the NESI initialization module.
 */
typedef struct {
    void (*init)(void);
} Nesi;

extern const Nesi nesi;

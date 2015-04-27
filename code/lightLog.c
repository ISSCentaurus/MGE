/**
 * File:   lightLog.h
 * Author: Sam Cuthbertson
 * 
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

/**
 * Change Log
 *
 * 4/24/2015 - Sam Cuthbertson
 *   Updated to from dataLog to lightLog for Centaurus MGE.
 * 
 * 12/30/2013 - Mickie Byrd
 *   Added type casts to suppress compiler warnings.
 */

#include "lightLog.h"
#include "file.h"

static FileStream dataFile;

/**
 * initialize() Information
 *
 * This function initializes the data logger module.
 */
static void initialize(void)
{
    dataFile = getFileStream();
}

/**
 * isRunning() Information
 *
 * This function checks if the data logger module has been properly initialized.
 */
static Boolean isRunning(void)
{
    return !!dataFile.open;
}

/**
 * openFile() Information
 * 
 * This function opens the lightlog file for editing
 * @returns False for fail, true for success
 */
static Boolean openFile(void)
{
    return !dataFile.open("lightLog.txt");
}

/**
 * openFile() Information
 * 
 * This function closes the lightlog file for editing
 * @returns False for fail, true for success
 */
static Boolean closeFile(void)
{
    return !dataFile.close();
}

/**
 * logValue() Information
 *
 * This function writes data to the log file.
 */
static void logValue(String label, Uint16 value)
{
 
    //if(!!dataFile.open) return;
    
    // determine label length
    int size = 0;
    String temp = label;

    for(temp = label; *temp; ++temp)
    {
        ++size;
    }

    // write label
    dataFile.write((Byte*)label, size); // to suppress compiler warning

    // convert value
    char dataString[8] = {"0x0000\n"};
    static const char toHex[] = {"0123456789ABCDEF"};

    dataString[2] = toHex[(value & 0xF000) >> 12];
    dataString[3] = toHex[(value & 0x0F00) >> 8];
    dataString[4] = toHex[(value & 0x00F0) >> 4];
    dataString[5] = toHex[(value & 0x000F)];

    // write value
    dataFile.write((Byte*)dataString, 7); // to suppress compiler warning
    
}

/**
 * dataLog Information
 *
 * dataLog is the software wrapper for the NESI data logger module.
 */
const LightLog lightLog = {
    init:initialize,
    add:logValue,
    isOn:isRunning,
    open:openFile,
    close:closeFile,
    file:&dataFile
};

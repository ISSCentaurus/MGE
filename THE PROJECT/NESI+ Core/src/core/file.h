/**
 * NESI File Driver License
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

#ifndef FILE_H
#define FILE_H

/**
 * Microcontroller Resource Allocation
 *
 * The following microcontroller resources are used in this module.
 *   - Pins: none
 *   - Peripherals: none
 *
 * Note: This module uses the 3 available files from the SD card.
 */

#include "system.h"
#include "sdcard.h"

/**
 * FileStream Information
 *
 * FileStream is the software wrapper that provides file input and output for
 * the NESI board.
 */
typedef struct FileStream FileStream;

struct FileStream {
    /**
     * Opens the file in read/write append mode
     * @param fileName - name of the file to open (must be a String constant - i.e. "a.txt")
     * @return <code>FALSE</code> if successful; <code>TRUE</code> if unsuccessful
     */
    int (*open)(const String fileName);
    /**
     * Closes the file
     * @return <code>FALSE</code> if successful; <code>TRUE</code> if unsuccessful
     */
    int (*close)(void);
    /**
     * Write data to the file
     * @param dataBuffer - pointer to the byte array to write to the file
     * @param bytesToWrite - number of bytes to write to the file from dataBuffer
     * @return number of bytes written to the file
     */
    int (*write)(Byte* dataBuffer, int bytesToWrite);
    /**
     * Read data from the file
     * @param storageBuffer - pointer to the byte array to store the data read from the file
     * @param bytesToRead - number of bytes to read from the file and store in the storageBuffer
     * @return number of bytes read from the file
     * #Note Data is read from the beginning of the file after a write takes place
     *       or the end-of-file is reached. Sequential reads continue where the
     *       previous read left off.
     */
    int (*read)(Byte* storageBuffer, int bytesToRead);

    /**
     * allow file to be used by others
     */
    void (*free)(void);
};

/**
 * Gets a FileStream
 * @return FileStream object if one is available, <code>NullFile</code> if all
 *         3 files are being used
 */
FileStream getFileStream(void);

/**
 * fileX Information
 *
 * fileX are the software wrappers for the file interfaces available on the NESI
 * board. NullFile is an empty FileStream (equivalent to "0" or "NONE").
 */
extern const FileStream file1, file2, file3, NullFile;

#endif /* FILE_H */

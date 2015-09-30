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

/**
 * Change Log
 *
 * 12/30/2013 - Mickie Byrd
 *   Modified openX() to use isOpenX() to check file status in order to
 *       suppress compiler warnings.
 */

#include "file.h"

/*------------------------------------ 1 ------------------------------------*/
/**
 * fileptr1 Information
 *
 * fileptr1 is the file pointer for the first file interface wrapper.
 */
static FSFILE* fileptr1 = NullPtr;

/**
 * close1() Information
 *
 * This function closes file1 and reports if it was unsuccessful.
 */
static int close1(void)
{
    if(!FSfclose(fileptr1))               // close file and
        return (int)(fileptr1 = NullPtr); // unassign pointer
    return EOF;                           // otherwise, report error
}

/**
 * isOpen1() Information
 *
 * This function checks if file1 is open.
 */
static Boolean isOpen1(void)
{
    /* file pointer is null until opened */
    return NullPtr != fileptr1;
}

/**
 * open1() Information
 *
 * This function opens file1 for reading and writing.
 */
static int open1(const String fileName)
{
    if(!fileptr1)                           // if file not yet created
        fileptr1 = FSfopen(fileName, "a+"); // open in r/w append mode
    //return NullPtr == fileptr1;             // file null upon failure
    return !isOpen1(); // primarily to suppress compiler warning
}

/**
 * write1() Information
 *
 * This function writes data to file1.
 */
static int write1(Byte* dataBuffer, int bytesToWrite)
{
    return  FSfwrite((void *)dataBuffer, sizeof(Byte), bytesToWrite, fileptr1);
}

/**
 * read1() Information
 *
 * This function reads data from the beginning of file1.
 */
static int read1(Byte* storageBuffer, int bytesToRead)
{
    if(FSfeof(fileptr1))  // if currently at the end of the file
        if(FSfseek(fileptr1, 0, SEEK_SET)) // move to the beginning
            return EOF; // report error if cannot read from the beginning

    /* return the number of bytes read from the file */
    return FSfread(storageBuffer, sizeof(Byte), bytesToRead, fileptr1);
}

/**
 * used1 Information
 *
 * used1 is the "in use" flag for file1.
 */
static Boolean used1 = NO;

/**
 * free1() Information
 *
 * This function frees file1 so that it can be used by someone else.
 */
static void free1(void)
{
    used1 = NO;
}

/**
 * use1() Information
 *
 * This function flags file1 as being used so that it cannot be allocated to
 * someone else.
 */
static void use1(void)
{
    used1 = YES;
}

/**
 * isUse1() Information
 *
 * This function checks if file1 is in use.
 */
static Boolean isUsed1(void)
{
    return used1;
}

/**
 * file1 Information
 *
 * file1 is the software wrapper for the 3rd file interfaces available on the NESI
 * board.
 */
const FileStream file1 = {open:open1, close:close1, read:read1, write:write1, free:free1};

/*------------------------------------ 2 ------------------------------------*/
/**
 * fileptr2 Information
 *
 * fileptr2 is the file pointer for the first file interface wrapper.
 */
static FSFILE* fileptr2 = NullPtr;

/**
 * close2() Information
 *
 * This function closes file2 and reports if it was unsuccessful.
 */
static int close2(void)
{
    if(!FSfclose(fileptr2))               // close file and
        return (int)(fileptr2 = NullPtr); // unassign pointer
    return EOF;                           // otherwise, report error
}

/**
 * isOpen2() Information
 *
 * This function checks if file2 is open.
 */
static Boolean isOpen2(void)
{
    /* file pointer is null until opened */
    return NullPtr != fileptr2;
}

/**
 * open2() Information
 *
 * This function opens file2 for reading and writing.
 */
static int open2(const String fileName)
{
    if(!fileptr2)                           // if file not yet created
        fileptr2 = FSfopen(fileName, "a+"); // open in r/w append mode
    //return NullPtr == fileptr2;             // file null upon failure
    return !isOpen2(); // primarily to suppress compiler warning
}

/**
 * write2() Information
 *
 * This function writes data to file2.
 */
static int write2(Byte* dataBuffer, int bytesToWrite)
{
    return  FSfwrite((void *)dataBuffer, sizeof(Byte), bytesToWrite, fileptr2);
}

/**
 * read2() Information
 *
 * This function reads data from the beginning of file2.
 */
static int read2(Byte* storageBuffer, int bytesToRead)
{
    if(FSfeof(fileptr2))  // if currently at the end of the file
        if(FSfseek(fileptr2, 0, SEEK_SET)) // move to the beginning
            return EOF; // report error if cannot read from the beginning

    /* return the number of bytes read from the file */
    return FSfread(storageBuffer, sizeof(Byte), bytesToRead, fileptr2);
}

/**
 * used2 Information
 *
 * used2 is the "in use" flag for file2.
 */
static Boolean used2 = NO;

/**
 * free2() Information
 *
 * This function frees file2 so that it can be used by someone else.
 */
static void free2(void)
{
    used2 = NO;
}

/**
 * use2() Information
 *
 * This function flags file2 as being used so that it cannot be allocated to
 * someone else.
 */
static void use2(void)
{
    used2 = YES;
}

/**
 * isUse2() Information
 *
 * This function checks if file2 is in use.
 */
static Boolean isUsed2(void)
{
    return used2;
}

/**
 * file2 Information
 *
 * file2 is the software wrapper for the 3rd file interfaces available on the NESI
 * board.
 */
const FileStream file2 = {open:open2, close:close2, read:read2, write:write2, free:free2};

/*------------------------------------ 3 ------------------------------------*/
/**
 * fileptr3 Information
 *
 * fileptr3 is the file pointer for the first file interface wrapper.
 */
static FSFILE* fileptr3 = NullPtr;

/**
 * close3() Information
 *
 * This function closes file3 and reports if it was unsuccessful.
 */
static int close3(void)
{
    if(!FSfclose(fileptr3))               // close file and
        return (int)(fileptr3 = NullPtr); // unassign pointer
    return EOF;                           // otherwise, report error
}

/**
 * isOpen3() Information
 *
 * This function checks if file3 is open.
 */
static Boolean isOpen3(void)
{
    /* file pointer is null until opened */
    return NullPtr != fileptr3;
}

/**
 * open3() Information
 *
 * This function opens file3 for reading and writing.
 */
static int open3(const String fileName)
{
    if(!fileptr3)                           // if file not yet created
        fileptr3 = FSfopen(fileName, "a+"); // open in r/w append mode
    //return NullPtr == fileptr3;             // file null upon failure
    return !isOpen3(); // primarily to suppress compiler warning
}

/**
 * write3() Information
 *
 * This function writes data to file3.
 */
static int write3(Byte* dataBuffer, int bytesToWrite)
{
    return  FSfwrite((void *)dataBuffer, sizeof(Byte), bytesToWrite, fileptr3);
}

/**
 * read3() Information
 *
 * This function reads data from the beginning of file3.
 */
static int read3(Byte* storageBuffer, int bytesToRead)
{
    if(FSfeof(fileptr3))  // if currently at the end of the file
        if(FSfseek(fileptr3, 0, SEEK_SET)) // move to the beginning
            return EOF; // report error if cannot read from the beginning

    /* return the number of bytes read from the file */
    return FSfread(storageBuffer, sizeof(Byte), bytesToRead, fileptr3);
}

/**
 * used3 Information
 *
 * used3 is the "in use" flag for file3.
 */
static Boolean used3 = NO;

/**
 * used3 Information
 *
 * used3 is the "in use" flag for file3.
 */
static void free3(void)
{
    used3 = NO;
}

/**
 * free3() Information
 *
 * This function frees file3 so that it can be used by someone else.
 */
static void use3(void)
{
    used3 = YES;
}

/**
 * use3() Information
 *
 * This function flags file3 as being used so that it cannot be allocated to
 * someone else.
 */
static Boolean isUsed3(void)
{
    return used3;
}

/**
 * file3 Information
 *
 * file3 is the software wrapper for the 3rd file interfaces available on the NESI
 * board.
 */
const FileStream file3 = {open:open3, close:close3, read:read3, write:write3, free:free3};

/**
 * NullFile Information
 *
 * NullFile is an empty FileStream (equivalent to "0" or "NONE").
 */
const FileStream NullFile = {NullPtr};

/**
 * Gets a FileStream
 * @return FileStream object if one is available, <code>NullFile</code> if all
 *         3 files are being used
 */
FileStream getFileStream(void) // TODO - write proper wrapper
{
    sdcard.init();

    if(sdcard.isOk())  // no files if FS uninitialized
    {
        #if 0
        if (!isOpen1())
            return file1;
        else if (!isOpen2())
            return file2;
        else if (!isOpen3())
            return file3;
        #else
        if (!isUsed1())
        {
            use1();
            return file1;
        }
        else if (!isUsed2())
        {
            use2();
            return file2;
        }
        else if (!isUsed3())
        {
            use3();
            return file3;
        }
        #endif
    }

    return NullFile;
}

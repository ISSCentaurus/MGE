/**
 * NESI C329-UART Camera Driver License
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
 * 11/17/2013 - Mickie Byrd
 *   Refactored CameraComPort to Uart module.
 * 12/27/2013
 *   Changed error code that results if retrievePic() cannot open file.
 *   Added wait after camera is powered down.
 *   Changed getPicture() to return integer error code indicating point of
 *       failure.
 *   Refactored imageFile from FileStream to FSFILE*.
 *   Increased syncLimit to increase success rate.
 *   Increased power up wait time so that camera is fully ready.
 */

#include "camera.h"
#include "file.h"
#include "uart1.h"

/**
 * cameraComPort Information
 *
 * cameraComPort is the software wrapper for the UART module used by the camera
 * module. uart1 will be used to provide serial I/O for the C329 camera.
 */
static Uart cameraComPort = {0};

/**
 * CameraPacket Information
 *
 * The C329-UART camera communicates using the following command set:
 *
 *   Command   | Cmd Token | Parameter 1   | Parameter 2   | Parameter 3   | Parameter 4
 *  -----------+-----------+---------------+---------------+---------------+--------------
 *   INITIAL   | FFFFFF01h | Baud Rate     | Color Type    | Preview Res.  | Compres. Res
 *   GET PIC   | FFFFFF04h | Pic. Type     | 00h           | 00h           | 00h
 *   SNAPSHOT  | FFFFFF05h | Snapshot Type | 00h           | 00h           | 00h
 *   RESET     | FFFFFF08h | Reset Type    | 00h           | 00h           | 00/FFh
 *   POWER OFF | FFFFFF09h | 00h           | 00h           | 00h           | 00h
 *   DATA      | FFFFFF0Ah | Data Type     | Length Byte 0 | Length Byte 1 | Length Byte 2
 *   SYNC      | FFFFFF0Dh | 00h           | 00h           | 00h           | 00h
 *   ACK       | FFFFFF0Eh | Cmd Token     | ACK counter   | 00h           | 00h
 *   NAK       | FFFFFF0Fh | 00h           | NAK counter   | Err. Number   | 00h
 *   QUALITY   | FFFFFF10h | Quality Level | 00h           | 00h           | 00h
 *
 * Each command is 8 bytes long. It starts with a 4 byte command token and up to four
 * parameters.
 */
typedef union {
  Uint8 raw[8];
  struct {
    Uint8 :8;
    Uint8 :8;
    Uint8 :8;
    Uint8 cmdToken;
    Uint8 parameter1;
    Uint8 parameter2;
    Uint8 parameter3;
    Uint8 parameter4;
  };
} CameraPacket;

/**
 * CmdToken Information
 *
 * The Cmd Token tells the camera which action to execute. The first 3 bytes of each Cmd Token
 * are all 0xFF, so they can be kept constant. The fourth byte corresponds to the action to
 * perform.
 */
typedef enum CmdToken {
    INITIAL   = 0x01,
    GET_PIC   = 0x04,
    SNAPSHOT  = 0x05,
    RESET     = 0x08,
    POWER_OFF = 0x09,
    DATA      = 0x0A,
    SYNC      = 0x0D,
    ACK       = 0x0E,
    NAK       = 0x0F,
    QUALITY   = 0x10
} CmdToken;


static CameraPacket newCameraPacket(void)
{
    static const CameraPacket emptyPacket = {{0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}};
    return emptyPacket;
}

static void sendPacket(CameraPacket packetToSend)
{
    cameraComPort.send(packetToSend.raw,8);
}

_ISR_ _T2Interrupt(void)
{
    T2CONbits.TON = 0;
    _T2IE = 0;
    _T2IF = 0;
}

static CameraPacket getPacket(void)
{
    T2CON = 0x0020;                 // disabled, prescaled, internal oscillator
    TMR2 = 0;                       // clear the timer
    PR2 = CLOCKS_PER_SEC/64/10 - 1; // interrupt at 1kHz (1ms)
    _T2IE = ON;                     // enable Timer2 to interrupt
    _T2IF = OFF;                    // clear the interrupt flag
    _T2IP = 4;                      // set priority to real-time
    T2CONbits.TON = 1;

    CameraPacket inPacket = newCameraPacket();
    while(cameraComPort.size() < 8) if(!T2CONbits.TON) break;
    cameraComPort.receive(inPacket.raw,8);
    return inPacket;
}

#define RX_BUFFER_SIZE 0x0800
//static FileStream imageFile;
FSFILE* imageFile = NullPtr;
static Byte tempBuffer[RX_BUFFER_SIZE];
static int tempSize = 0;
static Int32 bytesToGet = 0;
static Int32 picsize = 0;
static CameraPacket toCam, fromCam;

static Sint getPicture(void)
{
    toCam = newCameraPacket(),
    fromCam = newCameraPacket();
    toCam.cmdToken = SYNC;
    int syncTries = 0;

    //if(!imageFile.open)
    if(!imageFile)
        return 1;

    /* try and get sync with camera */
    while(!(fromCam.cmdToken == ACK && fromCam.parameter1 == SYNC)) // until ACK received
    {
        if(syncTries == 3)
            cameraComPort.baudrate(115200);
        cameraComPort.flush();
        sendPacket(toCam); // send SYNC command
        pause(25);
        fromCam = getPacket(); // get reply

        if(syncTries < 7)
            ++syncTries;
        else
            return 2;
    }

    /* acknowledge camera's sync request */
    toCam = newCameraPacket(); // initialize packet
    toCam.cmdToken = ACK;      // acknowledge
    toCam.parameter1 = SYNC;   // the sync

    while(!(fromCam.cmdToken == SYNC)) // wait for camera SYNC request
        fromCam = getPacket();

    sendPacket(toCam);

    /* configure camera */
    toCam = newCameraPacket(); // initialize packet
    toCam.cmdToken = INITIAL;  // initial configuration
    toCam.parameter1 = 0x04;   // 115,200 baud rate
    toCam.parameter2 = 0x87;   // compress color
    toCam.parameter3 = 0x01;   // 80x60 preview resolution
    toCam.parameter4 = 0x07;   // 640x480 compression resolution
    sendPacket(toCam);

    /* if camera acknowledges changes, change the UART baud rate */
    fromCam = getPacket();
    if(!(fromCam.cmdToken == ACK && fromCam.parameter1 == INITIAL))
        return 3;// if reconfiguration was not successful

    cameraComPort.baudrate(115200); // change UART baud rate

    /* specify image quality */
    toCam = newCameraPacket(); // initialize packet
    toCam.cmdToken = QUALITY;  // configure picture quality
    toCam.parameter1 = 0x00;   // to be the best
    sendPacket(toCam);

    /* if camera acknowledges change, then get an image */
    fromCam = getPacket();
    if(!(fromCam.cmdToken == ACK && fromCam.parameter1 == QUALITY))
        return 4;// if reconfiguration was not successful

    /* get an image */
    toCam = newCameraPacket(); // initialize packet
    toCam.cmdToken = GET_PIC;  // configure picture quality
    toCam.parameter1 = 0x05;   // get full size image
    sendPacket(toCam);

    /* if camera acknowledges request, then retrieve image data */
    fromCam = getPacket();
    if(!(fromCam.cmdToken == ACK && fromCam.parameter1 == GET_PIC))
        return 5; // if request was not successful

    /* get image size */
    fromCam = getPacket();
    if(!(fromCam.cmdToken == DATA))
        return 6; // if request was not successful

    /* read data size */
    picsize = bytesToGet = fromCam.parameter2 + fromCam.parameter3 * 0x100LL + fromCam.parameter4 * 0x10000LL;

    int cnt = 0;

    /* store the image */
    while(bytesToGet > 0) // until all bytes retrieved
    {
        tempSize = cameraComPort.receive(tempBuffer, RX_BUFFER_SIZE); // receive the bytes
        //imageFile.write(tempBuffer,tempSize); // store the bytes
        FSfwrite(tempBuffer, sizeof(char), tempSize, imageFile);
        bytesToGet -= tempSize; // update bytes remaining

        cnt = tempSize ? 0: cnt + 1;

        if(cnt > 200)
            break;
    }

    /* acknowledge that data was received */
    toCam = newCameraPacket(); // initialize packet
    toCam.cmdToken = ACK;      // notify the camera of successful
    toCam.parameter1 = DATA;   // data retrieval
    /* not needed */
    //sendPacket(toCam);
    pause(30);
    return FALSE; // successful

}

static void setPowerOutput(Boolean desiredOutputState)
{
    _TRISE1 = 0;    /* configure port as output */
    _RE1 = !!desiredOutputState; /* set the output (active high) */
}

static void turnOff(void)
{
    setPowerOutput(OFF);   // turn module power off
}

static void turnOn(void)
{
    setPowerOutput(ON);   // turn module power on
}

static void initialize(void)
{
    setPowerOutput(OFF);
    cameraComPort = uart1;
    imageFile = NullPtr;
}

static int retrievePic(String imgName)
{
    setPowerOutput(ON);
    /**
     * Increasing Power On Period
     *
     * Although the data sheet says that the camera needs ~1.5 sec to start-up,
     * in practice 2 sec makes for a much more reliable situation.
     */
    wait(2000);//1500);

    // initialize camera and image storage
    cameraComPort.init();
    cameraComPort.baudrate(14400);
    //imageFile = getFileStream();
    imageFile = FSfopen(imgName, "w");
    int error = -1;

    //if(imageFile.open)
    if(imageFile)
    {
        //FSfclose(FSfopen(imgName, "w")); // erase file
        //imageFile.open(imgName); // open file
        error = getPicture();
        FSfclose(imageFile);
        imageFile = NullPtr;
        //imageFile.close();
    }

    //imageFile.free();

    setPowerOutput(OFF);
    wait(1000);

    return error;
}

Boolean getStatus(void)
{
    return TRUE;
}

const Camera camera = {
    getPix:retrievePic,
    on:turnOn,
    off:turnOff,
    init:initialize,
    isOk:getStatus
};

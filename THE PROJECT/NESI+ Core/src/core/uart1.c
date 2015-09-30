/**
 * NESI COMM Port UART 1 Driver License
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
#include "uart1.h"

/**
 * BUFFER_SIZE Information
 *
 * BUFFER_SIZE defines the size of buffers used throughout the serial module.
 */
#define RX_BUFFER_SIZE    0x0800
#define TX_BUFFER_SIZE    0x0040

/**
 * TX_buffer Information
 *
 * The TX_buffer holds bytes that are waiting to be sent out the UART module.
 * This global circular buffer is serviced in the background via interrupts.
 * The buffer is indexed via TX_head and TX_next.
 */
static Byte TX_buffer[TX_BUFFER_SIZE] = {0};
static volatile Uint16 TX_head = 0;
static volatile Uint16 TX_next = 0;

/**
 * RX_buffer Information
 *
 * The RX_buffer holds bytes that have been received by the UART module. This
 * global circular buffer is serviced in the background via interrupts. The
 * buffer is indexed via RX_head and RX_next.
 */
static Byte RX_buffer[RX_BUFFER_SIZE] = {0};
static volatile Uint16 RX_head = 0;
static volatile Uint16 RX_next = 0;

/**
 * initUart() Information
 *
 * This function initializes the UART module.
 */
static void initUart(void)
{
    _U1RXIE = 0; // disable UART Rx interrupt
    _U1TXIE = 0; // disable UART Tx interrupt

    U1MODEbits.UARTEN = OFF;  // disable the UART module while configuring
    U1MODEbits.USIDL = OFF;   // do not stop in idle mode
    U1MODEbits.IREN = OFF;    // disable IrDA encode and decode
    U1MODEbits.UEN = 0b00;    // manual hardware flow control
    U1MODEbits.WAKE = ON;     // wake up on detection of start bit
    U1MODEbits.LPBACK = OFF;  // disable loopback mode
    U1MODEbits.ABAUD = OFF;   // disable baud rate auto-detect
    U1MODEbits.RXINV = OFF;   // do not invert Rx idle state
    U1MODEbits.BRGH = 1;      // high speed baud rate
    U1MODEbits.PDSEL = 0b00;  // 8-bit data, no parity
    U1MODEbits.STSEL = 0;     // 1 stop bit
    U1STAbits.UTXISEL1 = 0b1; // interrupt when Tx buffer is empty
    U1STAbits.UTXISEL0 = 0b0; // interrupt when Tx buffer is empty
    U1STAbits.UTXINV = OFF;   // do not invert Tx idle bit*/
    U1STAbits.UTXBRK = OFF;   // disable sync break transmission
    U1STAbits.UTXEN = YES;    // enable UART transmitter
    U1STAbits.URXISEL = 0b00; // interrupt when character received
    U1STAbits.OERR = 0;       // clear Rx overrun error flag

    U1BRG = ((FCY/9600)/4) - 1; // calculate value for baud register

    /* configure pin direction */
    _TRISG8 = 0; // TX -> output
    _TRISG7 = 1; // RX -> input

    /* map modules to pins */
    __builtin_write_OSCCONL(OSCCON & 0xBF); // unlock peripheral pin select registers
    _RP19R = 3;                             // Remmapable pin 19 is UART1 transmit
    _U1RXR = 26;                            // UART1 receive mapped to remmapable pin 26
    __builtin_write_OSCCONL(OSCCON | 0x40); // lock peripheral pin select registers

    /* clear interrupts so they don't immediately interrupt */
    _U1RXIF = OFF; // clear Rx interrupt flag
    _U1TXIF = OFF; // clear Tx interrupt flag

    _U1RXIP = 6; // set Rx interrupt priority
    _U1TXIP = 4; // set Tx interrupt priority

    U1MODEbits.UARTEN = ON; // enable the UART module now that its configured

    /* Note: UART Tx cannot be enabled until the UART module itself has been enabled. */
    U1STAbits.UTXEN = YES; // enable UART transmitter

    _U1RXIE = 1; // enable UART Rx interrupt
}

/**
 * getTxSize() Information
 *
 * This function returns the number of bytes in TX buffer.
 */
inline static Uint16 getTxSize(void)
{
    return  TX_next - TX_head
            + (TX_head > TX_next ? TX_BUFFER_SIZE: 0);
}

/**
 * getRxSize() Information
 *
 * This function returns number of bytes in RX buffer.
 */
inline static Uint16 getRxSize(void)
{
    return  RX_next - RX_head
            + (RX_head > RX_next ? RX_BUFFER_SIZE: 0);
}

/**
 * getTxSpace() Information
 *
 * This function returns the room (bytes) remaining TX buffer.
 */
inline static Uint16 getTxSpace(void)
{
    return  TX_head - TX_next - 1
            + (TX_head <= TX_next ? TX_BUFFER_SIZE: 0);
}

/**
 * UART Receive Interrupt
 *
 * As the microcontroller receives data from the connected peripheral, it will
 * be queued in order to be processed and interpreted later.
 */
_ISR_ _U1RXInterrupt(void)
{

    while(U1STAbits.URXDA) // while data is available
    {
        RX_buffer[RX_next++] = U1RXREG; // copy byte to buffer

        RX_next %= RX_BUFFER_SIZE; // protect against rollover

        /* accommodate overflow */
        if(RX_head == RX_next)
            RX_head = (RX_head + 1) % RX_BUFFER_SIZE;
    }

    _U1RXIF = OFF; // clear Rx interrupt flag
}

/**
 * clearRx() Information
 *
 * This function empties the receive buffer.
 */
inline static void clearRx(void)
{
    RX_head = RX_next;
}

/**
 * UART Transmit Interrupt
 *
 * As the microcontroller is able to sent data to the connected peripheral, it
 * is sent out an removed from the transmit queue until data. This process is
 * taken care of in the background by the interrupt. To send data, add it to
 * the transmit queue.
 */
_ISR_ _U1TXInterrupt (void)
{
    _U1TXIF = OFF; // clear Tx interrupt flag

    /* transmit if transmit queue has room and buffer is not empty */
    while (!(U1STAbits.UTXBF) && (TX_head != TX_next))
    {
        U1TXREG = TX_buffer[TX_head++];
        TX_head %= TX_BUFFER_SIZE;
    }
}

/**
 * sendFrom() Information
 *
 * @Purpose
 *     Sends the specified number of bytes from the pointed to string to the
 *     connected peripheral.
 * @Parameters
 *     startOfData: pointer to string of bytes to send
 *     bytesToSend: number of bytes to send from the byte array
 * @Returns
 *     Boolean indicating weather or not bytes were sent to the connected
 *     peripheral.
 */
static Boolean sendFrom(const Byte* startOfData, Uint16 bytesToSend)
{
    /* can't send nonexistent data or no data */
    if (!startOfData || !bytesToSend)
        return FALSE;

    /* make sure room is available in buffer */
    while(getTxSpace() < bytesToSend);

    /* pause background sending of data */
    _U1TXIE = 0; // disable UART Tx interrupt

    /* send bytes while bytes still need to be sent */
    while(bytesToSend--)
    {
        TX_buffer[TX_next++] = *startOfData;

        /* circular queue rollover protection */
        TX_next %= TX_BUFFER_SIZE;

        /* accommodate overflow */
        if(TX_head == TX_next)
            TX_head = (TX_head + 1) % TX_BUFFER_SIZE;

        ++startOfData;
    }

    /* continue with background transmitting */
    _U1TXIE = YES; // enable UART Tx interrupt

    /* force transmit process to start if haven't started already */
    _U1TXIF = YES; // trigger UART Tx interrupt

    return TRUE;
}

/**
 * startRx() Information
 *
 * This function starts the UART receive process.
 */
inline static void startRx(void)
{
    clearRx();   // empty the Rx buffer
    _U1RXIE = 1; // enable Rx interrupt
}

/**
 * setBaudRate() Information
 *
 * This function configures the UART baud rate.
 */
inline static void setBaudRate(Uint64 desiredBaudRate)
{
    U1BRG = ((FCY/desiredBaudRate)/4) - 1;  // calculate value for baud register
}

/**
 * copyFrom() Information
 *
 * @Purpose
 *     Reads the specified number of bytes from the receive buffer and stores them at
 *     the pointed to memory location.
 * @Parameters
 *     startOfStorage: pointer to location to store string of bytes received
 *     maxBytesToRead: maximum number of bytes to copy from the byte array
 * @Returns
 *     The number of bytes copied from the receive buffer.
 */
static int copyFrom(Byte* startOfStorage, int maxBytesToRead)
{
    if(!startOfStorage)
        return 0;

    _U1RXIE = 0;    /* disable UART Rx interrupt */

    int head = RX_head,
        next = RX_next,
        count = 0;

    while(maxBytesToRead-- && (head != next))
    {
        startOfStorage[count++] = RX_buffer[head++];
        head %= RX_BUFFER_SIZE;
    }

    _U1RXIE = 1;    /* enable UART Rx interrupt */

    return count;
}

/**
 * readFrom() Information
 *
 * @Purpose
 *     Retrieves the specified number of bytes from the receive buffer and
 *      stores them at the pointed to memory location.
 * @Parameters
 *     startOfStorage: pointer to location to store string of bytes received
 *     maxBytesToRead: maximum number of bytes to read from the byte array
 * @Returns
 *     The number of bytes read from the receive buffer.
 */
static int receiveFrom(Byte* startOfStorage, int maxBytesToRead)
{
    if(!startOfStorage)
        return 0;

    int count = 0;

    while(maxBytesToRead-- && (RX_head != RX_next))
    {
        startOfStorage[count++] = RX_buffer[RX_head++];
        RX_head %= RX_BUFFER_SIZE;
    }

    return count;
}

/**
 * dropFrom() Information
 *
 * @Purpose
 *     Drops the specified number of bytes from the receive buffer. Used with
 *     copyFrom() to get data from the receive buffer.
 * @Parameters
 *     bytesToDrop: number of bytes to drop from the receive buffer
 * @Returns
 *     nothing
 */
static void dropFrom(int bytesToDrop)
{
    if(bytesToDrop <= 0)
        return;
    _U1RXIE = 0;    /* disable UART Rx interrupt */
    RX_head = (RX_head + bytesToDrop) % RX_BUFFER_SIZE;
    if(RX_head > RX_next)
        RX_head = RX_next;
    _U1RXIE = 1;    /* enable UART Rx interrupt */
}

/**
 * flush() Information
 *
 * @Purpose
 *     Drops all bytes from the receive buffer.
 * @Parameters
 *     none
 * @Returns
 *     nothing
 */
static void flush(void)
{
     RX_head = RX_next;
}

/**
 * uart1 Information
 *
 * uart1 is the software wrapper for the first UART module available on the
 * NESI board.
 */
const Uart uart1 = {
    baudrate:setBaudRate,
    copy:copyFrom,
    drop:dropFrom,
    flush:flush,
    size:getRxSize,
    send:sendFrom,
    init:initUart,
    receive:receiveFrom
};

#ifndef _HARDWAREPROFILE_MDD_H
#define _HARDWAREPROFILE_MDD_H

// Sample clock speed for a 16-bit processor
#define GetSystemClock()        32000000
#define GetPeripheralClock()    GetSystemClock()
#define GetInstructionClock()   (GetSystemClock() / 2)

// Clock values
#define MILLISECONDS_PER_TICK   10                 // Definition for use with a tick timer
#define TIMER_PRESCALER         TIMER_PRESCALER_8  // Definition for use with a tick timer
#define TIMER_PERIOD            20000              // Definition for use with a tick timer


// Select your interface type
// This library currently only supports a single physical interface layer at a time

// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI


/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/* SD Card definitions: Change these to fit your application when using
   an SD-card-based physical layer                                   */

// Description: SD-SPI Chip Select Output bit
//#define SD_CS               LATDbits.LATD9  /* secondary */
#define SD_CS               LATDbits.LATD2  /* primary */
// Description: SD-SPI Chip Select TRIS bit
//#define SD_CS_TRIS          TRISDbits.TRISD9  /* secondary */
#define SD_CS_TRIS          TRISDbits.TRISD2  /* primary */
// Description: SD-SPI Card Detect Input bit
//#define SD_CD               PORTDbits.RD8  /* secondary */
#define SD_CD               PORTDbits.RD7  /* primary */
// Description: SD-SPI Card Detect TRIS bit
//#define SD_CD_TRIS          TRISDbits.TRISD8  /* secondary */
#define SD_CD_TRIS          TRISDbits.TRISD7  /* primary */

// Description: SD-SPI Write Protect Check Input bit
#define SD_WE               PORTEbits.RE5
// Description: SD-SPI Write Protect Check TRIS bit
#define SD_WE_TRIS          TRISEbits.TRISE5

// Registers for the SPI module you want to use

// Description: The main SPI control register
#define SPICON1             SPI1CON1
// Description: The SPI status register
#define SPISTAT             SPI1STAT
// Description: The SPI Buffer
#define SPIBUF              SPI1BUF
// Description: The receive buffer full bit in the SPI status register
#define SPISTAT_RBF         SPI1STATbits.SPIRBF
// Description: The bitwise define for the SPI control register (i.e. _____bits)
#define SPICON1bits         SPI1CON1bits
// Description: The bitwise define for the SPI status register (i.e. _____bits)
#define SPISTATbits         SPI1STATbits
// Description: The enable bit for the SPI module
#define SPIENABLE           SPISTATbits.SPIEN

// Tris pins for SCK/SDI/SDO lines

// Description: The TRIS bit for the SCK pin
#define SPICLOCK            TRISDbits.TRISD10
// Description: The TRIS bit for the SDI pin
#define SPIIN               TRISDbits.TRISD11
// Description: The TRIS bit for the SDO pin
#define SPIOUT              TRISDbits.TRISD0


// Will generate an error if the clock speed is too low to interface to the card
#if (GetSystemClock() < 100000)
    #error Clock speed must exceed 100 kHz
#endif

#endif /* _HARDWAREPROFILE_MDD_H */

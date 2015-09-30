#ifndef HARDWARE_PROFILE_USB_H
#define HARDWARE_PROFILE_USB_H

/*******************************************************************/
/******** USB stack hardware selection options *********************/
/*******************************************************************/
//This section is the set of definitions required by the MCHPFSUSB
//  framework.  These definitions tell the firmware what mode it is
//  running in, and where it can find the results to some information
//  that the stack needs.
//These definitions are required by every application developed with
//  this revision of the MCHPFSUSB framework.  Please review each
//  option carefully and determine which options are desired/required
//  for your application.

//#define USE_SELF_POWER_SENSE_IO
#define tris_self_power     TRISAbits.TRISA2    // Input
#define self_power          1

//#define USE_USB_BUS_SENSE_IO
#define tris_usb_bus_sense  TRISBbits.TRISB5    // Input
#define USB_BUS_SENSE       1

//Uncomment this to make the output HEX of this project
//   to be able to be bootloaded using the HID bootloader
//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER

//If the application is going to be used with the HID bootloader
//  then this will provide a function for the application to
//  enter the bootloader from the application (optional)
#if defined(PROGRAMMABLE_WITH_USB_HID_BOOTLOADER)
    #define EnterBootloader() __asm__("goto 0x400")
#endif

#endif  /* end of HARDWARE_PROFILE_USB_H */

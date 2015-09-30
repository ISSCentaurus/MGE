/**
 * NESI Button Driver License
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
#include "button.h"

/**
 * readInput() Information
 *
 * This function returns true when the button is pressed.
 */
static Boolean readInput(void)
{
    _TRISE4 = 1;  // configure port as input
    return !_RE4; // return the inverse value on the pin (active low)
}

/**
 * initialize() Information
 *
 * This function initializes the button module.
 */
static void initialize(void)
{
    /**
     * Overview
     *
     * Timer 5 will be used as the keystroke timeout timer. It will generate
     * an interrupt X milliseconds after it is started (@setTimeout(X)). The
     * timer will be started when a change in the button's state is detected,
     * and stopped automatically at the end of the timeout period. The keystroke
     * is determined by the number of times the button was pressed during the
     * timeout period.
     */

    /**
     * Timer5 Configuration Setup
     *
     * Timer5 will start off and be started once the button has been pressed
     * The prescaler will be set to 1:256 so that the period is of a reasonable
     * value and resolution. The internal system oscillator will be used.
     * Gating mode and synchronous operation will not be used.
     */
    T5CON = 0x0030;  // disabled, prescaled, internal oscillator
    TMR5 = 0;        // clear the timer

    /**
     * Timer5 Period Register Setup
     *
     * The period register contains the number of counts before Timer5 resets.
     * For 0.5s the count will be half that of the prescaled clocks per second.
     *
     * Note: The count begins at 0 (rather than 1), so 1 must be subtracted
     *       from the desired value.
     */
    PR5 = (Uint16)((CLOCKS_PER_SEC/256)/2 - 1);  // interrupt 1/2 second later

    /**
     * Timer5 Interrupt Setup
     *
     * Interrupts are required, so the interrupt enable bit must be set, and
     * the interrupt flag must be cleared. Also, the process has normal priority.
     */
    _T5IE = ON;   // enable Timer5 to interrupt
    _T5IF = OFF;  // clear the interrupt flag
    _T5IP = 3;    // set to sub normal priority

    /**
     * Change-of-State Notification Interrupt Setup
     *
     * The Change-of-State Notifier interrupts whenever a digital pin changes
     * its logical state. Interrupts are required, so the interrupt enable bit
     * must be set, and the interrupt flag must be cleared. The priority of the
     * detection process is not high, so it is assigned normal priority.
     */
    _CN62IE = ON;      // enable Interrupt on Change 62 to interrupt
    _CNIF   = OFF;     // clear the Interrupt on Change interrupt flag
    _CNIE   = ON;      // enable the Interrupt on Change interrupt
    _CNIP   = 4;       // set the Interrupt on Change interrupt priority to 4

    /**
     * Correct Port Mismatching
     *
     * The PORT register contains the instantaneous value present at the pin,
     * while the LAT register is latched at clock cycles. To be sure that the
     * ports match, and prevent a misfire the port is read to clear any
     * mismatching.
     */
    _LATE4 = _RE4; // read port to clear mismatching (for Change On Interrupt)
}

/**
 * Keeping Track of Button Presses
 *
 * The keystroke is determined by counting the number of times that the button
 * is pressed within the timeout period. pressCount is incremented each time
 * the button is pressed and released. At the end of the timeout, pressCount is
 * converted to a keystroke.
 */
static Uint pressCount = 0;

/**
 * Interrupt on Change Interrupt Service Routine
 *
 * The Change Notification ISR is called whenever a state change is detected on
 * one of the enabled pins. This is used to determine what actions are being
 * performed on the switch / push button.
 */
_ISR_ _CNInterrupt(void)
{
    /**
     * Track Changes in Pin State
     *
     * A persistent variable is used to keep track of the previous state of the
     * push button. This is used to determine the type of keystroke.
     */
    static Uint oldValue = 1; // active low button should start off high

    /**
     * Button Debouncing
     *
     * When the button is pressed, it takes a while for the mechanical connection
     * to stabilize. During this period the pin reading will be sporadic and
     * unreliable. Before taking a reading, the pin will be allowed to stabilize.
     */
    delay(50); // allow 50 ms for the pin to stabilize
    Uint currentValue = _RE4; // store the current value of the button

    /**
     * Determining Next Action
     *
     * Different actions are performed depending on how the button was pressed
     * and which events are detected. The sequence of events are used to
     * determine the next action, as well as type of keystroke performed.
     */
    if(currentValue == oldValue) // if no change occurred (false alarm)
    {
        _CNIF = OFF; // clear the interrupt flag and
        return;      // return (exit)
    }
    else if (currentValue == 0) // else, if button pressed
    {
        if(!T5CONbits.TON) // if the timeout has not started
        {
            TMR5 = 0;           // clear the timeout counter
            pressCount = 0;     // clear the stroke count
            _T5IE = ON;         // enable Timer5 to interrupt
            T5CONbits.TON = ON; // begin the timeout sequence
        }
    }
    else
    {
        /**
         * Button Press Accumulator
         *
         * If the state of the button pin has changed but the button is not
         * down, it was released. When the button is released the accumulator
         * is incremented. At the end of the timeout period the, accumulator
         * is used to determine the type of keystroke.
         */
        ++pressCount; // accumulate the number of button taps
    }

    /**
     * Finished Processing Pin Changes
     *
     * Now that the button pin state change has been processed, the current
     * value will be the old value in respect to the next iteration.
     */
    oldValue = currentValue;

    /**
     * Finished with CN ISR
     *
     * All that is left to do is to clear the interrupt flag so that the
     * button can be detected if pressed again.
     */
    _CNIF = OFF;

}

/**
 * latestKeystroke Information
 *
 * This variable stores the most recent keystroke that was detected. It is
 * cleared automatically by the module when read.
 */
static Keystroke latestKeystroke = KEYSTROKE_NONE;

/**
 * getKeystroke() Information
 *
 * This function clears the keystroke buffer and returns the most recent
 * keystroke that was detected.
 */
static Keystroke getKeystroke(void)
{
    static Keystroke temp = KEYSTROKE_NONE;

    temp = latestKeystroke;
    latestKeystroke = KEYSTROKE_NONE;
    return temp;
}

/**
 * enableKeystroke() Information
 *
 * This function enables the detection of keystrokes.
 */
static void enableKeystroke(void)
{
    _CN62IE = ON; // enable Interrupt on Change 62 to interrupt
    _CNIF = OFF;  // clear the Interrupt on Change interrupt flag
    _CNIE = ON;   // enable the Interrupt on Change interrupt
}

/**
 * disableKeystroke() Information
 *
 * This function disables the detection of keystrokes.
 */
static void disableKeystroke(void)
{
    _CN62IE = OFF;        // disable Interrupt on Change 62 from interrupting
    _CNIF = OFF;          // clear the Interrupt on Change interrupt flag
    _CNIE = OFF;          // disable the Interrupt on Change interrupt
    _T5IE = OFF;          // disable Timer5 from interrupt
    _T5IF = OFF;          // clear the interrupt flag
    TMR5 = 0;             // clear the timeout counter
    T5CONbits.TON = OFF;  // stop the timeout sequence

    latestKeystroke = KEYSTROKE_NONE;
}

/**
 * Timer 5 (T5) Interrupt Service Routine
 *
 * T5 is used as set the keystroke detection period. Once T5 expires, this ISR
 * is called and the keystroke is determined based on the number of times the
 * button has been pressed.
 */
_ISR_ _T5Interrupt(void)
{
    /**
     * Finished with Timer
     *
     * The timer is no longer needed now that the keystroke period is over, so
     * it is stopped and its count value is reset.
     */
    T5CONbits.TON = OFF;
    TMR5 = 0;

    /**
     * Keystroke Assignment
     *
     * The keystroke is assigned based on the number of times the button was
     * pressed during the keystroke timeout period.
     */
    switch(pressCount)
    {
        case 0:
            latestKeystroke = KEYSTROKE_NONE;
            break;
        case 1:
            latestKeystroke = KEYSTROKE_SINGLE;
            break;
        case 2: default:
            latestKeystroke = KEYSTROKE_DOUBLE;
            break;
    }


    /**
     * Finished with ISR
     *
     * The keystroke has been determined, so the accumulator is reinitialized
     * and the ISR is reset.
     */
    pressCount = 0; // reset accumulator

    _T5IE = OFF; // disable Timer5 from interrupt
    _T5IF = OFF; // clear the interrupt flag

}

/**
 * button Information
 *
 * button is the software wrapper for the NESI push button.
 */
const Button button = {
    isPressed:readInput,
    init:initialize,
    getStroke:getKeystroke,
    enableStroke:enableKeystroke,
    disableStroke:disableKeystroke
};

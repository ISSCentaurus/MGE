/**
 * NESI PowerDriver Driver License
 * Copyright (C) 2013 Dakotah Karrer
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
 * 09/24/2013 - Dakotah Karrer
 *   Migrated from solenoid software module.
 * 11/27/2013 - Mickie Byrd
 *   Added isOk() to make consistent with other NESI modules.
 */

#include "system.h"
#include "powerDriver.h"

/******************************************************
 *
 * Power Driver A
 *
 ******************************************************/

static void setOutputA(Boolean desiredOutputState)
{
    _TRISE7 = 0;    /* configure port as output */
    _RE7 = desiredOutputState; /* set the output */

}static void setOnA(void)
{
    setOutputA(1); /* set the output to be open */
}

static void setOffA(void)
{
    setOutputA(0); /* set the output to be closed */
}

static void initializeA(void)
{
    /* no initialization needed*/
}

static Boolean readInputA(void)
{
    _TRISG6 = 1; /* configure port as input */
    return _RG6; /* return the value on the pin */
}

static Boolean failureA(void)
{
    Boolean error = 0;      /* Initialize variable error  to 0 */
    powerDriverA.on();      /* Turns powerDriverA on */
    if( powerDriverA.status()== 1 )
    {                    /* Error if powerDriverA  is on and status = 1 */
        error = 1;       /** Note: something must be plugged into JP2 **/
    }
    powerDriverA.off();     /*Turns powerDriverA off*/

    if(powerDriverA.status()==0)
    {                               /* Error if powerDriverA is off and status = 0 */
        error = 1;                  /** Note: something must be plugged into JP2 **/
    }

    return error;       /* If error = 0 then nothing is wrong. If error = 1 then something is wrong*/
}

static Boolean isOkA (void)
{
    return !failureA();
}



/******************************************************
 *
 * Power Driver B
 *
 ******************************************************/


static void setOutputB(Boolean desiredOutputState)
{
    _TRISE0 = 0;    /* configure port as output */
    _RE0 = desiredOutputState; /* set the output */

}static void setOnB(void)
{
    setOutputB(1); /* set the output to be open */
}

static void setOffB(void)
{
    setOutputB(0); /* set the output to be closed */
}

static void initializeB(void)
{
   /* no Initialization needed*/
}

/*
   Warning:
      Not an actual test of InputB
 */
static Boolean readInputB(void)
{
    return 1; /* Assumption that powerDriverB is working */
}

static Boolean failureB (void)
{
    Boolean error = 0;      /* No way of testing error so set error = 0 */
    return error;       /* Note: Assumption that powerDriverB is working */
}

static Boolean isOkB (void)
{
    return !failureB();
}

const PowerDriver powerDriverA = {set:setOutputA, isFailure:failureA, status:readInputA, init:initializeA, on:setOnA, off:setOffA, isOk:isOkA},
                  powerDriverB = {set:setOutputB, isFailure:failureB, status:readInputB, init:initializeB, on:setOnB, off:setOffB, isOk:isOkB};

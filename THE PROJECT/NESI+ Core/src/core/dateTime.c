/**
 * NESI DateTime Module License
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
 * 12/19/2013 - Mickie Byrd
 *   Added .toStamp() and .parseStamp() to handle time stamp string processing.
 *   Added .new() member to be used for initializing DateAndTime variables.
 * 12/20/2013 - Mickie Byrd
 *   Modified .toStamp() to take DateAndTime instead of CalendarAndClock.
 * 12/24/2013 - Mickie Byrd
 *   Added .compare() to be used for comparing two DateAndTime objects.
 * 12/26/2013 - Mickie Byrd
 *   Modified parseStamp() to return invalid DateAndTime if error occurs.
 *   Reassigned .new() to toDateAndTime() so that parameters can be used to
 *       initialize returned DateAndTime object.
 *   Added .add() and .sub() to be used to add and subtract DateAndTime
 *       objects.
 *   Refactored .compare() to .cmp() for consistency.
 * 12/27/2013 - Mickie Byrd
 *   Modified .new() to newDateAndTime() which creates DateAndTime without
 *       normalizing value.
 *   Added .to() which does what .new() used to do.
 *   Modified dateAndTimeToStamp() to convert without normalization.
 * 12/30/2013 - Mickie Byrd
 *   Commented out setSystemClock(), setSystemCalendar(), toCalendarAndClock(),
 *       and toTimeStamp() to suppress compiler warnings.
 */

#include "system.h"
#include "dateTime.h"
#include <stdio.h>

/**
 * getSystemHalfSeconds() Information
 *
 * This macro returns the number of half seconds.
 */
#define getSystemHalfSeconds()    (RTC_HALF_SECONDS)

typedef struct {
    signed seconds;
    signed tensOfSeconds;
    signed minutes;
    signed tensOfMinutes;
    signed hours;
    signed tensOfHours;
} Clock;

typedef struct {
    signed weekday;
    signed days;
    signed tensOfDays;
    signed months;
    signed tensOfMonths;
    signed years;
    signed tensOfYears;
} Calendar;

typedef struct {
    union {
        Clock clock;
        struct {
            signed seconds;
            signed tensOfSeconds;
            signed minutes;
            signed tensOfMinutes;
            signed hours;
            signed tensOfHours;
        };
    };
    union {
        Calendar calendar;
        struct {
            signed weekday;
            signed days;
            signed tensOfDays;
            signed months;
            signed tensOfMonths;
            signed years;
            signed tensOfYears;
        };
    };
} CalendarAndClock;

typedef union {
    struct {
        unsigned long timeBits :24;
        unsigned long dateBits :32;
        unsigned long          :0;
    };
    struct {
      unsigned int minuteSecondRegister;
      unsigned int weekdayHourRegister;
      unsigned int monthDayRegister;
      unsigned int yearRegister;
    };
    struct {
      unsigned char seconds       :4;
      unsigned char tensOfSeconds :3;
      unsigned char               :1;
      unsigned char minutes       :4;
      unsigned char tensOfMinutes :3;
      unsigned char               :0;

      unsigned char hours         :4;
      unsigned char tensOfHours   :2;
      unsigned char               :2;
      unsigned char weekday       :3;
      unsigned char               :0;

      unsigned char days          :4;
      unsigned char tensOfDays    :2;
      unsigned char               :2;
      unsigned char months        :4;
      unsigned char tensOfMonths  :1;
      unsigned char               :0;

      unsigned char years         :4;
      unsigned char tensOfYears   :4;
      unsigned char               :0;
    };
} RTCCRegister;

/**
 * Oscillator Macros
 *
 * The RTCC (Real Time Clock and Calendar) utilities the 32.768 kHz oscillator
 * for its time base. Before the secondary oscillator can be used though, it
 * must first be enabled. This requires the execution of a special unlock
 * sequence. The same goes for disabling it also. To simplify this process and
 * clarify the intent of the unlock statement, the built-in instructions are
 * encapsulated into macros.
 */
#define enableSecondaryOscillator()     (__builtin_write_OSCCONL(OSCCON | 0x02))
#define disableSecondaryOscillator()    (__builtin_write_OSCCONL(OSCCON & 0xFD))

#define unlockRealTimeClockRegister()   (__builtin_write_RTCWEN())
#define lockRealTimeClockRegister()     (_RTCWREN = 0)

/**
 * RTC_X Defines
 *
 * These defines are used to configure the RTC Module.
 */
#define YEAR_POINTER                (0b11)
#define MONTH_DAY_POINTER           (0b10)
#define WEEKDAY_HOUR_POINTER        (0b01)
#define MINUTE_SECOND_POINTER       (0b00)

#define RTC_HALF_SECONDS            (_HALFSEC)

#define RTC_INTERRUPT_ENABLE        (_RTCIE)
#define REPEAT_RTC_ALARM_FOREVER    (_CHIME)
#define RTC_POINTER                 (_RTCPTR)
#define RTC_VALUE_REGISTER          (RTCVAL)
#define RTC_ALARM_INTERVAL          (_AMASK)
#define RTC_INTERRUPT_PRIORITY      (_RTCIP)
#define RTC_ENABLE                  (_RTCEN)
#define RTC_ALARM_ENABLE            (_ALRMEN)
#define RTC_OUTPUT_PIN_ENABLE       (_RTCOE)
#define RTC_INTERRUPT_FLAG          (_RTCIF)


static inline Calendar newCalendar(void)
{
    static Calendar temp = {.months = JANUARY, .weekday = SATURDAY, .days = 1};
    return temp;
}

static inline Clock newClock(void)
{
    static Clock temp = {0};
    return temp;
}

static inline CalendarAndClock newCalendarAndClock(void)
{
    static CalendarAndClock temp = {{{0}},{{.months = JANUARY, .weekday = SATURDAY, .days = 1}}};
    return temp;
}

/**
 * calculateWeekday Information
 *
 * @Purpose
 *     Calculates the weekday corresponding to the passed calendar.
 *
 * @Parameters
 *     @param month An unsigned integer representing the month. Alternatively, a
 *       value of the type Month, can be passed as the first parameter.
 *     @param day An unsigned integer representing the day. Make sure the day is
 *       valid for the month (parameter 1).
 *     @param year An unsigned one or two-digit integer representing the year. The
 *       year must be less than 100.
 *
 * @Returns
 *     The weekday as an unsigned integer. The return value will range from
 *     0 - 6 (Sunday - Saturday, respectively). Alternatively, the return
 *     value can be interpreted as a Weekday type.
 *
 * @Limitations
 *     calculateWeekday is limited to dates ranging from January 1, 2000 to
 *     December 31, 2099.
 *
 * @Warning
 *     All values are assumed to be valid, and no checks are done on the
 *     parameters. Invoking this function with an invalid date will yield an
 *     incorrect weekday value. Remember, trash in ... trash out.
 *
 * @References
 *     The algorithm implemented here is referenced from Wikipedia. The URL
 *     is http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week
 *     #An_algorithm_to_calculate_the_day_of_the_week
 */
static unsigned calculateWeekday(unsigned year, unsigned month, unsigned day)
{
    /**
     * Weekday Algorithm
     *
     * The following algorithm is used to calculate the weekday:
     *  1) Look up century number (it is '6' for 2000 - 2099).
     *  2) Note the last two digits of the year.
     *  3) Integer divide the last two digits of the year by 4.
     *  4) Look up the month number in the month table.
     *     a) During leap years, subtract 1 from January's and February's number
     *  5) Add the numbers from steps 1-4 to the day of the month.
     *  6) Modulo 7 the result of step 5 (this is the day of the week).
     */
    static const int monthsTable[] = {0,3,3,6,1,4,6,2,5,0,3,5};

    if((year & 0b11) || (month < 3))
        return (6 + year + (year >> 2) + monthsTable[month - 1] + day) % 7;
    else
        return (6 + year + (year >> 2) + monthsTable[month - 1] + day - 1) % 7;

}

#define getDaysInMonth(m,y) ((m == 2) && !(y & 0b11)? 29: daysInMonth[m])
static const unsigned daysInMonth[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

/**
 * numbersToClock Information
 *
 * @Purpose
 *     Converts the passed values to a simplified, correct Clock variable.
 * @Parameters
 *     @param tensOfHours      hour's tens digit
 *     @param hours            hour's ones digit
 *     @param tensOfMinutes    minute's tens digit
 *     @param minutes          minute's ones digit
 *     @param tensOfSeconds    second's tens digit
 *     @param seconds          second's ones digit
 * @Returns
 *     <code>Clock</code> object representing the passed clock
 */
static Clock numbersToClock(int tensOfHours,    int hours,
                            int tensOfMinutes,  int minutes,
                            int tensOfSeconds,  int seconds)
{
    /* variable used to store values temporarily during calculations */
    int extra, tempHours;

    if(seconds > 9)
    {
        extra = seconds / 10;
        tensOfSeconds += extra;
        seconds = seconds - extra * 10;
    }
    else if (seconds < 0)
    {
        extra = seconds / 10 - 1;
        tensOfSeconds += extra;
        seconds = seconds - extra * 10;
    }

    if (tensOfSeconds > 5)
    {
        extra = tensOfSeconds / 6;
        minutes += extra;
        tensOfSeconds = tensOfSeconds - extra * 6;
    }
    else if (tensOfSeconds < 0)
    {
        extra = tensOfSeconds / 6 - 1;
        minutes += extra;
        tensOfSeconds = tensOfSeconds - extra * 6;
    }

    if (minutes > 9)
    {
        extra = minutes / 10;
        tensOfMinutes += extra;
        minutes = minutes - extra * 10;
    }
    else if (minutes < 0)
    {
        extra = minutes / 10 - 1;
        tensOfMinutes += extra;
        minutes = minutes - extra * 10;
    }

    if (tensOfMinutes > 5)
    {
        extra = tensOfMinutes / 6;
        hours += extra;
        tensOfMinutes = tensOfMinutes - extra * 6;
    }
    else if (tensOfMinutes < 0)
    {
        extra = tensOfMinutes / 6 - 1;
        hours += extra;
        tensOfMinutes = tensOfMinutes - extra * 6;
    }

    tempHours = hours + tensOfHours * 10;
    if (tempHours >= 0)
    {
        extra = tempHours / 24;
        tempHours = tempHours - extra * 24;
        tensOfHours = tempHours / 10;
        hours = tempHours - tensOfHours * 10;
    }
    else if (tempHours < 0)
    {
        extra = tempHours / 24 - 1;
        tempHours = tempHours - extra * 24;
        tensOfHours = tempHours / 10;
        hours = tempHours - tensOfHours * 10;
    }

    Clock temp = newClock();
    temp.seconds = seconds;
    temp.tensOfSeconds = tensOfSeconds;
    temp.minutes = minutes;
    temp.tensOfMinutes = tensOfMinutes;
    temp.hours = hours;
    temp.tensOfHours = tensOfHours;

    return temp;
}

/* Converts the passed values to a simplified, correct Calendar variable from 2000 to 2099 */
static Calendar numbersToCalendar(int tensOfYears,   int years,
                                  int tensOfMonths,  int months,
                                  int tensOfDays,    int days)
{
    /* variable used to store values temporarily during calculations */
    int extra, tempDays, tempMonths, tempYears;

    tempMonths = months + tensOfMonths * 10;

    if (tempMonths > 12)
    {
        extra = tempMonths / 12;
        years += extra;
        tempMonths = tempMonths - extra * 12;
    }
    else if (tempMonths <= 0)
    {
        extra = tempMonths / 12 - 1;
        years += extra;
        tempMonths = tempMonths - extra * 12;
    }

    if (years > 9)
    {
        extra = years / 10;
        tensOfYears += extra;
        years = years - extra * 10;
    }
    else if (years < 0)
    {
        extra = years / 10 - 1;
        tensOfYears += extra;
        years = years - extra * 10;
    }

    if (tensOfYears > 9)
    {
        extra = tensOfYears / 10;
        tensOfYears = tensOfYears - extra * 10;
    }
    else if (tensOfYears < 0)
    {
        extra = tensOfYears / 10 - 1;
        tensOfYears = tensOfYears - extra * 10;
    }

    tempYears = years + tensOfYears * 10 + 100;

    tempDays = days + tensOfDays * 10;

    if (tempDays > 1460)
    {
        extra = tempDays / 1461;
        tempYears += extra << 2;
        tempDays = tempDays - extra * 1461;
    }
    else if (tempDays < 0)
    {
        extra = tempDays / 1461 - 1;
        tempYears += extra * 4;
        tempDays = tempDays - extra * 1461;
    }

    while (tempDays > 366)
    {
        extra = tempYears & 0b11;
        if (extra == 3 && tempMonths > FEBUARY)
            tempDays -= 366;
        else if (extra == 0 && tempMonths < MARCH)
            tempDays -= 366;
        else
            tempDays -= 365;

        ++tempYears;
    }

    while (tempDays <= 0)
    {
        if (tempYears == 0)
            tempYears = 99;
        else
            --tempYears;

        extra = tempYears & 0b11;
        if (extra == 3 && tempMonths > FEBUARY)
            tempDays += 366;
        else if (extra == 0 && tempMonths < MARCH)
            tempDays += 366;
        else
            tempDays += 365;
    }

    while (tempDays > getDaysInMonth(tempMonths,tempYears))
    {
        tempDays -= getDaysInMonth(tempMonths,tempYears);
        ++tempMonths;

        if (tempMonths > 12)
        {
            tempMonths -= 12;
            ++tempYears;
        }
   }

    tensOfDays = tempDays / 10;
    days = tempDays - tensOfDays * 10;

    if (tempMonths > 9)
    {
        tensOfMonths = 1;
        months = tempMonths - 10;
    }
    else
    {
        tensOfMonths = 0;
        months = tempMonths;
    }

    tempYears %= 100;
    tensOfYears = tempYears / 10;
    years = tempYears - tensOfYears * 10;
    int weekday = calculateWeekday(tempYears, tempMonths, tempDays);

    Calendar temp = newCalendar();
    temp.weekday = weekday;
    temp.days = days;
    temp.tensOfDays = tensOfDays;
    temp.months = months;
    temp.tensOfMonths = tensOfMonths;
    temp.years = years;
    temp.tensOfYears = tensOfYears;

    return temp;
}

/* Converts the passed values to a simplified, correct CalendarAndClock variable from 2000 to 2099 */
static CalendarAndClock numbersToCalendarAndClock(int tensOfYears,    int years,
                                                  int tensOfMonths,   int months,
                                                  int tensOfDays,     int days,
                                                  int tensOfHours,    int hours,
                                                  int tensOfMinutes,  int minutes,
                                                  int tensOfSeconds,  int seconds)
{
    /* variable used to store values temporarily during calculations */
    int extra, tempHours, tempDays, tempMonths, tempYears;

    if (seconds > 9)
    {
        extra = seconds / 10;
        tensOfSeconds += extra;
        seconds = seconds - extra * 10;
    }
    else if (seconds < 0)
    {
        extra = seconds / 10 - 1;
        tensOfSeconds += extra;
        seconds = seconds - extra * 10;
    }

    if (tensOfSeconds > 5)
    {
        extra = tensOfSeconds / 6;
        minutes += extra;
        tensOfSeconds = tensOfSeconds - extra * 6;
    }
    else if (tensOfSeconds < 0)
    {
        extra = tensOfSeconds / 6 - 1;
        minutes += extra;
        tensOfSeconds = tensOfSeconds - extra * 6;
    }

    if (minutes > 9)
    {
        extra = minutes / 10;
        tensOfMinutes += extra;
        minutes = minutes - extra * 10;
    }
    else if (minutes < 0)
    {
        extra = minutes / 10 - 1;
        tensOfMinutes += extra;
        minutes = minutes - extra * 10;
    }

    if (tensOfMinutes > 5)
    {
        extra = tensOfMinutes / 6;
        hours += extra;
        tensOfMinutes = tensOfMinutes - extra * 6;
    }
    else if (tensOfMinutes < 0)
    {
        extra = tensOfMinutes / 6 - 1;
        hours += extra;
        tensOfMinutes = tensOfMinutes - extra * 6;
    }

    tempHours = hours + tensOfHours * 10;
    if (tempHours >= 0)
    {
        extra = tempHours / 24;
        days += extra;
        extra = tempHours - extra * 24;
        tensOfHours = extra / 10;
        hours = extra - tensOfHours * 10;
    }
    else if (tempHours < 0)
    {
        extra = tempHours / 24 - 1;
        days += extra;
        extra = tempHours - extra * 24;
        tensOfHours = extra / 10;
        hours = extra - tensOfHours * 10;
    }

    tempMonths = months + tensOfMonths * 10;
    if (tempMonths > 12)
    {
        extra = tempMonths / 12;
        years += extra;
        tempMonths = tempMonths - extra * 12;
    }
    else if (tempMonths <= 0)
    {
        extra = tempMonths / 12 - 1;
        years += extra;
        tempMonths = tempMonths - extra * 12;
    }

    if (years > 9)
    {
        extra = years / 10;
        tensOfYears += extra;
        years = years - extra * 10;
    }
    else if (years < 0)
    {
        extra = years / 10 - 1;
        tensOfYears += extra;
        years = years - extra * 10;
    }

    if (tensOfYears > 9)
    {
        extra = tensOfYears / 10;
        tensOfYears = tensOfYears - extra * 10;
    }
    else if (tensOfYears < 0)
    {
        extra = tensOfYears / 10 - 1;
        tensOfYears = tensOfYears - extra * 10;
    }

    tempYears = years + tensOfYears * 10 + 100;

    tempDays = days + tensOfDays * 10;
    if (tempDays > 1460)
    {
        extra = tempDays / 1461;
        tempYears += extra << 2;
        tempDays = tempDays - extra * 1461;
    }
    else if (tempDays < 0)
    {
        extra = tempDays / 1461 - 1;
        tempYears += extra * 4;
        tempDays = tempDays - extra * 1461;
    }

    while (tempDays > 366)
    {
        extra = tempYears & 0b11;
        if (extra == 3 && tempMonths > FEBUARY)
            tempDays -= 366;
        else if (extra == 0 && tempMonths < MARCH)
            tempDays -= 366;
        else
            tempDays -= 365;
        ++tempYears;

    }

    while (tempDays <= 0)
    {
        if (tempYears == 0)
            tempYears = 99;
        else
            --tempYears;

        extra = tempYears & 0b11;

        if (extra == 3 && tempMonths > FEBUARY)
            tempDays += 366;
        else if (extra == 0 && tempMonths < MARCH)
            tempDays += 366;
        else
            tempDays += 365;
    }

    while (tempDays > getDaysInMonth(tempMonths,tempYears))
    {
        tempDays -= getDaysInMonth(tempMonths,tempYears);
        ++tempMonths;

        if (tempMonths > 12)
        {
            tempMonths -= 12;
            ++tempYears;
        }
   }

    tensOfDays = tempDays / 10;
    days = tempDays - tensOfDays * 10;

    if (tempMonths > 9)
    {
        tensOfMonths = 1;
        months = tempMonths - 10;
    }
    else
    {
        tensOfMonths = 0;
        months = tempMonths;
    }

    tempYears %= 100;
    tensOfYears = tempYears / 10;
    years = tempYears - tensOfYears * 10;
    int weekday = calculateWeekday(tempYears, tempMonths, tempDays);

    CalendarAndClock temp = newCalendarAndClock();
    temp.seconds = seconds;
    temp.tensOfSeconds = tensOfSeconds;
    temp.minutes = minutes;
    temp.tensOfMinutes = tensOfMinutes;
    temp.hours = hours;
    temp.tensOfHours = tensOfHours;
    temp.weekday = weekday;
    temp.days = days;
    temp.tensOfDays = tensOfDays;
    temp.months = months;
    temp.tensOfMonths = tensOfMonths;
    temp.years = years;
    temp.tensOfYears = tensOfYears;

    return temp;
}

static inline CalendarAndClock timeStampToCalendarAndClock(String timeStamp)
{
    /* use the numbers to Calendar Clock function to simplify the passed stamp, and then return it */
    return numbersToCalendarAndClock(timeStamp[0]  - '0', timeStamp[1]  - '0',
                                     timeStamp[3]  - '0', timeStamp[4]  - '0',
                                     timeStamp[6]  - '0', timeStamp[7]  - '0',
                                     timeStamp[9]  - '0', timeStamp[10] - '0',
                                     timeStamp[12] - '0', timeStamp[13] - '0',
                                     timeStamp[15] - '0', timeStamp[16] - '0');
}// simplifyDate end

static inline CalendarAndClock addCalendarAndClocks(CalendarAndClock first, CalendarAndClock second)
{
   return numbersToCalendarAndClock(first.tensOfYears   + second.tensOfYears,
                                    first.years         + second.years,
                                    first.tensOfMonths  + second.tensOfMonths,
                                    first.months        + second.months,
                                    first.tensOfDays    + second.tensOfDays,
                                    first.days          + second.days,
                                    first.tensOfHours   + second.tensOfHours,
                                    first.hours         + second.hours,
                                    first.tensOfMinutes + second.tensOfMinutes,
                                    first.minutes       + second.minutes,
                                    first.tensOfSeconds + second.tensOfSeconds,
                                    first.seconds       + second.seconds);
}

static inline CalendarAndClock subtractCalendarAndClocks(CalendarAndClock first, CalendarAndClock second)
{
   return numbersToCalendarAndClock(first.tensOfYears   - second.tensOfYears,
                                    first.years         - second.years,
                                    first.tensOfMonths  - second.tensOfMonths,
                                    first.months        - second.months,
                                    first.tensOfDays    - second.tensOfDays,
                                    first.days          - second.days,
                                    first.tensOfHours   - second.tensOfHours,
                                    first.hours         - second.hours,
                                    first.tensOfMinutes - second.tensOfMinutes,
                                    first.minutes       - second.minutes,
                                    first.tensOfSeconds - second.tensOfSeconds,
                                    first.seconds       - second.seconds);
}

static inline Calendar addCalendars(Calendar first, Calendar second)
{
   return numbersToCalendar(first.tensOfYears   + second.tensOfYears,
                            first.years         + second.years,
                            first.tensOfMonths  + second.tensOfMonths,
                            first.months        + second.months,
                            first.tensOfDays    + second.tensOfDays,
                            first.days          + second.days);
}

static inline Calendar subtractCalendars(Calendar first, Calendar second)
{
   return numbersToCalendar(first.tensOfYears   - second.tensOfYears,
                            first.years         - second.years,
                            first.tensOfMonths  - second.tensOfMonths,
                            first.months        - second.months,
                            first.tensOfDays    - second.tensOfDays,
                            first.days          - second.days);
}

static inline Clock addClocks(Clock first, Clock second)
{
   return numbersToClock(first.tensOfHours  + second.tensOfHours,
                        first.hours         + second.hours,
                        first.tensOfMinutes + second.tensOfMinutes,
                        first.minutes       + second.minutes,
                        first.tensOfSeconds + second.tensOfSeconds,
                        first.seconds       + second.seconds);
}

static inline Clock subtractClocks(Clock first, Clock second)
{
   return numbersToClock(first.tensOfHours  - second.tensOfHours,
                        first.hours         - second.hours,
                        first.tensOfMinutes - second.tensOfMinutes,
                        first.minutes       - second.minutes,
                        first.tensOfSeconds - second.tensOfSeconds,
                        first.seconds       - second.seconds);
}

/* Returns a simplified version of the passed CalendarAndClock */
static inline CalendarAndClock simplifyCalendarAndClock(CalendarAndClock calendarAndClock)
{
    /* use the numbers to Calendar function to simplify the calendarAndClock, and then return it */
    return numbersToCalendarAndClock(calendarAndClock.tensOfYears,   calendarAndClock.years,
                                     calendarAndClock.tensOfMonths,  calendarAndClock.months,
                                     calendarAndClock.tensOfDays,    calendarAndClock.days,
                                     calendarAndClock.tensOfHours,   calendarAndClock.hours,
                                     calendarAndClock.tensOfMinutes, calendarAndClock.minutes,
                                     calendarAndClock.tensOfSeconds, calendarAndClock.seconds);
}// simplifyCalendarAndClock end

/* Returns a simplified version of the passed Clock */
static inline Calendar simplifyCalendar(Calendar calendar)
{
    /* use the numbers to Calendar function to simplify the time, and then return it */
    return numbersToCalendar(calendar.tensOfYears,   calendar.years,
                             calendar.tensOfMonths,  calendar.months,
                             calendar.tensOfDays,    calendar.days);
}// simplifyCalendar end

/* Returns a simplified version of the passed Clock */
static inline Clock simplifyClock(Clock clock)
{
    /* use the number to Clock function to simplify the clock, and then return it */
    return numbersToClock(clock.tensOfHours,   clock.hours,
                          clock.tensOfMinutes, clock.minutes,
                          clock.tensOfSeconds, clock.seconds);
}// simplifyClock end

static inline RTCCRegister getRTCCRegister(void)
{
    RTCCRegister temp;
    RTC_POINTER = YEAR_POINTER;
    temp.yearRegister = RTC_VALUE_REGISTER;
    temp.monthDayRegister = RTC_VALUE_REGISTER;
    temp.weekdayHourRegister = RTC_VALUE_REGISTER;
    temp.minuteSecondRegister = RTC_VALUE_REGISTER;

    return temp;
}

/* Sets the system time to that of the passed RTCC value */
// only updates the time if it's valid, invalid times are ignored
static inline void setRTCCRegister(RTCCRegister newRTCC)
{
    unlockRealTimeClockRegister();
    RTC_POINTER = YEAR_POINTER;
    RTC_VALUE_REGISTER = newRTCC.yearRegister;
    RTC_VALUE_REGISTER = newRTCC.monthDayRegister;
    RTC_VALUE_REGISTER = newRTCC.weekdayHourRegister;
    RTC_VALUE_REGISTER = newRTCC.minuteSecondRegister;
    lockRealTimeClockRegister();

} // setRTCCRegister end

#if 0 /* unused functions - to suppress compiler warnings and save some space */
/* Sets the system time to that of the passed Clock value */
// only updates the time if it's valid, invalid times are ignored
static void setSystemClock(Clock newClock)
{
    //newClock = simplifyClock(newClock);

    RTCCRegister temp = getRTCCRegister();

    temp.tensOfHours   = newClock.tensOfHours;
    temp.hours         = newClock.hours;
    temp.tensOfMinutes = newClock.tensOfMinutes;
    temp.minutes       = newClock.minutes;
    temp.tensOfSeconds = newClock.tensOfSeconds;
    temp.seconds       = newClock.seconds;


    unlockRealTimeClockRegister();
    RTC_POINTER = WEEKDAY_HOUR_POINTER;
    RTC_VALUE_REGISTER = temp.weekdayHourRegister;
    RTC_VALUE_REGISTER = temp.minuteSecondRegister;
    lockRealTimeClockRegister();

} // setClock end

/* Sets the system date to that of the passed Calendar value */
// only updates the date if it's valid, invalid times are ignored
static void setSystemCalendar(Calendar newCalendar)
{
    newCalendar = simplifyCalendar(newCalendar);

    RTCCRegister temp = getRTCCRegister();

    temp.tensOfYears   = newCalendar.tensOfYears;
    temp.years         = newCalendar.years;
    temp.tensOfMonths  = newCalendar.tensOfMonths;
    temp.months        = newCalendar.months;
    temp.tensOfDays    = newCalendar.tensOfDays;
    temp.days          = newCalendar.days;
    temp.weekday       = newCalendar.weekday;


    unlockRealTimeClockRegister();
    RTC_POINTER = YEAR_POINTER;
    RTC_VALUE_REGISTER = temp.yearRegister;
    RTC_VALUE_REGISTER = temp.monthDayRegister;
    RTC_VALUE_REGISTER = temp.weekdayHourRegister;
    lockRealTimeClockRegister();

} // setCalendar end

#endif

/* Sets the system time to that of the passed CalendarAndClock value */
// only updates the time if it's valid, invalid times are ignored
static void setSystemCalendarAndClock(CalendarAndClock newCalendarAndClock)
{
    newCalendarAndClock = simplifyCalendarAndClock(newCalendarAndClock);
    RTCCRegister temp;

    temp.tensOfYears   = newCalendarAndClock.tensOfYears;
    temp.years         = newCalendarAndClock.years;
    temp.tensOfMonths  = newCalendarAndClock.tensOfMonths;
    temp.months        = newCalendarAndClock.months;
    temp.tensOfDays    = newCalendarAndClock.tensOfDays;
    temp.days          = newCalendarAndClock.days;
    temp.weekday       = newCalendarAndClock.weekday;
    temp.tensOfHours   = newCalendarAndClock.tensOfHours;
    temp.hours         = newCalendarAndClock.hours;
    temp.tensOfMinutes = newCalendarAndClock.tensOfMinutes;
    temp.minutes       = newCalendarAndClock.minutes;
    temp.tensOfSeconds = newCalendarAndClock.tensOfSeconds;
    temp.seconds       = newCalendarAndClock.seconds;

    setRTCCRegister(temp);

} // setCalendarAndClock end

/* Returns the value of the system's current time */
static inline Clock getSystemClock(void)
{
    RTCCRegister temp;
    RTC_POINTER = WEEKDAY_HOUR_POINTER;
    temp.weekdayHourRegister = RTC_VALUE_REGISTER;
    temp.minuteSecondRegister = RTC_VALUE_REGISTER;

    Clock clock = {temp.seconds, temp.tensOfSeconds,
                   temp.minutes, temp.tensOfMinutes,
                   temp.hours,   temp.tensOfHours};

    return clock;

}

/* Returns the value of the system's current calendar and clock */
static inline Calendar getSystemCalendar(void)
{
    RTCCRegister temp;
    RTC_POINTER = YEAR_POINTER;
    temp.yearRegister = RTC_VALUE_REGISTER;
    temp.monthDayRegister = RTC_VALUE_REGISTER;
    temp.weekdayHourRegister = RTC_VALUE_REGISTER;

    Calendar calendar = {temp.weekday,
                         temp.days,   temp.tensOfDays,
                         temp.months, temp.tensOfMonths,
                         temp.years,  temp.tensOfYears};
    return calendar;
}

static inline CalendarAndClock getSystemCalendarAndClock(void)
{
    RTCCRegister temp = getRTCCRegister();

    CalendarAndClock calendarAndClock = {{{temp.seconds, temp.tensOfSeconds,
                                           temp.minutes, temp.tensOfMinutes,
                                           temp.hours,   temp.tensOfHours}},
                                         {{temp.weekday,
                                           temp.days,    temp.tensOfDays,
                                           temp.months,  temp.tensOfMonths,
                                           temp.years,   temp.tensOfYears}}};
    return calendarAndClock;
}

static volatile Uint64 milisecondCount = 0;

Uint64 getMilisecondCount(void)
{
    return milisecondCount;
}

void Wait(Uint16 miliseconds)
{
    Uint64 endTime = getMilisecondCount() + miliseconds;
    while(getMilisecondCount() < endTime);
}

_ISR_ _T4Interrupt(void)
{
    ++milisecondCount;

    // clear the interrupt flag (we are now finished updating the time counters)
    _T4IF = 0;
}

/* Realtime Clock at 2Hz*/
// RTCC interrupt service routine
_ISR_ _RTCCInterrupt(void)
{
    if(getSystemHalfSeconds() == 0)
        milisecondCount = ((milisecondCount + 500) / 1000) * 1000; // synchronize the millisecond counter

    // clear the interrupt flag (we are now finished updating the time counters)
    _RTCIF = 0;

} // RTCCInterrupt end

static void initialize(void)
{
    enableSecondaryOscillator();
    unlockRealTimeClockRegister();
    _RTCIE = OFF; // disable real time clock and calendar interrupt
    _CHIME = NO; // don't repeat rtc alarm
    _ALRMEN = OFF; // disable the alarm

    /**
     * Real Time Clock and Calendar (RTCC) Interrupt
     *
     * Timekeeping is done in hardware, so the time is kept accurate and
     * precise. The interrupt is used to synchronize the millisecond counter.
     */
    _RTCIP = 3; // set interrupt priority to normal (medium)
    _RTCEN = ON; // enable rtcc module
    _ALRMEN = OFF; //disable the alarm
    lockRealTimeClockRegister();

    /**
     * Timer4 Usage Overview
     *
     * Timer 4 will be used as the time counter. It will use the internal
     * oscillator in conjunction with its period register in order to
     * generate an interrupt every millisecond for use in timing. To ensure
     * that the time remains precise, Timer4 must be the highest priority.
     */

    /**
     * Timer4 Configuration Setup
     *
     * Timer4 will start off and be started later. The prescaler will be set
     * to 1:64 so that the period is of a reasonable value and resolution. The
     * internal system oscillator will be used. Gating mode and synchronous
     * operation will not be used.
     */
    T4CON = 0x0020;  // disabled, prescaled, internal oscillator
    TMR4 = 0;        // clear the timer

    /**
     * Timer4 Period Register Setup
     *
     * The period register contains the number of counts before Timer4 resets.
     * For 1ms, the count will be 1/1000th of the prescaled clocks per second.
     *
     * Note: The count begins at 0 (rather than 1), so 1 must be subtracted
     *       from the desired value.
     */
    PR4 = CLOCKS_PER_SEC/64/1000 - 1;  // interrupt at 1kHz (1ms)

    /**
     * Timer4 Interrupt Setup
     *
     * Interrupts are required, so the interrupt enable bit must be set, and
     * the interrupt flag must be cleared.
     */
    _T4IE = OFF;//ON;   // enable Timer4 to interrupt
    _T4IF = OFF;    // clear the interrupt flag

    /**
     * Timer4 Interrupt Setup (continued)
     *
     * The priority of the interrupt must be set to real-time so that
     * precision is maintained.
     */
    _T4IP = 3; // set priority to real-time

    //T4CONbits.TON = 1;

    /**
     * Initialize Date and Time
     *
     * The RTCC will be initialized to a known time.
     */
    setSystemCalendarAndClock(newCalendarAndClock());
}

static DateAndTime newDateAndTime(signed year, signed month,  signed day,
                                  signed hour, signed minute, signed second)
{
    static DateAndTime dt;
    dt.second  = second;
    dt.minute  = minute;
    dt.hour    = hour;
    dt.day     = day;
    dt.month   = month;
    dt.weekday = calculateWeekday(year, month, day);
    dt.year    = year;
    return dt;
}

/* Converts the passed values to a simplified, correct DateAndTime variable from 2000 to 2099 */
static DateAndTime numbersToDateAndTime(signed years, signed months,  signed days,
                                        signed hours, signed minutes, signed seconds)
{
    /* variable used to store values temporarily during calculations */
    signed extra;

    if (seconds > 59)
    {
        extra = seconds / 60;
        minutes += extra;
        seconds = seconds - extra * 60;
    }
    else if (seconds < 0)
    {
        extra = seconds / 60 - 1;
        minutes += extra;
        seconds = seconds - extra * 60;
    }

    if (minutes > 59)
    {
        extra = minutes / 60;
        hours += extra;
        minutes = minutes - extra * 60;
    }
    else if (minutes < 0)
    {
        extra = minutes / 60 - 1;
        hours += extra;
        minutes = minutes - extra * 60;
    }

    if (hours >= 0)
    {
        extra = hours / 24;
        days += extra;
        hours = hours - extra * 24;
    }
    else if (hours < 0)
    {
        extra = hours / 24 - 1;
        days += extra;
        hours = hours - extra * 24;
    }

    if (months > 12)
    {
        extra = months / 12;
        years += extra;
        months = months - extra * 12;
    }
    else if (months <= 0)
    {
        extra = months / 12 - 1;
        years += extra;
        months = months - extra * 12;
    }

    if (years > 99)
    {
        extra = years / 100;
        years = years - extra * 100;
    }
    else if (years < 0)
    {
        extra = years / 100 - 1;
        years = years - extra * 100;
    }

    if (days > 1460)
    {
        extra = days / 1461;
        years += extra << 2;
        days = days - extra * 1461;
    }
    else if (days < 0)
    {
        extra = days / 1461 - 1;
        years += extra * 4;
        days = days - extra * 1461;
    }

    while (days > 366)
    {
        extra = years & 0b11;
        if (extra == 3 && months > FEBUARY)
            days -= 366;
        else if (extra == 0 && months < MARCH)
            days -= 366;
        else
            days -= 365;
        ++years;

    }

    while (days <= 0)
    {
        if (years == 0)
            years = 99;
        else
            --years;

        extra = years & 0b11;

        if (extra == 3 && months > FEBUARY)
            days += 366;
        else if (extra == 0 && months < MARCH)
            days += 366;
        else
            days += 365;
    }

    while (days > getDaysInMonth(months,years))
    {
        days -= getDaysInMonth(months,years);
        ++months;

        if (months > 12)
        {
            months -= 12;
            ++years;
        }
    }

    years %= 100;

    DateAndTime dt;
    dt.second  = seconds;
    dt.minute  = minutes;
    dt.hour    = hours;
    dt.weekday = calculateWeekday(years, months, days);
    dt.day     = days;
    dt.month   = months;
    dt.year    = years;

    return dt;
}

static DateAndTime toDateAndTime(signed years, signed months,  signed days,
                                 signed hours, signed minutes, signed seconds)
{
    return numbersToDateAndTime(years, months, days, hours, minutes, seconds);
}

static inline DateAndTime addDateAndTimes(DateAndTime first, DateAndTime second)
{
   return numbersToDateAndTime(first.year   + second.year,
                               first.month  + second.month,
                               first.day    + second.day,
                               first.hour   + second.hour,
                               first.minute + second.minute,
                               first.second + second.second);
}

static inline DateAndTime subtractDateAndTimes(DateAndTime first, DateAndTime second)
{
   return numbersToDateAndTime(first.year   - second.year,
                               first.month  - second.month,
                               first.day    - second.day,
                               first.hour   - second.hour,
                               first.minute - second.minute,
                               first.second - second.second);
}


static String getTimeStamp(void)
{
    static char stamp[] = {"00.01.01-00:00:00"};

    CalendarAndClock timeStamp = getSystemCalendarAndClock();

    stamp[0]  = '0' + timeStamp.tensOfYears;
    stamp[1]  = '0' + timeStamp.years;
    stamp[3]  = '0' + timeStamp.tensOfMonths;
    stamp[4]  = '0' + timeStamp.months;
    stamp[6]  = '0' + timeStamp.tensOfDays;
    stamp[7]  = '0' + timeStamp.days;
    stamp[9]  = '0' + timeStamp.tensOfHours;
    stamp[10] = '0' + timeStamp.hours;
    stamp[12] = '0' + timeStamp.tensOfMinutes;
    stamp[13] = '0' + timeStamp.minutes;
    stamp[15] = '0' + timeStamp.tensOfSeconds;
    stamp[16] = '0' + timeStamp.seconds;

    return stamp;
}

#if 0 /* unused functions - to suppress compiler warnings and save some space */

static String toTimeStamp(CalendarAndClock cc)
{
    static char stamp[] = {"00.01.01-00:00:00"};

    cc = simplifyCalendarAndClock(cc);

    stamp[0]  = '0' + cc.tensOfYears;
    stamp[1]  = '0' + cc.years;
    stamp[3]  = '0' + cc.tensOfMonths;
    stamp[4]  = '0' + cc.months;
    stamp[6]  = '0' + cc.tensOfDays;
    stamp[7]  = '0' + cc.days;
    stamp[9]  = '0' + cc.tensOfHours;
    stamp[10] = '0' + cc.hours;
    stamp[12] = '0' + cc.tensOfMinutes;
    stamp[13] = '0' + cc.minutes;
    stamp[15] = '0' + cc.tensOfSeconds;
    stamp[16] = '0' + cc.seconds;

    return stamp;
}

static CalendarAndClock toCalendarAndClock(DateAndTime dt)
{
    CalendarAndClock cc = newCalendarAndClock();

    cc.tensOfYears   = dt.year / 10;
    cc.years         = dt.year % 10;
    cc.tensOfMonths  = dt.month / 10;
    cc.months        = dt.month % 10;
    cc.tensOfDays    = dt.day / 10;
    cc.days          = dt.day % 10;
    cc.tensOfHours   = dt.hour / 10;
    cc.hours         = dt.hour % 10;
    cc.tensOfMinutes = dt.minute / 10;
    cc.minutes       = dt.minute % 10;
    cc.tensOfSeconds = dt.second / 10;
    cc.seconds       = dt.second % 10;

    return simplifyCalendarAndClock(cc);
}

#endif

static String dateAndTimeToTimeStamp(DateAndTime dt)
{
    //return toTimeStamp(toCalendarAndClock(dt));
    static char stamp[] = {"00.01.01-00:00:00"};

    stamp[0]  = '0' + dt.year / 10;
    stamp[1]  = '0' + dt.year % 10;
    stamp[3]  = '0' + dt.month / 10;
    stamp[4]  = '0' + dt.month % 10;
    stamp[6]  = '0' + dt.day / 10;
    stamp[7]  = '0' + dt.day % 10;
    stamp[9]  = '0' + dt.hour / 10;
    stamp[10] = '0' + dt.hour % 10;
    stamp[12] = '0' + dt.minute / 10;
    stamp[13] = '0' + dt.minute % 10;
    stamp[15] = '0' + dt.second / 10;
    stamp[16] = '0' + dt.second % 10;

    return stamp;
}

static DateAndTime parseTimeStamp(String stamp)
{
    DateAndTime dt;

    Uint count = sscanf(stamp, "%d.%d.%d-%d:%d:%d",
        &dt.year, &dt.month,   &dt.day,
        &dt.hour, &dt. minute, &dt.second);

    if(count == 6) // successfully parsed timestamp
        return dt;

    // zero (invalid date) to indicate error
    return newDateAndTime(0,0,0,0,0,0);
}

static DateAndTime getDateAndTime(void)
{
    DateAndTime dt;
    CalendarAndClock cc = getSystemCalendarAndClock();

    dt.second  = cc.tensOfSeconds * 10 + cc.seconds;
    dt.minute  = cc.tensOfMinutes * 10 + cc.minutes;
    dt.hour    = cc.tensOfHours   * 10 + cc.hours;
    dt.weekday = cc.weekday;
    dt.day     = cc.tensOfDays    * 10 + cc.days;
    dt.month   = cc.tensOfMonths  * 10 + cc.months;
    dt.year    = cc.tensOfYears   * 10 + cc.years;

    return dt;
}

static void setDateAndTime(DateAndTime dt)
{
    CalendarAndClock cc = newCalendarAndClock();
    cc.seconds = dt.second;
    cc.minutes = dt.minute;
    cc.hours   = dt.hour;
    cc.days    = dt.day;
    cc.months  = dt.month;
    cc.years   = dt.year;

    setSystemCalendarAndClock(cc);
}

// is time a after time b
Sint compareDateAndTime(DateAndTime a, DateAndTime b)
{
    Sint diff = 0;
    if(!(diff = (a.year - b.year)))
        if(!(diff = (a.month - b.month)))
            if(!(diff = (a.day - b.day)))
                if(!(diff = (a.hour - b.hour)))
                    if(!(diff = (a.minute - b.minute)))
                        diff = a.second - b.second;
    return diff;
}

const DateTime dateTime = {
    .init = initialize,
    .get = getDateAndTime,
    .set = setDateAndTime,
    .new = newDateAndTime,
    .to = toDateAndTime,
    .add = addDateAndTimes,
    .sub = subtractDateAndTimes,
    .cmp = compareDateAndTime,
    .toStamp = dateAndTimeToTimeStamp,
    .getStamp = getTimeStamp,
    .parseStamp = parseTimeStamp
};

/*
 * This file is part of Theia Space's ESAT Util library.
 *
 * Theia Space's ESAT Util library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT Util library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>

#ifndef ESATTimestamp_h
#define ESATTimestamp_h

// Timestamp representation:
// calendar date plus time of day with second resolution.
class ESATTimestamp
{
  public:
    // Timestamp validity statuses.
    static const byte INVALID_TIMESTAMP = 1;
    static const byte VALID_TIMESTAMP = 2;

    // Last two digits of the year (from 0 to 99).
    byte year;

    // Month number (from 1 to 12).
    byte month;

    // Day of the month (from 1 to 31).
    byte day;

    // Hours (from 0 to 23).
    byte hours;

    // Minutes (from 0 to 59).
    byte minutes;

    // Seconds (from 0 to 59).
    byte seconds;

    // Instantiate a new timestamp.
    ESATTimestamp();

    // Add a given number of days to the timestamp.
    // The hours, minutes and seconds stay untouched.
    // The month and year increase as needed.
    void addDays(unsigned long days);

    // Add a given number of hours to the timestamp.
    // The minutes and seconds stay untouched.
    // The day, month and year increase as needed.
    void addHours(unsigned long hours);

    // Add a given number of minutes to the timestamp.
    // The seconds stay untouched.
    // The hours, day, month and year increase as needed.
    void addMinutes(unsigned long minutes);

    // Add a given number of months to the timestamp.
    // The day, hours, minutes and seconds stay untouched.
    // The year increases as needed.
    void addMonths(unsigned long months);

    // Add a given number of seconds to the timestamp.
    // The minutes, hours, day, month and year increase as needed.
    void addSeconds(unsigned long seconds);

    // Add a given number of years to the timestamp.
    // The month, day, hours, minutes and seconds stay untouched.
    void addYears(unsigned long years);

    // Update the time and date with those taken from the argument timestamp.
    void update(ESATTimestamp timestamp);

    // Update the time and date with the argument values.
    void update(byte year,
                byte month,
                byte day,
                byte hours,
                byte minutes,
                byte seconds);

    // Return true if the argument timestamp happens before this timestamp.
    boolean operator>(ESATTimestamp timestamp) const;

    // Return true if the argument timestamp happens before this
    // timestamp or coincides with this timestamp; otherwise return
    // false.
    boolean operator>=(ESATTimestamp timestamp) const;

    // Return true if the argument timestamp happens after this
    // timestamp; otherwise return false.
    boolean operator<(ESATTimestamp timestamp) const;

    // Return true if the argument timestamp happens after this
    // timestamp or coincides with this timestamp; otherwise return
    // false.
    boolean operator<=(ESATTimestamp timestamp) const;

    // Return true if the argument timestamp coincides with this timestamp;
    // otherwise return false.
    boolean operator==(ESATTimestamp timestamp) const;

  private:
    // Comparison result: when this timestamp happens after another timestamp.
    static const byte THIS_IS_HIGHER = 1;

    // Comparison result: when this timestamp happens before another timestamp.
    static const byte THIS_IS_LOWER = 2;

    // Comparison result: when this timestamp coincides with another timestamp.
    static const byte THIS_IS_EQUAL = 3;

    // Return the number of days of a month in a given year.
    static byte daysPerMonth(unsigned int year, byte month);

    // Return true if "year" is a leap year,
    // otherwise return false
    static boolean isLeapYear(unsigned int year);

    // Compare this timestamp to another timestamp.  Return:
    // THIS_IS_LOWER if the argument happens before this timestamp;
    // THIS_IS_HIGHER if the argument happens after this timestamp;
    // THIS_IS_EQUAL if the arguments coincides with this timestamp.
    byte compare(ESATTimestamp timestamp) const;
};

#endif

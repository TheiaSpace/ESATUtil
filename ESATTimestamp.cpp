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

#include "ESATTimestamp.h"
#include <ESATUtil.h>


ESATTimestamp::ESATTimestamp()
{
  hours = 0;
  minutes = 0;
  seconds = 0;
  year = 0;
  month = 0;
  day = 0;
}

void ESATTimestamp::addDays(const unsigned long daysToAdd)
{
  for (long i = 0; i < daysToAdd; i++)
  {
    const byte DAYS_PER_MONTH = daysPerMonth(year, month);
    day = day + 1;
    if (day >= DAYS_PER_MONTH)
    {
      addMonths(1);
      day = 1;
    }
  }
}

void ESATTimestamp::addHours(const unsigned long hoursToAdd)
{
  const byte HOURS_PER_DAY = 24;
  const unsigned long daysToAdd = hoursToAdd / HOURS_PER_DAY;
  addDays(daysToAdd);
  hours = (hours + hoursToAdd) % HOURS_PER_DAY;
}

void ESATTimestamp::addMinutes(const unsigned long minutesToAdd)
{
  const byte MINUTES_PER_HOUR = 60;
  const unsigned long hoursToAdd = minutesToAdd / MINUTES_PER_HOUR;
  addHours(hoursToAdd);
  minutes = (minutes + minutesToAdd) % MINUTES_PER_HOUR;
}

void ESATTimestamp::addMonths(const unsigned long monthsToAdd)
{
  const byte MONTHS_PER_YEAR = 12;
  const unsigned long yearsToAdd = monthsToAdd / MONTHS_PER_YEAR;
  addYears(yearsToAdd);
  month = ((month + monthsToAdd - 1) % MONTHS_PER_YEAR) + 1;
}

void ESATTimestamp::addSeconds(const unsigned long secondsToAdd)
{
  const byte SECONDS_PER_MINUTE = 60;
  const unsigned long minutesToAdd = secondsToAdd / SECONDS_PER_MINUTE;
  addMinutes(minutesToAdd);
  seconds = (seconds + secondsToAdd) % SECONDS_PER_MINUTE;
}

void ESATTimestamp::addYears(const unsigned long yearsToAdd)
{
  year = year + yearsToAdd;
}

byte ESATTimestamp::compare(const ESATTimestamp timestamp) const
{
  if (timestamp.year > year)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.year < year)
  {
    return THIS_IS_HIGHER;
  }
  if (timestamp.month > month)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.month < month)
  {
    return THIS_IS_HIGHER;
  }
  if (timestamp.day > day)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.day < day)
  {
    return THIS_IS_HIGHER;
  }
  if (timestamp.hours > hours)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.hours < hours)
  {
    return THIS_IS_HIGHER;
  }
  if (timestamp.minutes > minutes)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.minutes < minutes)
  {
    return THIS_IS_HIGHER;
  }
  if (timestamp.seconds > seconds)
  {
    return THIS_IS_LOWER;
  }
  else if (timestamp.seconds < seconds)
  {
    return THIS_IS_HIGHER;
  }
  else
  {
    return THIS_IS_EQUAL;
  }
}

byte ESATTimestamp::daysPerMonth(const unsigned int year, const byte month)
{
  const byte MONTHS_PER_YEAR = 12;
  if (isLeapYear(year))
  {
    const byte DAYS_PER_MONTH[] = {
      31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    return DAYS_PER_MONTH[(month - 1) % MONTHS_PER_YEAR];
  }
  else
  {
    const byte DAYS_PER_MONTH[] = {
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    return DAYS_PER_MONTH[(month - 1) % MONTHS_PER_YEAR];
  }
}

boolean ESATTimestamp::isLeapYear(const unsigned int year)
{
  if ((year % 4) != 0)
  {
    return false;
  }
  if ((year % 100) != 0)
  {
    return true;
  }
  if ((year % 400) != 0)
  {
    return false;
  }
  return true;
}

void ESATTimestamp::update(const ESATTimestamp timestamp)
{
  year = timestamp.year;
  month = timestamp.month;
  day = timestamp.day;
  hours = timestamp.hours;
  minutes = timestamp.minutes;
  seconds = timestamp.seconds;
}

void ESATTimestamp::update(const byte newYear,
                           const byte newMonth,
                           const byte newDay,
                           const byte newHours,
                           const byte newMinutes,
                           const byte newSeconds)
{
  hours = newHours;
  minutes = newMinutes;
  seconds = newSeconds;
  year = newYear;
  month = newMonth;
  day = newDay;
}

boolean ESATTimestamp::operator==(const ESATTimestamp timestamp) const
{
  if (compare(timestamp) == THIS_IS_EQUAL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESATTimestamp::operator>(const ESATTimestamp timestamp) const
{
  if (compare(timestamp) == THIS_IS_HIGHER)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESATTimestamp::operator>=(const ESATTimestamp timestamp) const
{
  byte result = compare(timestamp);
  if (result == THIS_IS_HIGHER)
  {
    return true;
  }
  else if (result == THIS_IS_EQUAL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESATTimestamp::operator<(const ESATTimestamp timestamp) const
{
  if (compare(timestamp) == THIS_IS_LOWER)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESATTimestamp::operator<=(const ESATTimestamp timestamp) const
{
  byte result = compare(timestamp);
  if (result == THIS_IS_LOWER)
  {
    return true;
  }
  else if (result == THIS_IS_EQUAL)
  {
    return true;
  }
  else
  {
    return false;
  }
}

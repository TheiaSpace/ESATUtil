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

#include "ESAT_Timestamp.h"
#include <ESAT_Util.h>


ESAT_Timestamp::ESAT_Timestamp()
{
  year = 0;
  month = 0;
  day = 0;
  hours = 0;
  minutes = 0;
  seconds = 0;
}

ESAT_Timestamp::ESAT_Timestamp(const word theYear,
                               const byte theMonth,
                               const byte theDay,
                               const byte theHours,
                               const byte theMinutes,
                               const byte theSeconds)
{
  year = theYear;
  month = theMonth;
  day = theDay;
  minutes = theMinutes;
  hours = theHours;
  seconds = theSeconds;
}

void ESAT_Timestamp::addDays(const unsigned long daysToAdd)
{
  for (unsigned long i = 0; i < daysToAdd; i++)
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

void ESAT_Timestamp::addHours(const unsigned long hoursToAdd)
{
  const byte HOURS_PER_DAY = 24;
  const unsigned long daysToAdd =
    (hours + hoursToAdd) / HOURS_PER_DAY;
  addDays(daysToAdd);
  hours = (hours + hoursToAdd) % HOURS_PER_DAY;
}

void ESAT_Timestamp::addMinutes(const unsigned long minutesToAdd)
{
  const byte MINUTES_PER_HOUR = 60;
  const unsigned long hoursToAdd =
    (minutes + minutesToAdd) / MINUTES_PER_HOUR;
  addHours(hoursToAdd);
  minutes = (minutes + minutesToAdd) % MINUTES_PER_HOUR;
}

void ESAT_Timestamp::addMonths(const unsigned long monthsToAdd)
{
  const byte MONTHS_PER_YEAR = 12;
  const unsigned long yearsToAdd =
    (month + monthsToAdd - 1) / MONTHS_PER_YEAR;
  addYears(yearsToAdd);
  month = ((month + monthsToAdd - 1) % MONTHS_PER_YEAR) + 1;
}

void ESAT_Timestamp::addSeconds(const unsigned long secondsToAdd)
{
  const byte SECONDS_PER_MINUTE = 60;
  const unsigned long minutesToAdd =
    (seconds + secondsToAdd) / SECONDS_PER_MINUTE;
  addMinutes(minutesToAdd);
  seconds = (seconds + secondsToAdd) % SECONDS_PER_MINUTE;
}

void ESAT_Timestamp::addYears(const unsigned long yearsToAdd)
{
  year = year + yearsToAdd;
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareDayTo(const ESAT_Timestamp timestamp) const
{
  if (day == timestamp.day)
  {
    return compareHoursTo(timestamp);
  }
  else
  {
    if (day < timestamp.day)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareHoursTo(const ESAT_Timestamp timestamp) const
{
  if (hours == timestamp.hours)
  {
    return compareMinutesTo(timestamp);
  }
  else
  {
    if (minutes < timestamp.minutes)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareMinutesTo(const ESAT_Timestamp timestamp) const
{
  if (minutes == timestamp.minutes)
  {
    return compareSecondsTo(timestamp);
  }
  else
  {
    if (minutes < timestamp.minutes)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareMonthTo(const ESAT_Timestamp timestamp) const
{
  if (month == timestamp.month)
  {
    return compareDayTo(timestamp);
  }
  else
  {
    if (month < timestamp.month)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareSecondsTo(const ESAT_Timestamp timestamp) const
{
  if (seconds == timestamp.seconds)
  {
    return THIS_IS_EQUAL;
  }
  else
  {
    if (seconds < timestamp.seconds)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

ESAT_Timestamp::ComparisonResult ESAT_Timestamp::compareTo(const ESAT_Timestamp timestamp) const
{
  if (year == timestamp.year)
  {
    return compareMonthTo(timestamp);
  }
  else
  {
    if (year < timestamp.year)
    {
      return THIS_IS_LOWER;
    }
    else
    {
      return THIS_IS_HIGHER;
    }
  }
}

byte ESAT_Timestamp::daysPerMonth(const unsigned int year, const byte month)
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

boolean ESAT_Timestamp::isLeapYear(const unsigned int year)
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

size_t ESAT_Timestamp::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(year, DEC), '0', 4));
  bytesWritten =
    bytesWritten + output.print(String("-"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(month, DEC), '0', 2));
  bytesWritten =
    bytesWritten + output.print(String("-"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(day, DEC), '0', 2));
  bytesWritten =
    bytesWritten + output.print(String("T"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(hours, DEC), '0', 2));
  bytesWritten =
    bytesWritten + output.print(String(":"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(minutes, DEC), '0', 2));
  bytesWritten =
    bytesWritten + output.print(String(":"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.pad(String(seconds, DEC), '0', 2));
  return bytesWritten;
}

boolean ESAT_Timestamp::operator==(const ESAT_Timestamp timestamp) const
{
  const ComparisonResult result = compareTo(timestamp);
  switch (result)
  {
    case THIS_IS_LOWER:
      return false;
      break;
    case THIS_IS_EQUAL:
      return true;
      break;
    case THIS_IS_HIGHER:
      return false;
      break;
    default:
      return false;
      break;
  }
}

boolean ESAT_Timestamp::operator<(const ESAT_Timestamp timestamp) const
{
  const ComparisonResult result = compareTo(timestamp);
  switch (result)
  {
    case THIS_IS_LOWER:
      return true;
      break;
    case THIS_IS_EQUAL:
      return false;
      break;
    case THIS_IS_HIGHER:
      return false;
      break;
    default:
      return false;
      break;
  }
}

boolean ESAT_Timestamp::operator<=(const ESAT_Timestamp timestamp) const
{
  const ComparisonResult result = compareTo(timestamp);
  switch (result)
  {
    case THIS_IS_LOWER:
      return true;
      break;
    case THIS_IS_EQUAL:
      return true;
      break;
    case THIS_IS_HIGHER:
      return false;
      break;
    default:
      return false;
      break;
  }
}

boolean ESAT_Timestamp::operator>(const ESAT_Timestamp timestamp) const
{
  const ComparisonResult result = compareTo(timestamp);
  switch (result)
  {
    case THIS_IS_LOWER:
      return false;
      break;
    case THIS_IS_EQUAL:
      return false;
      break;
    case THIS_IS_HIGHER:
      return true;
      break;
    default:
      return false;
      break;
  }
}

boolean ESAT_Timestamp::operator>=(const ESAT_Timestamp timestamp) const
{
  const ComparisonResult result = compareTo(timestamp);
  switch (result)
  {
    case THIS_IS_LOWER:
      return false;
      break;
    case THIS_IS_EQUAL:
      return true;
      break;
    case THIS_IS_HIGHER:
      return true;
      break;
    default:
      return false;
      break;
  }
}

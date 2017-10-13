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

void ESATTimestamp::addSeconds(unsigned long seconds)
{
  ESATTimestamp Timestamp;
  Timestamp.update(*this);

  unsigned int numOfDays =    ((seconds / 3600) / 24);
  unsigned int numOfHours =   ((seconds / 3600) % 24);
  unsigned int numOfMinutes = ((seconds / 60) % 60);
  unsigned int numOfSeconds = (seconds % 60);
  
  numOfSeconds = numOfSeconds + Timestamp.seconds;
  Timestamp.seconds = numOfSeconds % 60;
  numOfMinutes = numOfMinutes + Timestamp.minutes + numOfSeconds / 60;
  Timestamp.minutes = numOfMinutes % 60;
  numOfHours = numOfHours + Timestamp.hours + numOfMinutes / 60;
  Timestamp.hours = numOfHours % 24;
  numOfDays = numOfDays + numOfHours / 24;
  
  update(Timestamp);
  if(numOfDays > 0)
  {
    addDays(numOfDays);
  }
}

void ESATTimestamp::addDays(unsigned int days)
{
  ESATTimestamp Timestamp;
  Timestamp.update(*this);
  byte monthDays[12] = {31,28,31,30,31,30, 31, 31, 30, 31, 30, 31};
  if(isLeapYear(Timestamp.year))
  {
    monthDays[1] = 29;
  }
  for(unsigned int day = 0; day < days; day++)
  {
    Timestamp.day = Timestamp.day + 1;
    if(Timestamp.day > monthDays[Timestamp.month - 1])
    {
      Timestamp.day = 1;
      Timestamp.month = Timestamp.month + 1;
      if(Timestamp.month > 12)
      {
        Timestamp.month = 1;
        Timestamp.year = Timestamp.year + 1;
        if(isLeapYear(Timestamp.year))
        {
          monthDays[1] = 29;
        }
        else
        {
          monthDays[1] = 28;
        }
      }
    }
  }
  update(Timestamp);
}

byte ESATTimestamp::compare(ESATTimestamp timestamp)
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

void ESATTimestamp::getDateWithoutDashes(char timestamp[])
{
  sprintf(timestamp,
          "20%02u%02u%02u",
          year % 100,
          month % 100,
          day % 100);
}

void ESATTimestamp::addDay()
{
  addDays(1);
  hours = 0;
  minutes = 0;
  seconds = 0;
}

boolean ESATTimestamp::isDivisor(unsigned int dividend, unsigned int divisor)
{
  return (dividend/divisor*divisor == dividend);
}

boolean ESATTimestamp::isLeapYear(unsigned int year)
{
  if(isDivisor(year,4))
  {
    if(isDivisor(year,100))
    {
      if(isDivisor(year,400))
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return true;
    }
  }
  else
  {
    return false;
  }
}

void ESATTimestamp::toStringTimeStamp(char timestamp[])
{
  sprintf(timestamp,
          "20%02u-%02u-%02uT%02u:%02u:%02u",
          year % 100,
          month % 100,
          day % 100,
          hours % 100,
          minutes % 100,
          seconds % 100);
}

void ESATTimestamp::update(ESATTimestamp timestamp)
{
  year = timestamp.year;
  month = timestamp.month;
  day = timestamp.day;
  hours = timestamp.hours;
  minutes = timestamp.minutes;
  seconds = timestamp.seconds;
}

void ESATTimestamp::update(byte newYear,
                           byte newMonth,
                           byte newDay,
                           byte newHours,
                           byte newMinutes,
                           byte newSeconds)
{
  hours = newHours;
  minutes = newMinutes;
  seconds = newSeconds;
  year = newYear;
  month = newMonth;
  day = newDay;
}

byte ESATTimestamp::update(char time[])
{
  int theYear, theMonth, theDay, theHours, theMinutes, theSeconds;
  int n = sscanf(time,
                 "20%2u-%2u-%2uT%2u:%2u:%2u",
                 &theYear,
                 &theMonth,
                 &theDay,
                 &theHours,
                 &theMinutes,
                 &theSeconds);
  if (n < 6)
  {
    return INVALID_TIMESTAMP;
  }
  year = (byte) theYear;
  month = (byte) theMonth;
  day = (byte) theDay;
  hours = (byte) theHours;
  minutes = (byte) theMinutes;
  seconds = (byte) theSeconds;
  return VALID_TIMESTAMP;
}

boolean ESATTimestamp::operator==(ESATTimestamp timestamp)
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

boolean ESATTimestamp::operator>(ESATTimestamp timestamp)
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

boolean ESATTimestamp::operator>=(ESATTimestamp timestamp)
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

boolean ESATTimestamp::operator<(ESATTimestamp timestamp)
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

boolean ESATTimestamp::operator<=(ESATTimestamp timestamp)
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

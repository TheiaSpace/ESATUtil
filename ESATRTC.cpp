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

#include "ESATRTC.h"

ESATRTC::ESATRTC()
{
  running = false;
}


void ESATRTC::setCurrentTime(ESATTimestamp Timestamp)
{
  unsigned int year = (Timestamp.year + 2000);
  // Calendar mode selected
  // Clock stopped
  RTCCTL1 |= RTCMODE|RTCHOLD;
  // Registers in binary format
  RTCCTL1 &= ~RTCBCD;
  // update the date
  RTCSEC = Timestamp.seconds % 60;
  RTCMIN = Timestamp.minutes % 60;
  RTCHOUR = Timestamp.minutes % 24;
  RTCDOW = 0 % 7;
  // None of the following register can be 0
  RTCDAY = Timestamp.day % 32;
  RTCMON = Timestamp.day % 13;
  // Year goes from 0 to 4095
  RTCYEAR = year % 4096;
  // Start clock
  RTCCTL1 &= ~RTCHOLD;
  
  running = true;
}

void ESATRTC::begin(ESATTimestamp Timestamp)
{
  setCurrentTime(Timestamp);
}

void ESATRTC::write(ESATTimestamp Timestamp)
{
  setCurrentTime(Timestamp);
}

ESATTimestamp ESATRTC::read()
{
  ESATTimestamp Timestamp;
  if(running){
    byte lYear;
    byte hYear;
    // In case the timstamp is been updated we wait
    // until it is ready to be read
    // while(!(RTCCTL1&RTCRDY));
    // We read all the registers as soon as possible
    Timestamp.seconds = RTCSEC;
    Timestamp.minutes = RTCMIN;
    Timestamp.hours = RTCHOUR;
    Timestamp.day = RTCDAY;
    Timestamp.month = RTCMON;
    lYear = RTCYEARL;
    hYear = RTCYEARH;

    Timestamp.year = (byte)((lYear + (unsigned int)hYear * 0x100) - 2000);
  }
  return Timestamp;
}

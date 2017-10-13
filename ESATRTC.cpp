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
boolean ESATRTC::isRunning()
{
  return running;
}

void ESATRTC::setCurrentTime(ESATTimestamp timestamp)
{
  setSeconds = millis()/1000;
  setTimestamp.update(timestamp);
  running = true;
}

void ESATRTC::begin(ESATTimestamp timestamp)
{
  setCurrentTime(timestamp);
}

void ESATRTC::write(ESATTimestamp timestamp)
{
  setCurrentTime(timestamp);
}

ESATTimestamp ESATRTC::read()
{
  ESATTimestamp timestamp;
  if(running)
  {
    timestamp.update(setTimestamp);
    unsigned long theSeconds = millis()/1000 - setSeconds;
    timestamp.addSeconds(theSeconds);
  }
  return timestamp;
}

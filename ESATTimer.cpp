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

#include "ESATTimer.h"

void ESATTimer::begin(word thePeriod)
{
  period = thePeriod;
  previousWakeUpTime = millis();
  previousWaitTime = period;
}

byte ESATTimer::load()
{
  if (previousWaitTime == 0)
  {
    return 100;
  }
  else
  {
    return 100 * (period - previousWaitTime) / period;
  }
}

unsigned long ESATTimer::ellapsedMilliseconds()
{
  return millis() - previousWakeUpTime;
}

void ESATTimer::waitUntilNextCycle()
{
  const unsigned long ellapsedTime = ellapsedMilliseconds();
  if (ellapsedTime < period)
  {
    const unsigned long waitTime = period - ellapsedTime;
    delay(waitTime);
    previousWakeUpTime = previousWakeUpTime + period;
    previousWaitTime = waitTime;
  }
  else
  {
    previousWakeUpTime = millis();
    previousWaitTime = 0;
  }
}

ESATTimer Timer;

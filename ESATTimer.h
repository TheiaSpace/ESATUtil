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

#ifndef ESATTimer_h
#define ESATTimer_h

#include <Energia.h>

class ESATTimer
{
  public:
    unsigned int period;

    // Set the period (in milliseconds)
    // and start the timer.
    void begin(unsigned int period);

    // Return the milliseconds ellapsed
    // since the last wake-up time.
    unsigned long ellapsedMilliseconds();

    // Wait until the next cycle is due.
    // The next cycle starts when the timer period has ellapsed since
    // the last wake-up time.
    void waitUntilNextCycle();

  private:
    unsigned long lastWakeUpTime;
};

extern ESATTimer Timer;

#endif
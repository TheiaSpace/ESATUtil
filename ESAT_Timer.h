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

#ifndef ESAT_Timer_h
#define ESAT_Timer_h

#include <Arduino.h>

// Simple locking timer for time-sensitive applications that require
// periodic execution.
// Use the global instance ESAT_Timer.
// Set up with ESAT_Timer.begin().
// Wait until the start of the next cycle with ESAT_Timer.waitUntilNextCycle().
class ESAT_TimerClass
{
  public:
    // Period of the timer in milliseconds.
    word period;

    // Set the period (in milliseconds)
    // and start the timer.
    void begin(word period);

    // Return the CPU load estimation as the percentage of busy time
    // during the previous cycle.
    byte load();

    // Return the milliseconds ellapsed
    // since the last wake-up time.
    unsigned long ellapsedMilliseconds();

    // Wait until the next cycle is due.
    // The next cycle starts when the timer period has ellapsed since
    // the last wake-up time.
    void waitUntilNextCycle();

  private:
    // Wait time (in milliseconds) of the previous cycle.
    unsigned long previousWaitTime;

    // Wake-up time (in uptime milliseconds) of the previous cycle.
    unsigned long previousWakeUpTime;
};

// Global instance of the ESATTimer library.
extern ESAT_TimerClass ESAT_Timer;

#endif /* ESAT_Timer_h */

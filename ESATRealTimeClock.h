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
#include "ESATTimestamp.h"

#ifndef ESATRealTimeclock_h
#define ESATRealTimeClock_h

// Software real-time clock.
// Once itialized with the current timestamp, it provides a real-time
// clock and calendar function with second resolution.
class ESATRealTimeClock
{
  public:
    // Instantiate a new software real-time clock.
    ESATRealTimeClock();

    // Initiate the clock so it starts counting time.
    // Set the clock to the given timestamp.
    void begin(ESATTimestamp timestamp);

    // Return true if the RTC was initialized and it is running,
    // otherwise returns false.
    boolean isRunning();

    // Return the current timestamp.
    // If the RTC is not initialized before calling this method,
    // return 2000-00-00T00:00:00.
    ESATTimestamp read();

    // Set the time to the given timestamp.
    void write(ESATTimestamp timestamp);

  private:
    // True if the clock is running; false otherwise.
    boolean running;

    // System uptime seconds at the last time setting.
    unsigned long setSeconds;

    // Timestamp given at the last time setting.
    ESATTimestamp setTimestamp;

    // Set the time to the given timestamp.
    void setCurrentTime(ESATTimestamp timestamp);
};

#endif

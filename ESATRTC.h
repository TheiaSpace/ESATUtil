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

#ifndef ESATRTC_h
#define ESATRTC_h

// Software real-time clock.
// Once itialized with the current timestamp, it provides a real-time
// clock and calendar function.
class ESATRTC
{
  public:
    // Instantiate a new RTC.
    ESATRTC();

    // Returns true if the RTC was initialized and it is running,
    // otherwise returns false
    boolean isRunning();

    // Method to initialize the RTC. The argumunt is used to set 
    // the current time.
    void begin(ESATTimestamp Timestamp);

    // Returns the current timestamp. If the RTC is not initialized 
    // before calling this method,
    // it returns 2000-00-00T00:00:00 as timestamp
    ESATTimestamp read();

    // Method to update the current time
    void write(ESATTimestamp Timestamp);

  private:
    ESATTimestamp setTimestamp;
    unsigned long setSeconds;
    boolean running;
    void setCurrentTime(ESATTimestamp Timestamp);

};

#endif

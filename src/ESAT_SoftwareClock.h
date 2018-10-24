/*
 * Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid
 *
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
#include "ESAT_Clock.h"

#ifndef ESAT_SoftwareClock_h
#define ESAT_SoftwareClock_h

// Software real-time clock.
// Once itialized with the current timestamp, it provides a real-time
// clock and calendar function with second resolution.
class ESAT_SoftwareClock: public ESAT_Clock
{
  public:
    // Deprecated method; use write(timestamp) instead.
    // Initiate the clock so it starts counting time.
    // Set the clock to the given timestamp.
    void begin(ESAT_Timestamp timestamp) __attribute__((deprecated("Use write(timestamp) instead.")));

    // Deprecated method.
    // Return true if the clock was initialized and it is running,
    // otherwise returns false.
    boolean isRunning() const __attribute__((deprecated));

    // Return the current timestamp.
    // If the clock is not initialized before calling this method,
    // return the invalid timestamp 0000-00-00T00:00:00.
    ESAT_Timestamp read();

    // Set the time to the given timestamp.
    void write(ESAT_Timestamp timestamp);

  private:
    // System uptime milliseconds at the last time setting.
    unsigned long setMilliseconds;

    // Timestamp given at the last time setting.
    ESAT_Timestamp setTimestamp;
};

#endif /* ESAT_SoftwareClock_h */

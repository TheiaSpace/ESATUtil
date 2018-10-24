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

#include "ESAT_SoftwareClock.h"

void ESAT_SoftwareClock::begin(ESAT_Timestamp timestamp)
{
  write(timestamp);
}

boolean ESAT_SoftwareClock::isRunning() const
{
  return setTimestamp == ESAT_Timestamp();
}

ESAT_Timestamp ESAT_SoftwareClock::read()
{
  // If the timestamp at the last time setting is the default
  // (invalid) timestamp, just return it, as we cannot count
  // time yet.
  if (setTimestamp == ESAT_Timestamp())
  {
    return setTimestamp;
  }
  // If the timestamp at the last time setting isn't the default
  // (invalid) timestamp, then compute and return the current
  // timestamp.
  else
  {
    ESAT_Timestamp timestamp = setTimestamp;
    const unsigned long ellapsedMilliseconds =
            (millis() - setMilliseconds);
    const unsigned long ellapsedSeconds = ellapsedMilliseconds / 1000;
    timestamp.addSeconds(ellapsedSeconds);
    return timestamp;
  }
}

void ESAT_SoftwareClock::write(ESAT_Timestamp timestamp)
{
  setMilliseconds = millis();
  setTimestamp = timestamp;
}

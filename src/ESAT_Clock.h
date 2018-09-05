/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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
#include "ESAT_Timestamp.h"

#ifndef ESAT_Clock_h
#define ESAT_Clock_h

// Real-time clock interface.
class ESAT_Clock
{
  public:
    virtual ~ESAT_Clock() {};

    // Return the current timestamp.
    virtual ESAT_Timestamp read() = 0;

    // Set the time to the given timestamp.
    void write(ESAT_Timestamp timestamp);
};

#endif /* ESAT_Clock_h */

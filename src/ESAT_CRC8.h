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
 * along with Theia Space's ESAT OBC library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef ESAT_CRC8_CHECKSUM_H
#define ESAT_CRC8_CHECKSUM_H

#include <Arduino.h>

// To use this class, first you have to enter the polinomial with begin().
// After that, you can get the CRC8 of a message with read().
// Also you can add more bytes at the end of the message without recalculing
// it from the beginning with add().
class ESAT_CRC8
{
  public:
    // We take into account that the most significant bit of
    // any generator polynomial is always a one. For example:
    // If polynomial is: x8+x2+x+1, POLYNOMIAL= 0b00000111.
    void begin(byte POLYNOMIAL);

    // Calculate the CRC of the given message and return it
    byte read(const byte message[], byte dataLength);

    // It allows to recalculate the CRC adding the new "message" to the
    // previous calculated CRC. In this way you can calculate the CRC of
    // splitted message.
    byte add(byte previousCRC, const byte message[], byte dataLength);

  private:
    byte CRCTable[256];
};

#endif


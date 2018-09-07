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
#include <Stream.h>

// To use this class, first you have to enter the polinomial with begin().
// After that, you can get the CRC8 of a message with read().
// Also you can add more bytes at the end of the message without recalculing
// it from the beginning with add().
class ESAT_CRC8: public Stream
{
  public:
    // We take into account that the most significant bit of
    // any generator polynomial is always a one. For example:
    // If polynomial is: x8+x2+x+1, POLYNOMIAL= 0b00000111.
    ESAT_CRC8(byte polynomial);

    // Return 1 if the CRC remainder is available
    // (you wrote data to this CRC stream since the last flush/reset);
    // otherwise return 0.
    int available();

    // Reset the CRC computation.
    void flush();

    // Return the CRC remainder if it is available
    // (you wrote data to this CRC stream since the last flush/reset);
    // otherwise return -1.
    int peek();

    // Return the CRC remainder if it is available
    // (you wrote data to this CRC stream since the last flush/reset);
    // otherwise return -1.
    // Reset the CRC computation.
    int read();

    // Update the CRC remainder with a new byte datum.
    // Return 1.
    size_t write(uint8_t datum);

    // Import size_t Print::write(const uint8_t* buffer, size_t bufferLength).
    // Update the CRC remainder with the given message buffer.
    // Return the number of bytes written.
    using Print::write;

  private:
    byte CRCTable[256];

    // Current CRC remainder.
    int remainder = -1;
};

#endif


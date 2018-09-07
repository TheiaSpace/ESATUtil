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

#include "ESAT_CRC8.h"

int ESAT_CRC8::available()
{
  if (peek() >= 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void ESAT_CRC8::begin(const byte POLYNOMIAL)
{
  byte  remainder;
  const byte TOP_BIT = 0x80;
  // Compute the remainder of each possible dividend.
  for (int dividend = 0; dividend < 256; ++dividend)
  {
    // Start with the dividend followed by zeros.
    remainder = dividend;
    // Perform modulo-2 division, a bit at a time.
    for (byte bit = 8; bit > 0; --bit)
    {
      // Try to divide the current data bit.
      if (remainder & TOP_BIT)
      {
          remainder = (remainder << 1) ^ POLYNOMIAL;
      }
      else
      {
          remainder = (remainder << 1);
      }
    }
    // Store the result into the table.
    CRCTable[dividend] = remainder;
  }
  flush();
}

void ESAT_CRC8::flush()
{
  remainder = -1;
}

int ESAT_CRC8::peek()
{
  return remainder;
}

int ESAT_CRC8::read()
{
  const int crc = peek();
  flush();
  return crc;
}

size_t ESAT_CRC8::write(const uint8_t datum)
{
  if (remainder == -1)
  {
    remainder = 0;
  }
  const byte index = byte(datum) ^ byte(remainder);
  remainder = CRCTable[index];
  return 1;
}

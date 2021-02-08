/*
 * Copyright (C) 2018, 2019 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_FlagContainer.h"

ESAT_FlagContainer::ESAT_FlagContainer()
{
  clearAll();
}

word ESAT_FlagContainer::available() const
{
  word availableFlags = 0;
  for (word flag = 0;
       flag < MAXIMUM_NUMBER_OF_FLAGS;
       flag++)
  {
    if (read(flag))
    {
      availableFlags = availableFlags + 1;
    }
  }
  return availableFlags;
}

byte ESAT_FlagContainer::bitIndex(const byte flag) const
{
  return flag % NUMBER_OF_FLAGS_PER_BYTE;
}

byte ESAT_FlagContainer::byteIndex(const byte flag) const
{
  return flag / NUMBER_OF_FLAGS_PER_BYTE;
}

void ESAT_FlagContainer::clear(const byte flag)
{
  bitClear(flagBytes[byteIndex(flag)],
           bitIndex(flag));
}

void ESAT_FlagContainer::clearAll()
{
  for (byte index = 0;
       index < NUMBER_OF_FLAG_STORAGE_BYTES;
       index++)
  {
    flagBytes[index] = 0;
  }
}

size_t ESAT_FlagContainer::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  boolean firstActiveFlagAlreadyPrinted = false;
  for (word flag = 0;
       flag < MAXIMUM_NUMBER_OF_FLAGS;
       flag++)
  {
    const boolean active = read(flag);
    if (active)
    {
      if (firstActiveFlagAlreadyPrinted)
      {
        bytesWritten = bytesWritten + output.print(F(", "));
      }
      firstActiveFlagAlreadyPrinted = true;
      bytesWritten = bytesWritten + output.print(flag, DEC);
    }
  }
  return bytesWritten;
}

boolean ESAT_FlagContainer::read(const byte flag) const
{
  return bitRead(flagBytes[byteIndex(flag)],
                 bitIndex(flag));
}

boolean ESAT_FlagContainer::readFrom(Stream& stream)
{
  const byte bytesToRead = NUMBER_OF_FLAG_STORAGE_BYTES;
  byte buffer[bytesToRead];
  const size_t bytesRead = stream.readBytes((char*) buffer, bytesToRead);
  if (bytesRead < bytesToRead)
  {
    return false;
  }
  else
  {
    for (int index = 0; index < NUMBER_OF_FLAG_STORAGE_BYTES; index = index + 1)
    {
      flagBytes[index] = buffer[index];
    }
  }
}

int ESAT_FlagContainer::readNext() const
{
  for (word flag = 0;
       flag < MAXIMUM_NUMBER_OF_FLAGS;
       flag++)
  {
    if (read(flag))
    {
      return flag;
    }
  }
  return NO_ACTIVE_FLAGS;
}

void ESAT_FlagContainer::set(const byte flag)
{
  bitSet(flagBytes[byteIndex(flag)],
         bitIndex(flag));
}

ESAT_FlagContainer ESAT_FlagContainer::operator&(const ESAT_FlagContainer flags) const
{
  ESAT_FlagContainer result;
  for (word byteIndex = 0;
       byteIndex < NUMBER_OF_FLAG_STORAGE_BYTES;
       byteIndex++)
  {
    result.flagBytes[byteIndex] =
      flagBytes[byteIndex] & flags.flagBytes[byteIndex];
  }
  return result;
}

ESAT_FlagContainer ESAT_FlagContainer::operator~() const
{
  ESAT_FlagContainer result;
  for (word byteIndex = 0;
       byteIndex < NUMBER_OF_FLAG_STORAGE_BYTES;
       byteIndex++)
  {
    result.flagBytes[byteIndex] =
      ~flagBytes[byteIndex];
  }
  return result;
}

ESAT_FlagContainer ESAT_FlagContainer::operator|(const ESAT_FlagContainer flags) const
{
  ESAT_FlagContainer result;
  for (word byteIndex = 0;
       byteIndex < NUMBER_OF_FLAG_STORAGE_BYTES;
       byteIndex++)
  {
    result.flagBytes[byteIndex] =
      flagBytes[byteIndex] | flags.flagBytes[byteIndex];
  }
  return result;
}

ESAT_FlagContainer ESAT_FlagContainer::operator^(const ESAT_FlagContainer flags) const
{
  ESAT_FlagContainer result;
  for (word byteIndex = 0;
       byteIndex < NUMBER_OF_FLAG_STORAGE_BYTES;
       byteIndex++)
  {
    result.flagBytes[byteIndex] =
      flagBytes[byteIndex] ^ flags.flagBytes[byteIndex];
  }
  return result;
}

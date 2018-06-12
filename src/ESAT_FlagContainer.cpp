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
 * along with Theia Space's ESAT OBC library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESAT_FlagContainer.h"

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
  bitClear(flagValue[byteIndex(flag)],
           bitIndex(flag));
}

void ESAT_FlagContainer::clearAll()
{
  for (byte index = 0;
       index < NUMBER_OF_FLAG_STORAGE_BYTES;
       index++)
  {
    flagValue[index] = 0;
  }
}

size_t ESAT_FlagContainer::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  boolean firstActiveFlagAlreadyPrinted = false;
  for (int flag = 0;
       flag < MAXIMUM_NUMBER_OF_FLAGS;
       flag++)
  {
    const boolean active = read(flag);
    if (active)
    {
      if (firstActiveFlagAlreadyPrinted)
      {
        bytesWritten = bytesWritten + output.print(String(", "));
      }
      firstActiveFlagAlreadyPrinted = true;
      bytesWritten = bytesWritten + output.print(String(flag, DEC));
    }
  }
  return bytesWritten;
}

boolean ESAT_FlagContainer::read(const byte flag) const
{
  return bitRead(flagValue[byteIndex(flag)],
                 bitIndex(flag));
}

int ESAT_FlagContainer::readNext() const
{
  for (int flag = 0;
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
  bitSet(flagValue[byteIndex(flag)],
         bitIndex(flag));
}

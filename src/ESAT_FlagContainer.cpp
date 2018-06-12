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

byte ESAT_FlagContainer::bitIndex(const byte flagIdentifier) const
{
  return flagIdentifier % NUMBER_OF_BITS_PER_BYTE;
}

byte ESAT_FlagContainer::byteIndex(const byte flagIdentifier) const
{
  return flagIdentifier / NUMBER_OF_BITS_PER_BYTE;
}

void ESAT_FlagContainer::clear()
{
  for (byte index = 0;
       index < MAXIMUM_NUMBER_OF_FLAGS / NUMBER_OF_BITS_PER_BYTE;
       index++)
  {
    flagValue[index] = 0;
  }
}

size_t ESAT_FlagContainer::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  boolean firstActiveFlagAlreadyPrinted = false;
  for (int flagIdentifier = 0;
       flagIdentifier < MAXIMUM_NUMBER_OF_FLAGS;
       flagIdentifier++)
  {
    const boolean active = read(flagIdentifier);
    if (active)
    {
      if (firstActiveFlagAlreadyPrinted)
      {
        bytesWritten = bytesWritten + output.print(String(", "));
      }
      firstActiveFlagAlreadyPrinted = true;
      bytesWritten = bytesWritten + output.print(String(flagIdentifier, DEC));
    }
  }
  return bytesWritten;
}

boolean ESAT_FlagContainer::read(const byte flagIdentifier) const
{
  return bitRead(flagValue[byteIndex(flagIdentifier)],
                 bitIndex(flagIdentifier));
}

int ESAT_FlagContainer::readNext() const
{
  for (int flagIdentifier = 0;
       flagIdentifier < MAXIMUM_NUMBER_OF_FLAGS;
       flagIdentifier++)
  {
    if (read(flagIdentifier))
    {
      return flagIdentifier;
    }
  }
  return NO_ACTIVE_FLAGS;
}

void ESAT_FlagContainer::write(const byte flagIdentifier, const boolean active)
{
  if (active)
  {
    bitSet(flagValue[byteIndex(flagIdentifier)],
           bitIndex(flagIdentifier));
  }
  else
  {
    bitClear(flagValue[byteIndex(flagIdentifier)],
             bitIndex(flagIdentifier));
  }
}

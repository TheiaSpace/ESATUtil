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

void ESAT_FlagContainer::copyFrom(ESAT_FlagContainer FlagContainer)
{
  for(byte index = 0; index < MAXIMUM_NUMBER_OF_FLAG/NUMBER_OF_BITS_PER_BYTE; index++)
  {
    flagValue[index] = FlagContainer.flagValue[index];
  }
}

boolean ESAT_FlagContainer::read(byte flagID)
{
  byte arrayIndex = flagID/NUMBER_OF_BITS_PER_BYTE;
  byte mask = 1 << (flagID % NUMBER_OF_BITS_PER_BYTE);
  if (flagValue[arrayIndex] & mask)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int ESAT_FlagContainer::readNext()
{
  for(byte index = 0; index < MAXIMUM_NUMBER_OF_FLAG/NUMBER_OF_BITS_PER_BYTE; index++)
  {
    if(flagValue[index] > 0)
    {
      for(byte bit = 0; bit < NUMBER_OF_BITS_PER_BYTE; bit++)
      {
        if(flagValue[index] & (1 << bit))
        {
          return index*NUMBER_OF_BITS_PER_BYTE + bit;
        }
      }
    }
  }
  return ERROR_STATUS;
}

void ESAT_FlagContainer::clear()
{
  for(byte index = 0; index < MAXIMUM_NUMBER_OF_FLAG/NUMBER_OF_BITS_PER_BYTE; index++)
  {
    flagValue[index] = 0;
  }
}

void ESAT_FlagContainer::write(byte flagID, boolean value)
{
  byte arrayIndex = flagID/NUMBER_OF_BITS_PER_BYTE;
  byte mask = 1 << (flagID % NUMBER_OF_BITS_PER_BYTE);
  if (value)
  {
    flagValue[arrayIndex] |= mask;
  }
  else
  {
    flagValue[arrayIndex] &= ~mask;
  }
  
}

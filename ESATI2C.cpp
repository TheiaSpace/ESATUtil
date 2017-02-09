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
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESATI2C.h"
#include <Wire.h>

void ESATI2C::begin()
{
  Wire.begin();
}

byte ESATI2C::read(byte address, byte registerNumber, byte readBuffer[], byte readBufferLength)
{
  Wire.beginTransmission(address);
  Wire.write(registerNumber);
  const byte errorCode = Wire.endTransmission();
  if (errorCode == 0)
  {
    Wire.requestFrom(address, readBufferLength);
    for (int index = 0; index < readBufferLength; index++)
    {
      readBuffer[index] = Wire.read();
    }
  }
  return errorCode;
}

byte ESATI2C::write(byte address, byte registerNumber, byte data)
{
  Wire.beginTransmission(address);
  Wire.write(registerNumber);
  Wire.write(data);
  return Wire.endTransmission();
}

byte ESATI2C::write(byte address, byte registerNumber, const byte data[], byte dataLength)
{
  Wire.beginTransmission(address);
  Wire.write(registerNumber);
  Wire.write(data, dataLength);
  return Wire.endTransmission();
}

ESATI2C I2C;

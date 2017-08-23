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

#include "ESATI2CDevice.h"
#include "ESATUtil.h"

ESATI2CDevice::ESATI2CDevice(TwoWire& bus, const byte address):
  bus(bus), address(address), error(false)
{
}

word ESATI2CDevice::readBigEndianWord(const byte registerNumber)
{
  bus.beginTransmission(address);
  bus.write(registerNumber);
  const byte status = bus.endTransmission();
  if (status != 0)
  {
    error = true;
    return 0;
  }
  bus.requestFrom(uint8_t(address), uint8_t(1));
  const int firstByte = bus.read();
  if (firstByte < 0)
  {
    error = true;
    return 0;
  }
  const int secondByte = bus.read();
  if (secondByte < 0)
  {
    error = true;
    return 0;
  }
  error = false;
  return word(firstByte, secondByte);
}

byte ESATI2CDevice::readByte(const byte registerNumber)
{
  bus.beginTransmission(address);
  bus.write(registerNumber);
  const byte status = bus.endTransmission();
  if (status != 0)
  {
    error = true;
    return 0;
  }
  bus.requestFrom(uint8_t(address), uint8_t(1));
  const int reading = bus.read();
  if (reading < 0)
  {
    error = true;
    return 0;
  }
  error = false;
  return byte(reading);
}

word ESATI2CDevice::readLittleEndianWord(const byte registerNumber)
{
  return Util.swapWordBytes(readBigEndianWord(registerNumber));
}

void ESATI2CDevice::writeBigEndianWord(const byte registerNumber, const word datum)
{
   bus.beginTransmission(address);
  bus.write(registerNumber);
  bus.write(highByte(datum));
  bus.write(lowByte(datum));
  const byte status = bus.endTransmission();
  if (status == 0)
  {
    error = false;
  }
  else
  {
    error = true;
  }
}

void ESATI2CDevice::writeByte(const byte registerNumber, const byte datum)
{
  const byte data[] = { datum };
  writeBytes(registerNumber, data, 1);
}

void ESATI2CDevice::writeBytes(const byte registerNumber, const byte* const data, const byte length)
{
  bus.beginTransmission(address);
  bus.write(registerNumber);
  bus.write(data, length);
  const byte status = bus.endTransmission();
  if (status == 0)
  {
    error = false;
  }
  else
  {
    error = true;
  }
}

void ESATI2CDevice::writeLittleEndianWord(const byte registerNumber, const word datum)
{
  writeBigEndianWord(registerNumber, Util.swapWordBytes(datum));
}

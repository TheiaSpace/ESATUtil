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

#ifndef ESATI2CDevice_h
#define ESATI2CDevice_h

#include <Energia.h>
#include <Wire.h>

// For communicating with I2C devices.
class ESATI2CDevice
{
  public:
    // Error flag.  Set to true on communication error.
    // Reset it manually.
    boolean error;

    // Instantiate a new I2C device.
    ESATI2CDevice(TwoWire& bus, byte address);

    // Read a big-endian word at the given resgister number.
    word readBigEndianWord(byte registerNumber);

    // Read a byte at the given register number.
    byte readByte(byte registerNumber);

    // Read a little-endian word at the given register number.
    word readLittleEndianWord(byte registerNumber);

    // Write a big-endian word to the given register number.
    void writeBigEndianWord(byte registerNumber, word datum);

    // Write a byte to the given register number.
    void writeByte(byte registerNumber, byte datum);

    // Write length data bytes to the given register number.
    void writeBytes(byte registerNumber, const byte data[], byte length);

    // Write a little-endian word to the given register number.
    void writeLittleEndianWord(byte registerNumber, word datum);

  private:
    // Communicate through this I2C bus.
    TwoWire& bus;

    // Communicate with this address.
    const byte address;
};

#endif

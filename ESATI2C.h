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

#ifndef ESATI2C_h
#define ESATI2C_h

#include <Energia.h>

class ESATI2C
{
  public:
    // Configure the I2C bus.
    void begin();

    // Read to readBuffer from address:registerNumber at the I2C bus.
    // Return the status of the transmission:
    //  0: success
    //  1: data too long to fit in transmit buffer
    //  2: received NACK on transmit of address
    //  3: received NACK on transmit of data
    //  4: other error
    byte read(byte address, byte registerNumber, byte readBuffer[], byte readBufferLength);

    // Write a byte to address:registerNumber at the I2C bus.
    // Return the status of the transmission:
    //  0: success
    //  1: data too long to fit in transmit buffer
    //  2: received NACK on transmit of address
    //  3: received NACK on transmit of data
    //  4: other error
    byte write(byte address, byte registerNumber, byte data);

    // Write an array of bytes to address:registerNumber at the I2C bus.
    // Return the status of the transmission:
    //  0: success
    //  1: data too long to fit in transmit buffer
    //  2: received NACK on transmit of address
    //  3: received NACK on transmit of data
    //  4: other error
    byte write(byte address, byte registerNumber, const byte data[], byte dataLength);
};

extern ESATI2C I2C;

#endif

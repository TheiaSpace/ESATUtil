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

#ifndef ESAT_FLAG_CONTAINER_H
#define ESAT_FLAG_CONTAINER_H

#include <Arduino.h>

// It handles a list of flags. The flag ID can go from 0 to 
// (MAXIMUM_NUMBER_OF_FLAG - 1). You can clear() all the flags (set as false),
// read() the value of one (true or false) or set the value (true or false)
// with write(). Also you can get the next activated flag (its value is true)
// with readNext().
class ESAT_FlagContainer: public Printable
{
  public:
    // Deactivate all the flags (all the flags value set as false).
    void clear();

    // Print the true flags in human-readable form.
    // Return the number of characters written.
    size_t printTo(Print& output) const;

    // Read the current status of the "flagIdentifier" flag.
    boolean read(byte flagIdentifier) const;

    // Return the flag ID of the first activated flag turned on.
    // It do not change the value of any flag.
    // If all the flags are deactivated, returns ERROR_STATUS.
    int readNext() const;

    // Write the value of a flag
    void write(byte flagIdentifier, boolean active);

  private:
    // MAXIMUM_NUMBER_OF_FLAG has to be a multiple of NUMBER_OF_BITS_PER_BYTE
    static const word MAXIMUM_NUMBER_OF_FLAG = 256;
    static const byte NUMBER_OF_BITS_PER_BYTE = 8;
    byte flagValue[MAXIMUM_NUMBER_OF_FLAG/NUMBER_OF_BITS_PER_BYTE];
    static const int ERROR_STATUS = -1;

    // Return the bit index within the byte corresponding to the given
    // flagIdentifier.
    byte bitIndex(byte flagIdentifier) const;

    // Return the byte index corresponding to the given flagIdentifier.
    byte byteIndex(byte flagIdentifier) const;
};

#endif


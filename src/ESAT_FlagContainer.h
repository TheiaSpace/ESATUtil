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

#ifndef ESAT_FlagContainer_h
#define ESAT_FlagContainer_h

#include <Arduino.h>

// A flag container is a list of numbered boolean flags.
// There are some operations, like clear(), set() and read(),
// that are analogous to bitClear(), bitSet() and bitRead():
// they clear, set and read boolean flags.
// In addition, it is possible to clear all the flags with clearAll()
// and get the number of the first true flag with readNext().
// ESAT_FlagContainer objects are Printable: Print (and thus Stream)
// objects can print them in human-readable form.
class ESAT_FlagContainer: public Printable
{
  public:
    // Return value of readNext() when all flags are false.
    static const int NO_ACTIVE_FLAGS = -1;

    // Return the number of set or active (true) flags.
    byte available() const;

    // Set a flag to false.
    void clear(byte flag);

    // Deactivate all the flags (all the flags value set as false).
    void clearAll();

    // Print the true flags in human-readable form.
    // Return the number of characters written.
    size_t printTo(Print& output) const;

    // Return the value of a flag.
    boolean read(byte flag) const;

    // Return the number of the first flag, starting
    // from 0, with a true value.
    // Calling this function doesn't change the value of any flag.
    // If all the flags are false, return a negative number
    // (NO_ACTIVE_FLAGS).
    int readNext() const;

    // Set a flag to true.
    void set(byte flag);

    // Return a flag container with flags that are the and operation
    // of the flags of the operands.
    ESAT_FlagContainer operator&(const ESAT_FlagContainer flags) const;

    // Return a flag container with flags that are the not operation
    // of the flags of the operand.
    ESAT_FlagContainer operator~() const;

    // Return a flag container with flags that are the inclusive-or
    // operation of the flags of the operands.
    ESAT_FlagContainer operator|(const ESAT_FlagContainer flags) const;

    // Return a flag container with flags that are the exclusive-or
    // operation of the flags of the operands.
    ESAT_FlagContainer operator^(const ESAT_FlagContainer flags) const;

  private:
    // Maximum number of flags a flag container can store.
    // This has to be a multiple of NUMBER_OF_FLAGS_PER_BYTE.
    static const word MAXIMUM_NUMBER_OF_FLAGS = 256;

    // Number of flags stored in each byte.
    static const byte NUMBER_OF_FLAGS_PER_BYTE = 8;

    // Number of bytes used to store the flags.
    static constexpr byte NUMBER_OF_FLAG_STORAGE_BYTES =
      MAXIMUM_NUMBER_OF_FLAGS / NUMBER_OF_FLAGS_PER_BYTE;

    // Store flags compactly in this array of bytes: 8 flags per byte.
    byte flagBytes[NUMBER_OF_FLAG_STORAGE_BYTES];

    // Return the bit index within the byte corresponding to the given
    // flag.
    byte bitIndex(byte flag) const;

    // Return the byte index corresponding to the given flag.
    byte byteIndex(byte flag) const;
};

#endif /* ESAT_FlagContainer_h */

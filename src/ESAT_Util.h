/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
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
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef ESAT_Util_h
#define ESAT_Util_h

#include <Arduino.h>

// General utility library.
// Use the global instance ESAT_Util.
class ESAT_UtilClass
{
  public:
    // Return the signed 8-bit integer corresponding to the given
    // two's-complement bits.
    signed char byteToChar(byte bits) const;

    // Return the bits of the two's-complement representation
    // of a signed 8-bit integer.
    byte charToByte(signed char number) const;

    // Compare 2 byte arrays. Return true if they are equal;
    // otherwise return false.
    boolean compareByteArray(byte array1[],byte array2[], byte arraysSize);

    // Decode a binary coded decimal 8-bit number.
    byte decodeBinaryCodedDecimalByte(byte number) const;

    // Decode a binary coded decimal 16-bit number.
    word decodeBinaryCodedDecimalWord(word number) const;

    // Encode an 8-bit number in binary coded decimal format.
    byte encodeBinaryCodedDecimalByte(byte number) const;

    // Encode a 16-bit number in binary coded decimal format.
    word encodeBinaryCodedDecimalWord(word number) const;

    // Return the hexadecimal representation of a one-byte number.
    String byteToHexadecimal(byte number) const;

    // Return the IEEE 754 binary32 bits of the given
    // single-precision floating-point number.
    unsigned long floatToUnsignedLong(float number) const;

    // Convert a hexadecimal string to a byte.
    byte hexadecimalToByte(String hexadecimalNumber) const;

    // Convert a hexadecimal string to a word.
    word hexadecimalToWord(String hexadecimalNumber) const;

    // Return the most-significant 16-bit unsigned integer word of a
    // 32-bit unsigned integer.
    word highWord(unsigned long number) const;

    // Return the bits of the two's-complement representation
    // of a signed 16-bit integer.
    word intToWord(int number) const;

    // Return the bits of the two's-complement representation
    // of a signed 32-bit integer.
    unsigned long longToUnsignedLong(long number) const;

    // Return the least-significant 16-bit unsigned integer word of a
    // 32-bit unsigned integer.
    word lowWord(unsigned long number) const;

    // Pad a string with a leading padding character to a given total length.
    String pad(String text, char padding, unsigned int length) const;

    // Swap the bytes of a word.
    word swapWordBytes(word number) const;

    // Return the 32-bit unsigned integer made from the given 8-bit
    // unsigned integer bytes, from most-significant byte to
    // least-significant byte.
    unsigned long unsignedLong(byte highByte,
                               byte mediumHighByte,
                               byte mediumLowByte,
                               byte lowByte) const;

    // Return the 32-bit unsigned integer made from the given 16-bit
    // unsigned integer words, from most-significant word to
    // least-significant word.
    unsigned long unsignedLong(word highWord, word lowWord) const;

    // Return the single-precision floating-point number corresponding
    // to the given IEEE 754 binary32 bits.
    float unsignedLongToFloat(unsigned long bits) const;

    // Return the signed 32-bit integer corresponding to the given
    // two's-complement bits.
    long unsignedLongToLong(unsigned long bits) const;

    // Return the hexadecimal representation of a two-byte number.
    String wordToHexadecimal(word number) const;

    // Return the signed 16-bit integer corresponding to the given
    // two's-complement bits.
    int wordToInt(word bits) const;
};

// Global instance of the utility library.
extern ESAT_UtilClass ESAT_Util;

#endif /* ESAT_Util_h */

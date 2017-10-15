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

#ifndef ESATUtil_h
#define ESATUtil_h

#include <Arduino.h>

class ESATUtil
{
  public:
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

    // Convert a hexadecimal string to a byte.
    byte hexadecimalToByte(String hexadecimalNumber) const;

    // Convert a hexadecimal string to a word.
    word hexadecimalToWord(String hexadecimalNumber) const;

    // Pad a string with a leading padding character to a given total length.
    String pad(String text, char padding, unsigned int length) const;

    // Swap the bytes of a word.
    word swapWordBytes(word number) const;

    // Return the hexadecimal representation of a two-byte number.
    String wordToHexadecimal(word number) const;
};

extern ESATUtil Util;

#endif

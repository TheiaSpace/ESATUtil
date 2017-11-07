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

#include "ESAT_Util.h"

signed char ESAT_UtilClass::byteToChar(const byte bits) const
{
  if (bits > 127)
  {
    return -((signed char) ((~bits) + 1));
  }
  else
  {
    return bits;
  }
}

byte ESAT_UtilClass::charToByte(const signed char number) const
{
  if (number < 0)
  {
    return ~((byte) -(number + 1));
  }
  else
  {
    return number;
  }
}

byte ESAT_UtilClass::decodeBinaryCodedDecimalByte(const byte number) const
{
  const byte tens =
    constrain(number / 0x10, 0, 9);
  const byte ones =
    number - tens * 0x10;
  return 10 * tens + ones;
}

word ESAT_UtilClass::decodeBinaryCodedDecimalWord(const word number) const
{
  const word thousands =
    constrain(number / 0x1000, 0, 9);
  const word hundreds =
    (number - thousands * 0x1000) / 0x0100;
  const word tens =
    (number - thousands * 0x1000 - hundreds * 0x0100) / 0x0010;
  const word ones =
    (number - thousands * 0x1000 - hundreds * 0x0100 - tens * 0x0010);
  return 1000 * thousands + 100 * hundreds + 10 * tens + ones;
}

byte ESAT_UtilClass::encodeBinaryCodedDecimalByte(const byte number) const
{
  const byte tens = (number % 100) / 10;
  const byte ones = number % 10;
  return (tens << 4) | ones;
}

word ESAT_UtilClass::encodeBinaryCodedDecimalWord(const word number) const
{
  const word thousands = (number % 10000) / 1000;
  const word hundreds = (number % 1000) / 100;
  const word tens = (number % 100) / 10;
  const word ones = number % 10;
  return (thousands << 12) | (hundreds << 8) | (tens << 4) | ones;
}

String ESAT_UtilClass::byteToHexadecimal(const byte number) const
{
  String unpaddedText = String(int(number), HEX);
  String paddedText = pad(unpaddedText, '0', 2);
  return paddedText;
}

unsigned long ESAT_UtilClass::floatToUnsignedLong(const float number) const
{
  if (number == 0)
  {
    return 0x00000000ul;
  }
  if (number == NAN)
  {
    return 0xFFFFFFFFul;
  }
  if (number == INFINITY)
  {
    return 0x7F800000ul;
  }
  if (number == -INFINITY)
  {
    return 0xFF800000ul;
  }
  const unsigned long signBit = ((number >= 0) ? 0 : 1);
  float mantissa = number;
  if (mantissa < 0)
  {
    mantissa = -mantissa;
  }
  int exponent = 0;
  while (mantissa >= 2)
  {
    mantissa = mantissa / 2;
    exponent = exponent + 1;
  }
  while (mantissa < 1)
  {
    mantissa = 2 * mantissa;
    exponent = exponent - 1;
  }
  mantissa = (mantissa - 1) * 8388608.5;
  const unsigned long mantissaBits = ((unsigned long) mantissa) & 0x007FFFFF;
  const unsigned long exponentBits = exponent + 127;
  return (signBit << 31) | (exponentBits << 23) | mantissaBits;
}

byte ESAT_UtilClass::hexadecimalToByte(const String hexadecimalNumber) const
{
  return byte(hexadecimalToWord(hexadecimalNumber));
}

word ESAT_UtilClass::hexadecimalToWord(const String hexadecimalNumber) const
{
  word number = 0;
  for (int index = 0; index < hexadecimalNumber.length(); index++)
  {
    const char character = hexadecimalNumber.charAt(index);
    byte digit = 0;
    switch (character)
    {
      case '0':
        digit = 0;
        break;
      case '1':
        digit = 1;
        break;
      case '2':
        digit = 2;
        break;
      case '3':
        digit = 3;
        break;
      case '4':
        digit = 4;
        break;
      case '5':
        digit = 5;
        break;
      case '6':
        digit = 6;
        break;
      case '7':
        digit = 7;
        break;
      case '8':
        digit = 8;
        break;
      case '9':
        digit = 9;
        break;
      case 'a':
      case 'A':
        digit = 0xa;
        break;
      case 'b':
      case 'B':
        digit = 0xb;
        break;
      case 'c':
      case 'C':
        digit = 0xc;
        break;
      case 'd':
      case 'D':
        digit = 0xd;
        break;
      case 'e':
      case 'E':
        digit = 0xe;
        break;
      case 'f':
      case 'F':
        digit = 0xf;
        break;
      default:
        return 0;
        break;
    }
    number = number * 0x10 + digit;
  }
  return number;
}

word ESAT_UtilClass::intToWord(const int number) const
{
  if (number < 0)
  {
    return ~((word) -(number + 1));
  }
  else
  {
    return number;
  }
}

unsigned long ESAT_UtilClass::longToUnsignedLong(const long number) const
{
  if (number < 0)
  {
    return ~((unsigned long) -(number + 1));
  }
  else
  {
    return number;
  }
}

String ESAT_UtilClass::pad(const String text,
                           const char padding,
                           const unsigned int length) const
{
  String paddingString = String(padding);
  String paddedText = text;
  while (paddedText.length() < length)
  {
    paddedText = paddingString + paddedText;
  }
  return paddedText;
}

word ESAT_UtilClass::swapWordBytes(const word number) const
{
  return word(lowByte(number), highByte(number));
}

float ESAT_UtilClass::unsignedLongToFloat(const unsigned long bits) const
{
  if (bits == 0x00000000ul)
  {
    return 0;
  }
  if (bits >= 0xFF000001ul)
  {
    return NAN;
  }
  if (bits == 0x7F800000ul)
  {
    return INFINITY;
  }
  if (bits == 0xFF800000ul)
  {
    return -INFINITY;
  }
  const unsigned long signBit = (bits >> 31) & 0x00000001;
  const unsigned long exponentBits = (bits >> 23) & 0x000000FF;
  const unsigned long mantissaBits = bits & 0x007FFFFF;
  float number = 1 + mantissaBits / 8388608.5;
  if (signBit == 1)
  {
    number = -number;
  }
  int exponent = exponentBits - 127;
  if (exponent > 0)
  {
    for (int i = 0; i < exponent; i++)
    {
      number = 2 * number;
    }
  }
  if (exponent < 0)
  {
    for (int i = 0; i > exponent; i--)
    {
      number = number / 2;
    }
  }
  return number;
}

long ESAT_UtilClass::unsignedLongToLong(const unsigned long bits) const
{
  if (bits > 2147483647UL)
  {
    return -long((~bits) + 1UL);
  }
  else
  {
    return bits;
  }
}

String ESAT_UtilClass::wordToHexadecimal(const word number) const
{
  String text =
    byteToHexadecimal(highByte(number)) + byteToHexadecimal(lowByte(number));
  return text;
}

int ESAT_UtilClass::wordToInt(const word bits) const
{
  if (bits > 32767U)
  {
    return -int((~bits) + 1U);
  }
  else
  {
    return bits;
  }
}

ESAT_UtilClass ESAT_Util;

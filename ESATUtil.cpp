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

#include "ESATUtil.h"

String ESATUtil::byteToHexadecimal(byte number)
{
  String unpaddedText = String(int(number), HEX);
  String paddedText = pad(unpaddedText, '0', 2);
  return paddedText;
}

byte ESATUtil::hexadecimalToByte(String hexadecimalNumber)
{
  return byte(hexadecimalToWord(hexadecimalNumber));
}

word ESATUtil::hexadecimalToWord(String hexadecimalNumber)
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

String ESATUtil::intToHexadecimal(unsigned int number)
{
  byte firstByte = byte(number >> 8);
  byte secondByte = byte(number & 255);
  String text = byteToHexadecimal(firstByte) + byteToHexadecimal(secondByte);
  return text;
}

String ESATUtil::pad(String text, char padding, unsigned int length)
{
  String paddingString = String(padding);
  String paddedText = text;
  while (paddedText.length() < length)
  {
    paddedText = paddingString + paddedText;
  }
  return paddedText;
}

word ESATUtil::swapWordBytes(const word number)
{
  return word(lowByte(number), highByte(number));
}

String ESATUtil::wordToHexadecimal(const word number)
{
  String text =
    byteToHexadecimal(highByte(number)) + byteToHexadecimal(lowByte(number));
  return text;
}

ESATUtil Util;

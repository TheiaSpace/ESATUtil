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

ESATUtil Util;

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

#include <Energia.h>

class ESATUtil
{
  public:
    // Return the hexadecimal representation of a one-byte number.
    String byteToHexadecimal(byte number);

    // Return the hexadecimal representation of a two-byte number.
    String intToHexadecimal(unsigned int number);

    // Pad a string with a leading padding character to a given total length.
    String pad(String text, char padding, unsigned int length);
};

extern ESATUtil Util;

#endif

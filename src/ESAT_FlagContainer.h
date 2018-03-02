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

#include <arduino.h>

// 
class ESAT_FlagContainer
{
  public:
    // 
    void copyFrom(ESAT_FlagContainer FlagContainer);

    // 
    boolean read(byte flagID);
    
    //
    int readNext();

    //
    void clear();

    // 
    void write(byte flagID, boolean value);

  private:
    static const word MAXIMUM_NUMBER_OF_FLAG = 256;
    static const byte NUMBER_OF_BITS_PER_BYTE = 8;
    byte flagValue[MAXIMUM_NUMBER_OF_FLAG/NUMBER_OF_BITS_PER_BYTE];
    static const int ERROR_STATUS = -1;
};

#endif


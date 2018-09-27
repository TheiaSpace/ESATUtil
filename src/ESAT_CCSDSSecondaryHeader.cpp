/*
 * Copyright (C) 2017 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_CCSDSSecondaryHeader.h"
#include "ESAT_Util.h"

size_t ESAT_CCSDSSecondaryHeader::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten =
    bytesWritten + output.println(F("{"));
  bytesWritten =
    bytesWritten + output.print(F("  \"preamble\": "));
  switch (preamble)
  {
    case CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION:
      bytesWritten =
        bytesWritten + output.print(F("\"CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION\""));
      break;
    default:
      bytesWritten =
        bytesWritten + output.print(F("0x"));
      bytesWritten =
        bytesWritten + output.print(ESAT_Util.byteToHexadecimal(preamble));
      break;
  }
  bytesWritten =
    bytesWritten + output.println(F(","));
  bytesWritten =
    bytesWritten + output.print(F("  \"timestamp\": "));
  bytesWritten =
    bytesWritten + output.print(timestamp);
  bytesWritten =
    bytesWritten + output.println(F(","));
  bytesWritten =
    bytesWritten + output.print(F("  \"versionNumber\": \""));
  bytesWritten =
    bytesWritten + output.print(majorVersionNumber, DEC);
  bytesWritten =
    bytesWritten + output.print(F("."));
  bytesWritten =
    bytesWritten + output.print(minorVersionNumber, DEC);
  bytesWritten =
    bytesWritten + output.print(F("."));
  bytesWritten =
    bytesWritten + output.print(patchVersionNumber, DEC);
  bytesWritten =
    bytesWritten + output.println(F("\","));
  bytesWritten =
    bytesWritten + output.print(F("  \"packetIdentifier\": 0x"));
  bytesWritten =
    bytesWritten + output.print(ESAT_Util.byteToHexadecimal(packetIdentifier));
  bytesWritten =
    bytesWritten + output.println(F(""));
  bytesWritten =
    bytesWritten + output.print(F("}"));
  return bytesWritten;
}

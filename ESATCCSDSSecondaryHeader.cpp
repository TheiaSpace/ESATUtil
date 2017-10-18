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

#include "ESATCCSDSSecondaryHeader.h"

size_t ESATCCSDSSecondaryHeader::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten =
    bytesWritten + output.println(String("{"));
  bytesWritten =
    bytesWritten + output.print(String("  \"preamble\": "));
  switch (preamble)
  {
    case CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION:
      bytesWritten =
        bytesWritten + output.print(String("\"CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION\""));
      break;
    default:
      bytesWritten =
        bytesWritten + output.print(String(preamble, HEX));;
      break;
  }
  bytesWritten =
    bytesWritten + output.println(String(","));
  bytesWritten =
    bytesWritten + output.print(String("  \"timestamp\": "));
  bytesWritten =
    bytesWritten + output.print(timestamp);
  bytesWritten =
    bytesWritten + output.println(String(","));
  bytesWritten =
    bytesWritten + output.print(String("  \"versionNumber\": \""));
  bytesWritten =
    bytesWritten + output.print(String(majorVersionNumber, DEC));
  bytesWritten =
    bytesWritten + output.print(String("."));
  bytesWritten =
    bytesWritten + output.print(String(minorVersionNumber, DEC));
  bytesWritten =
    bytesWritten + output.print(String("."));
  bytesWritten =
    bytesWritten + output.print(String(patchVersionNumber, DEC));
  bytesWritten =
    bytesWritten + output.println(String("\","));
  bytesWritten =
    bytesWritten + output.print(String("  \"packetIdentifier\": "));
  bytesWritten =
    bytesWritten + output.print(String(packetIdentifier, HEX));
  bytesWritten =
    bytesWritten + output.println(String(""));
  bytesWritten =
    bytesWritten + output.print(String("}"));
  return bytesWritten;
}

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

#ifndef ESAT_CCSDSSecondaryHeader_h
#define ESAT_CCSDSSecondaryHeader_h

#include <Arduino.h>
#include <ESAT_Timestamp.h>

// Secondary header used for ESAT's packets.  It contains the following fields:
// - A time code with a preamble (1 byte) followed by a timestamp (7 bytes).
// - A version number in major.minor.patch format (3 bytes).
// - A packet identifier (1 byte).
// The only supported time code format is calendar segmented time code,
// month of year/day of month variation, 1 second resolution.
class ESAT_CCSDSSecondaryHeader: public Printable
{
  public:
    // Supported time code types.  Currently, the only supported
    // calendar time code type is the calendar segmented time code,
    // month of year/day of month variation, 1 second resolution.
    enum Preamble
    {
      CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION =
        B01010000,
    };

    // Number of bytes the secondary header takes when stored in CCSDS
    // packets.
    static const byte LENGTH = 12;

    // Preamble field that identifies the time code type.
    Preamble preamble;

    // Timestamp.
    ESAT_Timestamp timestamp;

    // Version number in major.minor.patch format
    // as defined in the Semantic Versioning 2.0.0 standard.
    byte majorVersionNumber;
    byte minorVersionNumber;
    byte patchVersionNumber;

    // For telemetry: telemetry packet type (housekeeping, event...).
    // For telecommands: command code (, start experiment...).
    byte packetIdentifier;

    // Print the secondary header in human readable (JSON) form.
    // Return the number of characters written.
    size_t printTo(Print& output) const;
};

#endif /* ESAT_CCSDSSecondaryHeader_h */

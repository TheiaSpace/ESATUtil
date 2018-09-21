/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_SemanticVersionNumber_h
#define ESAT_SemanticVersionNumber_h

#include <Arduino.h>

// Semantic version number in major.minor.patch format
// as defined in the Semantic Versioning 2.0.0 standard.
class ESAT_SemanticVersionNumber: public Printable
{
  public:
    // Major version number.
    // An increment in the major version number denotes a
    // backwards-incompatible change.
    byte majorVersionNumber;

    // Minor version number.
    // An increment in the minor version number denotes a
    // backwards-compatible change that includes new features.
    byte minorVersionNumber;

    // Patch version number.
    // An increment in the patch version number denotes a
    // backwards-compatible change that fixes an error.
    byte patchVersionNumber;

    // Build a semantic version number with all numbers set to 0.
    ESAT_SemanticVersionNumber();

    // Build a semantic version number with the given numbers.
    ESAT_SemanticVersionNumber(byte major, byte minor, byte patch);

    // Print the version number in human readable
    // (majorVersionNumber.minorVersionNumber.patchVersionNumber)
    // form.
    // Return the number of characters written.
    size_t printTo(Print& output) const;

    // Read the three 8-bit unsigned integer version numbers
    // (majorVersionNumber, minorVersionNumber and patchVersionNumber)
    // from a stream.
    // Return true on success; otherwise return false.
    boolean readFrom(Stream& stream);

    // Write the three 8-bit unsigned integer version numbers
    // (majorVersionNumber, minorVersionNumber and patchVersionNumber)
    // to a stream.
    // Return true on success; otherwise return false.
    boolean writeTo(Stream& stream) const;
};

#endif /* ESAT_SemanticVersionNumber_h */

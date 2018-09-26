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

#include "ESAT_SemanticVersionNumber.h"

ESAT_SemanticVersionNumber::ESAT_SemanticVersionNumber()
{
}

ESAT_SemanticVersionNumber::ESAT_SemanticVersionNumber(const byte major,
                                                       const byte minor,
                                                       const byte patch)
{
  majorVersionNumber = major;
  minorVersionNumber = minor;
  patchVersionNumber = patch;
}

size_t ESAT_SemanticVersionNumber::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten = bytesWritten + output.print(String(majorVersionNumber, DEC));
  bytesWritten = bytesWritten + output.print(String("."));
  bytesWritten = bytesWritten + output.print(String(minorVersionNumber, DEC));
  bytesWritten = bytesWritten + output.print(String("."));
  bytesWritten = bytesWritten + output.print(String(patchVersionNumber, DEC));
  return bytesWritten;
}

boolean ESAT_SemanticVersionNumber::readFrom(Stream& stream)
{
  const byte bytesToRead = 3;
  byte buffer[bytesToRead];
  const size_t bytesRead = stream.readBytes((char*) buffer, bytesToRead);
  if (bytesRead < bytesToRead)
  {
    return false;
  }
  else
  {
    majorVersionNumber = buffer[0];
    minorVersionNumber = buffer[1];
    patchVersionNumber = buffer[2];
    return true;
  }
}

boolean ESAT_SemanticVersionNumber::writeTo(Stream& stream) const
{
  byte buffer[] =
    { majorVersionNumber, minorVersionNumber, patchVersionNumber };
  const size_t bytesToWrite = sizeof(buffer);
  const size_t bytesWritten = stream.write(buffer, bytesToWrite);
  if (bytesWritten < bytesToWrite)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean ESAT_SemanticVersionNumber::operator==(ESAT_SemanticVersionNumber version) const
{
  if (majorVersionNumber != version.majorVersionNumber)
  {
    return false;
  }
  if (minorVersionNumber != version.minorVersionNumber)
  {
    return false;
  }
  if (patchVersionNumber != version.patchVersionNumber)
  {
    return false;
  }
  return true;
}

boolean ESAT_SemanticVersionNumber::operator!=(const ESAT_SemanticVersionNumber version) const
{
  if (*this == version)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean ESAT_SemanticVersionNumber::operator<(const ESAT_SemanticVersionNumber version) const
{
  // Compare the major version numbers first.
  if (majorVersionNumber < version.minorVersionNumber)
  {
    return true;
  }
  if (majorVersionNumber > version.majorVersionNumber)
  {
    return false;
  }
  // The major version numbers match if we got here,
  // so it's time to compare the minor version number.
  if (minorVersionNumber < version.minorVersionNumber)
  {
    return true;
  }
  if (minorVersionNumber > version.minorVersionNumber)
  {
    return false;
  }
  // The major version numbers match and the minor version numbers
  // match if we got here, so it's time to compare the patch version
  // number.
  if (patchVersionNumber < version.patchVersionNumber)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_SemanticVersionNumber::operator<=(const ESAT_SemanticVersionNumber version) const
{
  if (*this == version)
  {
    return true;
  }
  if (*this < version)
  {
    return true;
  }
  return false;
}

boolean ESAT_SemanticVersionNumber::operator>(const ESAT_SemanticVersionNumber version) const
{
  if (*this <= version)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean ESAT_SemanticVersionNumber::operator>=(const ESAT_SemanticVersionNumber version) const
{
  if (*this < version)
  {
    return false;
  }
  else
  {
    return true;
  }
}

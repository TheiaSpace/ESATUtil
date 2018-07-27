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

#include "ESAT_CCSDSPacketBuilder.h"

ESAT_CCSDSPacketBuilder::ESAT_CCSDSPacketBuilder()
{
  clock = nullptr;
}

ESAT_CCSDSPacketBuilder::ESAT_CCSDSPacketBuilder(const word theApplicationProcessIdentifier,
                                                 const byte theMajorVersionNumber,
                                                 const byte theMinorVersionNumber,
                                                 const byte thePatchVersionNumber,
                                                 ESAT_Clock& theClock)
{
  applicationProcessIdentifier = theApplicationProcessIdentifier;
  majorVersionNumber = theMajorVersionNumber;
  minorVersionNumber = theMinorVersionNumber;
  patchVersionNumber = thePatchVersionNumber;
  clock = &theClock;
  packetSequenceCount = 0;
}

boolean ESAT_CCSDSPacketBuilder::buildTelecommandPacket(ESAT_CCSDSPacket& packet,
                                                        ESAT_CCSDSPacketContents& contents)
{
  if (!clock)
  {
    return false;
  }
  if (packet.capacity() < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return false;
  }
  packet.writeTelecommandHeaders(applicationProcessIdentifier,
                                 packetSequenceCount,
                                 clock->read(),
                                 majorVersionNumber,
                                 minorVersionNumber,
                                 patchVersionNumber,
                                 contents.packetIdentifier());
  const boolean userDataCorrect = contents.fillUserData(packet);
  if (userDataCorrect)
  {
    packetSequenceCount = packetSequenceCount + 1;
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_CCSDSPacketBuilder::buildTelemetryPacket(ESAT_CCSDSPacket& packet,
                                                      ESAT_CCSDSPacketContents& contents)
{
  if (!clock)
  {
    return false;
  }
  if (packet.capacity() < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return false;
  }
  packet.writeTelemetryHeaders(applicationProcessIdentifier,
                               packetSequenceCount,
                               clock->read(),
                               majorVersionNumber,
                               minorVersionNumber,
                               patchVersionNumber,
                               contents.packetIdentifier());
  const boolean userDataCorrect = contents.fillUserData(packet);
  if (userDataCorrect)
  {
    packetSequenceCount = packetSequenceCount + 1;
    return true;
  }
  else
  {
    return false;
  }
}

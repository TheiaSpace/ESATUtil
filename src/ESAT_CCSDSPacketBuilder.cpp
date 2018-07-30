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
  firstPacketContents = nullptr;
  packetContents = nullptr;
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
  firstPacketContents = nullptr;
  packetContents = nullptr;
}

void ESAT_CCSDSPacketBuilder::addPacketContents(ESAT_CCSDSPacketContents& newPacketContents)
{
  if (firstPacketContents == nullptr)
  {
    packetContents = &newPacketContents;
    firstPacketContents = &newPacketContents;
  }
  else
  {
    for (ESAT_CCSDSPacketContents* contents = firstPacketContents;
         contents != nullptr;
         contents = contents->nextPacketContents)
    {
      if (contents->nextPacketContents == nullptr)
      {
        contents->nextPacketContents = &newPacketContents;
        return;
      }
    }
  }
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
  if (!contents.enabled)
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
  if (!contents.enabled)
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

boolean ESAT_CCSDSPacketBuilder::buildNextTelecommandPacket(ESAT_CCSDSPacket& packet)
{
  if (packetContents == nullptr)
  {
    return false;
  }
  const boolean gotPacket = buildTelecommandPacket(packet,
                                                   *packetContents);
  packetContents = packetContents->nextPacketContents;
  return gotPacket;
}

boolean ESAT_CCSDSPacketBuilder::buildNamedTelemetryPacket(ESAT_CCSDSPacket& packet,
                                                           const byte identifier)
{
  for (ESAT_CCSDSPacketContents* contents = firstPacketContents;
       contents != nullptr;
       contents = contents->nextPacketContents)
  {
    if (contents->packetIdentifier() == identifier)
    {
      return buildTelemetryPacket(packet, *contents);
    }
  }
  return false;
}

boolean ESAT_CCSDSPacketBuilder::buildNextTelemetryPacket(ESAT_CCSDSPacket& packet)
{
  if (packetContents == nullptr)
  {
    return false;
  }
  const boolean gotPacket = buildTelemetryPacket(packet,
                                                 *packetContents);
  packetContents = packetContents->nextPacketContents;
  return gotPacket;
}

void ESAT_CCSDSPacketBuilder::disablePacket(const byte identifier)
{
  for (ESAT_CCSDSPacketContents* contents = packetContents;
       contents != nullptr;
       contents = contents->nextPacketContents)
  {
    if (contents->packetIdentifier() == identifier)
    {
      contents->enabled = false;
      return;
    }
  }
}

void ESAT_CCSDSPacketBuilder::enablePacket(const byte identifier)
{
  for (ESAT_CCSDSPacketContents* contents = packetContents;
       contents != nullptr;
       contents = contents->nextPacketContents)
  {
    if (contents->packetIdentifier() == identifier)
    {
      contents->enabled = true;
      return;
    }
  }
}

void ESAT_CCSDSPacketBuilder::rewindPacketContentsQueue()
{
  packetContents = firstPacketContents;
}

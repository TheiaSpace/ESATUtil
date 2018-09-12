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

#include "ESAT_CCSDSTelemetryPacketBuilder.h"

ESAT_CCSDSTelemetryPacketBuilder::ESAT_CCSDSTelemetryPacketBuilder()
{
  clock = nullptr;
  packetContents = nullptr;
}

ESAT_CCSDSTelemetryPacketBuilder::ESAT_CCSDSTelemetryPacketBuilder(const word theApplicationProcessIdentifier,
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
  packetContents = nullptr;
}

void ESAT_CCSDSTelemetryPacketBuilder::add(ESAT_CCSDSPacketContents& newPacketContents)
{
  newPacketContents.nextPacketContents = packetContents;
  packetContents = &newPacketContents;
}

ESAT_FlagContainer ESAT_CCSDSTelemetryPacketBuilder::available()
{
  ESAT_FlagContainer result;
  result.clearAll();
  for (ESAT_CCSDSPacketContents* contents = packetContents;
       contents != nullptr;
       contents = contents->nextPacketContents)
  {
    if (contents->available())
    {
      result.set(contents->packetIdentifier());
    }
  }
  return result;
}

boolean ESAT_CCSDSTelemetryPacketBuilder::build(ESAT_CCSDSPacket& packet,
                                                byte identifier)
{
  if (!clock)
  {
    return false;
  }
  if (packet.capacity() < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return false;
  }
  ESAT_CCSDSPacketContents* const contents = find(identifier);
  if (contents == nullptr)
  {
    return false;
  }
  packet.writeTelemetryHeaders(applicationProcessIdentifier,
                               packetSequenceCount,
                               clock->read(),
                               majorVersionNumber,
                               minorVersionNumber,
                               patchVersionNumber,
                               identifier);
  const boolean userDataCorrect = contents->fillUserData(packet);
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

boolean ESAT_CCSDSTelemetryPacketBuilder::buildNext(ESAT_CCSDSPacket& packet,
                                                    ESAT_FlagContainer& pendingPackets)
{
  const int next = pendingPackets.readNext();
  if (next == pendingPackets.NO_ACTIVE_FLAGS)
  {
    return false;
  }
  pendingPackets.clear(byte(next));
  return build(packet, byte(next));
}

ESAT_CCSDSPacketContents* ESAT_CCSDSTelemetryPacketBuilder::find(const byte identifier)
{
  for (ESAT_CCSDSPacketContents* contents = packetContents;
       contents != nullptr;
       contents = contents->nextPacketContents)
  {
    if (contents->packetIdentifier() == identifier)
    {
      return contents;
    }
  }
  // If we didn't find anything, just return nullptr.
  return nullptr;
}

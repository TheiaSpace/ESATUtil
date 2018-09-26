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

#include "ESAT_CCSDSTelecommandPacketHandler.h"

ESAT_CCSDSTelecommandPacketHandler::ESAT_CCSDSTelecommandPacketHandler()
{
}

ESAT_CCSDSTelecommandPacketHandler::ESAT_CCSDSTelecommandPacketHandler(const word theApplicationProcessIdentifier,
                                                                       const byte majorVersionNumber,
                                                                       const byte minorVersionNumber,
                                                                       const byte patchVersionNumber)
{
  applicationProcessIdentifier = theApplicationProcessIdentifier;
  versionNumber = ESAT_SemanticVersionNumber(majorVersionNumber,
                                             minorVersionNumber,
                                             patchVersionNumber);
}

void ESAT_CCSDSTelecommandPacketHandler::add(ESAT_CCSDSPacketConsumer& consumer)
{
  consumer.nextPacketConsumer = packetConsumer;
  packetConsumer = &consumer;
}

boolean ESAT_CCSDSTelecommandPacketHandler::compatiblePacket(ESAT_CCSDSPacket packet) const
{
  const ESAT_CCSDSPrimaryHeader primaryHeader
    = packet.readPrimaryHeader();
  if (primaryHeader.packetType
      != primaryHeader.TELECOMMAND)
  {
    return false;
  }
  if (primaryHeader.secondaryHeaderFlag
      != primaryHeader.SECONDARY_HEADER_IS_PRESENT)
  {
    return false;
  }
  if (primaryHeader.applicationProcessIdentifier
      != applicationProcessIdentifier)
  {
    return false;
  }
  if (packet.length() < ESAT_CCSDSSecondaryHeader::LENGTH)
  {
    return false;
  }
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  const ESAT_SemanticVersionNumber packetVersionNumber =
    ESAT_SemanticVersionNumber(secondaryHeader.majorVersionNumber,
                               secondaryHeader.minorVersionNumber,
                               secondaryHeader.patchVersionNumber);
  return packetVersionNumber.isBackwardsCompatibleWith(versionNumber);
}

boolean ESAT_CCSDSTelecommandPacketHandler::handle(ESAT_CCSDSPacket packet)
{
  packet.rewind();
  if (!compatiblePacket(packet))
  {
    return false;
  }
  for (ESAT_CCSDSPacketConsumer* consumer = packetConsumer;
       consumer != nullptr;
       consumer = consumer->nextPacketConsumer)
  {
    if (consumer->write(packet))
    {
      return true;
    }
  }
  return false;
}

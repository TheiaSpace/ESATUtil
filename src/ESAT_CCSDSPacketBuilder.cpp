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

ESAT_CCSDSPacketBuilder::ESAT_CCSDSPacketBuilder(const word applicationProcessIdentifier,
                                                 const byte majorVersionNumber,
                                                 const byte minorVersionNumber,
                                                 const byte patchVersionNumber,
                                                 const ESAT_CCSDSPrimaryHeader::PacketType packetType,
                                                 ESAT_Clock& theClock)
{
  clock = &theClock;
  primaryHeader.packetVersionNumber = 0;
  primaryHeader.packetType = packetType;
  primaryHeader.secondaryHeaderFlag = primaryHeader.SECONDARY_HEADER_IS_PRESENT;
  primaryHeader.applicationProcessIdentifier = applicationProcessIdentifier;
  primaryHeader.sequenceFlags = primaryHeader.UNSEGMENTED_USER_DATA;
  primaryHeader.packetSequenceCount = 0;
  secondaryHeader.preamble =
    secondaryHeader.CALENDAR_SEGMENTED_TIME_CODE_MONTH_DAY_VARIANT_1_SECOND_RESOLUTION;
  secondaryHeader.majorVersionNumber = majorVersionNumber;
  secondaryHeader.minorVersionNumber = minorVersionNumber;
  secondaryHeader.patchVersionNumber = patchVersionNumber;
}

boolean ESAT_CCSDSPacketBuilder::buildPacket(ESAT_CCSDSPacket& packet,
                                             ESAT_CCSDSPacketContents& contents)
{
  const byte packetIdentifier = contents.packetIdentifier();
  const boolean headersCorrect = fillHeaders(packet, packetIdentifier);
  if (!headersCorrect)
  {
    return false;
  }
  const boolean userDataCorrect = contents.fillUserData(packet);
  if (userDataCorrect)
  {
    incrementPacketSequenceCount();
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_CCSDSPacketBuilder::fillHeaders(ESAT_CCSDSPacket& packet,
                                             const byte packetIdentifier)
{
  if (!clock)
  {
    return false;
  }
  if (packet.capacity() < secondaryHeader.LENGTH)
  {
    return false;
  }
  packet.flush();
  packet.writePrimaryHeader(primaryHeader);
  secondaryHeader.timestamp = clock->read();
  secondaryHeader.packetIdentifier = packetIdentifier;
  packet.writeSecondaryHeader(secondaryHeader);
  return true;
}

void ESAT_CCSDSPacketBuilder::incrementPacketSequenceCount()
{
  primaryHeader.packetSequenceCount = primaryHeader.packetSequenceCount + 1;
}

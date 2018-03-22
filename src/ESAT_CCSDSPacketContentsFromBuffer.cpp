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

#include "ESAT_CCSDSPacketContentsFromBuffer.h"

ESAT_CCSDSPacketContentsFromBuffer::ESAT_CCSDSPacketContentsFromBuffer()
{
}

ESAT_CCSDSPacketContentsFromBuffer::ESAT_CCSDSPacketContentsFromBuffer(const byte theIdentifier,
                                                                       byte array[],
                                                                       const unsigned long length):
  identifier(theIdentifier),
  buffer(array, length)
{
}

ESAT_CCSDSPacketContentsFromBuffer::ESAT_CCSDSPacketContentsFromBuffer(const byte theIdentifier,
                                                                       ESAT_Buffer& theBuffer):
  identifier(theIdentifier),
  buffer(theBuffer)
{
}

boolean ESAT_CCSDSPacketContentsFromBuffer::available()
{
  return true;
}

byte ESAT_CCSDSPacketContentsFromBuffer::packetIdentifier()
{
  return identifier;
}

boolean ESAT_CCSDSPacketContentsFromBuffer::fillUserData(ESAT_CCSDSPacket& packet)
{
  const unsigned long neededCapacity =
    ESAT_CCSDSSecondaryHeader::LENGTH + buffer.length();
  if (packet.capacity() < neededCapacity)
  {
    return false;
  }
  return buffer.writeTo(packet);
}

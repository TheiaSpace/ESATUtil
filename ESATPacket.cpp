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

#include "ESATPacket.h"

ESATPacket::ESATPacket(byte* const byteBuffer):
  buffer(byteBuffer),
  readPosition(headerLength)
{
}

void ESATPacket::clear()
{
  buffer[payloadLengthOffset] = 0;
}

word ESATPacket::readBigEndianWord()
{
  const byte highByte = readByte();
  const byte lowByte = readByte();
  return word(highByte, lowByte);
}

byte ESATPacket::readByte()
{
  const byte datum = buffer[readPosition];
  readPosition = readPosition + 1;
  return datum;
}

word ESATPacket::readLittleEndianWord()
{
  const word lowByte = readByte();
  const word highByte = readByte();
  return word(highByte, lowByte);
}

byte ESATPacket::readMajorVersionNumber()
{
  return buffer[majorVersionNumberOffset];
}

byte ESATPacket::readMinorVersionNumber()
{
  return buffer[minorVersionNumberOffset];
}

byte ESATPacket::readPatchVersionNumber()
{
  return buffer[patchVersionNumberOffset];
}

byte ESATPacket::readPacketLength()
{
  return payloadOffset + readPayloadLength();
}

byte ESATPacket::readPacketIdentifier()
{
  return buffer[packetIdentifierOffset];
}

byte ESATPacket::readPayloadLength()
{
  return buffer[payloadLengthOffset];
}

byte ESATPacket::readSubsystemIdentifier()
{
  return buffer[subsystemIdentifierOffset];
}

void ESATPacket::writeByte(const byte datum)
{
  const byte payloadLength = readPayloadLength();
  buffer[payloadOffset + payloadLength] = datum;
  buffer[payloadLengthOffset] = payloadLength + 1;
}

void ESATPacket::writeBigEndianWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}

void ESATPacket::writeLittleEndianWord(const word datum)
{
  writeByte(lowByte(datum));
  writeByte(highByte(datum));
}

void ESATPacket::writeMajorVersionNumber(const byte majorVersionNumber)
{
  buffer[majorVersionNumberOffset] = majorVersionNumber;
}

void ESATPacket::writeMinorVersionNumber(const byte minorVersionNumber)
{
  buffer[minorVersionNumberOffset] = minorVersionNumber;
}

void ESATPacket::writePacketIdentifier(const byte packetIdentifier)
{
  buffer[packetIdentifierOffset] = packetIdentifier;
}

void ESATPacket::writePatchVersionNumber(const byte patchVersionNumber)
{
  buffer[patchVersionNumberOffset] = patchVersionNumber;
}

void ESATPacket::writeSubsystemIdentifier(const byte subsystemIdentifier)
{
  buffer[subsystemIdentifierOffset] = subsystemIdentifier;
}

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

#include "ESATCCSDSPacket.h"

ESATCCSDSPacket::ESATCCSDSPacket(byte* const byteBuffer,
                                 const word byteBufferLength):
  buffer(byteBuffer),
  bufferLength(byteBufferLength),
  readPosition(PRIMARY_HEADER_LENGTH)
{
}

void ESATCCSDSPacket::clear()
{
  for (int position = 0; position < PRIMARY_HEADER_LENGTH; position++)
  {
    buffer[position] = 0;
  }
}

word ESATCCSDSPacket::readApplicationProcessIdentifier()
{
  return readBits(applicationProcessIdentifierOffset,
                  applicationProcessIdentifierLength);
}

word ESATCCSDSPacket::readBits(const byte offset, const byte length)
{
  word bits = 0;
  for (int position = 0; position < length; position++)
  {
    const byte sourceByte = (offset + position) / 8;
    const byte sourceBit = 8 - ((offset + position) % 8) - 1;
    const byte targetBit = length - position - 1;
    const boolean bitValue = bitRead(buffer[sourceByte], sourceBit);
    bitWrite(bits, targetBit, bitValue);
  }
  return bits;
}

byte ESATCCSDSPacket::readByte()
{
  const byte datum = buffer[readPosition];
  readPosition = readPosition + 1;
  return datum;
}

word ESATCCSDSPacket::readPacketDataLength()
{
  return word(buffer[packetDataLengthOffset],
              buffer[packetDataLengthOffset+1]);
}

word ESATCCSDSPacket::readPacketSequenceCount()
{
  return readBits(packetSequenceCountOffset,
                  packetSequenceCountLength);
}

ESATCCSDSPacket::PacketType ESATCCSDSPacket::readPacketType()
{
  const word bits = readBits(packetTypeOffset,
                             packetTypeLength);
  return ESATCCSDSPacket::PacketType(bits);
}

byte ESATCCSDSPacket::readPacketVersionNumber()
{
  return readBits(packetVersionNumberOffset,
                  packetVersionNumberLength);
}

ESATCCSDSPacket::SecondaryHeaderFlag ESATCCSDSPacket::readSecondaryHeaderFlag()
{
  const word bits = readBits(secondaryHeaderFlagOffset,
                             secondaryHeaderFlagLength);
  return ESATCCSDSPacket::SecondaryHeaderFlag(bits);
}

ESATCCSDSPacket::SequenceFlags ESATCCSDSPacket::readSequenceFlags()
{
  const word bits = readBits(sequenceFlagsOffset,
                             sequenceFlagsLength);
  return ESATCCSDSPacket::SequenceFlags(bits);
}

word ESATCCSDSPacket::readWord()
{
  const byte highByte = readByte();
  const byte lowByte = readByte();
  return word(highByte, lowByte);
}

void ESATCCSDSPacket::rewind()
{
  readPosition = 0;
}

void ESATCCSDSPacket::writeApplicationProcessIdentifier(const word applicationProcessIdentifier)
{
  writeBits(applicationProcessIdentifierOffset,
            applicationProcessIdentifierLength,
            applicationProcessIdentifier);
}

void ESATCCSDSPacket::writeBits(const byte offset,
                                const byte length,
                                const word bits)
{
  for (int position = 0; position < length; position++)
  {
    const byte targetByte = (offset + position) / 8;
    const byte targetBit = 8 - ((offset + position) % 8) - 1;
    const byte sourceBit = length - position - 1;
    const boolean bitValue = bitRead(bits, sourceBit);
    bitWrite(buffer[targetByte], targetBit, bitValue);
  }
}

void ESATCCSDSPacket::writeByte(const byte datum)
{
  const byte packetDataLength = readPacketDataLength();
  buffer[PRIMARY_HEADER_LENGTH + packetDataLength] = datum;
  writePacketDataLength(packetDataLength + 1);
}

void ESATCCSDSPacket::writePacketDataLength(const word packetDataLength)
{
  buffer[packetDataLengthOffset] = highByte(packetDataLength);
  buffer[packetDataLengthOffset + 1] = lowByte(packetDataLength);
}

void ESATCCSDSPacket::writePacketSequenceCount(const word packetSequenceCount)
{
  writeBits(packetSequenceCountOffset,
            packetSequenceCountLength,
            packetSequenceCount);
}

void ESATCCSDSPacket::writePacketType(const ESATCCSDSPacket::PacketType packetType)
{
  writeBits(packetTypeOffset,
            packetTypeLength,
            packetType);
}

void ESATCCSDSPacket::writePacketVersionNumber(const byte packetVersionNumber)
{
  writeBits(packetVersionNumberOffset,
            packetVersionNumberLength,
            packetVersionNumber);
}

void ESATCCSDSPacket::writeSecondaryHeaderFlag(const ESATCCSDSPacket::SecondaryHeaderFlag secondaryHeaderFlag)
{
  writeBits(secondaryHeaderFlagOffset,
            secondaryHeaderFlagLength,
            secondaryHeaderFlag);
}

void ESATCCSDSPacket::writeSequenceFlags(const SequenceFlags sequenceFlags)
{
  writeBits(sequenceFlagsOffset,
            sequenceFlagsLength,
            sequenceFlags);
}

void ESATCCSDSPacket::writeWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}

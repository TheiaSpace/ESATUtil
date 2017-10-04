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
  for (int position = 0; position < bufferLength; position++)
  {
    buffer[position] = 0;
  }
}

word ESATCCSDSPacket::readApplicationProcessIdentifier()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return 0;
  }
  return readBits(applicationProcessIdentifierOffset,
                  applicationProcessIdentifierLength);
}

word ESATCCSDSPacket::readBits(const byte offset, const byte length)
{
  word bits = 0;
  for (int position = 0; position < length; position++)
  {
    const byte sourceByte = (offset + position) / 8;
    if (sourceByte >= bufferLength)
    {
      return 0;
    }
    const byte sourceBit = 8 - ((offset + position) % 8) - 1;
    const byte targetBit = length - position - 1;
    const boolean bitValue = bitRead(buffer[sourceByte], sourceBit);
    bitWrite(bits, targetBit, bitValue);
  }
  return bits;
}

byte ESATCCSDSPacket::readByte()
{
  const unsigned long nextReadPosition = readPosition + 1;
  if (nextReadPosition >= bufferLength)
  {
    return 0;
  }
  const byte datum = buffer[readPosition];
  readPosition = nextReadPosition;
  return datum;
}

unsigned long ESATCCSDSPacket::readLong()
{
  const unsigned long firstByte = readByte();
  const unsigned long secondByte = readByte();
  const unsigned long thirdByte = readByte();
  const unsigned long fourthByte = readByte();
  return (firstByte << 24) | (secondByte << 16) | (thirdByte << 8) | fourthByte;
}

word ESATCCSDSPacket::readPacketDataLength()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return 0;
  }
  return word(buffer[packetDataLengthOffset],
              buffer[packetDataLengthOffset+1]);
}

word ESATCCSDSPacket::readPacketSequenceCount()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return 0;
  }
  return readBits(packetSequenceCountOffset,
                  packetSequenceCountLength);
}

ESATCCSDSPacket::PacketType ESATCCSDSPacket::readPacketType()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return ESATCCSDSPacket::PacketType(0);
  }
  const word bits = readBits(packetTypeOffset,
                             packetTypeLength);
  return ESATCCSDSPacket::PacketType(bits);
}

byte ESATCCSDSPacket::readPacketVersionNumber()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return 0;
  }
  return readBits(packetVersionNumberOffset,
                  packetVersionNumberLength);
}

ESATCCSDSPacket::SecondaryHeaderFlag ESATCCSDSPacket::readSecondaryHeaderFlag()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return ESATCCSDSPacket::SecondaryHeaderFlag(0);
  }
  const word bits = readBits(secondaryHeaderFlagOffset,
                             secondaryHeaderFlagLength);
  return ESATCCSDSPacket::SecondaryHeaderFlag(bits);
}

ESATCCSDSPacket::SequenceFlags ESATCCSDSPacket::readSequenceFlags()
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return ESATCCSDSPacket::SequenceFlags(0);
  }
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
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
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
    if (targetByte >= bufferLength)
    {
      return;
    }
    const byte targetBit = 8 - ((offset + position) % 8) - 1;
    const byte sourceBit = length - position - 1;
    const boolean bitValue = bitRead(bits, sourceBit);
    bitWrite(buffer[targetByte], targetBit, bitValue);
  }
}

void ESATCCSDSPacket::writeByte(const byte datum)
{
  const word packetDataLength = readPacketDataLength();
  const long packetLength = packetDataLength + PRIMARY_HEADER_LENGTH;
  if (packetLength < bufferLength)
  {
    buffer[PRIMARY_HEADER_LENGTH + packetDataLength] = datum;
    writePacketDataLength(packetDataLength + 1);
  }
}

void ESATCCSDSPacket::writeLong(const unsigned long datum)
{
  writeByte((datum >> 24) & B11111111);
  writeByte((datum >> 16) & B11111111);
  writeByte((datum >> 8) & B11111111);
  writeByte(datum & B11111111);
}

void ESATCCSDSPacket::writePacketDataLength(const word packetDataLength)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  buffer[packetDataLengthOffset] = highByte(packetDataLength);
  buffer[packetDataLengthOffset + 1] = lowByte(packetDataLength);
}

void ESATCCSDSPacket::writePacketSequenceCount(const word packetSequenceCount)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  writeBits(packetSequenceCountOffset,
            packetSequenceCountLength,
            packetSequenceCount);
}

void ESATCCSDSPacket::writePacketType(const ESATCCSDSPacket::PacketType packetType)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  writeBits(packetTypeOffset,
            packetTypeLength,
            packetType);
}

void ESATCCSDSPacket::writePacketVersionNumber(const byte packetVersionNumber)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  writeBits(packetVersionNumberOffset,
            packetVersionNumberLength,
            packetVersionNumber);
}

void ESATCCSDSPacket::writeSecondaryHeaderFlag(const ESATCCSDSPacket::SecondaryHeaderFlag secondaryHeaderFlag)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  writeBits(secondaryHeaderFlagOffset,
            secondaryHeaderFlagLength,
            secondaryHeaderFlag);
}

void ESATCCSDSPacket::writeSequenceFlags(const SequenceFlags sequenceFlags)
{
  if (bufferLength < PRIMARY_HEADER_LENGTH)
  {
    return;
  }
  writeBits(sequenceFlagsOffset,
            sequenceFlagsLength,
            sequenceFlags);
}

void ESATCCSDSPacket::writeWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}

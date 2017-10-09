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

ESATCCSDSPacket::ESATCCSDSPacket():
  primaryHeader({0, 0, 0, 0, 0, 0}),
  packetData(nullptr),
  packetDataBufferLength(0),
  position(0)
{
}

ESATCCSDSPacket::ESATCCSDSPacket(byte* const buffer,
                                 const unsigned long bufferLength):
  primaryHeader({0, 0, 0, 0, 0, 0}),
  packetData(buffer),
  packetDataBufferLength(bufferLength),
  position(0)
{
}

void ESATCCSDSPacket::clear()
{
  for (byte i = 0; i < PRIMARY_HEADER_LENGTH; i++)
  {
    primaryHeader[i] = 0;
  }
  for (long i = 0; i < packetDataBufferLength; i++)
  {
    packetData[i] = 0;
  }
  position = 0;
}

boolean ESATCCSDSPacket::copyTo(ESATCCSDSPacket& target)
{
  if (target.packetDataBufferLength < readPacketDataLength())
  {
    return false;
  }
  for (byte i = 0; i < PRIMARY_HEADER_LENGTH; i++)
  {
    target.primaryHeader[i] = primaryHeader[i];
  }
  rewind();
  while (!endOfPacketDataReached())
  {
    target.writeByte(readByte());
  }
  return true;
}

boolean ESATCCSDSPacket::endOfPacketDataReached()
{
  if (position >= readPacketDataLength())
  {
    return true;
  }
  else
  {
    return false;
  }
}

unsigned long ESATCCSDSPacket::floatToLong(const float number)
{
  if (number == 0)
  {
    return 0x00000000ul;
  }
  if (number == NAN)
  {
    return 0xFFFFFFFFul;
  }
  if (number == INFINITY)
  {
    return 0x7F800000ul;
  }
  if (number == -INFINITY)
  {
    return 0xFF800000ul;
  }
  const unsigned long signBit = ((number >= 0) ? 0 : 1);
  float mantissa = number;
  if (mantissa < 0)
  {
    mantissa = -mantissa;
  }
  int exponent = 0;
  while (mantissa >= 2)
  {
    mantissa = mantissa / 2;
    exponent = exponent + 1;
  }
  while (mantissa < 1)
  {
    mantissa = 2 * mantissa;
    exponent = exponent - 1;
  }
  mantissa = (mantissa - 1) * 8388608.5;
  const unsigned long mantissaBits = ((unsigned long) mantissa) & 0x007FFFFF;
  const unsigned long exponentBits = exponent + 127;
  return (signBit << 31) | (exponentBits << 23) | mantissaBits;
}

float ESATCCSDSPacket::longToFloat(const unsigned long bits)
{
  if (bits == 0x00000000ul)
  {
    return 0;
  }
  if (bits >= 0xFF000001ul)
  {
    return NAN;
  }
  if (bits == 0x7F800000ul)
  {
    return INFINITY;
  }
  if (bits == 0xFF800000ul)
  {
    return -INFINITY;
  }
  const unsigned long signBit = (bits >> 31) & 0x00000001;
  const unsigned long exponentBits = (bits >> 23) & 0x000000FF;
  const unsigned long mantissaBits = bits & 0x007FFFFF;
  float number = 1 + mantissaBits / 8388608.5;
  if (signBit == 1)
  {
    number = -number;
  }
  int exponent = exponentBits - 127;
  if (exponent > 0)
  {
    for (int i = 0; i < exponent; i++)
    {
      number = 2 * number;
    }
  }
  if (exponent < 0)
  {
    for (int i = 0; i > exponent; i--)
    {
      number = number / 2;
    }
  }
  return number;
}

word ESATCCSDSPacket::readApplicationProcessIdentifier()
{
  return readPrimaryHeaderBits(APPLICATION_PROCESS_IDENTIFIER_OFFSET,
                               APPLICATION_PROCESS_IDENTIFIER_LENGTH);
}

boolean ESATCCSDSPacket::readBoolean()
{
  const byte datum = readByte();
  if (datum > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

byte ESATCCSDSPacket::readByte()
{
  if (position >= packetDataBufferLength)
  {
    return 0;
  }
  const byte datum = packetData[position];
  position = position + 1;
  return datum;
}

float ESATCCSDSPacket::readFloat()
{
  const unsigned long bits = readLong();
  return longToFloat(bits);
}

boolean ESATCCSDSPacket::readFrom(Stream& input)
{
  const size_t primaryHeaderBytesRead =
    input.readBytes((char*) primaryHeader,
                    PRIMARY_HEADER_LENGTH);
  if (primaryHeaderBytesRead != PRIMARY_HEADER_LENGTH)
  {
    return false;
  }
  const long packetDataLength = readPacketDataLength();
  if (packetDataBufferLength < packetDataLength)
  {
    return false;
  }
  const size_t packetDataBytesRead =
    input.readBytes((char*) packetData, packetDataLength);
  if (packetDataBytesRead != packetDataLength)
  {
    return false;
  }
}

unsigned long ESATCCSDSPacket::readLong()
{
  const unsigned long firstByte = readByte();
  const unsigned long secondByte = readByte();
  const unsigned long thirdByte = readByte();
  const unsigned long fourthByte = readByte();
  return (firstByte << 24) | (secondByte << 16) | (thirdByte << 8) | fourthByte;
}

long ESATCCSDSPacket::readPacketDataLength()
{
  return 1 + long(word(primaryHeader[PACKET_DATA_LENGTH_OFFSET],
                       primaryHeader[PACKET_DATA_LENGTH_OFFSET + 1]));
}

word ESATCCSDSPacket::readPacketSequenceCount()
{
  return readPrimaryHeaderBits(PACKET_SEQUENCE_COUNT_OFFSET,
                               PACKET_SEQUENCE_COUNT_LENGTH);
}

ESATCCSDSPacket::PacketType ESATCCSDSPacket::readPacketType()
{
  const word bits = readPrimaryHeaderBits(PACKET_TYPE_OFFSET,
                                          PACKET_TYPE_LENGTH);
  return ESATCCSDSPacket::PacketType(bits);
}

byte ESATCCSDSPacket::readPacketVersionNumber()
{
  return readPrimaryHeaderBits(PACKET_VERSION_NUMBER_OFFSET,
                               PACKET_VERSION_NUMBER_LENGTH);
}

word ESATCCSDSPacket::readPrimaryHeaderBits(const byte offset,
                                            const byte length)
{
  word bits = 0;
  for (int position = 0; position < length; position++)
  {
    const byte sourceByte = (offset + position) / 8;
    if (sourceByte >= PRIMARY_HEADER_LENGTH)
    {
      return 0;
    }
    const byte sourceBit = 8 - ((offset + position) % 8) - 1;
    const byte targetBit = length - position - 1;
    const boolean bitValue = bitRead(primaryHeader[sourceByte], sourceBit);
    bitWrite(bits, targetBit, bitValue);
  }
  return bits;
}

ESATCCSDSPacket::SecondaryHeaderFlag ESATCCSDSPacket::readSecondaryHeaderFlag()
{
  const word bits = readPrimaryHeaderBits(SECONDARY_HEADER_FLAG_OFFSET,
                                          SECONDARY_HEADER_FLAG_LENGTH);
  return ESATCCSDSPacket::SecondaryHeaderFlag(bits);
}

ESATCCSDSPacket::SequenceFlags ESATCCSDSPacket::readSequenceFlags()
{
  const word bits = readPrimaryHeaderBits(SEQUENCE_FLAGS_OFFSET,
                                          SEQUENCE_FLAGS_LENGTH);
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
  position = 0;
}

void ESATCCSDSPacket::updatePacketDataLength()
{
  writePacketDataLength(position);
}

void ESATCCSDSPacket::writeApplicationProcessIdentifier(const word applicationProcessIdentifier)
{
  writePrimaryHeaderBits(APPLICATION_PROCESS_IDENTIFIER_OFFSET,
                         APPLICATION_PROCESS_IDENTIFIER_LENGTH,
                         applicationProcessIdentifier);
}

void ESATCCSDSPacket::writeBoolean(const boolean datum)
{
  if (datum)
  {
    writeByte(1);
  }
  else
  {
    writeByte(0);
  }
}

void ESATCCSDSPacket::writeByte(const byte datum)
{
  if (position < packetDataBufferLength)
  {
    packetData[position] = datum;
    position = position + 1;
  }
}

void ESATCCSDSPacket::writeFloat(const float datum)
{
  const unsigned long bits = floatToLong(datum);
  writeLong(bits);
}

void ESATCCSDSPacket::writeLong(const unsigned long datum)
{
  writeByte((datum >> 24) & B11111111);
  writeByte((datum >> 16) & B11111111);
  writeByte((datum >> 8) & B11111111);
  writeByte(datum & B11111111);
}

void ESATCCSDSPacket::writePacketDataLength(const long packetDataLength)
{
  primaryHeader[PACKET_DATA_LENGTH_OFFSET] =
    highByte(packetDataLength - 1);
  primaryHeader[PACKET_DATA_LENGTH_OFFSET + 1] =
    lowByte(packetDataLength - 1);
}

void ESATCCSDSPacket::writePacketSequenceCount(const word packetSequenceCount)
{
  writePrimaryHeaderBits(PACKET_SEQUENCE_COUNT_OFFSET,
                         PACKET_SEQUENCE_COUNT_LENGTH,
                         packetSequenceCount);
}

void ESATCCSDSPacket::writePacketType(const ESATCCSDSPacket::PacketType packetType)
{
  writePrimaryHeaderBits(PACKET_TYPE_OFFSET,
                         PACKET_TYPE_LENGTH,
                         packetType);
}

void ESATCCSDSPacket::writePacketVersionNumber(const byte packetVersionNumber)
{
  writePrimaryHeaderBits(PACKET_VERSION_NUMBER_OFFSET,
                         PACKET_VERSION_NUMBER_LENGTH,
                         packetVersionNumber);
}

void ESATCCSDSPacket::writePrimaryHeaderBits(const byte offset,
                                             const byte length,
                                             const word bits)
{
  for (int position = 0; position < length; position++)
  {
    const byte targetByte = (offset + position) / 8;
    if (targetByte >= PRIMARY_HEADER_LENGTH)
    {
      return;
    }
    const byte targetBit = 8 - ((offset + position) % 8) - 1;
    const byte sourceBit = length - position - 1;
    const boolean bitValue = bitRead(bits, sourceBit);
    bitWrite(primaryHeader[targetByte], targetBit, bitValue);
  }
}

void ESATCCSDSPacket::writeSecondaryHeaderFlag(const ESATCCSDSPacket::SecondaryHeaderFlag secondaryHeaderFlag)
{
  writePrimaryHeaderBits(SECONDARY_HEADER_FLAG_OFFSET,
                         SECONDARY_HEADER_FLAG_LENGTH,
                         secondaryHeaderFlag);
}

void ESATCCSDSPacket::writeSequenceFlags(const SequenceFlags sequenceFlags)
{
  writePrimaryHeaderBits(SEQUENCE_FLAGS_OFFSET,
                         SEQUENCE_FLAGS_LENGTH,
                         sequenceFlags);
}

boolean ESATCCSDSPacket::writeTo(Stream& output)
{
  for (byte i = 0; i < PRIMARY_HEADER_LENGTH; i++)
  {
    const size_t bytesWritten = output.write(primaryHeader[i]);
    if (bytesWritten != 1)
    {
      return false;
    }
  }
  rewind();
  while (!endOfPacketDataReached())
  {
    const size_t bytesWritten = output.write(readByte());
    if (bytesWritten != 1)
    {
      return false;
    }
  }
}

void ESATCCSDSPacket::writeWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}

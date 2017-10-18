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
#include "ESATUtil.h"

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

int ESATCCSDSPacket::available()
{
  return constrain(availableBytesToRead(), 0, 0x7FFF);
}

unsigned long ESATCCSDSPacket::availableBytesToRead() const
{
  unsigned long packetDataLength = readPacketDataLength();
  return packetDataLength - position;
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

boolean ESATCCSDSPacket::endOfPacketDataReached() const
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

void ESATCCSDSPacket::flush()
{
  updatePacketDataLength();
  rewind();
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

int ESATCCSDSPacket::peek()
{
  if (available() > 0)
  {
    return packetData[position];
  }
  else
  {
    return -1;
  }
}

size_t ESATCCSDSPacket::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten =
    bytesWritten
    + output.println(String("{"));
  bytesWritten =
    bytesWritten
    + output.println(String("  \"primaryHeader\": {"));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"packetVersionNumber\": "));
  bytesWritten =
    bytesWritten
    + output.print(String(readPacketVersionNumber(), DEC));
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"packetType\": "));
  if (readPacketType() == TELEMETRY)
  {
    bytesWritten =
      bytesWritten
      + output.print(String("\"TELEMETRY\""));
  }
  else
  {
    bytesWritten =
      bytesWritten
      + output.print(String("\"TELECOMMAND\""));
  }
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"secondaryHeaderFlag\": "));
  if (readSecondaryHeaderFlag() == SECONDARY_HEADER_IS_NOT_PRESENT)
  {
    bytesWritten =
      bytesWritten
      + output.print(String("\"SECONDARY_HEADER_IS_NOT_PRESENT\""));
  }
  else
  {
    bytesWritten =
      bytesWritten
      + output.print(String("\"SECONDARY_HEADER_IS_PRESENT\""));
  }
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"applicationProcessIdentifier\": "));
  bytesWritten =
    bytesWritten
    + output.print(String(readApplicationProcessIdentifier(), DEC));
  bytesWritten =
    bytesWritten + output.println(String(","));
  bytesWritten =
    bytesWritten + output.print(String("    \"sequenceFlags\": "));
  switch (readSequenceFlags())
  {
    case CONTINUATION_SEGMENT_OF_USER_DATA:
      bytesWritten =
        bytesWritten
        + output.print(String("\"CONTINUATION_SEGMENT_OF_USER_DATA\""));
      break;
    case FIRST_SEGMENT_OF_USER_DATA:
      bytesWritten =
        bytesWritten
        + output.print(String("\"FIRST_SEGMENT_OF_USER_DATA\""));
      break;
    case LAST_SEGMENT_OF_USER_DATA:
      bytesWritten =
        bytesWritten
        + output.print(String("\"LAST_SEGMENT_OF_USER_DATA\""));
      break;
    default:
      bytesWritten =
        bytesWritten
        + output.print(String("\"UNSEGMENTED_USER_DATA\""));
      break;
  }
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"packetSequenceCount\": "));
  bytesWritten =
    bytesWritten
    + output.print(String(readPacketSequenceCount(), DEC));
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.print(String("    \"packetDataLength\": "));
  const long packetDataLength = readPacketDataLength();
  bytesWritten =
    bytesWritten
    + output.print(String(packetDataLength, DEC));
  bytesWritten =
    bytesWritten
    + output.println(String(""));
  bytesWritten =
    bytesWritten
    + output.println(String("  },"));
  bytesWritten =
    bytesWritten
    + output.println(String("  \"packetData\": ["));
  for (long i = 0; i < packetDataLength; i++)
  {
    bytesWritten =
      bytesWritten
      + output.print(String("    0x"));
    bytesWritten =
      bytesWritten
      + output.print(String(packetData[i], HEX));
    if (i == (packetDataLength - 1))
    {
      bytesWritten =
        bytesWritten
        + output.println(String(""));
    }
    else
    {
      bytesWritten =
        bytesWritten
        + output.println(String(","));
    }
  }
  bytesWritten =
    bytesWritten
    + output.println(String("  ],"));
  bytesWritten =
    bytesWritten
    + output.println(String("}"));
  return bytesWritten;
}

int ESATCCSDSPacket::read()
{
  const int datum = peek();
  if (datum >= 0)
  {
    position = position + 1;
  }
  return datum;
}

word ESATCCSDSPacket::readApplicationProcessIdentifier() const
{
  return readPrimaryHeaderBits(APPLICATION_PROCESS_IDENTIFIER_OFFSET,
                               APPLICATION_PROCESS_IDENTIFIER_LENGTH);
}

byte ESATCCSDSPacket::readBinaryCodedDecimalByte()
{
  const byte datum = readByte();
  return Util.decodeBinaryCodedDecimalByte(datum);
}

word ESATCCSDSPacket::readBinaryCodedDecimalWord()
{
  const word datum = readWord();
  return Util.decodeBinaryCodedDecimalWord(datum);
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
  const int datum = read();
  if (datum >= 0)
  {
    return datum;
  }
  else
  {
    return 0;
  }
}

signed char ESATCCSDSPacket::readChar()
{
  const byte datum = readByte();
  if (datum > 127)
  {
    return -((signed char) ((~datum) + 1));
  }
  else
  {
    return datum;
  }
}

float ESATCCSDSPacket::readFloat()
{
  const unsigned long bits = readUnsignedLong();
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
  return true;
}

int ESATCCSDSPacket::readInt()
{
  const word datum = readWord();
  if (datum > 32767U)
  {
    return -int((~datum) + 1U);
  }
  else
  {
    return datum;
  }
}

long ESATCCSDSPacket::readLong()
{
  const unsigned long datum = readUnsignedLong();
  if (datum > 2147483647UL)
  {
    return -long((~datum) + 1UL);
  }
  else
  {
    return datum;
  }
}

long ESATCCSDSPacket::readPacketDataLength() const
{
  return 1 + long(word(primaryHeader[PACKET_DATA_LENGTH_OFFSET],
                       primaryHeader[PACKET_DATA_LENGTH_OFFSET + 1]));
}

word ESATCCSDSPacket::readPacketSequenceCount() const
{
  return readPrimaryHeaderBits(PACKET_SEQUENCE_COUNT_OFFSET,
                               PACKET_SEQUENCE_COUNT_LENGTH);
}

ESATCCSDSPacket::PacketType ESATCCSDSPacket::readPacketType() const
{
  const word bits = readPrimaryHeaderBits(PACKET_TYPE_OFFSET,
                                          PACKET_TYPE_LENGTH);
  return ESATCCSDSPacket::PacketType(bits);
}

byte ESATCCSDSPacket::readPacketVersionNumber() const
{
  return readPrimaryHeaderBits(PACKET_VERSION_NUMBER_OFFSET,
                               PACKET_VERSION_NUMBER_LENGTH);
}

word ESATCCSDSPacket::readPrimaryHeaderBits(const byte offset,
                                            const byte length) const
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

ESATCCSDSSecondaryHeader ESATCCSDSPacket::readSecondaryHeader()
{
  ESATCCSDSSecondaryHeader datum;
  datum.preamble = (ESATCCSDSSecondaryHeader::Preamble) readByte();
  datum.timestamp = readTimestamp();
  datum.majorVersionNumber = readByte();
  datum.minorVersionNumber = readByte();
  datum.patchVersionNumber = readByte();
  datum.packetIdentifier = readByte();
  return datum;
}

ESATCCSDSPacket::SecondaryHeaderFlag ESATCCSDSPacket::readSecondaryHeaderFlag() const
{
  const word bits = readPrimaryHeaderBits(SECONDARY_HEADER_FLAG_OFFSET,
                                          SECONDARY_HEADER_FLAG_LENGTH);
  return ESATCCSDSPacket::SecondaryHeaderFlag(bits);
}

ESATCCSDSPacket::SequenceFlags ESATCCSDSPacket::readSequenceFlags() const
{
  const word bits = readPrimaryHeaderBits(SEQUENCE_FLAGS_OFFSET,
                                          SEQUENCE_FLAGS_LENGTH);
  return ESATCCSDSPacket::SequenceFlags(bits);
}

ESATTimestamp ESATCCSDSPacket::readTimestamp()
{
  ESATTimestamp datum;
  datum.year = readBinaryCodedDecimalWord();
  datum.month = readBinaryCodedDecimalByte();
  datum.day = readBinaryCodedDecimalByte();
  datum.hours = readBinaryCodedDecimalByte();
  datum.minutes = readBinaryCodedDecimalByte();
  datum.seconds = readBinaryCodedDecimalByte();
  return datum;
}

unsigned long ESATCCSDSPacket::readUnsignedLong()
{
  const unsigned long firstByte = readByte();
  const unsigned long secondByte = readByte();
  const unsigned long thirdByte = readByte();
  const unsigned long fourthByte = readByte();
  return (firstByte << 24) | (secondByte << 16) | (thirdByte << 8) | fourthByte;
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

size_t ESATCCSDSPacket::write(const uint8_t datum)
{
  if (position < packetDataBufferLength)
  {
    packetData[position] = datum;
    position = position + 1;
    return 1;
  }
  else
  {
    return 0;
  }
}

void ESATCCSDSPacket::writeApplicationProcessIdentifier(const word applicationProcessIdentifier)
{
  writePrimaryHeaderBits(APPLICATION_PROCESS_IDENTIFIER_OFFSET,
                         APPLICATION_PROCESS_IDENTIFIER_LENGTH,
                         applicationProcessIdentifier);
}

void ESATCCSDSPacket::writeBinaryCodedDecimalByte(const byte datum)
{
  writeByte(Util.encodeBinaryCodedDecimalByte(datum));
}

void ESATCCSDSPacket::writeBinaryCodedDecimalWord(const word datum)
{
  writeWord(Util.encodeBinaryCodedDecimalWord(datum));
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
  (void) write(datum);
}

void ESATCCSDSPacket::writeChar(const signed char datum)
{
  if (datum < 0)
  {
    writeByte(~((byte) -(datum + 1)));
  }
  else
  {
    writeByte(datum);
  }
}

void ESATCCSDSPacket::writeFloat(const float datum)
{
  const unsigned long bits = floatToLong(datum);
  writeUnsignedLong(bits);
}

void ESATCCSDSPacket::writeInt(const int datum)
{
  if (datum < 0)
  {
    writeWord(~((word) -(datum + 1)));
  }
  else
  {
    writeWord(datum);
  }
}

void ESATCCSDSPacket::writeLong(const long datum)
{
  if (datum < 0)
  {
    writeUnsignedLong(~((unsigned long) -(datum + 1)));
  }
  else
  {
    writeUnsignedLong(datum);
  }
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

void ESATCCSDSPacket::writeSecondaryHeader(const ESATCCSDSSecondaryHeader datum)
{
  writeByte(datum.preamble);
  writeTimestamp(datum.timestamp);
  writeByte(datum.majorVersionNumber);
  writeByte(datum.minorVersionNumber);
  writeByte(datum.patchVersionNumber);
  writeByte(datum.packetIdentifier);
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

void ESATCCSDSPacket::writeTimestamp(const ESATTimestamp datum)
{
  writeBinaryCodedDecimalWord(datum.year);
  writeBinaryCodedDecimalByte(datum.month);
  writeBinaryCodedDecimalByte(datum.day);
  writeBinaryCodedDecimalByte(datum.hours);
  writeBinaryCodedDecimalByte(datum.minutes);
  writeBinaryCodedDecimalByte(datum.seconds);
}

boolean ESATCCSDSPacket::writeTo(Stream& output) const
{
  for (byte i = 0; i < PRIMARY_HEADER_LENGTH; i++)
  {
    const size_t bytesWritten = output.write(primaryHeader[i]);
    if (bytesWritten == 0)
    {
      return false;
    }
  }
  const long packetDataLength = readPacketDataLength();
  if (packetDataLength > packetDataBufferLength)
  {
    return false;
  }
  for (long i = 0; i < packetDataLength; i++)
  {
    const size_t bytesWritten = output.write(packetData[i]);
    if (bytesWritten == 0)
    {
      return false;
    }
  }
  return true;
}

void ESATCCSDSPacket::writeUnsignedLong(const unsigned long datum)
{
  writeByte((datum >> 24) & B11111111);
  writeByte((datum >> 16) & B11111111);
  writeByte((datum >> 8) & B11111111);
  writeByte(datum & B11111111);
}

void ESATCCSDSPacket::writeWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}

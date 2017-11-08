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

#include "ESAT_CCSDSPacket.h"
#include "ESAT_Util.h"

ESAT_CCSDSPacket::ESAT_CCSDSPacket():
  primaryHeader(),
  packetData()
{
  setTimeout(0);
}

ESAT_CCSDSPacket::ESAT_CCSDSPacket(byte* const buffer,
                                   const unsigned long bufferLength):
  primaryHeader(),
  packetData(buffer, bufferLength)
{
  setTimeout(0);
}

int ESAT_CCSDSPacket::available()
{
  return constrain(availableBytesToRead(), 0, 0x7FFF);
}

unsigned long ESAT_CCSDSPacket::availableBytesToRead() const
{
  return packetData.availableBytes();
}

unsigned long ESAT_CCSDSPacket::capacity() const
{
  return packetData.capacity();
}

boolean ESAT_CCSDSPacket::copyTo(ESAT_CCSDSPacket& target)
{
  if (target.capacity() < packetData.length())
  {
    return false;
  }
  target.writePrimaryHeader(primaryHeader);
  target.rewind();
  return packetData.writeTo(target);
}

void ESAT_CCSDSPacket::flush()
{
  primaryHeader = ESAT_CCSDSPrimaryHeader();
  packetData.flush();
}

unsigned long ESAT_CCSDSPacket::length() const
{
  return primaryHeader.LENGTH + primaryHeader.packetDataLength;
}

int ESAT_CCSDSPacket::peek()
{
  return packetData.peek();
}

size_t ESAT_CCSDSPacket::printTo(Print& output) const
{
  size_t bytesWritten = 0;
  bytesWritten =
    bytesWritten
    + output.println(String("{"));
  bytesWritten =
    bytesWritten
    + output.println(String("  \"primaryHeader\":"));
  bytesWritten =
    bytesWritten
    + output.print(primaryHeader);
  bytesWritten =
    bytesWritten
    + output.println(String(","));
  bytesWritten =
    bytesWritten
    + output.println(String("  \"packetData\": ["));
  bytesWritten =
    bytesWritten
    + output.print(String("    "));
  bytesWritten =
    bytesWritten
    + output.print(packetData);
  bytesWritten =
    bytesWritten
    + output.println(String(""));
  bytesWritten =
    bytesWritten
    + output.println(String("  ],"));
  bytesWritten =
    bytesWritten
    + output.println(String("}"));
  return bytesWritten;
}

int ESAT_CCSDSPacket::read()
{
  return packetData.read();
}

byte ESAT_CCSDSPacket::readBinaryCodedDecimalByte()
{
  const byte datum = readByte();
  return ESAT_Util.decodeBinaryCodedDecimalByte(datum);
}

word ESAT_CCSDSPacket::readBinaryCodedDecimalWord()
{
  const word datum = readWord();
  return ESAT_Util.decodeBinaryCodedDecimalWord(datum);
}

boolean ESAT_CCSDSPacket::readBoolean()
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

byte ESAT_CCSDSPacket::readByte()
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

signed char ESAT_CCSDSPacket::readChar()
{
  const byte datum = readByte();
  return ESAT_Util.byteToChar(datum);
}

float ESAT_CCSDSPacket::readFloat()
{
  const unsigned long bits = readUnsignedLong();
  return ESAT_Util.unsignedLongToFloat(bits);
}

boolean ESAT_CCSDSPacket::readFrom(Stream& input)
{
  const boolean correctPrimaryHeader =
    primaryHeader.readFrom(input);
  if (!correctPrimaryHeader)
  {
    return false;
  }
  return packetData.readFrom(input, primaryHeader.packetDataLength);
}

int ESAT_CCSDSPacket::readInt()
{
  const word datum = readWord();
  return ESAT_Util.wordToInt(datum);
}

long ESAT_CCSDSPacket::readLong()
{
  const unsigned long datum = readUnsignedLong();
  return ESAT_Util.unsignedLongToLong(datum);
}

ESAT_CCSDSPrimaryHeader ESAT_CCSDSPacket::readPrimaryHeader() const
{
  return primaryHeader;
}

ESAT_CCSDSSecondaryHeader ESAT_CCSDSPacket::readSecondaryHeader()
{
  ESAT_CCSDSSecondaryHeader datum;
  datum.preamble = (ESAT_CCSDSSecondaryHeader::Preamble) readByte();
  datum.timestamp = readTimestamp();
  datum.majorVersionNumber = readByte();
  datum.minorVersionNumber = readByte();
  datum.patchVersionNumber = readByte();
  datum.packetIdentifier = readByte();
  return datum;
}

ESAT_Timestamp ESAT_CCSDSPacket::readTimestamp()
{
  ESAT_Timestamp datum;
  datum.year = readBinaryCodedDecimalWord();
  datum.month = readBinaryCodedDecimalByte();
  datum.day = readBinaryCodedDecimalByte();
  datum.hours = readBinaryCodedDecimalByte();
  datum.minutes = readBinaryCodedDecimalByte();
  datum.seconds = readBinaryCodedDecimalByte();
  return datum;
}

unsigned long ESAT_CCSDSPacket::readUnsignedLong()
{
  const unsigned long firstByte = readByte();
  const unsigned long secondByte = readByte();
  const unsigned long thirdByte = readByte();
  const unsigned long fourthByte = readByte();
  return (firstByte << 24) | (secondByte << 16) | (thirdByte << 8) | fourthByte;
}

word ESAT_CCSDSPacket::readWord()
{
  const byte highByte = readByte();
  const byte lowByte = readByte();
  return word(highByte, lowByte);
}

void ESAT_CCSDSPacket::rewind()
{
  packetData.rewind();
}

size_t ESAT_CCSDSPacket::write(const uint8_t datum)
{
  const size_t bytesWritten = packetData.write(datum);
  primaryHeader.packetDataLength = packetData.length();
  return bytesWritten;
}

void ESAT_CCSDSPacket::writeBinaryCodedDecimalByte(const byte datum)
{
  writeByte(ESAT_Util.encodeBinaryCodedDecimalByte(datum));
}

void ESAT_CCSDSPacket::writeBinaryCodedDecimalWord(const word datum)
{
  writeWord(ESAT_Util.encodeBinaryCodedDecimalWord(datum));
}

void ESAT_CCSDSPacket::writeBoolean(const boolean datum)
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

void ESAT_CCSDSPacket::writeByte(const byte datum)
{
  (void) write(datum);
}

void ESAT_CCSDSPacket::writeChar(const signed char datum)
{
  writeByte(ESAT_Util.charToByte(datum));
}

void ESAT_CCSDSPacket::writeFloat(const float datum)
{
  const unsigned long bits = ESAT_Util.floatToUnsignedLong(datum);
  writeUnsignedLong(bits);
}

void ESAT_CCSDSPacket::writeInt(const int datum)
{
  writeWord(ESAT_Util.intToWord(datum));
}

void ESAT_CCSDSPacket::writeLong(const long datum)
{
  writeUnsignedLong(ESAT_Util.longToUnsignedLong(datum));
}

void ESAT_CCSDSPacket::writePrimaryHeader(const ESAT_CCSDSPrimaryHeader datum)
{
  primaryHeader = datum;
}

void ESAT_CCSDSPacket::writeSecondaryHeader(const ESAT_CCSDSSecondaryHeader datum)
{
  writeByte(datum.preamble);
  writeTimestamp(datum.timestamp);
  writeByte(datum.majorVersionNumber);
  writeByte(datum.minorVersionNumber);
  writeByte(datum.patchVersionNumber);
  writeByte(datum.packetIdentifier);
}

void ESAT_CCSDSPacket::writeTimestamp(const ESAT_Timestamp datum)
{
  writeBinaryCodedDecimalWord(datum.year);
  writeBinaryCodedDecimalByte(datum.month);
  writeBinaryCodedDecimalByte(datum.day);
  writeBinaryCodedDecimalByte(datum.hours);
  writeBinaryCodedDecimalByte(datum.minutes);
  writeBinaryCodedDecimalByte(datum.seconds);
}

boolean ESAT_CCSDSPacket::writeTo(Stream& output) const
{
  const boolean correctPrimaryHeader = primaryHeader.writeTo(output);
  if (!correctPrimaryHeader)
  {
    return false;
  }
  return packetData.writeTo(output);
}

void ESAT_CCSDSPacket::writeUnsignedLong(const unsigned long datum)
{
  writeByte((datum >> 24) & B11111111);
  writeByte((datum >> 16) & B11111111);
  writeByte((datum >> 8) & B11111111);
  writeByte(datum & B11111111);
}

void ESAT_CCSDSPacket::writeWord(const word datum)
{
  writeByte(highByte(datum));
  writeByte(lowByte(datum));
}
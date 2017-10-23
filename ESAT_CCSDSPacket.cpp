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

void ESAT_CCSDSPacket::clear()
{
  primaryHeader = ESAT_CCSDSPrimaryHeader();
  packetData.flush();
}

boolean ESAT_CCSDSPacket::copyTo(ESAT_CCSDSPacket& target)
{
  if (target.capacity() < packetData.length())
  {
    return false;
  }
  target.writePrimaryHeader(primaryHeader);
  rewind();
  target.rewind();
  return packetData.writeTo(target);
}

unsigned long ESAT_CCSDSPacket::floatToLong(const float number)
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

void ESAT_CCSDSPacket::flush()
{
  primaryHeader.packetDataLength = packetData.length();
  rewind();
}

unsigned long ESAT_CCSDSPacket::length() const
{
  return primaryHeader.LENGTH + primaryHeader.packetDataLength;
}

float ESAT_CCSDSPacket::longToFloat(const unsigned long bits)
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
  if (datum > 127)
  {
    return -((signed char) ((~datum) + 1));
  }
  else
  {
    return datum;
  }
}

float ESAT_CCSDSPacket::readFloat()
{
  const unsigned long bits = readUnsignedLong();
  return longToFloat(bits);
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
  if (datum > 32767U)
  {
    return -int((~datum) + 1U);
  }
  else
  {
    return datum;
  }
}

long ESAT_CCSDSPacket::readLong()
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
  return packetData.write(datum);
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
  if (datum < 0)
  {
    writeByte(~((byte) -(datum + 1)));
  }
  else
  {
    writeByte(datum);
  }
}

void ESAT_CCSDSPacket::writeFloat(const float datum)
{
  const unsigned long bits = floatToLong(datum);
  writeUnsignedLong(bits);
}

void ESAT_CCSDSPacket::writeInt(const int datum)
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

void ESAT_CCSDSPacket::writeLong(const long datum)
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

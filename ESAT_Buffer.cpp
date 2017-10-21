#include "ESAT_Buffer.h"

ESAT_Buffer::ESAT_Buffer():
  buffer(nullptr),
  bufferCapacity(0),
  bytesInBuffer(0),
  readWritePosition(0)
{
}

ESAT_Buffer::ESAT_Buffer(byte array[], const unsigned long length):
  buffer(array),
  bufferCapacity(length),
  bytesInBuffer(0),
  readWritePosition(0)
{
}

int ESAT_Buffer::available()
{
  return constrain(availableBytes(), 0, 0x7FFF);
}

unsigned long ESAT_Buffer::availableBytes() const
{
  if (readWritePosition < bytesInBuffer)
  {
    return bytesInBuffer - readWritePosition;
  }
  else
  {
    return 0;
  }
}

unsigned long ESAT_Buffer::capacity() const
{
  return bufferCapacity;
}

void ESAT_Buffer::flush()
{
  bytesInBuffer = 0;
  readWritePosition = 0;
}

unsigned long ESAT_Buffer::length() const
{
  return bytesInBuffer;
}

int ESAT_Buffer::peek()
{
  if (availableBytes() == 0)
  {
    return -1;
  }
  return buffer[readWritePosition];
}

unsigned long ESAT_Buffer::position() const
{
  return readWritePosition;
}

size_t ESAT_Buffer::printTo(Print& output) const
{
  if (bytesInBuffer == 0)
  {
    return 0;
  }
  size_t bytesWritten =
    output.print(String(buffer[0], HEX));
  for (unsigned long i = 1; i < bytesInBuffer; i++)
  {
    bytesWritten =
      bytesWritten
      + output.print(String(", "));
    bytesWritten =
      bytesWritten
      + output.print(String(buffer[i], HEX));
  }
}

int ESAT_Buffer::read()
{
  const int datum = peek();
  if (datum > -1)
  {
    readWritePosition = readWritePosition + 1;
  }
  return datum;
}

boolean ESAT_Buffer::readFrom(Stream& input, const unsigned long bytesToRead)
{
  flush();
  if (bytesToRead > bufferCapacity)
  {
    return false;
  }
  bytesInBuffer = input.readBytes((char*) buffer, bytesToRead);
  if (bytesInBuffer == bytesToRead)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void ESAT_Buffer::rewind()
{
  readWritePosition = 0;
}

size_t ESAT_Buffer::write(const uint8_t datum)
{
  if (!buffer)
  {
    return 0;
  }
  if (readWritePosition >= bufferCapacity)
  {
    return 0;
  }
  buffer[readWritePosition] = datum;
  readWritePosition = readWritePosition + 1;
  bytesInBuffer = readWritePosition;
  return 1;
}

boolean ESAT_Buffer::writeTo(Stream& output) const
{
  if (!buffer)
  {
    return false;
  }
  if (bytesInBuffer == 0)
  {
    return true;
  }
  const size_t bytesWritten = output.write(buffer, bytesInBuffer);
  if (bytesWritten == bytesInBuffer)
  {
    return true;
  }
  else
  {
    return false;
  }
}

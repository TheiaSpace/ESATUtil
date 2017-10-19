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

#include "ESATKISSStream.h"

ESATKISSStream::ESATKISSStream():
  backendStream(nullptr),
  backendBuffer(nullptr),
  backendBufferLength(0),
  decoderState(WAITING_FOR_FRAME_START),
  position(0),
  decodedDataLength(0)
{
}

ESATKISSStream::ESATKISSStream(Stream& stream,
                               byte buffer[],
                               unsigned long bufferLength):
  backendStream(&stream),
  backendBuffer(buffer),
  backendBufferLength(bufferLength),
  decoderState(WAITING_FOR_FRAME_START),
  position(0),
  decodedDataLength(0)
{
}

int ESATKISSStream::available()
{
  return constrain(decodedDataLength - position, 0, 0x7FFF);
}

size_t ESATKISSStream::append(const byte datum)
{
  if (!backendStream)
  {
    return 0;
  }
  if (!backendBuffer)
  {
    return 0;
  }
  if (position >= backendBufferLength)
  {
    return 0;
  }
  backendBuffer[position] = datum;
  position = position + 1;
  return 1;
}

void ESATKISSStream::decode(const byte datum)
{
  switch (decoderState)
  {
    case WAITING_FOR_FRAME_START:
      decodeFrameStart(datum);
      break;
    case WAITING_FOR_DATA_FRAME:
      decodeDataFrame(datum);
      break;
    case DECODING_FRAME_DATA:
      decodeFrameData(datum);
      break;
    case DECODING_ESCAPED_FRAME_DATA:
      decodeEscapedFrameData(datum);
      break;
    default:
      break;
  }
}

void ESATKISSStream::decodeDataFrame(const byte datum)
{
  switch (datum)
  {
    case DATA_FRAME:
      decoderState = DECODING_FRAME_DATA;
      break;
    case FRAME_END:
      decoderState = WAITING_FOR_DATA_FRAME;
      break;
    default:
      decoderState = WAITING_FOR_FRAME_START;
      break;
  }
}

void ESATKISSStream::decodeEscapedFrameData(const byte datum)
{
  switch (datum)
  {
    case TRANSPOSED_FRAME_END:
      if (append(FRAME_END) > 0)
      {
        decoderState = DECODING_FRAME_DATA;
      }
      else
      {
        reset();
      }
      break;
    case TRANSPOSED_FRAME_ESCAPE:
      if (append(FRAME_ESCAPE) > 0)
      {
        decoderState = DECODING_FRAME_DATA;
      }
      else
      {
        reset();
      }
      break;
    default:
      decoderState = DECODING_FRAME_DATA;
      break;
  }
}

void ESATKISSStream::decodeFrameData(const byte datum)
{
  switch (datum)
  {
    case FRAME_END:
      decoderState = FINISHED;
      break;
    case FRAME_ESCAPE:
      decoderState = DECODING_ESCAPED_FRAME_DATA;
      break;
    default:
      (void) append(datum);
      break;
  }
}

void ESATKISSStream::decodeFrameStart(const byte datum)
{
  switch (datum)
  {
    case FRAME_END:
      decoderState = WAITING_FOR_DATA_FRAME;
      break;
    default:
      decoderState = WAITING_FOR_FRAME_START;
      break;
  }
}

size_t ESATKISSStream::beginFrame()
{
  reset();
  const size_t frameEndBytesWritten =
    append(FRAME_END);
  const size_t dataFrameBytesWritten =
    append(DATA_FRAME);
  return frameEndBytesWritten + dataFrameBytesWritten;
}

size_t ESATKISSStream::endFrame()
{
  const size_t frameEndBytesWritten = append(FRAME_END);
  flush();
  return frameEndBytesWritten;
}

void ESATKISSStream::flush()
{
  if (backendBufferLength == 0)
  {
    return;
  }
  if (!backendStream)
  {
    return;
  }
  if (!backendBuffer)
  {
    return;
  }
  (void) backendStream->write(backendBuffer, position);
  reset();
}

unsigned long ESATKISSStream::frameLength(const unsigned long dataLength)
{
  const unsigned long frameStartLength = 1;
  const unsigned long dataFrameLength = 1;
  const unsigned long frameEndLength = 1;
  const unsigned long escapeFactor = 2;
  const unsigned long totalLength =
    frameStartLength
    + dataFrameLength
    + escapeFactor * dataLength
    + frameEndLength;
  return totalLength;
}

int ESATKISSStream::peek()
{
  if (available() > 0)
  {
    return backendBuffer[position];
  }
  else
  {
    return -1;
  }
}

int ESATKISSStream::read()
{
  const int datum = peek();
  if (datum >= 0)
  {
    position = position + 1;
  }
  return datum;
}

boolean ESATKISSStream::receiveFrame()
{
  if (!backendStream)
  {
    return false;
  }
  if (!backendBuffer)
  {
    return false;
  }
  if (backendBufferLength == 0)
  {
    return false;
  }
  if (decoderState == FINISHED)
  {
    reset();
  }
  if (position >= backendBufferLength)
  {
    reset();
  }
  while ((backendStream->available() > 0)
         && (decoderState != FINISHED))
  {
    const byte datum = backendStream->read();
    if (datum >= 0)
    {
      decode(datum);
    }
  }
  decodedDataLength = position;
  if (decoderState == FINISHED)
  {
    position = 0;
    return true;
  }
  else
  {
    return false;
  }
}

void ESATKISSStream::reset()
{
  position = 0;
  decodedDataLength = 0;
  decoderState = WAITING_FOR_FRAME_START;
}

size_t ESATKISSStream::write(const uint8_t datum)
{
  switch (datum)
  {
    case FRAME_END:
      return writeEscapedFrameEnd();
      break;
    case FRAME_ESCAPE:
      return writeEscapedFrameEscape();
      break;
    default:
      return append(datum);
      break;
  }
}

size_t ESATKISSStream::writeEscapedFrameEnd()
{
  const size_t frameEscapeBytesWritten =
    append(FRAME_ESCAPE);
  const size_t transposedFrameEndBytesWritten =
    append(TRANSPOSED_FRAME_END);
  return frameEscapeBytesWritten + transposedFrameEndBytesWritten;
}

size_t ESATKISSStream::writeEscapedFrameEscape()
{
  const size_t frameEscapeBytesWritten =
    append(FRAME_ESCAPE);
  const size_t transposedFrameEscapeBytesWritten =
    append(TRANSPOSED_FRAME_ESCAPE);
  return frameEscapeBytesWritten + transposedFrameEscapeBytesWritten;
}

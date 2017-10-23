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

#include "ESAT_KISSStream.h"

ESAT_KISSStream::ESAT_KISSStream():
  backendStream(nullptr),
  backendBuffer(),
  decoderState(WAITING_FOR_FRAME_START)
{
}

ESAT_KISSStream::ESAT_KISSStream(Stream& stream,
                                 byte buffer[],
                                 unsigned long bufferLength):
  backendStream(&stream),
  backendBuffer(buffer, bufferLength),
  decoderState(WAITING_FOR_FRAME_START)
{
}

int ESAT_KISSStream::available()
{
  if (decoderState == FINISHED)
  {
    return backendBuffer.available();
  }
  else
  {
    return 0;
  }
}

size_t ESAT_KISSStream::append(const byte datum)
{
  return backendBuffer.write(datum);
}

void ESAT_KISSStream::decode(const byte datum)
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

void ESAT_KISSStream::decodeDataFrame(const byte datum)
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

void ESAT_KISSStream::decodeEscapedFrameData(const byte datum)
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

void ESAT_KISSStream::decodeFrameData(const byte datum)
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

void ESAT_KISSStream::decodeFrameStart(const byte datum)
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

size_t ESAT_KISSStream::beginFrame()
{
  reset();
  const size_t frameEndBytesWritten =
    append(FRAME_END);
  const size_t dataFrameBytesWritten =
    append(DATA_FRAME);
  return frameEndBytesWritten + dataFrameBytesWritten;
}

size_t ESAT_KISSStream::endFrame()
{
  const size_t frameEndBytesWritten = append(FRAME_END);
  flush();
  return frameEndBytesWritten;
}

void ESAT_KISSStream::flush()
{
  if (!backendStream)
  {
    return;
  }
  (void) backendBuffer.writeTo(*backendStream);
  reset();
}

unsigned long ESAT_KISSStream::frameLength(const unsigned long dataLength)
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

int ESAT_KISSStream::peek()
{
  return backendBuffer.peek();
}

int ESAT_KISSStream::read()
{
  return backendBuffer.read();
}

boolean ESAT_KISSStream::receiveFrame()
{
  if (!backendStream)
  {
    return false;
  }
  if (backendBuffer.capacity() == 0)
  {
    return false;
  }
  if (decoderState == FINISHED)
  {
    reset();
  }
  if (backendBuffer.length() >= backendBuffer.capacity())
  {
    reset();
  }
  const int available = backendStream->available();
  for (unsigned int i = 0;
       (i < available) && (decoderState != FINISHED);
       i++)
  {
    const byte datum = backendStream->read();
    if (datum >= 0)
    {
      decode(datum);
    }
  }
  if (decoderState == FINISHED)
  {
    backendBuffer.rewind();
    return true;
  }
  else
  {
    return false;
  }
}

void ESAT_KISSStream::reset()
{
  backendBuffer.flush();
  decoderState = WAITING_FOR_FRAME_START;
}

size_t ESAT_KISSStream::write(const uint8_t datum)
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

size_t ESAT_KISSStream::writeEscapedFrameEnd()
{
  const size_t frameEscapeBytesWritten =
    append(FRAME_ESCAPE);
  const size_t transposedFrameEndBytesWritten =
    append(TRANSPOSED_FRAME_END);
  return frameEscapeBytesWritten + transposedFrameEndBytesWritten;
}

size_t ESAT_KISSStream::writeEscapedFrameEscape()
{
  const size_t frameEscapeBytesWritten =
    append(FRAME_ESCAPE);
  const size_t transposedFrameEscapeBytesWritten =
    append(TRANSPOSED_FRAME_ESCAPE);
  return frameEscapeBytesWritten + transposedFrameEscapeBytesWritten;
}

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

ESATKISSStream::ESATKISSStream(Stream& stream,
                               byte buffer[],
                               unsigned long bufferLength):
  backendStream(stream),
  decoderBuffer(buffer),
  decoderBufferLength(bufferLength),
  decoderState(WAITING_FOR_FRAME_START),
  readPosition(0),
  decodedDataLength(0)
{
}

int ESATKISSStream::available()
{
  if (readPosition >= decodedDataLength)
  {
    reset();
    while ((backendStream.available() > 0)
           && (decoderState != FINISHED))
    {
      const byte datum = backendStream.read();
      if (datum >= 0)
      {
        decode(datum);
      }
    }
  }
  return constrain(decodedDataLength - readPosition, 0, 0x7FFF);
}

void ESATKISSStream::append(const byte datum)
{
  if (decodedDataLength < decoderBufferLength)
  {
    decoderBuffer[decodedDataLength] = datum;
    decodedDataLength = decodedDataLength + 1;
    decoderState = DECODING_FRAME_DATA;
  }
  else
  {
    reset();
  }
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
      append(FRAME_END);
      break;
    case TRANSPOSED_FRAME_ESCAPE:
      append(FRAME_ESCAPE);
      break;
    default:
      reset();
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
      append(datum);
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
  const size_t frameEndBytesWritten =
    backendStream.write(FRAME_END);
  const size_t dataFrameBytesWritten =
    backendStream.write(DATA_FRAME);
  return frameEndBytesWritten + dataFrameBytesWritten;
}

size_t ESATKISSStream::endFrame()
{
  return backendStream.write(FRAME_END);
}

void ESATKISSStream::flush()
{
  backendStream.flush();
}

int ESATKISSStream::peek()
{
  if (available() > 0)
  {
    return decoderBuffer[readPosition];
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
    readPosition = readPosition + 1;
  }
  return datum;
}

void ESATKISSStream::reset()
{
  readPosition = 0;
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
      return backendStream.write(datum);
      break;
  }
}

size_t ESATKISSStream::writeEscapedFrameEnd()
{
  const size_t frameEscapeBytesWritten =
    backendStream.write(FRAME_ESCAPE);
  const size_t transposedFrameEndBytesWritten =
    backendStream.write(TRANSPOSED_FRAME_END);
  return frameEscapeBytesWritten + transposedFrameEndBytesWritten;
}

size_t ESATKISSStream::writeEscapedFrameEscape()
{
  const size_t frameEscapeBytesWritten =
    backendStream.write(FRAME_ESCAPE);
  const size_t transposedFrameEscapeBytesWritten =
    backendStream.write(TRANSPOSED_FRAME_ESCAPE);
  return frameEscapeBytesWritten + transposedFrameEscapeBytesWritten;
}

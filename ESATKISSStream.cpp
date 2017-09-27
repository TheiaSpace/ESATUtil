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

ESATKISSStream::ESATKISSStream(Stream& stream):
  backendStream(stream),
  decoderState(WAITING_FOR_FRAME_START_MARK),
  nextByte(-1)
{
}

int ESATKISSStream::available()
{
  const int nextByte = peek();
  if (nextByte > -1)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

size_t ESATKISSStream::beginTransmission()
{
  const size_t frameEndMarkBytesWritten = backendStream.write(FRAME_END);
  const size_t dataFrameCommandCodeBytesWritten =
    backendStream.write(DATA_FRAME_COMMAND_CODE);
  return frameEndMarkBytesWritten + dataFrameCommandCodeBytesWritten;
}

int ESATKISSStream::decode(int datum)
{
  if (datum == -1)
  {
    return -1;
  }
  switch (decoderState)
  {
  case WAITING_FOR_FRAME_START_MARK:
    return decodeStartMark(datum);
    break;
  case WAITING_FOR_DATA_FRAME_COMMAND_CODE:
    return decodeFrameCommandCode(datum);
    break;
  case DECODING_FRAME_DATA:
    return decodeFrameData(datum);
    break;
  case DECODING_ESCAPED_FRAME_DATA:
    return decodeEscapedFrameData(datum);
    break;
  default:
    return -1;
    break;
  }
  return -1;
}

int ESATKISSStream::decodeEscapedFrameData(const int datum)
{
  switch (datum)
  {
  case TRANSPOSED_FRAME_END:
    decoderState = DECODING_FRAME_DATA;
    return FRAME_END;
    break;
  case TRANSPOSED_FRAME_ESCAPE:
    decoderState = DECODING_FRAME_DATA;
    return FRAME_ESCAPE;
    break;
  default:
    decoderState = WAITING_FOR_FRAME_START_MARK;
    return -1;
    break;
  }
  return -1;
}

int ESATKISSStream::decodeFrameCommandCode(const int datum)
{
  if (datum == DATA_FRAME_COMMAND_CODE)
  {
    decoderState = DECODING_FRAME_DATA;
  }
  else
  {
    decoderState = WAITING_FOR_FRAME_START_MARK;
  }
  return -1;
}

int ESATKISSStream::decodeFrameData(const int datum)
{
  switch (datum)
  {
  case FRAME_END:
    decoderState = WAITING_FOR_FRAME_START_MARK;
    return -1;
    break;
  case FRAME_ESCAPE:
    decoderState = DECODING_ESCAPED_FRAME_DATA;
    return -1;
    break;
  default:
    return datum;
    break;
  }
  return -1;
}

int ESATKISSStream::decodeStartMark(const int datum)
{
  if (datum == FRAME_END)
  {
    decoderState = WAITING_FOR_DATA_FRAME_COMMAND_CODE;
    return -1;
  }
}

size_t ESATKISSStream::endTransmission()
{
  return backendStream.write(FRAME_END);
}

void ESATKISSStream::flush()
{
  backendStream.flush();
}

int ESATKISSStream::peek()
{
  if (nextByte == -1)
  {
    nextByte = read();
  }
  return nextByte;
}

int ESATKISSStream::read()
{
  nextByte = -1;
  while (backendStream.available() > 0)
  {
    const int datum = backendStream.read();
    const int decodedDatum = decode(datum);
    if (decodedDatum > -1)
    {
      return decodedDatum;
    }
  }
  return -1;
}

size_t ESATKISSStream::write(const uint8_t datum)
{
  switch (datum)
  {
  case FRAME_END:
    return writeEscapedFrameEndMark();
    break;
  case FRAME_ESCAPE:
    return writeEscapedEscapeMark();
    break;
  default:
    return backendStream.write(datum);
    break;
  }
}

size_t ESATKISSStream::writeEscapedEscapeMark()
{
  const size_t escapeMarkBytesWritten =
    backendStream.write(FRAME_ESCAPE);
  const size_t escapedEscapeMarkBytesWritten =
    backendStream.write(TRANSPOSED_FRAME_ESCAPE);
  return escapeMarkBytesWritten + escapedEscapeMarkBytesWritten;
}

size_t ESATKISSStream::writeEscapedFrameEndMark()
{
  const size_t escapeMarkBytesWritten =
    backendStream.write(FRAME_ESCAPE);
  const size_t escapedFrameEndMarkBytesWritten =
    backendStream.write(TRANSPOSED_FRAME_END);
  return escapeMarkBytesWritten + escapedFrameEndMarkBytesWritten;
}

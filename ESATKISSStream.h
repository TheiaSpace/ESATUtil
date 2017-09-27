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

#ifndef ESATKISSStream_h
#define ESATKISSStream_h

#include <Arduino.h>
#include <Stream.h>

// KISS frame writer and reader.
// Operate on a backend stream.
class ESATKISSStream: public Stream
{
  public:
    // Instantiate a new KISS stream that will operate
    // on the given backend stream.
    ESATKISSStream(Stream& stream);

    // Return the number of bytes available.
    int available();

    // Start a KISS frame.
    // The next two bytes available for reading
    // will signal the start of a frame.
    // Return the number of bytes written.
    size_t beginTransmission();

    // Clear the output buffer once all the outgoing bytes have been
    // sent.
    void flush();

    // Return the next byte (or -1 if no byte could be read)
    // and advance to the next one.
    int read();

    // Return the next byte (or -1 if no byte could be read)
    // without advancing to the next one.
    int peek();

    // Write a byte.
    // Return the actual number of bytes written,
    // which may be greater than 1 due to escaping.
    size_t write(uint8_t datum);

    // End a KISS frame.
    // The next byte available for reading
    // will signal the end of a frame.
    // Return the number of bytes written.
    size_t endTransmission();

  private:
    // Special characters.
    static const byte FEND = 0xC0;
    static const byte FESC = 0xDB;
    static const byte TFEND = 0xDC;
    static const byte TFESC = 0xDD;

    // Supported command codes.
    static const byte DATA_FRAME_COMMAND_CODE = 0x00;

    // Decoder states.
    static const byte WAITING_FOR_FRAME_START_MARK = 0;
    static const byte WAITING_FOR_DATA_FRAME_COMMAND_CODE = 1;
    static const byte DECODING_FRAME_DATA = 2;
    static const byte DECODING_ESCAPED_FRAME_DATA = 3;

    // Backend stream.  Read and write operations are performed on it.
    Stream& backendStream;

    // True if the decoded input is escaped; false otherwise.
    boolean escaping;

    // Current state of the decoder state machine.
    byte decoderState;

    // Next available byte.
    int nextByte;

    // Decode an input byte.
    int decode(const int datum);

    // Decode escaped frame data.
    int decodeEscapedFrameData(const int datum);

    // Decode the frame command code.
    int decodeFrameCommandCode(const int datum);

    // Decode frame data.
    int decodeFrameData(const int datum);

    // Decode the frame start mark.
    int decodeStartMark(const int datum);

    // Write a sequence to output an escaped FESC.
    size_t writeEscapedEscapeMark();

    // Write a sequence to output an escaped FEND.
    size_t writeEscapedFrameEndMark();
};

#endif

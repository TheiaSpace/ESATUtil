/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
 *
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

#ifndef ESAT_KISSStream_h
#define ESAT_KISSStream_h

#include <Arduino.h>
#include <Stream.h>
#include <ESAT_Buffer.h>

// KISS frame writer and reader.
// Operate on a backend stream.
// Reads and writes are buffered.
// The stream is half-duplex:
// - if it is in the middle of the reception a frame, don't write;
// - if it is in the middle of the writing of a frame, don't read.
class ESAT_KISSStream: public Stream
{
  public:
    // Each escaped byte grows by this factor.
    static const byte ESCAPE_FACTOR = 2;

    // Number of bytes of the frame-begin mark (frame-start plus
    // data-frame).
    static const byte FRAME_BEGIN_LENGTH = 2;

    // Number of bytes of the frame-end mark.
    static const byte FRAME_END_LENGTH = 1;

    // Instantiate an empty KISS stream.
    // Empty KISS will not read and will not write.
    ESAT_KISSStream();

    // Instantiate a new unbuffered KISS stream that will operate
    // on the given backend stream.
    // Write operations will go unbuffered to the backend stream.
    // This KISS stream cannot use for reading: all read
    // operations will fail because there is no buffer
    // for storing the decoded data.
    ESAT_KISSStream(Stream& stream);

    // Instantiate a new buffered KISS stream that will operate
    // on the given backend stream.
    // Use the buffer for storing encoded or decoded data.
    // The KISS stream is half-duplex: it cannot be used
    // for reading frames simultaneously with writing frames.
    // The timeout for block read operations is zero.
    ESAT_KISSStream(Stream& stream,
                    byte buffer[],
                    unsigned long bufferLength);

    // Return the number of bytes available in the current frame.
    int available();

    // Start writing a KISS frame.
    // In buffered KISS streams, this writes the frame-start mark
    // to the buffer; in unbuffered KISS streams, this writes the
    // frame-start mark directly to the backend stream.
    // Return the number of bytes written.
    size_t beginFrame();

    // End writing a KISS frame.
    // In buffered KISS streams, this writes the frame-end mark to the
    // buffer and writes the buffer contents to the backend stream; in
    // unbuffered KISS streams, this writes the frame-end mark
    // directly to the backend stream.
    // Return the number of bytes written.
    // Reset the buffer so that the KISS stream can be used for
    // reading or writing a new frame.
    size_t endFrame();

    // Write the contents of the buffer to the backend stream
    // and reset buffer and the encoder state.
    void flush();

    // Return the worst case frame length for a given data length.
    static constexpr unsigned long frameLength(unsigned long dataLength)
    {
      const unsigned long escapeFactor = 2;
      const unsigned long totalLength =
        FRAME_BEGIN_LENGTH
        + escapeFactor * dataLength
        + FRAME_END_LENGTH;
      return totalLength;
    }

    // Return the next byte (or -1 if no byte could be read)
    // and advance to the next one.
    int read();

    // Return the next byte (or -1 if no byte could be read)
    // without advancing to the next one.
    int peek();

    // Receive a new frame.
    // Return true if a full frame has arrived; otherwise return false.
    // If there was a new frame in the last call to receiveFrame(),
    // start the reception of a new frame.
    boolean receiveFrame();

    // Encode and write a byte.
    // In buffered KISS streams, this writes the encoded byte
    // to the buffer; in unbuffered KISS streams, this writes
    // the encoded byte directly to the backend stream.
    // Return the actual number of bytes written,
    // which may be greater than 1 due to escaping.
    size_t write(uint8_t datum);

    // Import size_t Print::write(const uint8_t* buffer, size_t bufferLength).
    // Write a byte buffer of given length.
    // Return the number of bytes written.
    using Print::write;

  private:
    // Decoder states.
    enum DecoderState
    {
      WAITING_FOR_FRAME_START,
      WAITING_FOR_DATA_FRAME,
      DECODING_FRAME_DATA,
      DECODING_ESCAPED_FRAME_DATA,
      FINISHED,
    };

    // Special characters.
    enum SpecialCharacters
    {
      DATA_FRAME = 0x00,
      FRAME_END = 0xC0,
      FRAME_ESCAPE = 0xDB,
      TRANSPOSED_FRAME_END = 0xDC,
      TRANSPOSED_FRAME_ESCAPE = 0xDD,
    };

    // Backend stream.  Read and write operations are performed on it.
    Stream* backendStream;

    // Buffer used for encoding and decoding.
    ESAT_Buffer backendBuffer;

    // Current state of the decoder state machine.
    DecoderState decoderState;

    // Position of the next read/write operation on the backend buffer.
    unsigned long position;

    // Append a byte to the backend buffer.
    // Return the number of bytes written.
    size_t append(byte datum);

    // Decode an input byte.
    void decode(byte datum);

    // Decode the frame command code.
    void decodeDataFrame(byte datum);

    // Decode escaped frame data.
    void decodeEscapedFrameData(byte datum);

    // Decode frame data.
    void decodeFrameData(byte datum);

    // Decode the frame start mark.
    void decodeFrameStart(byte datum);

    // Reset the encoder/decoder:
    // - set decoderState to WAITING_FOR_FRAME_START;
    // - set decodedDataLength to 0;
    // - set position to 0.
    void reset();

    // Write a sequence to output an escaped frame end mark.
    size_t writeEscapedFrameEnd();

    // Write a sequence to output an escaped frame escape mark.
    size_t writeEscapedFrameEscape();
};

#endif /* ESAT_KISSStream_h */

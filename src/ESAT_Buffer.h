/*
 * Copyright (C) 2017 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_Buffer_h
#define ESAT_Buffer_h

#include <Arduino.h>
#include <Stream.h>

// Stream interface to a byte buffer with bounds checking:
// writes and reads will never go beyond the buffer limits.
class ESAT_Buffer: public Printable, public Stream
{
  public:
    // Instantiate an empty buffer.
    // An empty buffer will fail on reads and writes.
    ESAT_Buffer();

    // Instantiate a buffer backed by a byte array of given length.
    ESAT_Buffer(byte array[], unsigned long length);

    // Return the number of unread bytes available in the buffer,
    // truncated to an int.
    int available();

    // Return the number of unread bytes available in the buffer.
    unsigned long availableBytes() const;

    // Return the capacity of the buffer.
    unsigned long capacity() const;

    // Clear the contents of the buffer.
    // Set the read/write position to the start of the buffer.
    // Set the number of available unread bytes to 0.
    void flush();

    // Return the number of bytes stored in the buffer.
    unsigned long length() const;

    // Return the next byte (or -1 if no byte could be read)
    // without advancing to the next one.
    int peek();

    // Return the read/write position.
    unsigned long position() const;

    // Print the contents of the buffer in human readable form to an
    // output stream.
    size_t printTo(Print& output) const;

    // Return the next byte (or -1 if no byte could be read).
    // Advance the read/write position by 1, bounded by the capacity.
    int read();

    // Read a number of bytes from an input stream and fill the
    // buffer from the start with them.
    // Return true on success; otherwise return false.
    // The read/write position will be at the start of the buffer.
    boolean readFrom(Stream& input, unsigned long bytesToRead);

    // Set the read/write position to the start of the buffer.
    void rewind();

    // Return true if the last read() or peek() attempt was beyond
    // the length() of the buffer; otherwise return false.
    boolean triedToReadBeyondLength() const;

    // Write a byte if the read/write position hasn't reached the
    // capacity.
    // Return the actual number of bytes written.
    // Advance the read/write position by 1, bounded by the capacity.
    size_t write(uint8_t datum);

    // Import size_t Print::write(const uint8_t* buffer, size_t bufferLength).
    // Write a byte buffer of given length.
    // Return the number of bytes written.
    using Print::write;

    // Write the contents of the buffer to an output stream.
    // Return true on success; otherwise return false.
    boolean writeTo(Stream& output) const;

  private:
    // Backend buffer.
    byte* buffer;

    // Capacity of the backend buffer.
    unsigned long bufferCapacity;

    // Number of bytes written to the buffer.
    unsigned long bytesInBuffer;

    // Position of the next read or write operation.
    unsigned long readWritePosition;

    // Set to true if the last read() or peek() attempt was beyond
    // the length() of the buffer; otherwise set to false.
    boolean triedToReadBeyondBufferLength;
};

#endif /* ESAT_Buffer_h */

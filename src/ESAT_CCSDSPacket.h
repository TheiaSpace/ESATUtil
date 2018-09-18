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

#ifndef ESAT_CCSDSPacket_h
#define ESAT_CCSDSPacket_h

#include <Arduino.h>
#include <ESAT_Buffer.h>
#include <ESAT_CCSDSPrimaryHeader.h>
#include <ESAT_CCSDSSecondaryHeader.h>
#include <ESAT_Timestamp.h>

// ESAT's CCSDS space packets.
// These are simple packets following CCSDS Recommendation 133.0-B-1:
// Space Packet Protocol.  They contain a 6-byte primary header
// followed by a series of octets (at least 1) with the packet data.
// Multi-byte data are stored in big-endian order: the first byte is
// the most significant, the last byte is the least significant.
// Signed integers are stored in two's complement format.
// Floating-point numbers are stored in IEEE 754 format.
// The packet data can be read and written as a Stream.
class ESAT_CCSDSPacket: public Printable, public Stream
{
  public:
    // Instantiate a CCSDS packet with no packet data field.
    // The packet is invalid and it shouldn't be used in this state.
    ESAT_CCSDSPacket();

    // Instantiate a new packet backed with the packet data field
    // (packet payload) given by the given buffer.
    // The buffer must be at least 1 byte long.
    // The packet data buffer length will have the value of passed
    // buffer length, which must match the actual buffer length.
    // The primary header starts with all fields set to 0.
    // The read/write pointer starts at 0.
    ESAT_CCSDSPacket(byte buffer[], unsigned long bufferLength);

    // Return the number of unread bytes in the packet data (the
    // packet data length minus the position of the read pointer).
    int available();

    // Return the number of unread bytes in the packet data (the
    // packet data length minus the position of the read pointer).
    unsigned long availableBytesToRead() const;

    // Return the capacity in bytes of the packet data buffer.
    unsigned long capacity() const;

    // Copy the whole packet contents to a target packet.
    // The copy will fail if the target packet data buffer is too small.
    // Return true on successful copy; otherwise return false.
    boolean copyTo(ESAT_CCSDSPacket& target);

    // Clear the packet.
    // Set all bytes of the primary header to 0.
    // Set the read/write position to 0.
    // Set the packet data length to 0.
    void flush();

    // Return the total length of the packet, which is the sum
    // of the primary header length and the packet data length.
    unsigned long length() const;

    // Return the next 8-bit unsigned integer from the packet data
    // or, if the read/write pointer is at the end of the packet data,
    // return -1.
    int peek();

    // Print the packet in human-readable (JSON) form.
    // This leaves the read/write pointer untouched.
    size_t printTo(Print& output) const;

    // Return the next 8-bit unsigned integer from the packet data
    // or, if the read/write pointer is at the end of the packet data,
    // return -1.
    // This advances the read/write pointer by 1, but limited
    // to the packet data buffer length.
    int read();

    // Return the next 8-bit unsigned integer from the packet data.
    // The raw datum is stored as an 8-bit binary-coded-decimal number,
    // most significant bit first.
    // This advances the read/write pointer by 1, but limited
    // to the packet data buffer length.
    // The return value is undefined if there are fewer than 1 byte
    // before reaching the end of the packet data buffer.
    byte readBinaryCodedDecimalByte();

    // Return the next 16-bit unsigned integer from the packet data.
    // The raw datum is stored as an 16-bit binary-coded-decimal number,
    // most significant bit first.
    // This advances the read/write pointer by 2, but limited
    // to the packet data buffer length.
    // The return value is undefined if there are fewer than 2 bytes
    // before reaching the end of the packet data buffer.
    word readBinaryCodedDecimalWord();

    // Return the next boolean (an 8-bit entry) from the packet data.
    // The raw datum is stored as a 0 for false and as any other
    // 8-bit number for true.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 1 bytes
    // before reaching the end of the packet data buffer.
    boolean readBoolean();

    // Return the next 8-bit unsigned integer from the packet data.
    // This advances the read/write pointer by 1, but limited
    // to the packet data buffer length.
    // The return value is undefined if there are fewer than 1 byte
    // before reaching the end of the packet data buffer.
    byte readByte();

    // Return the next 8-bit signed integer from the packet data.
    // The raw datum is stored in two's complement format.
    // This advances the read/write pointer by 1, but limited
    // to the packet data buffer length.
    // The return value is undefined if there are fewer than 1 byte
    // before reaching the end of the packet data buffer.
    signed char readChar();

    // Return the next single-precision floating-point number
    // from the packet data.
    // The raw datum is stored in big-endian byte order, IEEE 754
    // format, single-precision (32-bit, binary32).
    // Denormal numbers are not properly handled.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 4 bytes
    // before reaching the end of the packet data buffer.
    float readFloat();

    // Fill the packet with incoming data from an input stream.
    // Return true on success; false otherwise.
    // The read/write pointer goes to the start of the packet data field.
    boolean readFrom(Stream& input);

    // Return the next 16-bit signed integer from the packet data.
    // The raw datum is stored in big-endian byte order,
    // two's complement format.
    // This advances the read/write pointer by 2, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 2 bytes
    // before reaching the end of the packet data buffer.
    int readInt();

    // Return the next 32-bit signed integer from the packet data.
    // The raw datum is stored in bin-endian byte order,
    // two's complement format.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 4 bytes
    // before reaching the end of the packet data buffer.
    long readLong();

    // Return the primary header of the packet.
    // The primary header is sent as 3 16-bit words.
    // This leaves the read/write pointer untouched.
    ESAT_CCSDSPrimaryHeader readPrimaryHeader() const;

    // Return the next secondary header from the packet data.
    // The raw datum is stored in big-endian byte order, with the
    // timestamp field encoded as a calendar segmented time code,
    // month of year/day of month variation, 1 second resolution.
    // The secondary header is intended to go right at the beginning
    // of the packet data, but it is possible to read a secondary
    // header from any point of the packet data if that's the user's need.
    // This advances the read/write pointer by 12, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 12 bytes before
    // reaching the end of the packet data buffer.
    ESAT_CCSDSSecondaryHeader readSecondaryHeader();

    // Return the next timestamp from the packet data.
    // The raw datum is stored in big-endian byte order, encoded as a
    // calendar segmented time code, month of year/day of month
    // variation, 1 second resolution.
    // This advances the read/write pointer by 7, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 7 bytes before
    // reaching the end of the packet data buffer.
    ESAT_Timestamp readTimestamp();

    // Return the next 32-bit unsigned integer from the packet data.
    // The raw datum is stored in big-endian byte order.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 4 bytes
    // before reaching the end of the packet data buffer.
    unsigned long readUnsignedLong();

    // Return the next 16-bit unsigned integer from the packet data.
    // The raw datum is stored in big-endian byte order.
    // This advances the read/write pointer by 2, but limited to the
    // packet data buffer length.
    // The return value is undefined if there are fewer than 2 bytes
    // before reaching the end of the packet data buffer.
    word readWord();

    // Move the read/write pointer to 0: back to the start of the
    // packet data field (packet payload).
    void rewind();

    // Return true if the last read*() or peek() attempt was beyond
    // the length of the packet data field; otherwise return false.
    boolean triedToReadBeyondLength() const;

    // Return true if the last write*() attempt was beyond the capacity
    // of the packet data field; otherwise return false.
    boolean triedToWriteBeyondCapacity() const;

    // Append an 8-bit unsigned integer to the packet data.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // Don't append anything if there are fewer than 1 bytes before
    // reaching the end of the packet data.
    // Return the number of bytes written.
    size_t write(uint8_t datum);

    // Import size_t Print::write(const uint8_t* buffer, size_t bufferLength).
    // Append the contents of a byte buffer of given length into the packet
    // data buffer.
    // This advances the read/write pointer by bufferLength, but limited
    // to the packet data buffer length.
    // Don't append data beyond the end of the packet data buffer.
    // Return the number of bytes written.
    using Print::write;

    // Append an 8-bit unsigned integer to the packet data.
    // The raw datum is stored in binary-coded-decimal format, most
    // significant bit first.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 1 byte before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeBinaryCodedDecimalByte(byte datum);

    // Append a 16-bit unsigned integer to the packet data.
    // The raw datum is stored in binary-coded-decimal format, most
    // significant bit first.
    // This advances the read/write pointer by 2, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 2 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeBinaryCodedDecimalWord(word datum);

    // Append a boolean to the packet data.
    // The raw datum is stored as an 8-bit unsigned integer:
    // 0 for false, 1 for true.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 1 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeBoolean(boolean datum);

    // Append an 8-bit unsigned integer to the packet data.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 1 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeByte(byte datum);

    // Append an 8-bit signed integer to the packet data.
    // This advances the read/write pointer by 1, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 1 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeChar(signed char datum);

    // Append a floating-point number to the packet data.
    // The raw datum is stored in big-endian byte order, IEEE 754
    // format, single-precision (32-bit, binary32).
    // Denormal numbers are not properly handled.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 4 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeFloat(float datum);

    // Append a 16-bit signed integer to the packet data.
    // The raw datum is stored in big-endian byte order,
    // two's complement format.
    // This advances the read/write pointer by 2, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 2 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeInt(int datum);

    // Append a 32-bit signed integer to the packet data.
    // The raw datum is stored in big-endian byte order,
    // two's complement format.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 4 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeLong(long datum);

    // Append the secondary header to the packet data.
    // The raw datum is stored in big-endian byte order, with the
    // timestamp field encoded as a calendar segmented time code,
    // month of year/day of month variation, 1 second resolution.
    // The secondary header is intended to go right at the beginning
    // of the packet data, but it is possible to append a secondary
    // header at any point of the packet data if that's the user's need.
    // This advances the read/write pointer by 12, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 12 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeSecondaryHeader(ESAT_CCSDSSecondaryHeader datum);

    // Write the primary header of the packet.
    // The primary header is sent as 3 16-bit words.
    // This leaves the read/write pointer untouched.
    void writePrimaryHeader(ESAT_CCSDSPrimaryHeader primaryHeader);

    // Rewind and then write the primary header and secondary header
    // of a telecommand packet with secondary header present and
    // unsegmented user data.
    // Use the provided application process identifier, packet
    // sequence count, timestamp, major version number, minor version
    // number, patch version number and packet identifier for the
    // headers.
    // This advances the read/write pointer just past the secondary
    // header, but limited to the packet data buffer length.
    // The written value is undefined if there are fewer than 12 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeTelecommandHeaders(word applicationProcessIdentifier,
                                 word packetSequenceCount,
                                 ESAT_Timestamp timestamp,
                                 byte majorVersionNumber,
                                 byte minorVersionNumber,
                                 byte patchVersionNumber,
                                 byte packetIdentifier);

    // Rewind and then write the primary header and secondary header
    // of a telemetry packet with secondary header present and
    // unsegmented user data.
    // Use the provided application process identifier, packet
    // sequence count, timestamp, major version number, minor version
    // number, patch version number and packet identifier for the
    // headers.
    // This moves the read/write pointer to 12, just after the
    // secondary header, but limited to the packet data buffer length.
    // The written value is undefined if there are fewer than 12 bytes
    // before reaching the end of the packet data, but no data will be
    // written beyond the packet data buffer.
    void writeTelemetryHeaders(word applicationProcessIdentifier,
                               word packetSequenceCount,
                               ESAT_Timestamp timestamp,
                               byte majorVersionNumber,
                               byte minorVersionNumber,
                               byte patchVersionNumber,
                               byte packetIdentifier);

    // Append a timestamp to the packet data.
    // The raw datum is stored in big-endian byte order, encoded as a
    // calendar segmented time code, month of year/day of month
    // variation, 1 second resolution.
    // This advances the read/write pointer by 7, but limited to the
    // packet data buffer length.
    // The written data is undefined if there are fewer than 7 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeTimestamp(ESAT_Timestamp datum);

    // Write the raw contents of the packet to an output stream.
    // Return true on success; otherwise return false.
    // The operation will fail on write errors, but also
    // when the packet data buffer is smaller than the packet
    // data length.
    // This leaves the read/write pointer untouched.
    boolean writeTo(Stream& output) const;

    // Append a 32-bit unsigned integer to the packet data.
    // The raw datum is stored in big-endian byte order.
    // This advances the read/write pointer by 4, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 4 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeUnsignedLong(unsigned long datum);

    // Append a 16-bit unsigned integer to the packet data.
    // The raw datum is stored in big-endian byte order.
    // This advances the read/write pointer by 2, but limited to the
    // packet data buffer length.
    // The written value is undefined if there are fewer than 2 bytes before
    // reaching the end of the packet data, but no data will be written beyond
    // the packet data buffer.
    void writeWord(word datum);

  private:
    // Buffer with the raw packet data field.
    ESAT_Buffer packetData;

    // Primary header field of the packet.
    ESAT_CCSDSPrimaryHeader primaryHeader;
};

#endif /* ESAT_CCSDSPacket_h */

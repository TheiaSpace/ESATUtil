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

#ifndef ESAT_CCSDSPacket_h
#define ESAT_CCSDSPacket_h

#include <Arduino.h>
#include <ESAT_Buffer.h>
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
    // Packet type: either telemetry or telecommand.
    // The packet type field is part of the primary header.
    enum PacketType
    {
      TELEMETRY = 0,
      TELECOMMAND = 1,
    };

    // Secondary header flag.
    // The secondary header may either be present of not.
    enum SecondaryHeaderFlag
    {
      SECONDARY_HEADER_IS_NOT_PRESENT = 0,
      SECONDARY_HEADER_IS_PRESENT = 1,
    };

    // Sequence flags.
    // User data may be either segmented or unsegmented.  For
    // segmented user data, the packet may contain the first segment,
    // a continuation segment, or the last segment.
    enum SequenceFlags
    {
      CONTINUATION_SEGMENT_OF_USER_DATA = B00,
      FIRST_SEGMENT_OF_USER_DATA = B01,
      LAST_SEGMENT_OF_USER_DATA = B10,
      UNSEGMENTED_USER_DATA = B11,
    };

    // Length of the primary header.
    static const byte PRIMARY_HEADER_LENGTH = 6;

    // Buffer with the raw primary header.
    byte primaryHeader[PRIMARY_HEADER_LENGTH];

    // Instantiate a CCSDS packet with no packet data field.
    // Useful for just generating a primary header.
    // The primary header starts with a raw value of all zeros.
    // The read/write pointer starts at 0.
    // The packet data buffer is null and must be assigned
    // together with the packet data buffer length before
    // using the packet data.
    // The packet data buffer length is set to 0 and must be
    // assigned together with the packet data buffer before using
    // the packet data.
    ESAT_CCSDSPacket();

    // Instantiate a new packet backed with the packet data field
    // (packet payload) given by the given buffer.
    // The buffer must be at least 1 byte long.
    // The packet data buffer length will have the value of passed
    // buffer length, which must match the actual buffer length.
    // The primary header starts with a raw value of all zeros.
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

    // Clear the packet by setting all bytes to 0.
    void clear();

    // Copy the whole packet contents to a target packet.
    // The copy will fail if the target packet data buffer is too small.
    // Return true on successful copy; otherwise return false.
    boolean copyTo(ESAT_CCSDSPacket& target);

    // Return true if the last read operation reached the end of the
    // packet.  Otherwise return false.
    boolean endOfPacketDataReached() const;

    // Update the packet data length to match the number of bytes written
    // to the packet data (the position of the next write operation).
    // The raw datum is stored as a 16-bit unsigned integer in
    // big-endian byte order, as the actual packet data length minus 1.
    // If the number of bytes written to the packet is 0, the packet
    // data length will be erroneous, but empty CCSDS packets
    // are forbidden by the standard.
    // This moves the read/write pointer back to the start of the
    // packet data.
    void flush();

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

    // Return the CCSDS application process identifier.
    // This field is part of the primary header.
    // There should be one application process identifier per logical
    // subsystem (e.g., the attitude determination and control
    // subsystem should have its own application process identifier).
    // The application process identifier is a 14-bit unsigned integer,
    // most significant bit first.
    word readApplicationProcessIdentifier() const;

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
    // This leaves the read/write pointer untouched.
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

    // Return the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    // It can go from 1 to 65536.
    // The raw datum is stored as a 16-bit unsigned integer
    // in big-endian byte order, as the actual packet data
    // length minus 1.
    // This leaves the read/write pointer untouched.
    long readPacketDataLength() const;

    // Return the CCSDS packet sequence count.
    // This field is part of the primary header.
    // There is a packet sequence count for every application process
    // and it should be incremented every time a new packet is generated.
    // The raw datum is stored as a 14-bit unsigned integer,
    // most significant bit first.
    // This leaves the read/write pointer untouched.
    word readPacketSequenceCount() const;

    // Return the CCSDS packet type: either telemetry or telecommand.
    // This field is part of the primary header.
    // The raw datum is stored as a bit:
    // 0 for TELEMETRY, 1 for TELECOMMAND.
    // This leaves the read/write pointer untouched.
    PacketType readPacketType() const;

    // Return the CCSDS packet version number.
    // This field is part of the primary header.
    // The raw datum is stored as a 3-bit unsigned integer, most
    // significant bit first.
    // This leaves the read/write pointer untouched.
    byte readPacketVersionNumber() const;

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

    // Return the CCSDS secondary header flag.
    // This field is part of the primary header.
    // The raw datum is stored as a bit:
    // 0 for SECONDARY_HEADER_IS_NOT_PRESENT,
    // 1 for SECONDARY_HEADER_IS_PRESENT.
    // This leaves the read/write pointer untouched.
    SecondaryHeaderFlag readSecondaryHeaderFlag() const;

    // Return the CCSDS sequence flags.
    // This field is part of the primary header.
    // The raw datum is stored as a 2-bit unsigned integer, most
    // significant bit first:
    // 0 for CONTINUATION_SEGMENT_OF_USER_DATA,
    // 1 for FIRST_SEGMENT_OF_USER_DATA,
    // 2 for LAST_SEGMENT_OF_USER_DATA,
    // 3 for UNSEGMENTED_USER_DATA.
    // This leaves the read/write pointer untouched.
    SequenceFlags readSequenceFlags() const;

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

    // Update the packet data length to match the number of bytes written
    // to the packet data (the position of the next write operation).
    // The raw datum is stored as a 16-bit unsigned integer in
    // big-endian byte order, as the actual packet data length minus 1.
    // If the number of bytes written to the packet is 0, the packet
    // data length will be erroneous, but empty CCSDS packets
    // are forbidden by the standard.
    // This leaves the read/write pointer untouched.
    void updatePacketDataLength();

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

    // Write the CCSDS application process identifier.
    // This field is part of the primary header.
    // There should be one application process identifier per logical
    // subsystem (e.g., the attitude determination and control
    // subsystem should have its own application process identifier).
    // The raw datum is stored as an 11-bit unsigned integer,
    // most significant bit first.
    // This leaves the read/write pointer untouched.
    void writeApplicationProcessIdentifier(word applicationProcessIdentifier);

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

    // Write the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    // It can go from 1 to 65536 (both included).
    // The raw datum is stored as a 16-bit unsigned integer
    // in big-endian byte order, as the actual packet data
    // length minus 1.  As valid packet data lengths go from
    // 1 to 65536 (both included), the behaviour when given
    // an argument outside of that interval is undefined.
    // This leaves the read/write pointer untouched.
    void writePacketDataLength(long packetDataLength);

    // Read the CCSDS packet sequence count.
    // This field is part of the primary header.
    // There is a packet sequence count for every application process
    // and it should be incremented every time a new packet is generated.
    // The raw datum is stored as a 14-bit unsigned integer,
    // most significant bit first.
    // This leaves the read/write pointer untouched.
    void writePacketSequenceCount(word packetSequenceCount);

    // Write the CCSDS packet type: wether telemetry or telecommand.
    // This field is part of the primary header.
    // The raw datum is stored as a bit:
    // 0 for TELEMETRY, 1 for TELECOMMAND.
    // The behaviour with arguments other than TELEMETRY or TELECOMMAND
    // is undefined.
    // This leaves the read/write pointer untouched.
    void writePacketType(PacketType packetType);

    // Write the CCSDS packet version number.
    // This field is part of the primary header.
    // Should be 0.
    // The packet version number can go from 0 to 7 (both included).
    // The raw datum is stored as a 3-bit unsigned integer, most
    // significant bit first.
    // As valid packet version numbers go from 0 to 7 (both included),
    // the behaviour when given an argument outside of that interval
    // is undefined.
    // This leaves the read/write pointer untouched.
    void writePacketVersionNumber(byte packetVersionNumber);

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

    // Write the CCSDS secondary header flag.
    // This field is part of the primary header.
    // The raw datum is stored as a bit:
    // 0 for SECONDARY_HEADER_IS_NOT_PRESENT,
    // 1 for SECONDARY_HEADER_IS_PRESENT.
    // The behaviour with arguments other than SECONDARY_HEADER_IS_NOT_PRESENT
    // or SECONDARY_HEADER_IS_PRESENT is undefined.
    // This leaves the read/write pointer untouched.
    void writeSecondaryHeaderFlag(SecondaryHeaderFlag secondaryHeaderFlag);

    // Write the CCSDS sequence flags.
    // This field is part of the primary header.
    // The raw datum is stored as a 2-bit unsigned integer,
    // most significant bit first:
    // 0 for CONTINUATION_SEGMENT_OF_USER_DATA,
    // 1 for FIRST_SEGMENT_OF_USER_DATA,
    // 2 for LAST_SEGMENT_OF_USER_DATA,
    // 3 for UNSEGMENTED_USER_DATA.
    // The behaviour with arguments other than
    // CONTINUATION_SEGMENT_OF_USER_DATA, FIRST_SEGMENT_OF_USER_DATA,
    // LAST_SEGMENT_OF_USER_DATA or UNSEGMENTED_USER_DATA is undefined.
    // This leaves the read/write pointer untouched.
    void writeSequenceFlags(SequenceFlags sequenceFlags);

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
    boolean writeTo(Stream& output);

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
    // Structure of the primary header.
    // The following offsets and lengths are expressed in bits:
    static const byte PACKET_VERSION_NUMBER_OFFSET = 0;
    static const byte PACKET_VERSION_NUMBER_LENGTH = 3;
    static const byte PACKET_TYPE_OFFSET =
      PACKET_VERSION_NUMBER_OFFSET + PACKET_VERSION_NUMBER_LENGTH;
    static const byte PACKET_TYPE_LENGTH = 1;
    static const byte SECONDARY_HEADER_FLAG_OFFSET =
      PACKET_TYPE_OFFSET + PACKET_TYPE_LENGTH;
    static const byte SECONDARY_HEADER_FLAG_LENGTH = 1;
    static const byte APPLICATION_PROCESS_IDENTIFIER_OFFSET =
      SECONDARY_HEADER_FLAG_OFFSET + SECONDARY_HEADER_FLAG_LENGTH;
    static const byte APPLICATION_PROCESS_IDENTIFIER_LENGTH = 11;
    static const byte SEQUENCE_FLAGS_OFFSET =
      APPLICATION_PROCESS_IDENTIFIER_OFFSET
      + APPLICATION_PROCESS_IDENTIFIER_LENGTH;
    static const byte SEQUENCE_FLAGS_LENGTH = 2;
    static const byte PACKET_SEQUENCE_COUNT_OFFSET =
      SEQUENCE_FLAGS_OFFSET + SEQUENCE_FLAGS_LENGTH;
    static const byte PACKET_SEQUENCE_COUNT_LENGTH = 14;
    // The following offset is expressed in bytes:
    static const byte PACKET_DATA_LENGTH_OFFSET = 4;

    // Buffer with the raw packet data field.
    ESAT_Buffer packetData;

    // Return the bits of a floating-point number packed into a 32-bit
    // integer.
    unsigned long floatToLong(float number);

    // Return the floating point number encoded in the bits of a
    // 32-bit integer.
    float longToFloat(unsigned long bits);

    // Read up to 16 bits at a given offset from the primary header.
    word readPrimaryHeaderBits(byte offset, byte length) const;

    // Write up to 16 bits at a given bit offset on the primary header.
    void writePrimaryHeaderBits(byte offset, byte length, word bits);
};

#endif /* ESAT_CCSDSPacket_h */

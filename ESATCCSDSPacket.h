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

#ifndef ESATCCSDSPacket_h
#define ESATCCSDSPacket_h

#include <Arduino.h>

// ESAT's CCSDS space packets.
// These are simple packets following CCSDS Recommendation 133.0-B-1:
// Space Packet Protocol.  They contain a 6-byte primary header
// followed by a series of octets (at least 1) with the packet data.
class ESATCCSDSPacket
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

    // Buffer with the raw packet data field.
    byte* packetData;

    // Packet data buffer length in bytes.
    word packetDataBufferLength;

    // Buffer with the raw primary header.
    byte primaryHeader[PRIMARY_HEADER_LENGTH];

    // Instantiate a CCSDS packet with no packet data field.
    // Useful for just generating a primary header.
    ESATCCSDSPacket();

    // Instantiate a new packet backed with the packet data field
    // (packet payload) by the given buffer.
    // The buffer must be at least 1 byte long.
    ESATCCSDSPacket(byte buffer[], word bufferLength);

    // Clear the packet by setting all bytes to 0.
    void clear();

    // Copy the whole packet contents to a target packet.
    // The copy will fail if the target packet data buffer is too small.
    // Return true on successful copy; otherwise return false.
    boolean copyTo(ESATCCSDSPacket& target);

    // Return true if the last read operation reached the end of the
    // packet.  Otherwise return false.
    boolean endOfPacketDataReached();

    // Read the CCSDS application process identifier.
    // This field is part of the primary header.
    // There should be one application process identifier per logical
    // subsystem (e.g., the attitude determination and control
    // subsystem should have its own application process identifier).
    word readApplicationProcessIdentifier();

    // Read the next boolean (an 8-bit entry) from the packet data.
    boolean readBoolean();

    // Read the next 8-bit integer from the packet data.
    byte readByte();

    // Read the next IEEE 754 single-precision floating-point number
    // from the packet data.
    float readFloat();

    // Read the next 32-bit integer from the packet data.
    unsigned long readLong();

    // Return the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    // It can go from 1 to 65536 for valid packets.
    // If it reads as 0, the packet is empty, and thus malformed.
    // The packet data length is stored as its value - 1
    // in the primary header, but this method returns the actual
    // value, not the stored one.
    long readPacketDataLength();

    // Read the CCSDS packet sequence count.
    // This field is part of the primary header.
    word readPacketSequenceCount();

    // Read the CCSDS packet type: either telemetry or telecommand.
    // This field is part of the primary header.
    PacketType readPacketType();

    // Return the CCSDS packet version number.
    // This field is part of the primary header.
    // There is a packet sequence count for every application process
    // and it should be incremented every time a new packet is generated.
    byte readPacketVersionNumber();

    // Read the CCSDS secondary header flag.
    // This field is part of the primary header.
    SecondaryHeaderFlag readSecondaryHeaderFlag();

    // Read the CCSDS sequence flags.
    // This field is part of the primary header.
    SequenceFlags readSequenceFlags();

    // Read the next 16-bit integer from the packet data.
    word readWord();

    // Move the read pointer back to the start of the packet data
    // field (packet payload).
    void rewind();

    // Write the CCSDS application process identifier.
    // This field is part of the primary header.
    // There should be one application process identifier per logical
    // subsystem (e.g., the attitude determination and control
    // subsystem should have its own application process identifier).
    void writeApplicationProcessIdentifier(word applicationProcessIdentifier);

    // Append a boolean to the packet data.
    // This increments the packet data length by 1.
    void writeBoolean(boolean datum);

    // Append an 8-bit integer to the packet data.
    // This increments the packet data length by 1.
    void writeByte(byte datum);

    // Append an IEEE 754 single-precision floating-point number to
    // the packet data.
    // This increments the packet data length by 4.
    void writeFloat(float datum);

    // Append a 32-bit integer to the packet data.
    // This increments the packet data length by 4.
    void writeLong(unsigned long datum);

    // Write the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    // It can go from 1 to 65536 for valid packets.
    // If it is out of that range, the packet is malformed.
    // The packet data length is stored as its value - 1
    // in the primary header, but this method accepts the actual
    // value, not the stored one, as an argument.
    void writePacketDataLength(long packetDataLength);

    // Read the CCSDS packet sequence count.
    // This field is part of the primary header.
    // There is a packet sequence count for every application process
    // and it should be incremented every time a new packet is generated.
    void writePacketSequenceCount(word packetSequenceCount);

    // Write the CCSDS packet type: wether telemetry or telecommand.
    // This field is part of the primary header.
    void writePacketType(PacketType packetType);

    // Write the CCSDS packet version number.
    // This field is part of the primary header.
    // Should be 0.
    void writePacketVersionNumber(byte packetVersionNumber);

    // Write the CCSDS secondary header flag.
    // This field is part of the primary header.
    void writeSecondaryHeaderFlag(SecondaryHeaderFlag secondaryHeaderFlag);

    // Write the CCSDS sequence flags.
    // This field is part of the primary header.
    void writeSequenceFlags(SequenceFlags sequenceFlags);

    // Append a 16-bit integer to the packet data.
    // This increments the packet data length by 2.
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

    // True if the packet has a non-emtpy packet data field;
    // false otherwise.
    boolean hasData;

    // Position of the next read operation.
    word readPosition;

    // Return the bits of a floating-point number packed into a 32-bit
    // integer.
    unsigned long floatToLong(float number);

    // Return the floating point number encoded in the bits of a
    // 32-bit integer.
    float longToFloat(unsigned long bits);

    // Read up to 16 bits at a given offset from the primary header.
    word readPrimaryHeaderBits(byte offset, byte length);

    // Write up to 16 bits at a given bit offset on the primary header.
    void writePrimaryHeaderBits(byte offset, byte length, word bits);
};

#endif

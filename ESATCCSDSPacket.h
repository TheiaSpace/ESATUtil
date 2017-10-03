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

    // Buffer with the raw packet data.
    byte* const buffer;

    // Buffer length in bytes.
    const word bufferLength;

    // Instantiate a new packet backed by the given buffer.
    // The buffer must be at least 6 bytes long plus the amount
    // of space necessary for storing the packet data field.
    ESATCCSDSPacket(byte buffer[], word bufferLength);

    // Clear the packet by setting all buffer bytes to 0.
    void clear();

    // Read the CCSDS application process identifier.
    // This field is part of the primary header.
    // There should be one application process identifier per logical
    // subsystem (e.g., the attitude determination and control
    // subsystem should have its own application process identifier).
    word readApplicationProcessIdentifier();

    // Read the next 8-bit integer from the packet payload.
    byte readByte();

    // Return the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    word readPacketDataLength();

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

    // Read the next 16-bit integer from the packet payload.
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

    // Append an 8-bit integer to the packet buffer.
    // This increments the packet data length by 1.
    void writeByte(byte datum);

    // Write the packet data length (expressed in octets).
    // This field is part of the primary header.
    // The packet data length includes the secondary header
    // and the user data/packet payload.
    void writePacketDataLength(word packetDataLength);

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

    // Append a 16-bit integer to the packet buffer.
    // This increments the packet data length by 2.
    void writeWord(word datum);

  private:
    // Structure of the primary header.
    // The following offsets and lengths are expressed in bits:
    static const byte packetVersionNumberOffset = 0;
    static const byte packetVersionNumberLength = 3;
    static const byte packetTypeOffset =
      packetVersionNumberOffset + packetVersionNumberLength;
    static const byte packetTypeLength = 1;
    static const byte secondaryHeaderFlagOffset =
      packetTypeOffset + packetTypeLength;
    static const byte secondaryHeaderFlagLength = 1;
    static const byte applicationProcessIdentifierOffset =
      secondaryHeaderFlagOffset + secondaryHeaderFlagLength;
    static const byte applicationProcessIdentifierLength = 11;
    static const byte sequenceFlagsOffset =
      applicationProcessIdentifierOffset
      + applicationProcessIdentifierLength;
    static const byte sequenceFlagsLength = 2;
    static const byte packetSequenceCountOffset =
      sequenceFlagsOffset + sequenceFlagsLength;
    static const byte packetSequenceCountLength = 14;
    // The following offset is expressed in bytes:
    static const byte packetDataLengthOffset = 4;

    // Position of the next read operation.
    unsigned long readPosition;

    // Read up to 16 bits at a given offset.
    word readBits(byte offset, byte length);

    // Write up to 16 bits at a given bit offset.
    void writeBits(byte offset, byte length, word bits);
};

#endif

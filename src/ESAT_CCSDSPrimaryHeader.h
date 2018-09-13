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

#ifndef ESAT_CCSDSPrimaryHeader_h
#define ESAT_CCSDSPrimaryHeader_h

#include <Arduino.h>

// Primary header used for ESAT's packets.  It contains the following fields:
// - The version number of the CCSDS Space Packet (3 bits).
// - The packet type (1 bit).
// - The secondary header flag (1 bit).
// - The application process identifier (11 bits).
// - The sequence flags (2 bits).
// - The packet sequence count (14 bits).
// - The packet data length (16 bits).
class ESAT_CCSDSPrimaryHeader: public Printable
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

    // Number of bytes the primary header takes when stored in CCSDS
    // packets.
    static const byte LENGTH = 6;

    // Packet version number.
    // The packet version number is sent as a 3-bit unsigned integer,
    // most significant bit first, on the first 16-bit word of the
    // header.  It should be 0.
    byte packetVersionNumber;

    // Packet type.
    // The packet type is sent as 1 bit on the first 16-bit word of
    // the header:
    // 0 for TELEMETRY,
    // 1 for TELECOMMAND.
    PacketType packetType;

    // Secondary header flag.
    // The secondary header flag is sent as 1 bit on the first 16-bit
    // word of the header:
    // 0 for SECONDARY_HEADER_IS_NOT_PRESENT,
    // 1 for SECONDARY_HEADER_IS_PRESENT.
    SecondaryHeaderFlag secondaryHeaderFlag;

    // Application process identifier.
    // Each logical subsystem should have its own unique application
    // process identifier (e.g., the attitude determination and
    // control subsystem has its own application process identifier).
    // The application process identifier is sent as an 11-bit
    // unsigned integer, most significant bit first, on the first
    // 16-bit word of the header.
    word applicationProcessIdentifier;

    // Packet sequence flags.
    // The packet sequence flags are sent as a 2-bit unsigned
    // integer, most significant bit first, on the second 16-word of
    // the header:
    // 0 for CONTINUATION_SEGMENT_OF_USER_DATA,
    // 1 for FIRST_SEGMENT_OF_USER_DATA,
    // 2 for LAST_SEGMENT_OF_USER_DATA,
    // 3 for UNSEGMENTED_USER_DATA.
    SequenceFlags sequenceFlags;

    // Packet sequence count.
    // Each application process keeps its own packet sequence count,
    // which is incremented every time it generates a new packet.
    // The packet sequence count is sent as a 14-bit unsigned
    // integer number, most significant bit first, on the second
    // 16-bit word of the header.
    word packetSequenceCount;

    // Packet data length.  Valid values go from 1 to 65536.
    // The packet data length is sent as its actual value,
    // minus 1, most significant bit first, on the third 16-bit
    // word of the header.
    // The on-wire representation of the primary header when the packet
    // data length has an invalid value is undefined.
    unsigned long packetDataLength;

    // Print the primary header in human readable (JSON) form.
    // Return the number of characters written.
    size_t printTo(Print& output) const;

    // Read the primary header from an input stream.
    // Return true on success; otherwise return false.
    boolean readFrom(Stream& input);

    // Write the primary header to an output stream.
    // Return true on success; otherwise return false.
    boolean writeTo(Stream& output) const;

  private:
    // Bit masks of the fields of the first 16-bit word
    // of the header.
    static const word PACKET_VERSION_NUMBER_MASK =
      (B11100000 << 8) | B00000000;
    static const word PACKET_TYPE_MASK =
      (B00010000 << 8) | B00000000;
    static const word SECONDARY_HEADER_FLAG_MASK =
      (B00001000 << 8) | B00000000;
    static const word APPLICATION_PROCESS_IDENTIFIER_MASK =
      (B00000111 << 8) | B11111111;

    // Bit masks of the fields of the second 16-bit word
    // of the header.
    static const word SEQUENCE_FLAGS_MASK =
      (B11000000 << 8) | B00000000;
    static const word PACKET_SEQUENCE_COUNT_MASK =
      (B00111111 << 8) | B11111111;

    // Bit masks of the fields of the third 16-bit word
    // of the header.
    static const word PACKET_DATA_LENGTH_MASK =
      (B11111111 << 8) | B11111111;

    // Bit offsets (starting from the least-significant bit)
    // of the fields of the first 16-bit word of the header.
    static const byte PACKET_VERSION_NUMBER_OFFSET = 13;
    static const byte PACKET_TYPE_OFFSET = 12;
    static const byte SECONDARY_HEADER_FLAG_OFFSET = 11;
    static const byte APPLICATION_PROCESS_IDENTIFIER_OFFSET = 0;

    // Bit offsets (starting from the least-significant bit)
    // of the fields of the second 16-bit word of the header.
    static const byte SEQUENCE_FLAGS_OFFSET = 14;
    static const byte PACKET_SEQUENCE_COUNT_OFFSET = 0;

    // Bit offsets (starting from the least-significant bit)
    // of the fields of the third 16-bit word of the header.
    static const byte PACKET_DATA_LENGTH_OFFSET = 0;

    // The packet data length is sent as its actual value minus this
    // value.
    static const byte PACKET_DATA_LENGTH_INCREMENT = 1;
};

#endif /* ESAT_CCSDSPrimaryHeader_h */

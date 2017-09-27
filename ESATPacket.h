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

#ifndef ESATPacket_h
#define ESATPacket_h

#include <Arduino.h>

// Basic ESAT packets.
// These are simple packets backed by a user-provided byte buffer.
// The packet consists of a header followed by the payload.
// The header contains 6 bytes:
//   - subsystem identifier;
//   - packet identifier;
//   - major version number;
//   - minor version number;
//   - patch version number;
//   - payload length.
// Multi-byte words are stored in big-endian byte order.
class ESATPacket
{
  public:
    // Length of the packet header:
    // 1 byte for the subsystem identifier,
    // 1 byte for the packet identifier,
    // 1 byte for the major version number,
    // 1 byte for the minor version number,
    // 1 byte for the patch version number,
    // 1 byte for the payload length.
    static const byte headerLength = 6;

    // Instantiate a new packet backed by the given buffer.
    // The buffer should be at least 6 bytes long
    // for the packet header plus the space needed
    // for the payload.
    ESATPacket(byte buffer[]);

    // Clear the packet by setting the payload length to 0.
    void clear();

    // Read the next 8-bit integer from the packet payload.
    byte readByte();

    // Read the major version number of the packet.
    byte readMajorVersionNumber();

    // Read the minor version number of the packet.
    byte readMinorVersionNumber();

    // Read the patch version number of the packet.
    byte readPatchVersionNumber();

    // Read the packet identifier of the packet.
    byte readPacketIdentifier();

    // Return the total length in bytes of the packet.
    byte readPacketLength();

    // Return the length in bytes of the packet payload.
    byte readPayloadLength();

    // Read the subsystem identifier of the packet.
    byte readSubsystemIdentifier();

    // Read the next 16-bit integer from the packet payload.
    word readWord();

    // Append an 8-bit integer to the packet buffer.
    void writeByte(byte datum);

    // Set the major version number of the packet.
    void writeMajorVersionNumber(byte majorVersionNumber);

    // Set the minor version number of the packet.
    void writeMinorVersionNumber(byte minorVersionNumber);

    // Set the packet identifier of the packet.
    void writePacketIdentifier(byte packetIdentifier);

    // Set the patch version number of the packet.
    void writePatchVersionNumber(byte patchVersionNumber);

    // Set the subsystem identifier of the packet.
    void writeSubsystemIdentifier(byte subsystemIdentifier);

    // Append a 16-bit integer to the packet buffer.
    void writeWord(word datum);

  private:
    // Field offsets.
    static const byte subsystemIdentifierOffset = 0;
    static const byte packetIdentifierOffset = 1;
    static const byte majorVersionNumberOffset = 2;
    static const byte minorVersionNumberOffset = 3;
    static const byte patchVersionNumberOffset = 4;
    static const byte payloadLengthOffset = 5;
    static const byte payloadOffset = 6;

    // Buffer with the raw packet data.
    byte* const buffer;

    // Position of the next read operation.
    byte readPosition;
};

#endif

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

#ifndef ESAT_CCSDSPacketContentsFromBuffer_h
#define ESAT_CCSDSPacketContentsFromBuffer_h

#include <Arduino.h>
#include "ESAT_Buffer.h"
#include "ESAT_CCSDSPacketContents.h"

// Buffered packet contents object.
// Use a buffer to store packet contents.
// Pass ESAT_CCSDSPacketContentsFromBuffer objects to
// ESAT_CCSDSPacketBuilder.buildPacket() to build CCSDS packets
// with user data based on the contents of a buffer.
class ESAT_CCSDSPacketContentsFromBuffer
{
  public:
    // Instantiate an empty buffered packet contents object.
    ESAT_CCSDSPacketContentsFromBuffer();

    // Instantiate a new buffered packet contents object with contents
    // given by the provided array of given content length.
    ESAT_CCSDSPacketContentsFromBuffer(byte identifier,
                                       byte array[],
                                       unsigned long length);

    // Instantiate a new buffered packet contents object with contents
    // given by the provided buffer.
    ESAT_CCSDSPacketContentsFromBuffer(byte identifier,
                                       ESAT_Buffer& buffer);

    // Return true.
    boolean available();

    // Return the packet identifier.
    byte packetIdentifier();

    // Fill the user data field of the given packet.
    // The write pointer of the packet is already at the start
    // of the user data field.
    // Return true on success; otherwise return false.
    boolean fillUserData(ESAT_CCSDSPacket& packet);

  private:
    // Packet identifier.
    byte identifier;

    // Backend buffer.
    ESAT_Buffer buffer;
};

#endif /* ESAT_CCSDSPacketContentsFromBuffer_h */

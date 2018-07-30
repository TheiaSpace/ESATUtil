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

#ifndef ESAT_CCSDSPacketContents_h
#define ESAT_CCSDSPacketContents_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// Packet contents interface.
// Pass ESAT_CCSDSPacketContents to
// ESAT_CCSDSPacketBuilder.buildPacket() to build CCSDS packets.
class ESAT_CCSDSPacketContents
{
  public:
    // Next packet contents object in the list of packet contents objects.
    // ESAT_CCSDSPacketBuilder uses this to keep a linked list of
    // registered packet contents: it can traverse the list by going
    // from one packet contents object to the next packet contents
    // object until reaching the end of the list at nullptr.
    // Only ESAT_CCSDSPacketBuilder should care about this.
    ESAT_CCSDSPacketContents* nextPacketContents;

    virtual ~ESAT_CCSDSPacketContents() {};

    // Return true if a new packet is available:
    // periodic packets are available once every period, and event
    // packets are available once some event happens.
    virtual boolean available() = 0;

    // Return the packet identifier.
    virtual byte packetIdentifier() = 0;

    // Fill the user data field of the given packet.
    // The write pointer of the packet is already at the start
    // of the user data field.
    // Return true on success; otherwise return false.
    virtual boolean fillUserData(ESAT_CCSDSPacket& packet) = 0;
};

#endif /* ESAT_CCSDSPacketContents_h */

/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_CCSDSTelemetryPacketContents_h
#define ESAT_CCSDSTelemetryPacketContents_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// Packet contents interface.
// Use together with ESAT_CCSDSTelemetryPacketBuilder to build
// telemetry packets.
class ESAT_CCSDSTelemetryPacketContents
{
  public:
    // Next packet contents object in the list of packet contents objects.
    // ESAT_CCSDSTelemetryPacketBuilder uses this to keep a linked list of
    // registered packet contents: it can traverse the list by going
    // from one packet contents object to the next packet contents
    // object until reaching the end of the list at nullptr.
    // Only ESAT_CCSDSTelemetryPacketBuilder should care about this.
    // Each packet contents object should be added just one time to
    // just one ESAT_CCSDSTelemetryPacketBuilder object.
    ESAT_CCSDSTelemetryPacketContents* nextTelemetryPacketContents;

    // Trivial destructor.
    // We need to define it because the C++ programming language
    // works this way.
    virtual ~ESAT_CCSDSTelemetryPacketContents() {};

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

#endif /* ESAT_CCSDSTelemetryPacketContents_h */

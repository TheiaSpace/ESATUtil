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

#ifndef ESAT_CCSDSTelecommandPacketHandler_h
#define ESAT_CCSDSTelecommandPacketHandler_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_CCSDSPacketConsumer.h"
#include "ESAT_SemanticVersionNumber.h"

// Telecommand packet handler for ESAT's CCSDS space packets.
class ESAT_CCSDSTelecommandPacketHandler
{
  public:
    // Set up the telecommand packet handler for the given application
    // process identifier and semantic version numbers.
    // The telecommand handler will only handle a packet if the
    // following conditions are met:
    // - The packet is a telecommand packet.
    // - The packet has a secondary header.
    // - The packet's application process identifier is the same
    //   as the telecommand handler's application process identifier.
    // - The packet's semantic version number is backwards-compatible
    //   with the telecommand handler's semantic version number.
    ESAT_CCSDSTelecommandPacketHandler(word applicationProcessIdentifier,
                                       byte majorVersionNumber,
                                       byte minorVersionNumber,
                                       byte patchVersionNumber);

    // Add a new entry to the list of packet consumers.
    void add(ESAT_CCSDSPacketConsumer& consumer);

    // Handle a telecommand packet.
    // This will work through the list of packet consumers until one
    // succeeds at handling the telecommand.
    // The packet will be passed to the consumers with the read/write
    // pointer at the start of the packet data field.
    // The telecommand handler will fail to handle a packet if any
    // of the following conditions isn't met:
    // - The packet is a telecommand packet.
    // - The packet has a secondary header.
    // - The packet's application process identifier is the same
    //   as the telecommand handler's application process identifier.
    // - The packet's semantic version number is backwards-compatible
    //   with the telecommand handler's semantic version number.
    // Return true on success; otherwise return false.
    boolean handle(ESAT_CCSDSPacket packet);

  private:
    // Application process identifier.
    // Each logical subsystem should have its own unique application
    // process identifier (e.g., the attitude determination and
    // control subsystem has its own application process identifier).
    word applicationProcessIdentifier;

    // Version number in major.minor.patch format
    // as defined in the Semantic Versioning 2.0.0 standard.
    // Each application process has a version number.
    ESAT_SemanticVersionNumber versionNumber;

    // Head of the list of packet consumer objects.
    ESAT_CCSDSPacketConsumer* packetConsumer;

    // Return true if the packet is compatible; otherwise return
    // false.
    // A packet is compatible with this telecommand handler when
    // all the following conditions are met:
    // - The packet is a telecommand packet.
    // - The packet has a secondary header.
    // - The packet's application process identifier is the same
    //   as the telecommand handler's application process identifier.
    // - The packet's version number is backwards-compatible with the
    // - telecommand handler's version number.
    boolean compatiblePacket(ESAT_CCSDSPacket packet) const;
};

#endif /* ESAT_CCSDSTelecommandPacketHandler_h */

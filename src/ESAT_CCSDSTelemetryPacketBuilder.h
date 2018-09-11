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

#ifndef ESAT_CCSDSTelemetryPacketBuilder_h
#define ESAT_CCSDSTelemetryPacketBuilder_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_CCSDSPacketContents.h"
#include "ESAT_Clock.h"
#include "ESAT_FlagContainer.h"

// Telemetry packet builder for ESAT's CCSDS space packets.
class ESAT_CCSDSTelemetryPacketBuilder
{
  public:
    ESAT_CCSDSTelemetryPacketBuilder();

    // Set up the telemetry packet builder for the given application
    // process identifier, and version numbers.
    // Start with a 0 packet sequence count.
    // Get the packet time from the provided clock.
    ESAT_CCSDSTelemetryPacketBuilder(word applicationProcessIdentifier,
                                     byte majorVersionNumber,
                                     byte minorVersionNumber,
                                     byte patchVersionNumber,
                                     ESAT_Clock& clock);

    // Add a new entry to the list of packet contents.
    void add(ESAT_CCSDSPacketContents& contents);

    // Return a list of available packets as a flag container: flags
    // set as true correspond to available packets (available()
    // returns true) and flags set as false correspond to unavailable
    // packets (available() returns false).
    ESAT_FlagContainer available();

    // Build a new CCSDS telemetry packet with the contents of the
    // packet contents object that matches the given identifier.
    // Don't modify the queue of packet contents.
    // Return true on success; otherwise return false.
    boolean build(ESAT_CCSDSPacket& packet,
                  byte identifier);

    // Build a new CCSDS telemetry packet with the contents
    // that match the next identifier in pendingPackets.
    // Clear the used identifier in pendingPackets.
    // Return true on success; otherwise return false.
    boolean buildNext(ESAT_CCSDSPacket& packet,
                      ESAT_FlagContainer& pendingPackets);

    // Enable the emission of the packet with given identifier.
    void enable(byte identifier);

  private:
    // Application process identifier.
    // Each logical subsystem should have its own unique application
    // process identifier (e.g., the attitude determination and
    // control subsystem has its own application process identifier).
    word applicationProcessIdentifier;

    // Packet sequence count.
    // Each application process keeps its own packet sequence count,
    // which is incremented every time it generates a new packet.
    word packetSequenceCount;

    // Version number in major.minor.patch format
    // as defined in the Semantic Versioning 2.0.0 standard.
    // Each application process has a version number.
    byte majorVersionNumber;
    byte minorVersionNumber;
    byte patchVersionNumber;

    // Use this clock to fill the timestamp of the packets.
    ESAT_Clock* clock;

    // Head of the list of packet contents objects.
    ESAT_CCSDSPacketContents* packetContents;

    // Return the packet contents object with the given identifier
    // or nullptr if none can be found.
    ESAT_CCSDSPacketContents* find(byte identififer);
};

#endif /* ESAT_CCSDSTelemetryPacketBuilder_h */

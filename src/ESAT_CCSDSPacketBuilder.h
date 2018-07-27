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

#ifndef ESAT_CCSDSPacketBuilder_h
#define ESAT_CCSDSPacketBuilder_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_CCSDSPacketContents.h"
#include "ESAT_Clock.h"

// Packet builder for ESAT's CCSDS space packets.
class ESAT_CCSDSPacketBuilder
{
  public:
    ESAT_CCSDSPacketBuilder();

    // Set up the packet builder for the given application process identifier,
    // and version numbers.
    // Start with a 0 packet sequence count.
    // Get the packet time from the provided clock.
    ESAT_CCSDSPacketBuilder(word applicationProcessIdentifier,
                            byte majorVersionNumber,
                            byte minorVersionNumber,
                            byte patchVersionNumber,
                            ESAT_Clock& clock);

    // Build a new CCSDS telecommand packet with the provided packet
    // contents.
    // Increment the packet sequence count on success.
    // Return true on success; otherwise return false.
    boolean buildTelecommandPacket(ESAT_CCSDSPacket& packet,
                                   ESAT_CCSDSPacketContents& contents);

    // Build a new CCSDS telemetry packet with the provided packet
    // contents.
    // Increment the packet sequence count on success.
    // Return true on success; otherwise return false.
    boolean buildTelemetryPacket(ESAT_CCSDSPacket& packet,
                                 ESAT_CCSDSPacketContents& contents);

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
};

#endif /* ESAT_CCSDSPacketBuilder_h */

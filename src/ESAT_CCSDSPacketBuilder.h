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
    // version numbers and packet type.
    // Start the sequence count at the given value.
    // Get the packet time from the provided clock.
    ESAT_CCSDSPacketBuilder(word applicationProcessIdentifier,
                            byte majorVersionNumber,
                            byte minorVersionNumber,
                            byte patchVersionNumber,
                            ESAT_CCSDSPrimaryHeader::PacketType packetType,
                            word packetSequenceCount,
                            ESAT_Clock& clock);

    // Build a new CCSDS packet with the provided packet contents.
    // Return true on success; otherwise return false.
    boolean buildPacket(ESAT_CCSDSPacket& packet,
                        ESAT_CCSDSPacketContents& contents);

    // Fill the primary header and secondary header fields
    // of a CCSDS packet.
    // Return true on success; otherwise return false.
    boolean fillHeaders(ESAT_CCSDSPacket& packet,
                        const byte packetIdentifier);

  private:
    // Use this clock to fill the timestamp of the packets.
    ESAT_Clock* clock;

    // Primary header template.
    ESAT_CCSDSPrimaryHeader primaryHeader;

    // Secondary header template.
    ESAT_CCSDSSecondaryHeader secondaryHeader;
};

#endif /* ESAT_CCSDSPacketBuilder_h */

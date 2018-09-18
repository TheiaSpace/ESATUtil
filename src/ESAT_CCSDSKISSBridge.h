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

#ifndef ESAT_CCSDSKISSBridge_h
#define ESAT_CCSDSKISSBridge_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_KISSStream.h"

// CCSDS-to-KISS bridge.
// Pass CCSDS Space Packets in KISS frames through a backend Stream.
class ESAT_CCSDSKISSBridge
{
  public:
    // Instantiate an empty CCSDS-to-KISS bridge without a backend
    // stream.  Reads and writes will do nothing.
    ESAT_CCSDSKISSBridge();

    // Instantiate a CCSDS-to-KISS bridge that will move data through
    // this backend stream.
    // Reading is disabled; call enableReading() to enable it.
    // Writing is disabled; call enableWriting() to enable it.
    ESAT_CCSDSKISSBridge(Stream& backend);

    // Disable reading packets.  All calls to read() will fail.
    void disableReading();

    // Disable writing packets.  All calls to write() will do nothing.
    void disableWriting();

    // Enable reading packets.
    // Use the buffer of given capacity for accumulating the
    // partially-received packets from one call to read() to the next.
    // The buffer must live for as long as the CCSDS-to-KISS bridge
    // is in use.
    void enableReading(byte buffer[], unsigned long capacity);

    // Enable writing packets.
    // Writes will be unbuffered.
    void enableWriting();

    // Enable writing packets.
    // Writes will be buffered using the buffer of given capacity.
    // The buffer must live for as long as the CCSDS-to-KISS bridge is
    // in use.
    void enableWriting(byte buffer[], unsigned long capacity);

    // Read and fill the contents of packet from a KISS frame coming
    // from the backend stream.
    // Return true on success; otherwise return false.
    boolean read(ESAT_CCSDSPacket& packet);

    // Write packet in a KISS frame to the backend stream.
    void write(ESAT_CCSDSPacket packet);

  private:
    // Move KISS-framed CCSDS packets through this stream.
    Stream* backendStream;

    // Read packets from this KISS stream.
    ESAT_KISSStream reader;

    // Write packets to this KISS stream.
    ESAT_KISSStream writer;
};

#endif /* ESAT_CCSDSKISSBridge_h */

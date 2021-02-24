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

#ifndef ESAT_CCSDSPacketFromKISSFrameReader_h
#define ESAT_CCSDSPacketFromKISSFrameReader_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_KISSStream.h"

// CCSDS-from-KISS reader.
// Read CCSDS Space Packets from KISS frames coming from a backend
// Stream.
class ESAT_CCSDSPacketFromKISSFrameReader
{
  public:
    // Instantiate an empty CCSDS-from-KISS reader without a backend
    // stream.
    // Reads will do nothing.
    ESAT_CCSDSPacketFromKISSFrameReader();

    // Instantiate a CCSDS-from-KISS reader that will read data from
    // this backend stream.
    // Buffer packets with the given maximum packet data length;
    ESAT_CCSDSPacketFromKISSFrameReader(Stream& backend,
                                        unsigned long maximumPacketDataLength);

    // Instantiate a CCSDS-from-KISS reader that will read data from
    // this backend stream.
    // Use the buffer of given capacity to store frame contents.
    ESAT_CCSDSPacketFromKISSFrameReader(Stream& backend,
                                        byte buffer[],
                                        unsigned long capacity);

    // Read and fill the contents of CCSDS packet from a KISS frame
    // coming from the backend stream.
    // Return true on success; otherwise return false.
    boolean read(ESAT_CCSDSPacket& packet);

  private:
    // Read frames from KISS stream.
    ESAT_KISSStream reader;
};

#endif /* ESAT_CCSDSPacketFromKISSFrameReader_h */

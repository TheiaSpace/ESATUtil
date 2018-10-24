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

#ifndef ESAT_CCSDSPacketToKISSFrameWriter_h
#define ESAT_CCSDSPacketToKISSFrameWriter_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// CCSDS-to-KISS writer.
// Write CCSDS Space Packets in KISS frames to a backend Stream.
class ESAT_CCSDSPacketToKISSFrameWriter
{
  public:
    // Instantiate an empty CCSDS-to-KISS writer without a backend
    // stream.
    // Writes will do nothing.
    ESAT_CCSDSPacketToKISSFrameWriter();

    // Instantiate a CCSDS-to-KISS writer that will write data to
    // this backend stream.
    ESAT_CCSDSPacketToKISSFrameWriter(Stream& backend);

    // Write the given packet in a KISS frame to the backend stream.
    // The write will be buffered and the frame will be written in one
    // operation, which may be faster with some streams, but it will
    // consume more memory than an unbuffered write.
    // Return true on success; otherwise return false.
    boolean bufferedWrite(ESAT_CCSDSPacket packet);

    // Write the given packet in a KISS frame to the backend stream.
    // The write will be unbuffered and the frame will be written byte
    // by byte, which may be slower with some streams, but it will
    // consume less memory than a buffered write.
    // Return true on success; otherwise return false.
    boolean unbufferedWrite(ESAT_CCSDSPacket packet);

  private:
    // Write frames to this stream.
    Stream* backendStream;
};

#endif /* ESAT_CCSDSPacketToKISSFrameWriter_h */

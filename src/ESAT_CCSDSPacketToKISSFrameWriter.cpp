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

#include "ESAT_CCSDSPacketToKISSFrameWriter.h"
#include "ESAT_KISSStream.h"

ESAT_CCSDSPacketToKISSFrameWriter::ESAT_CCSDSPacketToKISSFrameWriter()
{
  backendStream = nullptr;
}

ESAT_CCSDSPacketToKISSFrameWriter::ESAT_CCSDSPacketToKISSFrameWriter(Stream& backend)
{
  backendStream = &backend;
}

void ESAT_CCSDSPacketToKISSFrameWriter::bufferedWrite(ESAT_CCSDSPacket packet)
{
  if (backendStream)
  {
    const unsigned long capacity =
      ESAT_KISSStream::frameLength(packet.length());
    byte buffer[capacity];
    ESAT_KISSStream writer(*backendStream, buffer, capacity);
    (void) writer.beginFrame();
    (void) packet.writeTo(writer);
    (void) writer.endFrame();
  }
}

void ESAT_CCSDSPacketToKISSFrameWriter::unbufferedWrite(ESAT_CCSDSPacket packet)
{
  if (backendStream)
  {
    ESAT_KISSStream writer(*backendStream);
    (void) writer.beginFrame();
    (void) packet.writeTo(writer);
    (void) writer.endFrame();
  }
}

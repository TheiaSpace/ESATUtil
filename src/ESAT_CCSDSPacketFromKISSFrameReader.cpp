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

#include "ESAT_CCSDSPacketFromKISSFrameReader.h"

ESAT_CCSDSPacketFromKISSFrameReader::ESAT_CCSDSPacketFromKISSFrameReader()
{
  reader = ESAT_KISSStream();
}

ESAT_CCSDSPacketFromKISSFrameReader::ESAT_CCSDSPacketFromKISSFrameReader(Stream& backend,
                                                                         const unsigned long capacity)
{
  reader = ESAT_KISSStream(backend, capacity);
}

ESAT_CCSDSPacketFromKISSFrameReader::ESAT_CCSDSPacketFromKISSFrameReader(Stream& backend,
                                                                         byte buffer[],
                                                                         const unsigned long capacity)
{
  reader = ESAT_KISSStream(backend, buffer, capacity);
}

boolean ESAT_CCSDSPacketFromKISSFrameReader::read(ESAT_CCSDSPacket& packet)
{
  const boolean gotFrame = reader.receiveFrame();
  if (gotFrame)
  {
    return packet.readFrom(reader);
  }
  else
  {
    return false;
  }
}

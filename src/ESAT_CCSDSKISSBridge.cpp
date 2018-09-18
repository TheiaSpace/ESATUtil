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

#include "ESAT_CCSDSKISSBridge.h"

ESAT_CCSDSKISSBridge::ESAT_CCSDSKISSBridge()
{
  backendStream = nullptr;
}

ESAT_CCSDSKISSBridge::ESAT_CCSDSKISSBridge(Stream& backend)
{
  backendStream = &backend;
  disableReading();
  disableWriting();
}

void ESAT_CCSDSKISSBridge::disableReading()
{
  // An empty KISS stream does nothing, so this disables reading.
  reader = ESAT_KISSStream();
}

void ESAT_CCSDSKISSBridge::disableWriting()
{
  // An empty KISS stream does nothing, so this disables writing.
  writer = ESAT_KISSStream();
}

void ESAT_CCSDSKISSBridge::enableReading(byte buffer[],
                                         const unsigned long capacity)
{
  if (backendStream)
  {
    reader = ESAT_KISSStream(*backendStream, buffer, capacity);
  }
}

void ESAT_CCSDSKISSBridge::enableWriting()
{
  if (backendStream)
  {
    writer = ESAT_KISSStream(*backendStream);
  }
}

void ESAT_CCSDSKISSBridge::enableWriting(byte buffer[],
                                         const unsigned long capacity)
{
  writer = ESAT_KISSStream(*backendStream, buffer, capacity);
}

boolean ESAT_CCSDSKISSBridge::read(ESAT_CCSDSPacket& packet)
{
  if (reader.receiveFrame())
  {
    return packet.readFrom(reader);
  }
  else
  {
    return false;
  }
}

void ESAT_CCSDSKISSBridge::write(ESAT_CCSDSPacket packet)
{
  (void) writer.beginFrame();
  (void) packet.writeTo(writer);
  (void) writer.endFrame();
}

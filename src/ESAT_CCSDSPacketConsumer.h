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

#ifndef ESAT_CCSDSPacketConsumer_h
#define ESAT_CCSDSPacketConsumer_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// Packet consumer interface.
// Use together with ESAT_CCSDSTelecommandHandler to handle
// telecommand packets.
class ESAT_CCSDSPacketConsumer
{
  public:
    // Next packet consumer object in the list of packet consumer objects.
    // ESAT_CCSDSTelecommandHandler uses this to keep a linked list of
    // registered packet consumers: it can traverse the list by going
    // from one packet consumer object to the next packet consumer
    // object until reaching the end of the list at nullptr.
    // Only ESAT_CCSDSTelecommandHandler should care about this.
    // Each packet consumer object should be added just one time to
    // just one ESAT_CCSDSTelecommandHandler object.
    ESAT_CCSDSPacketConsumer* nextPacketConsumer;

    // Trivial destructor.
    // We need to define it because the C++ programming language
    // works this way.
    virtual ~ESAT_CCSDSPacketConsumer() {};

    // Consume/process a packet.
    // Return true on success; otherwise return false.
    // Packet consumers used by ESAT_CCSDSTelecommandHandler return true
    // on telecommands they handle and false on the rest of telecommands.
    virtual boolean consume(ESAT_CCSDSPacket packet) = 0;
};

#endif /* ESAT_CCSDSPacketConsumer_h */

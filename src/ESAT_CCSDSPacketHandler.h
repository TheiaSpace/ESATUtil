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

#ifndef ESAT_CCSDSPacketHandler_h
#define ESAT_CCSDSPacketHandler_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// Packet handler interface.
// Use together with ESAT_CCSDSTelecommandHandler to handle
// telecommand packets.
class ESAT_CCSDSPacketHandler
{
  public:
    // Next packet handler object in the list of packet handler objects.
    // ESAT_CCSDSTelecommandHandler uses this to keep a linked list of
    // registered packet handlers: it can traverse the list by going
    // from one packet handler object to the next packet handler
    // object until reaching the end of the list at nullptr.
    // Only ESAT_CCSDSTelecommandPacketDispatcher should care about this.
    // Each packet handler object should be added just one time to
    // just one ESAT_CCSDSTelecommandPacketDispatcher object.
    ESAT_CCSDSPacketHandler* nextPacketHandler;

    // Trivial destructor.
    // We need to define it because the C++ programming language
    // works this way.
    virtual ~ESAT_CCSDSPacketHandler() {};

    // Consume/process a packet.
    // Return true on success; otherwise return false.
    // Packet handlers used by ESAT_CCSDSTelecommandHandler return true
    // on telecommands they handle and false on the rest of telecommands.
    virtual boolean consume(ESAT_CCSDSPacket packet) = 0;
};

#endif /* ESAT_CCSDSPacketHandler_h */

/*
 * Copyright (C) 2020 Theia Space, Universidad Politécnica de Madrid
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

#ifndef ESAT_CCSDSPacketQueue_h
#define ESAT_CCSDSPacketQueue_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"

// Queue of ESAT's CCSDS space packets.
class ESAT_CCSDSPacketQueue
{
  public:
    // Instantiate a zero-capacity packet queue.
    ESAT_CCSDSPacketQueue();

    // Instantiate a packet queue that can hold a number of packets,
    // each one of them with a given packet data capacity.
    ESAT_CCSDSPacketQueue(unsigned long numberOfPackets,
                          unsigned long packetDataCapacity);

    // Copy constructor.
    // Instantiate a packet queue as a copy of another packet queue.
    ESAT_CCSDSPacketQueue(const ESAT_CCSDSPacketQueue& original);

    // Destroy a packet queue.
    ~ESAT_CCSDSPacketQueue();

    // Return the number of packets that this queue can hold.
    unsigned long capacity() const;

    // Clear the queue.
    void flush();

    // Return the number of unread packets in the queue.
    unsigned long length() const;

    // Pop the next packet of the queue and copy its contents
    // to the given packet object.
    // Return true on success; otherwise return false.
    boolean read(ESAT_CCSDSPacket& packet);

    // Push a new packet to the queue.
    // Return true on success; otherwise return false.
    boolean write(ESAT_CCSDSPacket packet);

    // Assignment operator: make this queue a copy of another packet queue.
    ESAT_CCSDSPacketQueue& operator=(const ESAT_CCSDSPacketQueue& original);

  private:
    // Capacity of the packet queue.
    unsigned long queueCapacity;

    // Packet buffer.
    ESAT_CCSDSPacket* packets;

    // Index of the next packet to be read.
    unsigned long readPosition;

    // Index of the next packet to be written.
    unsigned long writePosition;
};

#endif /* ESAT_CCSDSPacketQueue_h */

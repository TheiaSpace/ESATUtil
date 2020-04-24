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

#include "ESAT_CCSDSPacketQueue.h"

ESAT_CCSDSPacketQueue::ESAT_CCSDSPacketQueue()
{
  queueCapacity = 0;
  packets = nullptr;
  readPosition = 0;
  writePosition = 0;
}

ESAT_CCSDSPacketQueue::ESAT_CCSDSPacketQueue(const unsigned long numberOfPackets,
                                             const unsigned long packetDataCapacity)
{
  queueCapacity = numberOfPackets;
  if (queueCapacity != 0)
  {
    packets = ::new ESAT_CCSDSPacket[queueCapacity];
    for (unsigned long index = 0; index < queueCapacity; index = index + 1)
    {
      packets[index] = ESAT_CCSDSPacket(packetDataCapacity);
    }
  }
  readPosition = 0;
  writePosition = 0;
}

ESAT_CCSDSPacketQueue::ESAT_CCSDSPacketQueue(const ESAT_CCSDSPacketQueue& original)
{
  if (packets != nullptr)
  {
    ::delete[] packets;
  }
  queueCapacity = original.queueCapacity;
  readPosition = original.readPosition;
  writePosition = original.writePosition;
  if ((queueCapacity != 0) && (original.packets != nullptr))
  {
    packets = ::new ESAT_CCSDSPacket[queueCapacity];
    for (unsigned long index = 0; index < queueCapacity; index = index + 1)
    {
      packets[index] = ESAT_CCSDSPacket(original.packets[index].capacity());
      (void) original.packets[index].copyTo(packets[index]);
    }
  }
}

ESAT_CCSDSPacketQueue::~ESAT_CCSDSPacketQueue()
{
  ::delete[] packets;
}

unsigned long ESAT_CCSDSPacketQueue::availableForRead() const
{
  unsigned long currentLength = 0;
  for (unsigned long index = 0; index < capacity(); index = index + 1)
  {
    if (packets[index].packetDataLength() > 0)
    {
      currentLength = currentLength + 1;
    }
  }
  return currentLength;
}

unsigned long ESAT_CCSDSPacketQueue::availableForWrite() const
{
  return capacity() - availableForRead();
}

unsigned long ESAT_CCSDSPacketQueue::capacity() const
{
  return queueCapacity;
}

void ESAT_CCSDSPacketQueue::flush()
{
  if (packets == nullptr)
  {
    return;
  }
  for (unsigned long index = 0; index < queueCapacity; index = index + 1)
  {
    packets[index].flush();
  }
  readPosition = 0;
  writePosition = 0;
}

boolean ESAT_CCSDSPacketQueue::read(ESAT_CCSDSPacket& packet)
{
  if (packets == nullptr)
  {
    return false;
  }
  if (packets[readPosition].packetDataLength() == 0)
  {
    return false;
  }
  if (packets[readPosition].copyTo(packet))
  {
    unsigned long nextReadPosition = readPosition + 1;
    if (nextReadPosition == capacity())
    {
      nextReadPosition = 0;
    }
    packets[readPosition].flush();
    readPosition = nextReadPosition;
    return true;
  }
  return false;
}

boolean ESAT_CCSDSPacketQueue::write(ESAT_CCSDSPacket packet)
{
  if (packets == nullptr)
  {
     return false;
  }
  if (packets[writePosition].packetDataLength() > 0)
  {
    return false;
  }
  if (packet.copyTo(packets[writePosition]))
  {
    unsigned long nextWritePosition = writePosition + 1;
    if (nextWritePosition == capacity())
    {
      nextWritePosition = 0;
    }
    writePosition = nextWritePosition;
    return true;
  }
  return false;
}

ESAT_CCSDSPacketQueue& ESAT_CCSDSPacketQueue::operator=(const ESAT_CCSDSPacketQueue& original)
{
  if (this != &original)
  {
    if (packets != nullptr)
    {
      ::delete[] packets;
    }
    queueCapacity = original.queueCapacity;
    readPosition = original.readPosition;
    writePosition = original.writePosition;
    if ((queueCapacity != 0) && (original.packets != nullptr))
    {
      packets = ::new ESAT_CCSDSPacket[queueCapacity];
      for (unsigned long index = 0; index < queueCapacity; index = index + 1)
      {
        packets[index] = ESAT_CCSDSPacket(original.packets[index].capacity());
        (void) original.packets[index].copyTo(packets[index]);
      }
    }
  }
  return *this;
}

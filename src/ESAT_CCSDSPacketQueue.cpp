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
  queueIsEmpty = true;
  queueIsFull = false;
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
  queueIsFull = false;
  queueIsEmpty = true;
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
  queueIsEmpty = original.queueIsEmpty;
  queueIsFull = original.queueIsFull;
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

unsigned long ESAT_CCSDSPacketQueue::available() const
{
	return capacity() - length();
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
  queueIsEmpty = true;
  queueIsFull = false;
}

unsigned long ESAT_CCSDSPacketQueue::length() const
{
  if (queueIsFull)
  {
	  return capacity();
  }
  if (queueIsEmpty)
  {
	  return 0;
  }
  if (writePosition >= readPosition)
  {
    return writePosition - readPosition;
  }
  else
  {
    return (writePosition + capacity()) - readPosition;
  }
}

boolean ESAT_CCSDSPacketQueue::read(ESAT_CCSDSPacket& packet)
{
  // Serial.println("Inside ESAT_CCSDSPacketQueue.read()");
  // Serial.print("Write position: ");
  // Serial.println(writePosition, DEC);
  // Serial.print("Read position: ");
  // Serial.println(readPosition, DEC);
  // Serial.print("Is queue empty: ");
  // Serial.println(queueIsEmpty, DEC);
  // Serial.print("Is queue full: ");
  // Serial.println(queueIsFull, DEC);
  // Serial.print("Queue length: ");
  // Serial.println(length(), DEC);
  // Serial.print("Queue free space: ");
  // Serial.println(available(), DEC);
  if (packets == nullptr)
  {
    return false;
  }
  if (length() == 0)
  {
    return false;
  }
  if (packets[readPosition].copyTo(packet))
  {	  
	queueIsFull = false;
	readPosition = readPosition + 1;
	if (readPosition == capacity())
	{
		readPosition = 0;
	}
	if (readPosition == writePosition)
	{
	  queueIsEmpty = true;
	}
  // // Serial.println("ESAT_CCSDSPacketQueue.read() after copy");
  // Serial.print("Write position: ");
  // Serial.println(writePosition, DEC);
  // Serial.print("Read position: ");
  // Serial.println(readPosition, DEC);
  // Serial.print("Is queue empty: ");
  // Serial.println(queueIsEmpty, DEC);
  // Serial.print("Is queue full: ");
  // Serial.println(queueIsFull, DEC);
  // Serial.print("Queue length: ");
  // Serial.println(length(), DEC);
  // Serial.print("Queue free space: ");
  // Serial.println(available(), DEC);
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
  if (length() == capacity())
  {
	return false;
  }
  packets[writePosition].rewind();
  if (packet.copyTo(packets[writePosition]))
  {
	queueIsEmpty = false;
	writePosition = writePosition + 1;
	if (writePosition == capacity())
	{
		writePosition = 0;
	}
	if (readPosition == writePosition)
	{
	  queueIsFull = true;
	}
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

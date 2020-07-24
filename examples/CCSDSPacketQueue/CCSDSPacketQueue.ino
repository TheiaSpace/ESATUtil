/*
 * Copyright (C) 2020 Theia Space, Universidad Politécnica de Madrid
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ESAT_CCSDSPacketQueue.h>

// ESAT_CCSDSPacketQueue example program.
// Queues of CCSDS Space Packets.

// Work with this packet.
const unsigned long packets = 5;
const unsigned long packetDataCapacity = 4;
ESAT_CCSDSPacketQueue packetQueue(packets, packetDataCapacity);

long readLong()
{
  while (Serial.available() == 0)
  {
  }
  String input = Serial.readString();
  return input.toInt();
}

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Seed the random number generator.
  randomSeed(0);
}

void loop()
{
  (void) Serial.println(F("#########################################"));
  (void) Serial.println(F("CCSDS Space Packet queue example program."));
  (void) Serial.println(F("#########################################"));
  (void) Serial.print(F("Queue total capacity: "));
  (void) Serial.println(packetQueue.capacity(), DEC);
  (void) Serial.print(F("Queue used capacity: "));
  (void) Serial.println(packetQueue.length(), DEC);
  (void) Serial.print(F("Queue unused capacity: "));
  (void) Serial.println(packetQueue.available(), DEC);
  (void) Serial.println(F("How many packets do you want to write to the queue?"));
  const long packetsToWrite = readLong();
  (void) Serial.print(F("Writing "));
  (void) Serial.print(packetsToWrite, DEC);
  (void) Serial.println(F(" packets to the queue..."));
  for (unsigned long iteration = 0;
       iteration < packetsToWrite;
       iteration = iteration + 1)
  {
    ESAT_CCSDSPacket packet(packetDataCapacity);
    (void) Serial.println(F("What integer number do you want to store in the packet?"));
    const long number = readLong();
    packet.writeLong(number);
    packet.rewind();
    (void) Serial.print(F("Writing packet with the following packet data: "));
    (void) Serial.print(packet.readLong(), DEC);
    (void) Serial.println(F("... "));
    const boolean goodWrite = packetQueue.write(packet);
    if (goodWrite)
    {
      (void) Serial.println(F("success!"));
    }
    else
    {
      (void) Serial.println(F("failure (full queue)!"));
    }
  }
  (void) Serial.print(F("Queue total capacity: "));
  (void) Serial.println(packetQueue.capacity(), DEC);
  (void) Serial.print(F("Queue used capacity: "));
  (void) Serial.println(packetQueue.length(), DEC);
  (void) Serial.print(F("Queue unused capacity: "));
  (void) Serial.println(packetQueue.available(), DEC);
  (void) Serial.println(F("How many packets do you want to read from the queue?"));
  const long packetsToRead = readLong();
  (void) Serial.print(F("Reading "));
  (void) Serial.print(packetsToRead, DEC);
  (void) Serial.println(F(" packets from the queue..."));
  for (unsigned long iteration = 0;
       iteration < packetsToRead;
       iteration = iteration + 1)
  {
    ESAT_CCSDSPacket packet(packetDataCapacity);
    (void) Serial.print(F("Reading packet from the queue... "));
    const boolean goodRead = packetQueue.read(packet);
    if (goodRead)
    {
      packet.rewind();
      (void) Serial.print(F("success (packet data is "));
      (void) Serial.print(packet.readLong(), DEC);
      (void) Serial.println(F(")!"));
    }
    else
    {
      (void) Serial.println(F("failure (empty queue)!"));
    }
  }
}

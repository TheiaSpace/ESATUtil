/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
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

#include <ESAT_CCSDSTelecommandPacketHandler.h>

// ESAT_CCSDSTelecommandPacketHandler example program.
// A way of handling telecommand packets.

// This handles telecommand packet number 1.
class OneTelecommandConsumerClass: public ESAT_CCSDSPacketConsumer
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet)
    {
      const ESAT_CCSDSSecondaryHeader secondaryHeader =
        packet.readSecondaryHeader();
      if (secondaryHeader.packetIdentifier == 1)
      {
        (void) Serial.println(F("OneTelecommandConsumer handles identifier 1 and is working."));
        return true;
      }
      else
      {
        return false;
      }
    }
};

OneTelecommandConsumerClass OneTelecommandConsumer;

// This handles telecommand packets with an even number.
class EvenTelecommandConsumerClass: public ESAT_CCSDSPacketConsumer
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet)
    {
      const ESAT_CCSDSSecondaryHeader secondaryHeader =
        packet.readSecondaryHeader();
      if ((secondaryHeader.packetIdentifier % 2) == 0)
      {
        (void) Serial.println(F("EvenTelecommandConsumer handles even identifier and is working."));
        return true;
      }
      else
      {
        return false;
      }
    }
};

EvenTelecommandConsumerClass EvenTelecommandConsumer;


// Work with this packet.
const byte capacity = ESAT_CCSDSSecondaryHeader::LENGTH;
byte buffer[capacity];
ESAT_CCSDSPacket packet(buffer, capacity);

// Header contents.
const word applicationProcessIdentifier = 5;
const word packetSequenceCount = 0;
const byte majorVersionNumber = 2;
const byte minorVersionNumber = 1;
const byte patchVersionNumber = 0;
byte packetIdentifier = 0;
const word maximumPacketIdentifier = 5;
// Approximate launch time of the first ISS module (Zarya).
const ESAT_Timestamp timestamp(1998, 11, 20, 6, 40, 0);

// Use this to handle telecommand packets.
ESAT_CCSDSTelecommandPacketHandler handler(applicationProcessIdentifier,
                                           majorVersionNumber,
                                           minorVersionNumber,
                                           patchVersionNumber);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Add the consumers to the telecommand handler.
  handler.add(OneTelecommandConsumer);
  handler.add(EvenTelecommandConsumer);
}

void loop()
{
  (void) Serial.println(F("###########################################"));
  (void) Serial.println(F("Telecommand packet handler example program."));
  (void) Serial.println(F("###########################################"));
  // Build the telecommand packet.
  (void) Serial.print(F("Building telecommand packet with identifier number "));
  (void) Serial.print(packetIdentifier, DEC);
  (void) Serial.println(F("..."));
  packet.flush();
  packet.writeTelecommandHeaders(applicationProcessIdentifier,
                                 packetSequenceCount,
                                 timestamp,
                                 majorVersionNumber,
                                 minorVersionNumber,
                                 patchVersionNumber,
                                 packetIdentifier);
  // Handle the telecommand packet.
  (void) Serial.println(F("Handling the telecommand packet..."));
  const boolean handled = handler.handle(packet);
  if (handled)
  {
    (void) Serial.println(F("Telecommand packet handled successfully."));
  }
  else
  {
    (void) Serial.println(F("Couldn't handle the telecommand packet!"));
  }
  if (packetIdentifier < maximumPacketIdentifier)
  {
    packetIdentifier = packetIdentifier + 1;
  }
  else
  {
    packetIdentifier = 0;
  }
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

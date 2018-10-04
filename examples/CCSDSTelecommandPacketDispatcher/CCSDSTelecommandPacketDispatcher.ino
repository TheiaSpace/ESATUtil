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

#include <ESAT_CCSDSTelecommandPacketDispatcher.h>
#include <ESAT_SemanticVersionNumber.h>

// ESAT_CCSDSTelecommandPacketDispatcher example program.
// A way of handling telecommand packets.

// This handles telecommand packet number 1 with version number 2.0.0.
class OneTelecommandHandlerClass: public ESAT_CCSDSPacketHandler
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet)
    {
      (void) packet;
      (void) Serial.println(F("OneTelecommandHandler handles identifier 1 and is working."));
      return true;
    }

    byte packetIdentifier()
    {
      return 1;
    }

    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(2, 0, 0);
    }
};

OneTelecommandHandlerClass OneTelecommandHandler;

// This handles telecommand packet number 2 with version number 2.0.0.
class TwoTelecommandHandlerClass: public ESAT_CCSDSPacketHandler
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet)
    {
      (void) packet;
      (void) Serial.println(F("TwoTelecommandHandler handles identifier 2 and is working."));
      return true;
    }

    byte packetIdentifier()
    {
      return 2;
    }

    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(2, 0, 0);
    }
};

TwoTelecommandHandlerClass TwoTelecommandHandler;

// This handles telecommand packet number 1 with version number 3.0.0.
class IncompatibleTelecommandHandlerClass: public ESAT_CCSDSPacketHandler
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean consume(ESAT_CCSDSPacket packet)
    {
      (void) packet;
      (void) Serial.println(F("IncompatibleTelecommandHandler handles interface version 3.0.0 and is working."));
      return true;
    }

    byte packetIdentifier()
    {
      return 1;
    }

    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(3, 0, 0);
    }
};

IncompatibleTelecommandHandlerClass IncompatibleTelecommandHandler;

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
ESAT_CCSDSTelecommandPacketDispatcher dispatcher(applicationProcessIdentifier);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Add the handlers to the telecommand dispatcher.
  dispatcher.add(OneTelecommandHandler);
  dispatcher.add(TwoTelecommandHandler);
  dispatcher.add(IncompatibleTelecommandHandler);
}

void loop()
{
  (void) Serial.println(F("##############################################"));
  (void) Serial.println(F("Telecommand packet dispatcher example program."));
  (void) Serial.println(F("##############################################"));
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
  const boolean handled = dispatcher.handle(packet);
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

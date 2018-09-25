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

#include <ESAT_CCSDSTelemetryPacketBuilder.h>
#include <ESAT_SoftwareClock.h>

// ESAT_CCSDSTelemetryPacketBuilder example program.
// Another way of building multiple telemetry packets.

// This packet is always available.
class AlwaysAvailablePacketClass: public ESAT_CCSDSPacketContents
{
  public:
    // Return true if a new packet is available; otherwise return
    // false.
    boolean available()
    {
      return true;
    }

    // Fill the user data field of a packet.
    // Return true on success; otherwise return false.
    boolean fillUserData(ESAT_CCSDSPacket& packet)
    {
      (void) packet.print("Hello, World!");
      return true;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 1;
    }
};

AlwaysAvailablePacketClass AlwaysAvailablePacket;

// This packet is available once every second.
class PeriodicPacketClass: public ESAT_CCSDSPacketContents
{
  public:
    // Return true if a new packet is available; otherwise return
    // false.
    boolean available()
    {
      if ((millis() - lastPacketTime) >= 1000)
      {
        return true;
      }
      else
      {
        return false;
      }
    }

    // Fill the user data field of a packet.
    // Return true on success; otherwise return false.
    boolean fillUserData(ESAT_CCSDSPacket& packet)
    {
      lastPacketTime = millis();
      (void) packet.writeUnsignedLong(lastPacketTime);
      return true;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 10;
    }

  private:
    unsigned long lastPacketTime = 0;
};

PeriodicPacketClass PeriodicPacket;

// This packet is always available, but fails when filling the packet
// contents.
class FailingPacketClass: public ESAT_CCSDSPacketContents
{
    // Return true if a new packet is available; otherwise return
    // false.
    boolean available()
    {
      return true;
    }

    // Fill the user data field of a packet.
    // Return true on success; otherwise return false.
    boolean fillUserData(ESAT_CCSDSPacket& packet)
    {
      (void) packet;
      return false;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 100;
    }
};

FailingPacketClass FailingPacket;


// Work with this packet.
const byte capacity = 255;
byte buffer[capacity];
ESAT_CCSDSPacket packet(buffer, capacity);

// Header contents.
const word applicationProcessIdentifier = 5;
const byte majorVersionNumber = 2;
const byte minorVersionNumber = 1;
const byte patchVersionNumber = 0;
// Approximate launch time of the first ISS module (Zarya).
const ESAT_Timestamp timestamp(1998, 11, 20, 6, 40, 0);

// Use this software clock for the packet time.
ESAT_SoftwareClock clock;

// Use this to build packets.
ESAT_CCSDSTelemetryPacketBuilder builder(applicationProcessIdentifier,
                                         majorVersionNumber,
                                         minorVersionNumber,
                                         patchVersionNumber,
                                         clock);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Set the clock.
  clock.write(timestamp);
  // Add the packets to the telemetry packet builder.
  builder.add(AlwaysAvailablePacket);
  builder.add(PeriodicPacket);
  builder.add(FailingPacket);
}

void loop()
{
  (void) Serial.println("#########################################");
  (void) Serial.println("Telemetry packet builder example program.");
  (void) Serial.println("#########################################");
  // Get the list of available packets.
  (void) Serial.print("Identifier numbers of the available packets: ");
  ESAT_FlagContainer pendingPackets = builder.available();
  (void) Serial.println(pendingPackets);
  while (pendingPackets.available() > 0)
  {
    (void) Serial.print("Building packet ");
    const byte identifier = byte(pendingPackets.readNext());
    (void) Serial.print(identifier, DEC);
    (void) Serial.println("...");
    pendingPackets.clear(identifier);
    const boolean gotPacket = builder.build(packet, identifier);
    if (gotPacket)
    {
      (void) Serial.print("Packet contents: ");
      (void) Serial.println(packet);
    }
    else
    {
      (void) Serial.println("Couldn't build the packet!");
    }
  }
  // End.
  (void) Serial.println("End.");
  (void) Serial.println("");
  delay(1000);
}

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

#include <ESAT_SubsystemPacketHandler.h>
#include <ESAT_SoftwareClock.h>

// ESAT_SubsystemPacketHandler example program.
// Simple subsystem with USB and I2C telemetry and telecommands.
// This subsystem the following telecommands:
//   - setting the clock;
//   - enabling the generation of a telemetry packet;
//   - disabling the generation of a telemetry packet;
//   - relaying an encapsulated telecommand to the I2C master.
// This subsystem has the following telemetry:
//   - uptime in milliseconds and microseconds;
//   - text message;
//   - random number.
// This subsystem writes telemetry packets to USB and I2C,
// reads telecommand packets from USB and I2C, and can relay
// telecommand packets to I2C.

// Packet handler configuration parameters.
const byte address = 10;
const word applicationProcessIdentifier = 10;
const byte majorVersionNumber = 1;
const byte minorVersionNumber = 0;
const byte patchVersionNumber = 0;
const unsigned long packetDataCapacity = 256;
ESAT_SoftwareClock clock;
// Use this for incoming and outgoing packets.
ESAT_CCSDSPacket packet(packetDataCapacity);
// Use this for relayed telecommands.
ESAT_CCSDSPacket telecommand(packetDataCapacity);

// Telecommand for setting the time.
class SetTimeTelecommandClass: public ESAT_CCSDSTelecommandPacketHandler
{
  public:
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean handleUserData(ESAT_CCSDSPacket packet)
    {
      clock.write(packet.readTimestamp());
      return true;
    }

    // Return the packet identifier of this telecommand.
    byte packetIdentifier()
    {
      return 0;
    }

    // Return the interface version number of this telecommand.
    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(majorVersionNumber,
                                        minorVersionNumber,
                                        patchVersionNumber);
    }
};
SetTimeTelecommandClass SetTimeTelecommand;

// Telecommand for enabling the generation of a telemetry packet.
class EnableTelemetryTelecommandClass: public ESAT_CCSDSTelecommandPacketHandler
{
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean handleUserData(ESAT_CCSDSPacket packet)
    {
      ESAT_SubsystemPacketHandler.enableTelemetry(packet.readByte());
      return true;
    }

    // Return the packet identifier of this telecommand.
    byte packetIdentifier()
    {
      return 1;
    }

    // Return the interface version number of this telecommand.
    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(majorVersionNumber,
                                        minorVersionNumber,
                                        patchVersionNumber);
    }
};
EnableTelemetryTelecommandClass EnableTelemetryTelecommand;

// Telecommand for disabling the generation of a telemetry packet.
class DisableTelemetryTelecommandClass: public ESAT_CCSDSTelecommandPacketHandler
{
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean handleUserData(ESAT_CCSDSPacket packet)
    {
      ESAT_SubsystemPacketHandler.disableTelemetry(packet.readByte());
      return true;
    }

    // Return the packet identifier of this telecommand.
    byte packetIdentifier()
    {
      return 2;
    }

    // Return the interface version number of this telecommand.
    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(majorVersionNumber,
                                        minorVersionNumber,
                                        patchVersionNumber);
    }
};
DisableTelemetryTelecommandClass DisableTelemetryTelecommand;

// Telecommand for relaying another telecommand to the I2C master.
class RelayTelecommandTelecommandClass: public ESAT_CCSDSTelecommandPacketHandler
{
    // Handle a telecommand packet.
    // Return true on success; otherwise return false.
    boolean handleUserData(ESAT_CCSDSPacket packet)
    {
      (void) telecommand.readFrom(packet);
      ESAT_SubsystemPacketHandler.queueTelecommandToI2C(telecommand);
      return true;
    }

    // Return the packet identifier of this telecommand.
    byte packetIdentifier()
    {
      return 3;
    }

    // Return the interface version number of this telecommand.
    ESAT_SemanticVersionNumber versionNumber()
    {
      return ESAT_SemanticVersionNumber(majorVersionNumber,
                                        minorVersionNumber,
                                        patchVersionNumber);
    }
};
RelayTelecommandTelecommandClass RelayTelecommandTelecommand;

// Telemetry with the uptime in milliseconds and microseconds.
class UptimeTelemetryClass: public ESAT_CCSDSTelemetryPacketContents
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
      (void) packet.writeUnsignedLong(millis());
      (void) packet.writeUnsignedLong(micros());
      return true;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 0;
    }
};
UptimeTelemetryClass UptimeTelemetry;

// Telemetry with a short text.
class TextTelemetryClass: public ESAT_CCSDSTelemetryPacketContents
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
      (void) packet.print(F("Hello, World!"));
      return true;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 1;
    }
};
TextTelemetryClass TextTelemetry;

// Telemetry with a random number.
class RandomTelemetryClass: public ESAT_CCSDSTelemetryPacketContents
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
      (void) packet.writeUnsignedLong(random());
      return true;
    }

    // Return the identifier number of this packet.
    byte packetIdentifier()
    {
      return 2;
    }
};
RandomTelemetryClass RandomTelemetry;

// Subsystem configuration.
void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Configure the I2C interface.
  Wire.begin(address);
  // Seed the random number generator.
  randomSeed(0);
  // Start the packet handler.
  ESAT_SubsystemPacketHandler.begin(applicationProcessIdentifier,
                                    majorVersionNumber,
                                    minorVersionNumber,
                                    patchVersionNumber,
                                    clock,
                                    Wire,
                                    packetDataCapacity);
  // Configure the telecommands.
  ESAT_SubsystemPacketHandler.addTelecommand(SetTimeTelecommand);
  ESAT_SubsystemPacketHandler.addTelecommand(EnableTelemetryTelecommand);
  ESAT_SubsystemPacketHandler.addTelecommand(DisableTelemetryTelecommand);
  ESAT_SubsystemPacketHandler.addTelecommand(RelayTelecommandTelecommand);
  // Configure the telemetry.
  ESAT_SubsystemPacketHandler.addTelemetry(UptimeTelemetry);
  ESAT_SubsystemPacketHandler.enableTelemetry(UptimeTelemetry.packetIdentifier());
  ESAT_SubsystemPacketHandler.addTelemetry(TextTelemetry);
  ESAT_SubsystemPacketHandler.enableTelemetry(TextTelemetry.packetIdentifier());
  ESAT_SubsystemPacketHandler.addTelemetry(RandomTelemetry);
  ESAT_SubsystemPacketHandler.enableTelemetry(RandomTelemetry.packetIdentifier());
}

// On each main loop cycle, the subsystem prepares its telemetry
// packets, responds to USB telecommands, responds to I2C telecommands,
// writes its telemetry to USB and responds to I2C packet requests.
void loop()
{
  ESAT_SubsystemPacketHandler.prepareSubsystemsOwnTelemetry();
  while (ESAT_SubsystemPacketHandler.readPacketFromUSB(packet))
  {
    ESAT_SubsystemPacketHandler.dispatchTelecommand(packet);
  }
  while (ESAT_SubsystemPacketHandler.readPacketFromI2C(packet))
  {
    ESAT_SubsystemPacketHandler.dispatchTelecommand(packet);
  }
  while (ESAT_SubsystemPacketHandler.readSubsystemsOwnTelemetry(packet))
  {
    ESAT_SubsystemPacketHandler.writePacketToUSB(packet);
  }
  while (ESAT_SubsystemPacketHandler.pendingI2CPacketRequest())
  {
    ESAT_SubsystemPacketHandler.respondToI2CPacketRequest();
  }
}

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

#include <ESAT_I2CSlave.h>

// ESAT_I2CSlave example program (writing named-telemetry packets).
// Write specific telemetry CCSDS Space Packets requested by the I2C master.

// Store packets here.
const byte packetDataLength = ESAT_CCSDSSecondaryHeader::LENGTH;
byte packetData[packetDataLength];
ESAT_CCSDSPacket packet(packetData, packetDataLength);

// Header contents.
const word applicationProcessIdentifier = 5;
const unsigned long packetSequenceCount = 0;
const byte majorVersionNumber = 2;
const byte minorVersionNumber = 1;
const byte patchVersionNumber = 0;
// Launch time of the Mars 3 mission.
const ESAT_Timestamp timestamp(1971, 5, 28, 15, 26, 30);

// Address of the slave node.
const byte slaveAddress = 64;

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Configure the I2C bus in slave mode.
  Wire.begin(slaveAddress);
  // Configure the I2C slave to just support I2C master reads (I2C
  // slave writes) by leaving the I2C master write (I2C slave read)
  // buffer empty.
  ESAT_I2CSlave.begin(Wire,
                      nullptr,
                      0,
                      packetData,
                      packetDataLength);
}

void loop()
{
  (void) Serial.println(F("#######################################################"));
  (void) Serial.println(F("I2C slave write named-packet telemetry example program."));
  (void) Serial.println(F("#######################################################"));
  // Get the request.
  const int request = ESAT_I2CSlave.requestedPacket();
  if (request > 0)
  {
    // Named-packet telemetry request.
    (void) Serial.print(F("Telemetry packet number "));
    (void) Serial.print(request, DEC);
    (void) Serial.println(F(" requested."));
    (void) Serial.println(F("Building the packet..."));
    packet.flush();
    packet.writeTelemetryHeaders(applicationProcessIdentifier,
                                 packetSequenceCount,
                                 timestamp,
                                 majorVersionNumber,
                                 minorVersionNumber,
                                 patchVersionNumber,
                                 request);
    (void) Serial.print(F("Packet contents: "));
    (void) Serial.println(packet);
    (void) Serial.println(F("Writing the packet..."));
    ESAT_I2CSlave.writePacket(packet);
  }
  else
  {
    switch (request)
    {
      // No packet was requested.
      case ESAT_I2CSlave.NO_PACKET_REQUESTED:
        (void) Serial.println(F("No packet was requested."));
        break;
      // Default case: just reject the request.
      default:
        (void) Serial.println(F("Unsupported request; rejecting..."));
        ESAT_I2CSlave.rejectPacket();
        break;
    }
  }
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

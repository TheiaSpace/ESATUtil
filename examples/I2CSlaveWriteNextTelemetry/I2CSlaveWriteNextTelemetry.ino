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

// ESAT_I2CSlave example program (writing next-telemetry packets).
// Write a series of telemetry CCSDS Space Packets requested by the I2C master.

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
word identifier = 256;
const word maximumIdentifier = 3;
// Launch time of the SCORE communications satellite.
const ESAT_Timestamp timestamp(1958, 12, 18, 23, 2, 0);
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
  (void) Serial.println(String("I2C slave write next-packet telemetry example program."));
  // Reset the telemetry queue if commanded to do so.
  if (ESAT_I2CSlave.telemetryQueueResetReceived())
  {
    (void) Serial.println(String("Resetting the telemetry queue..."));
    identifier = 0;
  }
  // Get the request.
  const int request = ESAT_I2CSlave.requestedPacket();
  switch (request)
  {
    // No packet was requested.
    case ESAT_I2CSlave.NO_PACKET_REQUESTED:
      (void) Serial.println(String("No packet was requested."));
      break;
    // Named-packet telemetry request.
    case ESAT_I2CSlave.NEXT_TELEMETRY_PACKET_REQUESTED:
      (void) Serial.print(String("Next-telemetry packet requested."));
      if (identifier < maximumIdentifier)
      {
        // Build and send the packet.
        (void) Serial.println(String("Building the packet..."));
        packet.flush();
        packet.writeTelemetryHeaders(applicationProcessIdentifier,
                                     packetSequenceCount,
                                     timestamp,
                                     majorVersionNumber,
                                     minorVersionNumber,
                                     patchVersionNumber,
                                     identifier);
        (void) Serial.print(String("Packet contents: "));
        (void) Serial.println(packet);
        (void) Serial.println(String("Writing the packet..."));
        ESAT_I2CSlave.writePacket(packet);
        // Increment the identifier number of the next packet.
        identifier = identifier + 1;
      }
      else
      {
        // No more packets available.
        (void) Serial.println(String("No more packets available."));
        (void) Serial.println(String("Rejecting the request..."));
        ESAT_I2CSlave.rejectPacket();
      }
    break;
    // Default case: just reject the request.
    default:
      (void) Serial.println(String("Unsupported request; rejecting..."));
      ESAT_I2CSlave.rejectPacket();
      break;
  }
  // End.
  (void) Serial.println(String("End."));
  delay(1000);
}

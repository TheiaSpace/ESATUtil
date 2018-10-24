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

#include <ESAT_I2CMaster.h>

// ESAT_I2CMaster example program (writing packets).
// Write CCSDS Space Packets to I2C slaves.

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
const byte packetIdentifier = 3;
// Launch time of the Rosetta mission.
const ESAT_Timestamp timestamp(2004, 3, 2, 7, 17, 51);

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
  // Configure the I2C bus and the I2C master.
  Wire.begin();
  ESAT_I2CMaster.begin(Wire);
}

void loop()
{
  (void) Serial.println(F("#################################"));
  (void) Serial.println(F("I2C master write example program."));
  (void) Serial.println(F("#################################"));
  // Build the input packet.
  (void) Serial.println(F("Building the packet..."));
  packet.flush();
  packet.writeTelecommandHeaders(applicationProcessIdentifier,
                                      packetSequenceCount,
                                      timestamp,
                                      majorVersionNumber,
                                      minorVersionNumber,
                                      patchVersionNumber,
                                      packetIdentifier);
  (void) Serial.print(F("Packet contents: "));
  (void) Serial.println(packet);
  // Write the packet to the slave.
  (void) Serial.println(F("Writing the input packet to the slave..."));
  const boolean wrotePacket = ESAT_I2CMaster.writePacket(packet, slaveAddress);
  if (wrotePacket)
  {
    (void) Serial.println(F("Packet written successfully."));
  }
  else
  {
    (void) Serial.println(F("Couldn't write the packet."));
  }
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

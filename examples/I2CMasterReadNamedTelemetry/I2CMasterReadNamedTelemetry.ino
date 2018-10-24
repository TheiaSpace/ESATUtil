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

// ESAT_I2CMaster example program (reading named-telemetry packets).
// Read specific telemetry CCSDS Space Packets from I2C slaves.

// Store packets here.
const byte packetDataLength = ESAT_CCSDSSecondaryHeader::LENGTH;
byte packetData[packetDataLength];
ESAT_CCSDSPacket packet(packetData, packetDataLength);

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
  // Seed the random number generator.
  randomSeed(0);
}

void loop()
{
  (void) Serial.println(F("################################################"));
  (void) Serial.println(F("I2C master read named telemetry example program."));
  (void) Serial.println(F("################################################"));
  // Read the packet.
  (void) Serial.print(F("Reading packet number "));
  const byte identifier = random(0, 256);
  (void) Serial.print(identifier, DEC);
  (void) Serial.println(F("..."));
  const boolean gotPacket = ESAT_I2CMaster.readNamedTelemetry(packet,
                                                              identifier,
                                                              slaveAddress);
  if (gotPacket)
  {
    (void) Serial.print(F("Packet contents: "));
    (void) Serial.println(packet);
  }
  else
  {
    (void) Serial.println(F("Couldn't read the packet!"));
  }
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

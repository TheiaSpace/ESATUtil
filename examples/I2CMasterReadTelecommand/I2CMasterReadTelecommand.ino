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

// ESAT_I2CMaster example program (reading telecommand packets).
// Read telecommand CCSDS Space Packets from I2C slaves.

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
}

void loop()
{
  (void) Serial.println(String("I2C master read telecommand example program."));
  // Read the packet.
  (void) Serial.println(String("Reading a telecommand packet..."));
  const boolean gotPacket =
    ESAT_I2CMaster.readTelecommand(packet, slaveAddress);
  if (gotPacket)
  {
    (void) Serial.print(String("Packet contents: "));
    (void) Serial.println(packet);
  }
  else
  {
    (void) Serial.println(String("Couldn't read the packet!"));
  }
  // End.
  (void) Serial.println(String("End."));
  delay(1000);
}

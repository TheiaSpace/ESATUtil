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

// ESAT_I2CMaster example program (reading the protocol version number).
// Read CCSDS Space Packet-over-I2C protocol version from an I2C slave.

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
  (void) Serial.println(String("I2C master read protocol version number example program."));
  // Get the protocol version from the slave.
  (void) Serial.print(String("Slave protocol version: "));
  const ESAT_SemanticVersionNumber protocolVersionNumber =
    ESAT_I2CMaster.readProtocolVersionNumber(slaveAddress);
  (void) Serial.println(protocolVersionNumber);
  // End.
  (void) Serial.println(String("End."));
  delay(1000);
}

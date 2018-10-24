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

// ESAT_I2CMaster example program (reading next-telemetry packets).
// Read a series of telemetry CCSDS Space Packets from I2C slaves.

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
  (void) Serial.println(F("###############################################"));
  (void) Serial.println(F("I2C master read next telemetry example program."));
  (void) Serial.println(F("###############################################"));
  // Reset the telemetry queue.
  (void) Serial.println(F("Resetting the telemetry queue..."));
  const boolean goodQueue = ESAT_I2CMaster.resetTelemetryQueue(slaveAddress);
  if (goodQueue)
  {
    // Read telemetry packets.
    boolean gotPacket = false;
    do
    {
      (void) Serial.println(F("Reading the next telemetry packet..."));
      gotPacket = ESAT_I2CMaster.readNextTelemetry(packet, slaveAddress);
      if (gotPacket)
      {
        (void) Serial.print(F("Packet contents: "));
        (void) Serial.println(packet);
      }
      else
      {
        (void) Serial.println(F("Couldn't read the packet!"));
      }
    } while (gotPacket);
  }
  else
  {
    (void) Serial.println(F("Couldn't reset the telemetry queue!"));
  }
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

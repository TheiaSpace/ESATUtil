/*
 * This file is part of Theia Space's ESAT Util library.
 *
 * Theia Space's ESAT Util library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT Util library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "ESAT_I2CMaster.h"

boolean ESAT_I2CMasterClass::readTelecommandStatus(TwoWire& bus,
                                                   const byte address,
                                                   const byte millisecondsAfterWrites,
                                                   const byte attempts,
                                                   const word millisecondsBetweenAttempts)
{
  for (int i = 0; i < attempts; i++)
  {
    bus.beginTransmission(address);
    (void) bus.write(TELECOMMAND_STATUS);
    const byte writeStatus = bus.endTransmission();
    delay(millisecondsAfterWrites);
    if (writeStatus != 0)
    {
      return false;
    }
    const byte bytesToRead = 1;
    const byte bytesRead = bus.requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    const byte telecommandStatus = bus.read();
    switch (telecommandStatus)
    {
      case TELECOMMAND_NOT_PENDING:
        return true;
        break;
      case TELECOMMAND_PENDING:
        delay(millisecondsBetweenAttempts);
        break;
      default:
        return false;
        break;
    }
  }
  return false;
}

boolean ESAT_I2CMasterClass::readTelemetry(TwoWire& bus,
                                           const byte address,
                                           const byte packetIdentifier,
                                           ESAT_CCSDSPacket& packet,
                                           const byte millisecondsAfterWrites,
                                           const byte attempts,
                                           const word millisecondsBetweenAttempts)
{
  const boolean telemetryRequestCorrect =
    writeTelemetryRequest(bus,
                          address,
                          packetIdentifier,
                          millisecondsAfterWrites);
  if (!telemetryRequestCorrect)
  {
    return false;
  }
  const boolean telemetryReady =
    readTelemetryStatus(bus,
                        address,
                        millisecondsAfterWrites,
                        attempts,
                        millisecondsBetweenAttempts);
  if (!telemetryReady)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    readTelemetryPrimaryHeader(bus,
                               address,
                               packet,
                               millisecondsAfterWrites);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const boolean packetDataCorrect =
    readTelemetryPacketData(bus,
                            address,
                            packet);
  if (!packetDataCorrect)
  {
    return false;
  }
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (secondaryHeader.packetIdentifier != packetIdentifier)
  {
    return false;
  }
  return true;
}

boolean ESAT_I2CMasterClass::readTelemetryPacketData(TwoWire& bus,
                                                     const byte address,
                                                     ESAT_CCSDSPacket& packet)
{
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  packet.rewind();
  unsigned long totalBytesRead = 0;
  while (totalBytesRead < primaryHeader.packetDataLength)
  {
    byte bytesToRead = I2C_CHUNK_LENGTH;
    if ((totalBytesRead + bytesToRead) > primaryHeader.packetDataLength)
    {
      bytesToRead = primaryHeader.packetDataLength - totalBytesRead;
    }
    const byte bytesRead = bus.requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    for (byte i = 0; i < bytesRead; i++)
    {
      packet.writeByte(bus.read());
    }
    totalBytesRead = totalBytesRead + bytesRead;
  }
  return true;
}

boolean ESAT_I2CMasterClass::readTelemetryPrimaryHeader(TwoWire& bus,
                                                        const byte address,
                                                        ESAT_CCSDSPacket& packet,
                                                        const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(TELEMETRY_VECTOR);
  const byte writeStatus = bus.endTransmission();
  if (writeStatus != 0)
  {
    return false;
  }
  delay(millisecondsAfterWrites);
  ESAT_CCSDSPrimaryHeader primaryHeader;
  const byte headerBytesRead =
    bus.requestFrom(address, primaryHeader.LENGTH);
  if (headerBytesRead != primaryHeader.LENGTH)
  {
    return false;
  }
  const boolean correctRead = primaryHeader.readFrom(bus);
  if (!correctRead)
  {
    return false;
  }
  if (primaryHeader.packetType != primaryHeader.TELEMETRY)
  {
    return false;
  }
  if (primaryHeader.packetDataLength > packet.capacity())
  {
    return false;
  }
  packet.writePrimaryHeader(primaryHeader);
  return true;
}

boolean ESAT_I2CMasterClass::readTelemetryStatus(TwoWire& bus,
                                                 const byte address,
                                                 const byte millisecondsAfterWrites,
                                                 const byte attempts,
                                                 const word millisecondsBetweenAttempts)
{
  for (int i = 0; i < attempts; i++)
  {
    bus.beginTransmission(address);
    (void) bus.write(TELEMETRY_STATUS);
    const byte writeStatus = bus.endTransmission();
    delay(millisecondsAfterWrites);
    if (writeStatus != 0)
    {
      return false;
    }
    const byte bytesToRead = 1;
    const byte bytesRead = bus.requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    const byte telemetryStatus = bus.read();
    switch (telemetryStatus)
    {
      case TELEMETRY_NOT_REQUESTED:
        return false;
        break;
      case TELEMETRY_NOT_READY:
        delay(millisecondsBetweenAttempts);
        break;
      case TELEMETRY_READY:
        return true;
        break;
      case TELEMETRY_REQUEST_REJECTED:
        return false;
        break;
      case TELEMETRY_INVALID:
        return false;
        break;
      default:
        return false;
        break;
    }
  }
  return false;
}

boolean ESAT_I2CMasterClass::writeTelecommand(TwoWire& bus,
                                              const byte address,
                                              ESAT_CCSDSPacket& packet,
                                              const byte millisecondsAfterWrites,
                                              const byte attempts,
                                              const word millisecondsBetweenAttempts)
{
  const boolean telecommandStatusCorrect =
    readTelecommandStatus(bus,
                          address,
                          millisecondsAfterWrites,
                          attempts,
                          millisecondsBetweenAttempts);
  if (!telecommandStatusCorrect)
  {
    return false;
  }
  const boolean telecommandPrimaryHeaderCorrect =
    writeTelecommandPrimaryHeader(bus,
                                  address,
                                  packet,
                                  millisecondsAfterWrites);
  if (!telecommandPrimaryHeaderCorrect)
  {
    return false;
  }
  const boolean telecommandPacketDataCorrect =
    writeTelecommandPacketData(bus,
                               address,
                               packet,
                               millisecondsAfterWrites);
  return telecommandPacketDataCorrect;
}

boolean ESAT_I2CMasterClass::writeTelecommandPacketData(TwoWire& bus,
                                                        const byte address,
                                                        ESAT_CCSDSPacket& packet,
                                                        const byte millisecondsAfterWrites)
{
  packet.rewind();
  while (packet.available() > 0)
  {
    bus.beginTransmission(address);
    (void) bus.write(TELECOMMAND_PACKET_DATA);
    const byte bytesToWrite = I2C_CHUNK_LENGTH - 1;
    for (byte i = 0;
         (i < bytesToWrite) && (packet.available() > 0);
         i++)
    {
      (void) bus.write(packet.readByte());
    }
    const byte writeStatus = bus.endTransmission();
    delay(millisecondsAfterWrites);
    if (writeStatus != 0)
    {
      return false;
    }
  }
  return true;
}

boolean ESAT_I2CMasterClass::writeTelecommandPrimaryHeader(TwoWire& bus,
                                                           const byte address,
                                                           ESAT_CCSDSPacket& packet,
                                                           const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(TELECOMMAND_PRIMARY_HEADER);
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  (void) primaryHeader.writeTo(bus);
  const byte writeStatus = bus.endTransmission();
  delay(millisecondsAfterWrites);
  if (writeStatus == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_I2CMasterClass::writeTelemetryRequest(TwoWire& bus,
                                                   const byte address,
                                                   const byte packetIdentifier,
                                                   const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(TELEMETRY_REQUEST);
  (void) bus.write(packetIdentifier);
  const byte writeStatus = bus.endTransmission();
  delay(millisecondsAfterWrites);
  if (writeStatus == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

ESAT_I2CMasterClass ESAT_I2CMaster;

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

#include "ESATI2CMaster.h"

boolean ESATI2CMaster::readTelecommandStatus(TwoWire& bus,
                                             const byte address,
                                             const byte millisecondsAfterWrites,
                                             const byte tries,
                                             const word millisecondsBetweenRetries)
{
  for (int i = 0; i < tries; i++)
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
        delay(millisecondsBetweenRetries);
        break;
      default:
        return false;
        break;
    }
  }
  return false;
}

boolean ESATI2CMaster::readTelemetry(TwoWire& bus,
                                     const byte address,
                                     const byte packetIdentifier,
                                     ESATCCSDSPacket& packet,
                                     const byte millisecondsAfterWrites,
                                     const byte tries,
                                     const word millisecondsBetweenRetries)
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
                        tries,
                        millisecondsBetweenRetries);
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
  const ESATCCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (secondaryHeader.packetIdentifier != packetIdentifier)
  {
    return false;
  }
  return true;
}

boolean ESATI2CMaster::readTelemetryPacketData(TwoWire& bus,
                                               const byte address,
                                               ESATCCSDSPacket& packet)
{
  const long packetDataLength = packet.readPacketDataLength();
  packet.rewind();
  long totalBytesRead = 0;
  while (totalBytesRead < packetDataLength)
  {
    byte bytesToRead = I2C_CHUNK_LENGTH;
    if ((totalBytesRead + bytesToRead) > packetDataLength)
    {
      bytesToRead = packetDataLength - totalBytesRead;
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

boolean ESATI2CMaster::readTelemetryPrimaryHeader(TwoWire& bus,
                                                  const byte address,
                                                  ESATCCSDSPacket& packet,
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
  const byte headerBytesRead =
    bus.requestFrom(address, packet.PRIMARY_HEADER_LENGTH);
  if (headerBytesRead != packet.PRIMARY_HEADER_LENGTH)
  {
    return false;
  }
  for (byte i = 0; i < packet.PRIMARY_HEADER_LENGTH; i++)
  {
    packet.primaryHeader[i] = bus.read();
  }
  if (packet.readPacketType() != packet.TELEMETRY)
  {
    return false;
  }
  if (packet.readPacketDataLength() == 0)
  {
    return false;
  }
  if (packet.readPacketDataLength() > packet.packetDataBufferLength)
  {
    return false;
  }
  return true;
}

#include <USBSerial.h>
boolean ESATI2CMaster::readTelemetryStatus(TwoWire& bus,
                                           const byte address,
                                           const byte millisecondsAfterWrites,
                                           const byte tries,
                                           const word millisecondsBetweenRetries)
{
  for (int i = 0; i < tries; i++)
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
        delay(millisecondsBetweenRetries);
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

boolean ESATI2CMaster::writeTelecommand(TwoWire& bus,
                                        const byte address,
                                        ESATCCSDSPacket& packet,
                                        const byte millisecondsAfterWrites,
                                        const byte tries,
                                        const word millisecondsBetweenRetries)
{
  const boolean telecommandStatusCorrect =
    readTelecommandStatus(bus,
                          address,
                          millisecondsAfterWrites,
                          tries,
                          millisecondsBetweenRetries);
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

boolean ESATI2CMaster::writeTelecommandPacketData(TwoWire& bus,
                                                  const byte address,
                                                  ESATCCSDSPacket& packet,
                                                  const byte millisecondsAfterWrites)
{
  packet.rewind();
  while (!packet.endOfPacketDataReached())
  {
    bus.beginTransmission(address);
    (void) bus.write(TELECOMMAND_PACKET_DATA);
    const byte bytesToWrite = I2C_CHUNK_LENGTH - 1;
    for (byte i = 0;
         (i < bytesToWrite) && !packet.endOfPacketDataReached();
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

boolean ESATI2CMaster::writeTelecommandPrimaryHeader(TwoWire& bus,
                                                     const byte address,
                                                     ESATCCSDSPacket& packet,
                                                     const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(TELECOMMAND_PRIMARY_HEADER);
  for (byte i = 0; i < packet.PRIMARY_HEADER_LENGTH; i++)
  {
    (void) bus.write(packet.primaryHeader[i]);
  }
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

boolean ESATI2CMaster::writeTelemetryRequest(TwoWire& bus,
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

ESATI2CMaster I2CMaster;

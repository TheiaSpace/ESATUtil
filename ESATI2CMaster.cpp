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

void ESATI2CMaster::begin(TwoWire& i2cInterface)
{
  bus = &i2cInterface;
}

boolean ESATI2CMaster::readTelemetry(const byte address,
                                     const byte packetIdentifier,
                                     ESATCCSDSPacket& packet,
                                     const byte tries,
                                     const word millisecondsBetweenRetries)
{
  const boolean telemetryRequestCorrect =
    writeTelemetryRequest(address, packetIdentifier);
  if (!telemetryRequestCorrect)
  {
    return false;
  }
  const boolean telemetryReady =
    readTelemetryStatus(address,
                       tries,
                       millisecondsBetweenRetries);
  if (!telemetryReady)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    readTelemetryPrimaryHeader(address, packet);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const boolean packetDataCorrect =
    readTelemetryPacketData(address, packet);
  if (!packetDataCorrect)
  {
    return false;
  }
  (void) packet.readByte();
  (void) packet.readByte();
  (void) packet.readByte();
  const byte receivedPacketIdentifier = packet.readByte();
  if (receivedPacketIdentifier != packetIdentifier)
  {
    return false;
  }
  return true;
}

boolean ESATI2CMaster::readTelecommandStatus(const byte address,
                                             const byte tries,
                                             const word millisecondsBetweenRetries)
{
  for (int i = 0; i < tries; i++)
  {
    bus->beginTransmission(address);
    (void) bus->write(TELECOMMAND_STATUS);
    const byte writeStatus = bus->endTransmission();
    if (writeStatus != 0)
    {
      return false;
    }
    const byte bytesToRead = 1;
    const byte bytesRead = bus->requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    const byte telecommandState = bus->read();
    switch (telecommandState)
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

boolean ESATI2CMaster::readTelemetryPacketData(const byte address,
                                               ESATCCSDSPacket& packet)
{
  const long packetDataLength = packet.readPacketDataLength();
  packet.writePacketDataLength(0);
  while (packet.readPacketDataLength() < packetDataLength)
  {
    byte bytesToRead = I2C_CHUNK_LENGTH;
    if ((packet.readPacketDataLength() + bytesToRead) > packetDataLength)
    {
      bytesToRead = packetDataLength - packet.readPacketDataLength();
    }
    const byte bytesRead = bus->requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    for (byte i = 0; i < bytesRead; i++)
    {
      packet.writeByte(bus->read());
    }
  }
  return true;
}

boolean ESATI2CMaster::readTelemetryPrimaryHeader(const byte address,
                                                  ESATCCSDSPacket& packet)
{
  bus->beginTransmission(address);
  (void) bus->write(TELEMETRY_VECTOR);
  const byte writeStatus = bus->endTransmission();
  if (writeStatus != 0)
  {
    return false;
  }
  const byte headerBytesRead =
    bus->requestFrom(address, packet.PRIMARY_HEADER_LENGTH);
  if (headerBytesRead != packet.PRIMARY_HEADER_LENGTH)
  {
    return false;
  }
  for (byte i = 0; i < packet.PRIMARY_HEADER_LENGTH; i++)
  {
    packet.primaryHeader[i] = bus->read();
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

boolean ESATI2CMaster::readTelemetryStatus(const byte address,
                                           const byte tries,
                                           const word millisecondsBetweenRetries)
{
  for (int i = 0; i < tries; i++)
  {
    bus->beginTransmission(address);
    (void) bus->write(TELEMETRY_STATUS);
    const byte writeStatus = bus->endTransmission();
    if (writeStatus != 0)
    {
      return false;
    }
    const byte bytesToRead = 1;
    const byte bytesRead = bus->requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    const byte telemetryState = bus->read();
    switch (telemetryState)
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

boolean ESATI2CMaster::writeTelecommand(const byte address,
                                        ESATCCSDSPacket& packet,
                                        const byte tries,
                                        const word millisecondsBetweenRetries)
{
  const boolean telecommandStatusCorrect =
    readTelecommandStatus(address,
                          tries,
                          millisecondsBetweenRetries);
  if (!telecommandStatusCorrect)
  {
    return false;
  }
  const boolean telecommandPrimaryHeaderCorrect =
    writeTelecommandPrimaryHeader(address, packet);
  if (!telecommandPrimaryHeaderCorrect)
  {
    return false;
  }
  const boolean telecommandPacketDataCorrect =
    writeTelecommandPacketData(address, packet);
  return telecommandPacketDataCorrect;
}

boolean ESATI2CMaster::writeTelecommandPacketData(const byte address,
                                                  ESATCCSDSPacket& packet)
{
  packet.rewind();
  while (!packet.endOfPacketDataReached())
  {
    bus->beginTransmission(address);
    (void) bus->write(TELECOMMAND_PACKET_DATA);
    const byte bytesToWrite = I2C_CHUNK_LENGTH - 1;
    for (byte i = 0;
         i < bytesToWrite, !packet.endOfPacketDataReached();
         i++)
    {
      (void) bus->write(packet.readByte());
    }
    const byte packetDataStatus = bus->endTransmission();
    if (packetDataStatus != 0)
    {
      return false;
    }
  }
  return true;
}

boolean ESATI2CMaster::writeTelecommandPrimaryHeader(const byte address,
                                                     ESATCCSDSPacket& packet)
{
  bus->beginTransmission(address);
  (void) bus->write(TELECOMMAND_PRIMARY_HEADER);
  for (byte i = 0; i < packet.PRIMARY_HEADER_LENGTH; i++)
  {
    (void) bus->write(packet.primaryHeader[i]);
  }
  const byte writeStatus = bus->endTransmission();
  if (writeStatus != 0)
  {
    return false;
  }
}

boolean ESATI2CMaster::writeTelemetryRequest(const byte address,
                                             const byte packetIdentifier)
{
  bus->beginTransmission(address);
  (void) bus->write(TELEMETRY_REQUEST);
  (void) bus->write(packetIdentifier);
  const byte writeStatus = bus->endTransmission();
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

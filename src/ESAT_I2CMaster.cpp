/*
 * Copyright (C) 2017-2018 Theia Space, Universidad Politécnica de Madrid
 *
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

void ESAT_I2CMasterClass::begin(TwoWire& i2cInterface,
                                const word numberOfAttempts,
                                const word initialNumberOfMillisecondsBetweenAttempts,
                                const float numberOfMillisecondsBetweenAttemptsGrowthFactor)
{
  bus = &i2cInterface;
  millisecondsAfterWrites = 0;
  attempts = numberOfAttempts;
  initialDelay = initialNumberOfMillisecondsBetweenAttempts;
  growthFactor = numberOfMillisecondsBetweenAttemptsGrowthFactor;
}

boolean ESAT_I2CMasterClass::canReadPacket(const byte address)
{
  if (!bus)
  {
    return false;
  }
  for (unsigned long i = 0; i < attempts; i++)
  {
    word retryDelay = initialDelay;
    bus->beginTransmission(address);
    (void) bus->write(READ_STATE);
    const byte writeStatus = bus->endTransmission();
    // Delay for compatiblity with deprecated method readTelemetry().
    delay(millisecondsAfterWrites);
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
    const byte readState = bus->read();
    switch (readState)
    {
      case PACKET_NOT_REQUESTED:
        return false;
        break;
      case PACKET_NOT_READY:
        delay(retryDelay);
        retryDelay = growthFactor * retryDelay;
        break;
      case PACKET_READY:
        return true;
        break;
      case PACKET_REJECTED:
        return false;
        break;
      case PACKET_INVALID:
        return false;
        break;
      case PACKET_DATA_READ_IN_PROGRESS:
        return false;
        break;
      default:
        return false;
        break;
    }
  }
  return false;
}

boolean ESAT_I2CMasterClass::canWritePacket(const byte address)
{
  if (!bus)
  {
    return false;
  }
  for (unsigned long i = 0; i < attempts; i++)
  {
    word retryDelay = initialDelay;
    bus->beginTransmission(address);
    (void) bus->write(WRITE_STATE);
    const byte writeStatus = bus->endTransmission();
    // Delay for compatiblity with deprecated method writeTelecommand().
    delay(millisecondsAfterWrites);
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
    const byte writeState = bus->read();
    switch (writeState)
    {
      case WRITE_BUFFER_EMPTY:
        return true;
        break;
      case PACKET_DATA_WRITE_IN_PROGRESS:
        return true;
      case WRITE_BUFFER_FULL:
        delay(retryDelay);
        retryDelay = growthFactor * retryDelay;
        break;
      default:
        return false;
        break;
    }
  }
  return false;
}

boolean ESAT_I2CMasterClass::packetMatchesRequest(ESAT_CCSDSPacket& packet,
                                                  const int requestedPacket)
{
  const ESAT_CCSDSPrimaryHeader primaryHeader
    = packet.readPrimaryHeader();
  if (requestedPacket == NEXT_TELEMETRY_PACKET_REQUESTED)
  {
    if (primaryHeader.packetType != primaryHeader.TELEMETRY)
    {
      return false;
    }
  }
  if (requestedPacket == NEXT_TELECOMMAND_PACKET_REQUESTED)
  {
    if (primaryHeader.packetType != primaryHeader.TELECOMMAND)
    {
      return false;
    }
  }
  if (requestedPacket > 0) // Named-packet telemmetry request.
  {
    if (primaryHeader.secondaryHeaderFlag !=
        primaryHeader.SECONDARY_HEADER_IS_PRESENT)
    {
      return false;
    }
    if (primaryHeader.packetDataLength < ESAT_CCSDSSecondaryHeader::LENGTH)
    {
      return false;
    }
    packet.rewind();
    const ESAT_CCSDSSecondaryHeader secondaryHeader
      = packet.readSecondaryHeader();
    packet.rewind();
    if (secondaryHeader.packetIdentifier != requestedPacket)
    {
      return false;
    }
  }
  return true;
}

boolean ESAT_I2CMasterClass::readNamedTelemetry(ESAT_CCSDSPacket& packet,
                                                const byte identifier,
                                                const byte address)
{
  if (!bus)
  {
    return false;
  }
  return readPacket(packet,
                    identifier,
                    address);
}

boolean ESAT_I2CMasterClass::readNextTelemetry(ESAT_CCSDSPacket& packet,
                                               const byte address)
{
  if (!bus)
  {
    return false;
  }
  return readPacket(packet,
                    NEXT_TELEMETRY_PACKET_REQUESTED,
                    address);
}

boolean ESAT_I2CMasterClass::readPacket(ESAT_CCSDSPacket& packet,
                                        const int requestedPacket,
                                        const byte address)
{
  if (!bus)
  {
    return false;
  }
  const boolean packetRequestCorrect =
    requestPacket(requestedPacket, address);
  if (!packetRequestCorrect)
  {
    return false;
  }
  const boolean canRead =
    canReadPacket(address);
  if (!canRead)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    readPrimaryHeader(packet, address);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const boolean packetDataCorrect =
    readPacketData(packet, address);
  if (!packetDataCorrect)
  {
    return false;
  }
  const boolean requestMatched =
    packetMatchesRequest(packet, requestedPacket);
  if (!requestMatched)
  {
    return false;
  }
  return true;
}

boolean ESAT_I2CMasterClass::readPacketData(ESAT_CCSDSPacket& packet,
                                            byte address)
{
  if (!bus)
  {
    return false;
  }
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
    const byte bytesRead = bus->requestFrom(address, bytesToRead);
    if (bytesRead != bytesToRead)
    {
      return false;
    }
    for (byte i = 0; i < bytesRead; i++)
    {
      packet.writeByte(bus->read());
    }
    totalBytesRead = totalBytesRead + bytesRead;
  }
  return true;
}

boolean ESAT_I2CMasterClass::readPrimaryHeader(ESAT_CCSDSPacket& packet,
                                               const byte address)
{
  if (!bus)
  {
    return false;
  }
  bus->beginTransmission(address);
  (void) bus->write(READ_PACKET);
  const byte writeStatus = bus->endTransmission();
  if (writeStatus != 0)
  {
    return false;
  }
  // Delay for compatiblity with deprecated method readTelemetry().
  delay(millisecondsAfterWrites);
  ESAT_CCSDSPrimaryHeader primaryHeader;
  const byte headerBytesRead =
    bus->requestFrom(address, primaryHeader.LENGTH);
  if (headerBytesRead != primaryHeader.LENGTH)
  {
    return false;
  }
  const boolean correctRead = primaryHeader.readFrom(*bus);
  if (!correctRead)
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

ESAT_SemanticVersionNumber ESAT_I2CMasterClass::readProtocolVersionNumber(const byte address)
{
  ESAT_SemanticVersionNumber version(0, 0, 0);
  if (!bus)
  {
    return version;
  }
  bus->beginTransmission(address);
  (void) bus->write(PROTOCOL_VERSION_NUMBER);
  const byte writeStatus = bus->endTransmission();
  if (writeStatus != 0)
  {
    return version;
  }
  const byte bytesToRead = ESAT_SemanticVersionNumber::LENGTH;
  const byte bytesRead = bus->requestFrom(address, bytesToRead);
  if (bytesRead != bytesToRead)
  {
    return version;
  }
  (void) version.readFrom(*bus);
  return version;
}

boolean ESAT_I2CMasterClass::readTelemetry(TwoWire& i2cInterface,
                                           const byte address,
                                           const byte packetIdentifier,
                                           ESAT_CCSDSPacket& packet,
                                           const byte numberOfMillisecondsAfterWrites,
                                           const byte numberOfAttempts,
                                           const word numberOfMillisecondsBetweenAttempts)
{
  // Save the original configuration.
  TwoWire* const originalBus =
    bus;
  const word originalMillisecondsAfterWrites =
    millisecondsAfterWrites;
  const word originalAttempts =
    attempts;
  const word originalInitialDelay =
    initialDelay;
  const float originalGrowthFactor =
    growthFactor;
  // Set a temporary configuration in agreement with the arguments.
  bus = &i2cInterface;
  millisecondsAfterWrites = numberOfMillisecondsAfterWrites;
  attempts = numberOfAttempts;
  initialDelay = numberOfMillisecondsBetweenAttempts;
  growthFactor = 1;
  // Use the new application programming interface to read the packet.
  const boolean correctPacket = readPacket(packet,
                                           packetIdentifier,
                                           address);
  // Recover the original configuration.
  bus = originalBus;
  millisecondsAfterWrites = originalMillisecondsAfterWrites;
  attempts = originalAttempts;
  initialDelay = originalInitialDelay;
  growthFactor = originalGrowthFactor;
  // Return the result of the packet read.
  return correctPacket;
}

boolean ESAT_I2CMasterClass::readTelecommand(ESAT_CCSDSPacket& packet,
                                             const byte address)
{
  if (!bus)
  {
    return false;
  }
  return readPacket(packet,
                    NEXT_TELECOMMAND_PACKET_REQUESTED,
                    address);
}

boolean ESAT_I2CMasterClass::requestPacket(const int requestedPacket,
                                           const byte address)
{
  if (!bus)
  {
    return false;
  }
  bus->beginTransmission(address);
  switch (requestedPacket)
  {
    case NEXT_TELEMETRY_PACKET_REQUESTED:
      (void) bus->write(READ_TELEMETRY);
      break;
    case NEXT_TELECOMMAND_PACKET_REQUESTED:
      (void) bus->write(READ_TELECOMMAND);
      break;
    default:
      (void) bus->write(READ_TELEMETRY);
      (void) bus->write(byte(requestedPacket));
      break;
  }
  const byte writeStatus = bus->endTransmission();
  // Delay for compatiblity with deprecated method readTelemetry().
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

boolean ESAT_I2CMasterClass::resetTelemetryQueue(const byte address)
{
  if (!bus)
  {
    return false;
  }
  bus->beginTransmission(address);
  (void) bus->write(RESET_TELEMETRY_QUEUE);
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

boolean ESAT_I2CMasterClass::writePacket(ESAT_CCSDSPacket& packet,
                                         const byte address)
{
  if (!bus)
  {
    return false;
  }
  const boolean canWrite =
    canWritePacket(address);
  if (!canWrite)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    writePrimaryHeader(packet, address);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const boolean packetDataCorrect =
    writePacketData(packet, address);
  return packetDataCorrect;
}

boolean ESAT_I2CMasterClass::writePacketData(ESAT_CCSDSPacket& packet,
                                             const byte address)
{
  if (!bus)
  {
    return false;
  }
  packet.rewind();
  while (packet.available() > 0)
  {
    bus->beginTransmission(address);
    (void) bus->write(WRITE_PACKET_DATA);
    const byte bytesToWrite = I2C_CHUNK_LENGTH - 1;
    for (byte i = 0;
         (i < bytesToWrite) && (packet.available() > 0);
         i++)
    {
      (void) bus->write(packet.readByte());
    }
    const byte writeStatus = bus->endTransmission();
    // Delay for compatiblity with deprecated method writeTelecommand().
    delay(millisecondsAfterWrites);
    if (writeStatus != 0)
    {
      return false;
    }
  }
  return true;
}

boolean ESAT_I2CMasterClass::writePrimaryHeader(ESAT_CCSDSPacket& packet,
                                                const byte address)
{
  if (!bus)
  {
    return false;
  }
  bus->beginTransmission(address);
  (void) bus->write(WRITE_PRIMARY_HEADER);
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  (void) primaryHeader.writeTo(*bus);
  const byte writeStatus = bus->endTransmission();
  // Delay for compatiblity with deprecated method writeTelecommand().
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

boolean ESAT_I2CMasterClass::writeTelecommand(TwoWire& i2cInterface,
                                              const byte address,
                                              ESAT_CCSDSPacket& packet,
                                              const byte numberOfMillisecondsAfterWrites,
                                              const byte numberOfAttempts,
                                              const word numberOfMillisecondsBetweenAttempts)
{
  // Save the original configuration.
  TwoWire* const originalBus =
    bus;
  const word originalMillisecondsAfterWrites =
    millisecondsAfterWrites;
  const word originalAttempts =
    attempts;
  const word originalInitialDelay =
    initialDelay;
  const float originalGrowthFactor =
    growthFactor;
  // Set a temporary configuration in agreement with the arguments.
  bus = &i2cInterface;
  millisecondsAfterWrites = numberOfMillisecondsAfterWrites;
  attempts = numberOfAttempts;
  initialDelay = numberOfMillisecondsBetweenAttempts;
  growthFactor = 1;
  // Use the new application programming interface to write the packet.
  const boolean correctPacket = writePacket(packet, address);
  // Recover the original configuration.
  bus = originalBus;
  millisecondsAfterWrites = originalMillisecondsAfterWrites;
  attempts = originalAttempts;
  initialDelay = originalInitialDelay;
  growthFactor = originalGrowthFactor;
  // Return the result of the packet write.
  return correctPacket;
}

ESAT_I2CMasterClass ESAT_I2CMaster;

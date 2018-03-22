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

boolean ESAT_I2CMasterClass::canReadPacket(TwoWire& bus,
                                           const byte address,
                                           const byte millisecondsAfterWrites,
                                           const byte attempts,
                                           const word millisecondsBetweenAttempts)
{
  for (int i = 0; i < attempts; i++)
  {
    bus.beginTransmission(address);
    (void) bus.write(READ_STATE);
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
    const byte readState = bus.read();
    switch (readState)
    {
      case PACKET_NOT_REQUESTED:
        return false;
        break;
      case PACKET_NOT_READY:
        delay(millisecondsBetweenAttempts);
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

boolean ESAT_I2CMasterClass::canWritePacket(TwoWire& bus,
                                            const byte address,
                                            const byte millisecondsAfterWrites,
                                            const byte attempts,
                                            const word millisecondsBetweenAttempts)
{
  for (int i = 0; i < attempts; i++)
  {
    bus.beginTransmission(address);
    (void) bus.write(WRITE_STATE);
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
    const byte writeState = bus.read();
    switch (writeState)
    {
      case WRITE_BUFFER_EMPTY:
        return true;
        break;
      case PACKET_DATA_WRITE_IN_PROGRESS:
        return true;
      case WRITE_BUFFER_FULL:
        delay(millisecondsBetweenAttempts);
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

boolean ESAT_I2CMasterClass::readTelemetry(TwoWire& bus,
                                           const byte address,
                                           ESAT_CCSDSPacket& packet,
                                           const byte millisecondsAfterWrites,
                                           const byte attempts,
                                           const word millisecondsBetweenAttempts)
{
  return readPacket(bus,
                    address,
                    NEXT_TELEMETRY_PACKET_REQUESTED,
                    packet,
                    millisecondsAfterWrites,
                    attempts,
                    millisecondsBetweenAttempts);
}

boolean ESAT_I2CMasterClass::readTelemetry(TwoWire& bus,
                                           const byte address,
                                           const byte packetIdentifier,
                                           ESAT_CCSDSPacket& packet,
                                           const byte millisecondsAfterWrites,
                                           const byte attempts,
                                           const word millisecondsBetweenAttempts)
{
  return readPacket(bus,
                    address,
                    packetIdentifier,
                    packet,
                    millisecondsAfterWrites,
                    attempts,
                    millisecondsBetweenAttempts);
}

boolean ESAT_I2CMasterClass::readTelecommand(TwoWire& bus,
                                             const byte address,
                                             ESAT_CCSDSPacket& packet,
                                             const byte millisecondsAfterWrites,
                                             const byte attempts,
                                             const word millisecondsBetweenAttempts)
{
  return readPacket(bus,
                    address,
                    NEXT_TELECOMMAND_PACKET_REQUESTED,
                    packet,
                    millisecondsAfterWrites,
                    attempts,
                    millisecondsBetweenAttempts);
}

boolean ESAT_I2CMasterClass::readPacket(TwoWire& bus,
                                        const byte address,
                                        const int requestedPacket,
                                        ESAT_CCSDSPacket& packet,
                                        const byte millisecondsAfterWrites,
                                        const byte attempts,
                                        const byte millisecondsBetweenAttempts)
{
  const boolean packetRequestCorrect =
    requestPacket(bus,
                  address,
                  NEXT_TELEMETRY_PACKET_REQUESTED,
                  millisecondsAfterWrites);
  if (!packetRequestCorrect)
  {
    return false;
  }
  const boolean canRead =
    canReadPacket(bus,
                  address,
                  millisecondsAfterWrites,
                  attempts,
                  millisecondsBetweenAttempts);
  if (!canRead)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    readPrimaryHeader(bus,
                      address,
                      packet,
                      millisecondsAfterWrites);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const ESAT_CCSDSPrimaryHeader primaryHeader = packet.readPrimaryHeader();
  const boolean packetDataCorrect =
    readPacketData(bus,
                   address,
                   packet);
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

boolean ESAT_I2CMasterClass::readPacketData(TwoWire& bus,
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

boolean ESAT_I2CMasterClass::readPrimaryHeader(TwoWire& bus,
                                               const byte address,
                                               ESAT_CCSDSPacket& packet,
                                               const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(READ_PACKET);
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
  if (primaryHeader.packetDataLength > packet.capacity())
  {
    return false;
  }
  packet.writePrimaryHeader(primaryHeader);
  return true;
}

boolean ESAT_I2CMasterClass::requestPacket(TwoWire& bus,
                                           const byte address,
                                           const int requestedPacket,
                                           const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  switch (requestedPacket)
  {
    case NEXT_TELEMETRY_PACKET_REQUESTED:
      (void) bus.write(READ_TELEMETRY);
      break;
    case NEXT_TELECOMMAND_PACKET_REQUESTED:
      (void) bus.write(READ_TELECOMMAND);
      break;
    default:
      (void) bus.write(READ_TELEMETRY);
      (void) bus.write(byte(requestedPacket));
      break;
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

boolean ESAT_I2CMasterClass::resetTelemetryQueue(TwoWire& bus,
                                                 const byte address)
{
  bus.beginTransmission(address);
  (void) bus.write(RESET_TELEMETRY_QUEUE);
  const byte writeStatus = bus.endTransmission();
  if (writeStatus == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

boolean ESAT_I2CMasterClass::writePacket(TwoWire& bus,
                                         const byte address,
                                         ESAT_CCSDSPacket& packet,
                                         const byte millisecondsAfterWrites,
                                         const byte attempts,
                                         const word millisecondsBetweenAttempts)
{
  const boolean canWrite =
    canWritePacket(bus,
                   address,
                   millisecondsAfterWrites,
                   attempts,
                   millisecondsBetweenAttempts);
  if (!canWrite)
  {
    return false;
  }
  const boolean primaryHeaderCorrect =
    writePrimaryHeader(bus,
                       address,
                       packet,
                       millisecondsAfterWrites);
  if (!primaryHeaderCorrect)
  {
    return false;
  }
  const boolean packetDataCorrect =
    writePacketData(bus,
                    address,
                    packet,
                    millisecondsAfterWrites);
  return packetDataCorrect;
}

boolean ESAT_I2CMasterClass::writePacketData(TwoWire& bus,
                                             const byte address,
                                             ESAT_CCSDSPacket& packet,
                                             const byte millisecondsAfterWrites)
{
  packet.rewind();
  while (packet.available() > 0)
  {
    bus.beginTransmission(address);
    (void) bus.write(WRITE_PACKET_DATA);
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

boolean ESAT_I2CMasterClass::writePrimaryHeader(TwoWire& bus,
                                                const byte address,
                                                ESAT_CCSDSPacket& packet,
                                                const byte millisecondsAfterWrites)
{
  bus.beginTransmission(address);
  (void) bus.write(WRITE_PRIMARY_HEADER);
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

ESAT_I2CMasterClass ESAT_I2CMaster;

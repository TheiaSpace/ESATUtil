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

#include "ESATI2CSlave.h"

void ESATI2CSlave::begin(TwoWire& i2cInterface,
                         byte telecommandPacketDataBuffer[],
                         const word telecommandPacketDataBufferLength,
                         byte telemetryPacketDataBuffer[],
                         const word telemetryPacketDataBufferLength)
{
  bus = &i2cInterface;
  receiveState = IDLE;
  requestState = IDLE;
  telecommand = ESATCCSDSPacket(telecommandPacketDataBuffer,
                                telecommandPacketDataBufferLength);
  telecommandState = TELECOMMAND_NOT_PENDING;
  telemetry = ESATCCSDSPacket(telemetryPacketDataBuffer,
                              telemetryPacketDataBufferLength);
  telemetryState = TELEMETRY_NOT_REQUESTED;
  bus->onReceive(receiveEvent);
  bus->onRequest(requestEvent);
}

void ESATI2CSlave::handleTelecommandPacketDataReception(const byte message[],
                                                        const int messageLength)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (telecommandState != HANDLE_TELECOMMAND_PACKET_DATA)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  for (long i = 0; i < messageLength; i++)
  {
    telecommand.writeByte(message[i]);
    if (telecommand.readPacketDataLength()
        >= telecommandPacketDataLength)
    {
      telecommandState = TELECOMMAND_PENDING;
      receiveState = IDLE;
    }
  }
  requestState = IDLE;
}

void ESATI2CSlave::handleTelecommandPrimaryHeaderReception(const byte message[],
                                                           const int messageLength)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (messageLength != telecommand.PRIMARY_HEADER_LENGTH)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  for (byte i = 0; i < messageLength; i++)
  {
    telecommand.primaryHeader[i] = message[i];
  }
  if (telecommand.readPacketType() != telecommand.TELECOMMAND)
  {
    telecommand.writePacketDataLength(0);
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  telecommandPacketDataLength = telecommand.readPacketDataLength();
  telecommand.writePacketDataLength(0);
  if ((telecommandPacketDataLength < 1)
      || (telecommandPacketDataLength > 65536))
  {
    receiveState = IDLE;
  }
  else
  {
    receiveState = HANDLE_TELECOMMAND_PACKET_DATA;
  }
  requestState = IDLE;
}

void ESATI2CSlave::handleTelecommandStatusReception(const byte message[],
                                                    const int messageLength)
{
  if (messageLength != 1)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELECOMMAND_STATUS;
}

void ESATI2CSlave::handleTelemetryRequestReception(const byte message[],
                                                   const int messageLength)
{
  if (messageLength != 1)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  telemetryPacketIdentifier = message[0];
  telemetryState = TELEMETRY_NOT_READY;
  receiveState = IDLE;
  requestState = HANDLE_TELEMETRY_REQUEST;
}

void ESATI2CSlave::handleTelemetryStatusReception(const byte message[],
                                                  const int messageLength)
{
  if (messageLength != 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELEMETRY_STATUS;
}

void ESATI2CSlave::handleTelemetryVectorReception(const byte message[],
                                                  const int messageLength)
{
  if (messageLength != 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (telemetry.readPacketDataLength() == 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELEMETRY_VECTOR_PRIMARY_HEADER;
}

void ESATI2CSlave::handleTelecommandStatusRequest()
{
  (void) bus->write(telecommandState);
  requestState = IDLE;
}

void ESATI2CSlave::handleTelemetryStatusRequest()
{
  (void) bus->write(telemetryState);
  requestState = IDLE;
}

void ESATI2CSlave::handleTelemetryVectorPacketDataRequest()
{
  if (telemetryState != TELEMETRY_READY)
  {
    requestState = IDLE;
    return;
  }
  for (byte i = 0; i < I2C_CHUNK_LENGTH; i++)
  {
    (void) bus->write(telemetry.readByte());
    if (telemetry.endOfPacketDataReached())
    {
      telemetry.writePacketDataLength(0);
      requestState = IDLE;
      telemetryState = TELEMETRY_NOT_REQUESTED;
    }
  }
}

void ESATI2CSlave::handleTelemetryVectorPrimaryHeaderRequest()
{
  if (telemetryState != TELEMETRY_READY)
  {
    requestState = IDLE;
    return;
  }
  for (byte i = 0; i < telemetry.PRIMARY_HEADER_LENGTH; i++)
  {
    (void) bus->write(telemetry.primaryHeader[i]);
  }
  requestState = HANDLE_TELEMETRY_VECTOR_PACKET_DATA;
}

boolean ESATI2CSlave::readTelecommand(ESATCCSDSPacket& packet)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    boolean successfulCopy = telecommand.copyTo(packet);
    telecommandState = TELECOMMAND_NOT_PENDING;
    return successfulCopy;
  }
  else
  {
    return false;
  }
}

void ESATI2CSlave::receiveEvent(const int numberOfBytes)
{
  if (numberOfBytes < 1)
  {
    return;
  }
  const byte registerNumber = I2CSlave.bus->read();
  const int messageLength = numberOfBytes - 1;
  byte message[messageLength];
  for (long i = 0; i < numberOfBytes; i++)
  {
    message[i] = I2CSlave.bus->read();
  }
  switch (registerNumber)
  {
    case TELECOMMAND_PRIMARY_HEADER:
      I2CSlave.handleTelecommandPrimaryHeaderReception(message, messageLength);
      break;
    case TELECOMMAND_PACKET_DATA:
      I2CSlave.handleTelecommandPacketDataReception(message, messageLength);
      break;
    case TELECOMMAND_STATUS:
      I2CSlave.handleTelecommandStatusReception(message, messageLength);
      break;
    case TELEMETRY_REQUEST:
      I2CSlave.handleTelemetryRequestReception(message, messageLength);
      break;
    case TELEMETRY_STATUS:
      I2CSlave.handleTelemetryStatusReception(message, messageLength);
      break;
    case TELEMETRY_VECTOR:
      I2CSlave.handleTelemetryVectorReception(message, messageLength);
      break;
    default:
      I2CSlave.receiveState = IDLE;
      break;
  }
}

int ESATI2CSlave::requestedTelemetryPacket()
{
  if ((telemetryPacketIdentifier >= 0)
      && (telemetryPacketIdentifier <= 255))
  {
    return telemetryPacketIdentifier;
  }
  else
  {
    return -1;
  }
}

void ESATI2CSlave::requestEvent()
{
  switch (I2CSlave.requestState)
  {
    case HANDLE_TELECOMMAND_STATUS:
      I2CSlave.handleTelecommandStatusRequest();
      break;
    case HANDLE_TELEMETRY_STATUS:
      I2CSlave.handleTelemetryStatusRequest();
      break;
    case HANDLE_TELEMETRY_VECTOR_PRIMARY_HEADER:
      I2CSlave.handleTelemetryVectorPrimaryHeaderRequest();
      break;
    case HANDLE_TELEMETRY_VECTOR_PACKET_DATA:
      I2CSlave.handleTelemetryVectorPacketDataRequest();
      break;
    default:
      break;
  }
}

void ESATI2CSlave::rejectTelemetryRequest()
{
  if (telemetryState == TELEMETRY_NOT_READY)
  {
    telemetryState = TELEMETRY_REQUEST_REJECTED;
  }
}

void ESATI2CSlave::writeTelemetry(ESATCCSDSPacket& packet)
{
  if (telemetryState != TELEMETRY_NOT_READY)
  {
    return;
  }
  if (packet.readPacketType() != packet.TELEMETRY)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  packet.rewind();
  (void) packet.readByte();
  (void) packet.readByte();
  (void) packet.readByte();
  const byte packetIdentifier = packet.readByte();
  if (packetIdentifier != telemetryPacketIdentifier)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  const boolean successfulCopy =  packet.copyTo(telemetry);
  if (!successfulCopy)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  telemetryState = TELEMETRY_READY;
}

ESATI2CSlave I2CSlave;

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

#include "ESAT_I2CSlave.h"

void ESAT_I2CSlaveClass::begin(TwoWire& i2cInterface,
                               byte telecommandPacketDataBuffer[],
                               const word telecommandPacketDataBufferLength,
                               byte telemetryPacketDataBuffer[],
                               const word telemetryPacketDataBufferLength)
{
  bus = &i2cInterface;
  receiveState = IDLE;
  requestState = IDLE;
  telecommand = ESAT_CCSDSPacket(telecommandPacketDataBuffer,
                                 telecommandPacketDataBufferLength);
  telecommandState = TELECOMMAND_NOT_PENDING;
  telemetry = ESAT_CCSDSPacket(telemetryPacketDataBuffer,
                               telemetryPacketDataBufferLength);
  telemetryState = TELEMETRY_NOT_REQUESTED;
  bus->onReceive(receiveEvent);
  bus->onRequest(requestEvent);
}

void ESAT_I2CSlaveClass::handleTelecommandPacketDataReception(const byte message[],
                                                              const int messageLength)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (receiveState != HANDLE_TELECOMMAND_PACKET_DATA)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  const long packetDataLength = telecommand.readPacketDataLength();
  for (long i = 0;
       (i < messageLength)
         && (telecommandPacketDataBytesReceived < packetDataLength);
       i++)
  {
    telecommand.packetData[telecommandPacketDataBytesReceived] = message[i];
    telecommandPacketDataBytesReceived = telecommandPacketDataBytesReceived + 1;
    if (telecommandPacketDataBytesReceived >= packetDataLength)
    {
      telecommandState = TELECOMMAND_PENDING;
      receiveState = IDLE;
    }
  }
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelecommandPrimaryHeaderReception(const byte message[],
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
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  telecommand.rewind();
  telecommandPacketDataBytesReceived = 0;
  receiveState = HANDLE_TELECOMMAND_PACKET_DATA;
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelecommandStatusReception(const byte message[],
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

void ESAT_I2CSlaveClass::handleTelemetryRequestReception(const byte message[],
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

void ESAT_I2CSlaveClass::handleTelemetryStatusReception(const byte message[],
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

void ESAT_I2CSlaveClass::handleTelemetryVectorReception(const byte message[],
                                                        const int messageLength)
{
  if (messageLength != 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELEMETRY_VECTOR_PRIMARY_HEADER;
}

void ESAT_I2CSlaveClass::handleTelecommandStatusRequest()
{
  (void) bus->write(telecommandState);
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelemetryStatusRequest()
{
  (void) bus->write(telemetryState);
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelemetryVectorPacketDataRequest()
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
      requestState = IDLE;
      telemetryState = TELEMETRY_NOT_REQUESTED;
    }
  }
}

void ESAT_I2CSlaveClass::handleTelemetryVectorPrimaryHeaderRequest()
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
  telemetry.rewind();
  requestState = HANDLE_TELEMETRY_VECTOR_PACKET_DATA;
}

boolean ESAT_I2CSlaveClass::readTelecommand(ESAT_CCSDSPacket& packet)
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

void ESAT_I2CSlaveClass::receiveEvent(const int numberOfBytes)
{
  if (numberOfBytes < 1)
  {
    return;
  }
  const byte registerNumber = ESAT_I2CSlave.bus->read();
  const int messageLength = numberOfBytes - 1;
  byte message[messageLength];
  for (long i = 0; i < messageLength; i++)
  {
    message[i] = ESAT_I2CSlave.bus->read();
  }
  switch (registerNumber)
  {
    case TELECOMMAND_PRIMARY_HEADER:
      ESAT_I2CSlave.handleTelecommandPrimaryHeaderReception(message,
                                                            messageLength);
      break;
    case TELECOMMAND_PACKET_DATA:
      ESAT_I2CSlave.handleTelecommandPacketDataReception(message,
                                                         messageLength);
      break;
    case TELECOMMAND_STATUS:
      ESAT_I2CSlave.handleTelecommandStatusReception(message,
                                                     messageLength);
      break;
    case TELEMETRY_REQUEST:
      ESAT_I2CSlave.handleTelemetryRequestReception(message,
                                                    messageLength);
      break;
    case TELEMETRY_STATUS:
      ESAT_I2CSlave.handleTelemetryStatusReception(message,
                                                   messageLength);
      break;
    case TELEMETRY_VECTOR:
      ESAT_I2CSlave.handleTelemetryVectorReception(message,
                                                   messageLength);
      break;
    default:
      ESAT_I2CSlave.receiveState = IDLE;
      break;
  }
}

int ESAT_I2CSlaveClass::requestedTelemetryPacket()
{
  if (telemetryState != TELEMETRY_NOT_READY)
  {
    return NO_TELEMETRY_PACKET_REQUESTED;
  }
  if (telemetryPacketIdentifier < 0)
  {
    return NO_TELEMETRY_PACKET_REQUESTED;
  }
  if (telemetryPacketIdentifier > 255)
  {
    return NO_TELEMETRY_PACKET_REQUESTED;
  }
  return telemetryPacketIdentifier;
}

void ESAT_I2CSlaveClass::requestEvent()
{
  switch (ESAT_I2CSlave.requestState)
  {
    case HANDLE_TELECOMMAND_STATUS:
      ESAT_I2CSlave.handleTelecommandStatusRequest();
      break;
    case HANDLE_TELEMETRY_STATUS:
      ESAT_I2CSlave.handleTelemetryStatusRequest();
      break;
    case HANDLE_TELEMETRY_VECTOR_PRIMARY_HEADER:
      ESAT_I2CSlave.handleTelemetryVectorPrimaryHeaderRequest();
      break;
    case HANDLE_TELEMETRY_VECTOR_PACKET_DATA:
      ESAT_I2CSlave.handleTelemetryVectorPacketDataRequest();
      break;
    default:
      break;
  }
}

void ESAT_I2CSlaveClass::rejectTelemetryRequest()
{
  if (telemetryState == TELEMETRY_NOT_READY)
  {
    telemetryState = TELEMETRY_REQUEST_REJECTED;
  }
}

void ESAT_I2CSlaveClass::writeTelemetry(ESAT_CCSDSPacket& packet)
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
  const ESAT_CCSDSSecondaryHeader secondaryHeader =
    packet.readSecondaryHeader();
  if (secondaryHeader.packetIdentifier != telemetryPacketIdentifier)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  telemetry.clear();
  const boolean successfulCopy = packet.copyTo(telemetry);
  if (!successfulCopy)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  telemetryState = TELEMETRY_READY;
}

ESAT_I2CSlaveClass ESAT_I2CSlave;

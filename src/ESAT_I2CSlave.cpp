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

void ESAT_I2CSlaveClass::handleTelecommandPacketDataReception(ESAT_Buffer message)
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
  while ((message.available() > 0)
         && (telecommandPacketDataBytesReceived
             < telecommandPacketDataLength))
  {
    telecommand.writeByte(message.read());
    telecommandPacketDataBytesReceived =
      telecommandPacketDataBytesReceived + 1;
    if (telecommandPacketDataBytesReceived >=
        telecommandPacketDataLength)
    {
      telecommand.rewind();
      telecommandState = TELECOMMAND_PENDING;
      receiveState = IDLE;
    }
  }
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelecommandPrimaryHeaderReception(ESAT_Buffer message)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (message.length() != ESAT_CCSDSPrimaryHeader::LENGTH)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  message.rewind();
  ESAT_CCSDSPrimaryHeader primaryHeader;
  const boolean correctRead = primaryHeader.readFrom(message);
  if (!correctRead)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  if (primaryHeader.packetType
      != primaryHeader.TELECOMMAND)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  telecommand.rewind();
  telecommand.writePrimaryHeader(primaryHeader);
  telecommandPacketDataBytesReceived = 0;
  telecommandPacketDataLength = primaryHeader.packetDataLength;
  receiveState = HANDLE_TELECOMMAND_PACKET_DATA;
  requestState = IDLE;
}

void ESAT_I2CSlaveClass::handleTelecommandStatusReception(ESAT_Buffer message)
{
  if (message.length() != 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELECOMMAND_STATUS;
}

void ESAT_I2CSlaveClass::handleTelemetryRequestReception(ESAT_Buffer message)
{
  if (message.length() == 1)
  {
    telemetryPacketIdentifier = (int)message.read();
    telemetryState = TELEMETRY_NOT_READY;
    receiveState = IDLE;
    requestState = HANDLE_TELEMETRY_REQUEST;
  }
  else if (message.length() == 0)
  {
    telemetryPacketIdentifier = NEXT_TELEMETRY_PACKET_REQUESTED;
    telemetryState = TELEMETRY_NOT_READY;
    receiveState = IDLE;
    requestState = HANDLE_TELEMETRY_REQUEST;
  }
  else
  {
    receiveState = IDLE;
    requestState = IDLE;
  }
}

void ESAT_I2CSlaveClass::handleTelemetryStatusReception(ESAT_Buffer message)
{
  if (message.length() != 0)
  {
    receiveState = IDLE;
    requestState = IDLE;
    return;
  }
  receiveState = IDLE;
  requestState = HANDLE_TELEMETRY_STATUS;
}

void ESAT_I2CSlaveClass::handleTelemetryVectorReception(ESAT_Buffer message)
{
  if (message.length() != 0)
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
    if (telemetry.available() == 0)
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
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    telemetry.readPrimaryHeader();
  (void) primaryHeader.writeTo(*bus);
  telemetry.rewind();
  requestState = HANDLE_TELEMETRY_VECTOR_PACKET_DATA;
}

boolean ESAT_I2CSlaveClass::readTelecommand(ESAT_CCSDSPacket& packet)
{
  if (telecommandState == TELECOMMAND_PENDING)
  {
    boolean successfulCopy = telecommand.copyTo(packet);
    packet.rewind();
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
  byte octets[messageLength];
  ESAT_Buffer message(octets, sizeof(octets));
  const boolean correctRead = message.readFrom(*(ESAT_I2CSlave.bus),
                                               messageLength);
  if (!correctRead)
  {
    return;
  }
  switch (registerNumber)
  {
    case TELECOMMAND_PRIMARY_HEADER:
      ESAT_I2CSlave.handleTelecommandPrimaryHeaderReception(message);
      break;
    case TELECOMMAND_PACKET_DATA:
      ESAT_I2CSlave.handleTelecommandPacketDataReception(message);
      break;
    case TELECOMMAND_STATUS:
      ESAT_I2CSlave.handleTelecommandStatusReception(message);
      break;
    case TELEMETRY_REQUEST:
      ESAT_I2CSlave.handleTelemetryRequestReception(message);
      break;
    case TELEMETRY_STATUS:
      ESAT_I2CSlave.handleTelemetryStatusReception(message);
      break;
    case TELEMETRY_VECTOR:
      ESAT_I2CSlave.handleTelemetryVectorReception(message);
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
  if (telemetryPacketIdentifier == NEXT_TELEMETRY_PACKET_REQUESTED)
  {
    return NEXT_TELEMETRY_PACKET_REQUESTED;
  }
  else if (telemetryPacketIdentifier < 0)
  {
    return NO_TELEMETRY_PACKET_REQUESTED;
  }
  else if (telemetryPacketIdentifier > 255)
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
  packet.rewind();
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    packet.readPrimaryHeader();
  if (primaryHeader.packetType != primaryHeader.TELEMETRY)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  if (telemetryPacketIdentifier != NEXT_TELEMETRY_PACKET_REQUESTED)
  {
    const ESAT_CCSDSSecondaryHeader secondaryHeader =
      packet.readSecondaryHeader();
    if (secondaryHeader.packetIdentifier != telemetryPacketIdentifier)
    {
      telemetryState = TELEMETRY_INVALID;
      return;
    }
  }
  const boolean successfulCopy = packet.copyTo(telemetry);
  if (!successfulCopy)
  {
    telemetryState = TELEMETRY_INVALID;
    return;
  }
  telemetryState = TELEMETRY_READY;
}

ESAT_I2CSlaveClass ESAT_I2CSlave;

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
                               byte masterWritePacketDataBuffer[],
                               const word masterWritePacketDataBufferLength,
                               byte masterReadPacketDataBuffer[],
                               const word masterReadPacketDataBufferLength)
{
  bus = &i2cInterface;
  i2cState = IDLE;
  masterWritePacket = ESAT_CCSDSPacket(masterWritePacketDataBuffer,
                                       masterWritePacketDataBufferLength);
  masterWriteState = WRITE_BUFFER_EMPTY;
  masterReadPacket = ESAT_CCSDSPacket(masterReadPacketDataBuffer,
                                      masterReadPacketDataBufferLength);
  masterReadState = PACKET_NOT_REQUESTED;
  bus->onReceive(receiveEvent);
  bus->onRequest(requestEvent);
}

void ESAT_I2CSlaveClass::handleWritePrimaryHeaderReception(ESAT_Buffer message)
{
  i2cState = IDLE;
  if (masterWriteState == WRITE_BUFFER_FULL)
  {
    return;
  }
  if (message.length() != ESAT_CCSDSPrimaryHeader::LENGTH)
  {
    return;
  }
  message.rewind();
  ESAT_CCSDSPrimaryHeader primaryHeader;
  const boolean correctRead = primaryHeader.readFrom(message);
  if (!correctRead)
  {
    return;
  }
  masterWritePacket.rewind();
  masterWritePacket.writePrimaryHeader(primaryHeader);
  masterWritePacketDataBytesReceived = 0;
  masterWritePacketDataLength = primaryHeader.packetDataLength;
  masterWriteState = PACKET_DATA_WRITE_IN_PROGRESS;
}

void ESAT_I2CSlaveClass::handleWritePacketDataReception(ESAT_Buffer message)
{
  i2cState = IDLE;
  if (masterWriteState != PACKET_DATA_WRITE_IN_PROGRESS)
  {
    return;
  }
  while ((message.available() > 0)
         && (masterWritePacketDataBytesReceived
             < masterWritePacketDataLength))
  {
    masterWritePacket.writeByte(message.read());
    masterWritePacketDataBytesReceived =
      masterWritePacketDataBytesReceived + 1;
    if (masterWritePacketDataBytesReceived >=
        masterWritePacketDataLength)
    {
      masterWritePacket.rewind();
      masterWriteState = WRITE_BUFFER_FULL;
    }
  }
}

void ESAT_I2CSlaveClass::handleWriteStateReception(ESAT_Buffer message)
{
  if (message.length() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_WRITE_STATE;
  }
}

void ESAT_I2CSlaveClass::handleReadTelemetryReception(ESAT_Buffer message)
{
  i2cState = IDLE;
  if (message.length() == 1)
  {
    masterReadRequestedPacket = message.read();
    masterReadState = PACKET_NOT_READY;
  }
  else if (message.length() == 0)
  {
    masterReadRequestedPacket = NEXT_TELEMETRY_PACKET_REQUESTED;
    masterReadState = PACKET_NOT_READY;
  }
}

void ESAT_I2CSlaveClass::handleReadStateReception(ESAT_Buffer message)
{
  if (message.length() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_READ_STATE;
  }
}

void ESAT_I2CSlaveClass::handleReadPacketReception(ESAT_Buffer message)
{
  if (message.length() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_READ_PACKET;
  }
}

void ESAT_I2CSlaveClass::handleReadTelecommandReception(ESAT_Buffer message)
{
  i2cState = IDLE;
  if (message.length() == 0)
  {
    masterReadRequestedPacket = NEXT_TELECOMMAND_PACKET_REQUESTED;
    masterReadState = PACKET_NOT_READY;
  }
}

void ESAT_I2CSlaveClass::handleResetTelemetryQueueReception(ESAT_Buffer message)
{
  i2cState = IDLE;
  if (message.length() == 0)
  {
    resetTelemetryQueue = true;
  }
}

void ESAT_I2CSlaveClass::handleWriteStateRequest()
{
  (void) bus->write(masterWriteState);
}

void ESAT_I2CSlaveClass::handleReadStateRequest()
{
  (void) bus->write(masterReadState);
}

void ESAT_I2CSlaveClass::handleReadPacketRequest()
{
  switch (masterReadState)
  {
    case PACKET_READY:
      handleReadPacketPrimaryHeaderRequest();
      break;
    case PACKET_DATA_READ_IN_PROGRESS:
      handleReadPacketPacketDataRequest();
      break;
    default:
      break;
  }
}

void ESAT_I2CSlaveClass::handleReadPacketPrimaryHeaderRequest()
{
  if (masterReadState != PACKET_READY)
  {
    return;
  }
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    masterReadPacket.readPrimaryHeader();
  (void) primaryHeader.writeTo(*bus);
  masterReadPacket.rewind();
  masterReadState = PACKET_DATA_READ_IN_PROGRESS;
}

void ESAT_I2CSlaveClass::handleReadPacketPacketDataRequest()
{
  if (masterReadState != PACKET_DATA_READ_IN_PROGRESS)
  {
    return;
  }
  for (byte i = 0; i < I2C_CHUNK_LENGTH; i++)
  {
    (void) bus->write(masterReadPacket.readByte());
    if (masterReadPacket.available() == 0)
    {
      masterReadState = PACKET_NOT_REQUESTED;
    }
  }
}

boolean ESAT_I2CSlaveClass::readPacket(ESAT_CCSDSPacket& packet)
{
  if (masterWriteState == WRITE_BUFFER_FULL)
  {
    const boolean successfulCopy = masterWritePacket.copyTo(packet);
    packet.rewind();
    masterWriteState = WRITE_BUFFER_EMPTY;
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
    case WRITE_PRIMARY_HEADER:
      ESAT_I2CSlave.handleWritePrimaryHeaderReception(message);
      break;
    case WRITE_PACKET_DATA:
      ESAT_I2CSlave.handleWritePacketDataReception(message);
      break;
    case WRITE_STATE:
      ESAT_I2CSlave.handleWriteStateReception(message);
      break;
    case READ_TELEMETRY:
      ESAT_I2CSlave.handleReadTelemetryReception(message);
      break;
    case READ_STATE:
      ESAT_I2CSlave.handleReadStateReception(message);
      break;
    case READ_PACKET:
      ESAT_I2CSlave.handleReadPacketReception(message);
      break;
    case READ_TELECOMMAND:
      ESAT_I2CSlave.handleReadTelecommandReception(message);
      break;
    case RESET_TELEMETRY_QUEUE:
      ESAT_I2CSlave.handleResetTelemetryQueueReception(message);
      break;
    default:
      ESAT_I2CSlave.i2cState = IDLE;
      break;
  }
}

int ESAT_I2CSlaveClass::requestedPacket()
{
  if (masterReadState != PACKET_NOT_READY)
  {
    return NO_PACKET_REQUESTED;
  }
  return masterReadRequestedPacket;
}

void ESAT_I2CSlaveClass::requestEvent()
{
  switch (ESAT_I2CSlave.i2cState)
  {
    case REQUEST_WRITE_STATE:
      ESAT_I2CSlave.handleWriteStateRequest();
      break;
    case REQUEST_READ_STATE:
      ESAT_I2CSlave.handleReadStateRequest();
      break;
    case REQUEST_READ_PACKET:
      ESAT_I2CSlave.handleReadPacketRequest();
      break;
    default:
      break;
  }
}

void ESAT_I2CSlaveClass::rejectPacket()
{
  if (masterReadState == PACKET_NOT_READY)
  {
    masterReadState = PACKET_REJECTED;
  }
}

boolean ESAT_I2CSlaveClass::telemetryQueueResetReceived()
{
  return resetTelemetryQueue;
}

void ESAT_I2CSlaveClass::updateTelemetryQueueState()
{
  if (masterReadRequestedPacket == NEXT_TELEMETRY_PACKET_REQUESTED)
  {
    resetTelemetryQueue = false;
  }
}

boolean ESAT_I2CSlaveClass::packetMatchesReadRequest(ESAT_CCSDSPacket& packet)
{
  const ESAT_CCSDSPrimaryHeader primaryHeader =
    packet.readPrimaryHeader();
  if ((masterReadRequestedPacket == NEXT_TELECOMMAND_PACKET_REQUESTED)
      && (primaryHeader.packetType != primaryHeader.TELECOMMAND))
  {
    return false;
  }
  if ((masterReadRequestedPacket == NEXT_TELEMETRY_PACKET_REQUESTED)
      && (primaryHeader.packetType != primaryHeader.TELEMETRY))
  {
    return false;
  }
  if (masterReadRequestedPacket >= 0) // Named-packet telemetry read request.
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
    const ESAT_CCSDSSecondaryHeader secondaryHeader =
      packet.readSecondaryHeader();
    if (primaryHeader.packetType != primaryHeader.TELEMETRY)
    {
      return false;
    }
  }
  return true;
}

void ESAT_I2CSlaveClass::writePacket(ESAT_CCSDSPacket& packet)
{
  if (masterReadState != PACKET_NOT_READY)
  {
    return;
  }
  if (packetMatchesReadRequest(packet))
  {
    updateTelemetryQueueState();
  }
  else
  {
    masterReadState = PACKET_INVALID;
  }
  packet.rewind();
  const boolean successfulCopy = packet.copyTo(masterReadPacket);
  if (!successfulCopy)
  {
    masterReadState = PACKET_INVALID;
  }
  else
  {
    masterReadState = PACKET_READY;
  }
}

ESAT_I2CSlaveClass ESAT_I2CSlave;

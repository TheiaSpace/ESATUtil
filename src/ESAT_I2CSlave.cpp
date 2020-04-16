/*
 * Copyright (C) 2017, 2018, 2019 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_I2CSlave.h"

const ESAT_SemanticVersionNumber ESAT_I2CSlaveClass::VERSION_NUMBER(1, 0, 0);

void ESAT_I2CSlaveClass::begin(TwoWire& i2cInterface,
                               const unsigned long masterWritePacketDataCapacity,
                               const unsigned long masterReadPacketDataCapacity)
{
	begin(i2cInterface, masterWritePacketDataCapacity, masterReadPacketDataCapacity, 1);
}

void ESAT_I2CSlaveClass::begin(TwoWire& i2cInterface,
                               const unsigned long masterWritePacketDataCapacity,
                               const unsigned long masterReadPacketDataCapacity,
							   const unsigned long inputPacketBufferCapacity)
{
  bus = &i2cInterface;
  i2cState = IDLE;
  masterWritePacket = ESAT_CCSDSPacket(masterWritePacketDataCapacity);
  masterWriteState = WRITE_BUFFER_EMPTY;
  masterWrittenPacketsQueue = ESAT_CCSDSPacketQueue(inputPacketBufferCapacity, masterWritePacketDataCapacity);
  masterReadPacket = ESAT_CCSDSPacket(masterReadPacketDataCapacity);
  masterReadState = PACKET_NOT_REQUESTED;
  bus->onReceive(receiveEvent);
  bus->onRequest(requestEvent);
}

void ESAT_I2CSlaveClass::begin(TwoWire& i2cInterface,
                               byte masterWritePacketDataBuffer[],
                               const unsigned long masterWritePacketDataBufferLength,
                               byte masterReadPacketDataBuffer[],
                               const unsigned long masterReadPacketDataBufferLength)
{
	begin(i2cInterface, masterWritePacketDataBuffer, masterWritePacketDataBufferLength,
		  masterReadPacketDataBuffer, masterReadPacketDataBufferLength, 1);
}

void ESAT_I2CSlaveClass::begin(TwoWire& i2cInterface,
                               byte masterWritePacketDataBuffer[],
                               const unsigned long masterWritePacketDataBufferLength,
                               byte masterReadPacketDataBuffer[],
                               const unsigned long masterReadPacketDataBufferLength,
							   const unsigned long inputPacketBufferCapacity)
{
  bus = &i2cInterface;
  i2cState = IDLE;
  masterWritePacket = ESAT_CCSDSPacket(masterWritePacketDataBuffer,
                                       masterWritePacketDataBufferLength);
  masterWriteState = WRITE_BUFFER_EMPTY;
  masterWrittenPacketsQueue = ESAT_CCSDSPacketQueue(inputPacketBufferCapacity, masterWritePacketDataBufferLength);
  masterReadPacket = ESAT_CCSDSPacket(masterReadPacketDataBuffer,
                                      masterReadPacketDataBufferLength);
  masterReadState = PACKET_NOT_REQUESTED;
  bus->onReceive(receiveEvent);
  bus->onRequest(requestEvent);
}

unsigned long ESAT_I2CSlaveClass::getQueueAvailability()
{
	return masterWrittenPacketsQueue.available();
}	

unsigned long ESAT_I2CSlaveClass::getQueueLength()
{
	return masterWrittenPacketsQueue.length();
}

void ESAT_I2CSlaveClass::handleWritePrimaryHeaderReception()
{
  i2cState = IDLE;
  if (masterWriteState == WRITE_BUFFER_FULL)
  {
    return;
  }
  if (bus->available() != ESAT_CCSDSPrimaryHeader::LENGTH)
  {
    return;
  }
  ESAT_CCSDSPrimaryHeader primaryHeader;
  const boolean correctRead = primaryHeader.readFrom(*bus);
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

void ESAT_I2CSlaveClass::handleWritePacketDataReception()
{
  i2cState = IDLE;
  if (masterWriteState != PACKET_DATA_WRITE_IN_PROGRESS)
  {
    return;
  }
  while ((bus->available() > 0)
         && (masterWritePacketDataBytesReceived
             < masterWritePacketDataLength))
  {
    masterWritePacket.writeByte(bus->read());
    masterWritePacketDataBytesReceived =
      masterWritePacketDataBytesReceived + 1;
    if (masterWritePacketDataBytesReceived >=
        masterWritePacketDataLength)
    {
      masterWritePacket.rewind();
	  if (masterWrittenPacketsQueue.write(masterWritePacket))
	  {
		  
	  }
	  else
	  {
		
	  }		
	  if (masterWrittenPacketsQueue.length() < masterWrittenPacketsQueue.capacity())
	  {
		masterWriteState = WRITE_BUFFER_EMPTY;
	  }
	  else
	  {
		masterWriteState = WRITE_BUFFER_FULL;
	  }
	}
  }
}

void ESAT_I2CSlaveClass::handleWriteStateReception()
{
  if (bus->available() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_WRITE_STATE;
  }
}

void ESAT_I2CSlaveClass::handleReadTelemetryReception()
{
  i2cState = IDLE;
  if (bus->available() == 1)
  {
    masterReadRequestedPacket = bus->read();
    masterReadState = PACKET_NOT_READY;
  }
  else if (bus->available() == 0)
  {
    masterReadRequestedPacket = NEXT_TELEMETRY_PACKET_REQUESTED;
    masterReadState = PACKET_NOT_READY;
  }
}

void ESAT_I2CSlaveClass::handleReadStateReception()
{
  if (bus->available() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_READ_STATE;
  }
}

void ESAT_I2CSlaveClass::handleReadPacketReception()
{
  if (bus->available() != 0)
  {
    i2cState = IDLE;
  }
  else
  {
    i2cState = REQUEST_READ_PACKET;
  }
}

void ESAT_I2CSlaveClass::handleReadTelecommandReception()
{
  i2cState = IDLE;
  if (bus->available() == 0)
  {
    masterReadRequestedPacket = NEXT_TELECOMMAND_PACKET_REQUESTED;
    masterReadState = PACKET_NOT_READY;
  }
}

void ESAT_I2CSlaveClass::handleResetTelemetryQueueReception()
{
  i2cState = IDLE;
  if (bus->available() == 0)
  {
    resetTelemetryQueue = true;
  }
}

void ESAT_I2CSlaveClass::handleProtocolVersionNumberReception()
{
  i2cState = REQUEST_PROTOCOL_VERSION_NUMBER;
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

void ESAT_I2CSlaveClass::handleProtocolVersionNumberRequest()
{
  (void) VERSION_NUMBER.writeTo(*bus);
}

boolean ESAT_I2CSlaveClass::readPacket(ESAT_CCSDSPacket& packet)
{
  if (masterWrittenPacketsQueue.length() > 0)
  {
	// Serial.println("Now inside I2CSlave read packet (bef. read) ");
	// Serial.print("I2C queue space to write: ");
	// Serial.println(masterWrittenPacketsQueue.available(), DEC);
	// Serial.print("I2C queue packets to be read: ");
	// Serial.println(masterWrittenPacketsQueue.length(), DEC);  
	
    const boolean successfulCopy = masterWrittenPacketsQueue.read(packet);
	// Serial.println("After read: ");
	// Serial.print("I2C queue space to write: ");
	// Serial.println(masterWrittenPacketsQueue.available(), DEC);
	// Serial.print("I2C queue packets to be read: ");
	// Serial.println(masterWrittenPacketsQueue.length(), DEC); 
    packet.rewind();
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
  switch (registerNumber)
  {
    case WRITE_PRIMARY_HEADER:
      ESAT_I2CSlave.handleWritePrimaryHeaderReception();
      break;
    case WRITE_PACKET_DATA:
      ESAT_I2CSlave.handleWritePacketDataReception();
      break;
    case WRITE_STATE:
      ESAT_I2CSlave.handleWriteStateReception();
      break;
    case READ_TELEMETRY:
      ESAT_I2CSlave.handleReadTelemetryReception();
      break;
    case READ_STATE:
      ESAT_I2CSlave.handleReadStateReception();
      break;
    case READ_PACKET:
      ESAT_I2CSlave.handleReadPacketReception();
      break;
    case READ_TELECOMMAND:
      ESAT_I2CSlave.handleReadTelecommandReception();
      break;
    case RESET_TELEMETRY_QUEUE:
      ESAT_I2CSlave.handleResetTelemetryQueueReception();
      break;
    case PROTOCOL_VERSION_NUMBER:
      ESAT_I2CSlave.handleProtocolVersionNumberReception();
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
    case REQUEST_PROTOCOL_VERSION_NUMBER:
      ESAT_I2CSlave.handleProtocolVersionNumberRequest();
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
    if (primaryHeader.packetType != primaryHeader.TELEMETRY)
    {
      return false;
    }
    const ESAT_CCSDSSecondaryHeader secondaryHeader =
      packet.readSecondaryHeader();
    if (secondaryHeader.packetIdentifier != masterReadRequestedPacket)
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

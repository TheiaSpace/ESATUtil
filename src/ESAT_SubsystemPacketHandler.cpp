/*
 * Copyright (C) 2019 Theia Space, Universidad Politécnica de Madrid
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

#include "ESAT_SubsystemPacketHandler.h"

void ESAT_SubsystemPacketHandlerClass::addTelecommand(ESAT_CCSDSTelecommandPacketHandler& telecommand)
{
  telecommandPacketDispatcher.add(telecommand);
}

void ESAT_SubsystemPacketHandlerClass::addTelemetry(ESAT_CCSDSTelemetryPacketContents& telemetry)
{
  telemetryPacketBuilder.add(telemetry);
  enableTelemetry(telemetry.packetIdentifier());
}

void ESAT_SubsystemPacketHandlerClass::begin(const word applicationProcessIdentifier,
                                             const byte majorVersionNumber,
                                             const byte minorVersionNumber,
                                             const byte patchVersionNumber,
                                             ESAT_Clock& clock,
                                             TwoWire& i2cInterface,
                                             const unsigned long packetDataCapacity,
											 const unsigned long i2cInputPacketBufferCapacity)
{
  telecommandPacketDispatcher =
    ESAT_CCSDSTelecommandPacketDispatcher(applicationProcessIdentifier);
  telemetryPacketBuilder =
    ESAT_CCSDSTelemetryPacketBuilder(applicationProcessIdentifier,
                                     majorVersionNumber,
                                     minorVersionNumber,
                                     patchVersionNumber,
                                     clock);
  telemetryClock = &clock;
  usbReader = ESAT_CCSDSPacketFromKISSFrameReader(Serial,
                                                  packetDataCapacity
                                                  + ESAT_CCSDSPrimaryHeader::LENGTH);
  usbWriter = ESAT_CCSDSPacketToKISSFrameWriter(Serial);
  ESAT_I2CSlave.begin(i2cInterface,
                      packetDataCapacity,
                      packetDataCapacity,
					  i2cInputPacketBufferCapacity);
  i2cTelecommandPacket = ESAT_CCSDSPacket(packetDataCapacity);
  i2cTelemetryPacket = ESAT_CCSDSPacket(packetDataCapacity);
}

void ESAT_SubsystemPacketHandlerClass::disableTelemetry(const byte packetIdentifier)
{
  enabledTelemetry.clear(packetIdentifier);
}

boolean ESAT_SubsystemPacketHandlerClass::dispatchTelecommand(ESAT_CCSDSPacket& telecommandPacket)
{
  return telecommandPacketDispatcher.dispatch(telecommandPacket);
}

void ESAT_SubsystemPacketHandlerClass::enableTelemetry(const byte packetIdentifier)
{
  enabledTelemetry.set(packetIdentifier);
}

boolean ESAT_SubsystemPacketHandlerClass::pendingI2CPacketRequest()
{
  if (ESAT_I2CSlave.requestedPacket() == ESAT_I2CSlave.NO_PACKET_REQUESTED)
  {
    return false;
  }
  else
  {
    return true;
  }
}

void ESAT_SubsystemPacketHandlerClass::prepareSubsystemsOwnTelemetry()
{
  pendingTelemetry = telemetryPacketBuilder.available() & enabledTelemetry;
  pendingI2CTelemetryBuffer = pendingI2CTelemetryBuffer | pendingTelemetry;
}

boolean ESAT_SubsystemPacketHandlerClass::queueTelecommandToI2C(ESAT_CCSDSPacket telecommandPacket)
{
  if (i2cTelecommandPacket.packetDataLength() == 0)
  {    
    return telecommandPacket.copyTo(i2cTelecommandPacket);
  }
  else
  {
    return false;
  }
}

boolean ESAT_SubsystemPacketHandlerClass::readPacketFromI2C(ESAT_CCSDSPacket& packet)
{
  return ESAT_I2CSlave.readPacket(packet);
}

boolean ESAT_SubsystemPacketHandlerClass::readPacketFromUSB(ESAT_CCSDSPacket& packet)
{
  return usbReader.read(packet);
}

boolean ESAT_SubsystemPacketHandlerClass::readSubsystemsOwnTelemetry(ESAT_CCSDSPacket& packet)
{
  pendingTelemetry = pendingTelemetry & enabledTelemetry;
  if (pendingTelemetry.available() == 0)
  {
    return false;
  }
  const byte identifier = byte(pendingTelemetry.readNext());
  pendingTelemetry.clear(identifier);
  const boolean gotPacket = telemetryPacketBuilder.build(packet, identifier);
  return gotPacket;
}

void ESAT_SubsystemPacketHandlerClass::respondToI2CPacketRequest()
{
  const int requestedPacket = ESAT_I2CSlave.requestedPacket();
  switch (requestedPacket)
  {
    case ESAT_I2CSlave.NO_PACKET_REQUESTED:
      break;
    case ESAT_I2CSlave.NEXT_TELEMETRY_PACKET_REQUESTED:
      respondToNextPacketTelemetryRequest();
      break;
    case ESAT_I2CSlave.NEXT_TELECOMMAND_PACKET_REQUESTED:
      respondToNextPacketTelecommandRequest();
      break;
    default:
      respondToNamedPacketTelemetryRequest(byte(requestedPacket));
      break;
  }
}

void ESAT_SubsystemPacketHandlerClass::respondToNamedPacketTelemetryRequest(const byte identifier)
{
  // We try to satisfy named-packet telemetry requests, but only for
  // enabled telemetry packets.  If the requested packet isn't enabled
  // or we don't recognise it, we reject the request.
  const boolean enabled = enabledTelemetry.read(identifier);
  if (!enabled)
  {
    ESAT_I2CSlave.rejectPacket();
    return;
  }
  const boolean gotPacket =
    telemetryPacketBuilder.build(i2cTelemetryPacket, identifier);
  if (gotPacket)
  {
    ESAT_I2CSlave.writePacket(i2cTelemetryPacket);
  }
  else
  {
    ESAT_I2CSlave.rejectPacket();
  }
}

void ESAT_SubsystemPacketHandlerClass::respondToNextPacketTelecommandRequest()
{
  if (i2cTelecommandPacket.length() > 0)
  {
    ESAT_I2CSlave.writePacket(i2cTelecommandPacket);
    i2cTelecommandPacket.flush();
  }
  else
  {
    ESAT_I2CSlave.rejectPacket();
  }
}

void ESAT_SubsystemPacketHandlerClass::respondToNextPacketTelemetryRequest()
{
  // We only update the list of pending I2C telemetry packets
  // with the contents of the buffered list when we receive
  // a command to reset the telemetry queue.
  if (ESAT_I2CSlave.telemetryQueueResetReceived())
  {
    pendingI2CTelemetry = pendingI2CTelemetryBuffer;
    pendingI2CTelemetryBuffer.clearAll();
  }
  // Some pending I2C telemetry packet might have been disabled
  // since the last I2C request.
  pendingI2CTelemetry = pendingI2CTelemetry & enabledTelemetry;
  // We try to satisfy requests until we run out of packets.
  // Then, we just reject the request.
  if (pendingI2CTelemetry.available() > 0)
  {
    const byte identifier = byte(pendingI2CTelemetry.readNext());
    pendingI2CTelemetry.clear(identifier);
    respondToNamedPacketTelemetryRequest(identifier);
  }
  else
  {
    ESAT_I2CSlave.rejectPacket();
  }
}

void ESAT_SubsystemPacketHandlerClass::setTime(ESAT_Timestamp timestamp)
{
  if (telemetryClock != nullptr)
  {
    telemetryClock->write(timestamp);
  }
}

void ESAT_SubsystemPacketHandlerClass::writePacketToUSB(ESAT_CCSDSPacket packet)
{
  (void) usbWriter.unbufferedWrite(packet);
}

// Global instance of the subsystem packet handler library.
ESAT_SubsystemPacketHandlerClass ESAT_SubsystemPacketHandler;

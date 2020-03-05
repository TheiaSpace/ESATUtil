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

#ifndef ESAT_SubsystemPacketHandler_h
#define ESAT_SubsystemPacketHandler_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_CCSDSPacketFromKISSFrameReader.h"
#include "ESAT_CCSDSPacketToKISSFrameWriter.h"
#include "ESAT_CCSDSTelecommandPacketDispatcher.h"
#include "ESAT_CCSDSTelemetryPacketBuilder.h"
#include "ESAT_Clock.h"
#include "ESAT_FlagContainer.h"
#include "ESAT_I2CSlave.h"
#include <Wire.h>

// ESAT subsystem packet handler: I2C and USB telemetry and telecommands.
// The subsystem packet handler takes care of common packet management
// details so that subsystem developers may concentrate on the specifics
// of their subsystems.  See the bottom of this file for a detailed
// explanation.
// Use the global instance ESAT_SubsystemPacketHandler.  Call
// ESAT_SubsystemPacketHandler.begin() before doing any other
// work with the subsystem packet handler.
class ESAT_SubsystemPacketHandlerClass
{
  public:
    // Register a telecommand handler.
    void addTelecommand(ESAT_CCSDSTelecommandPacketHandler& telecommand);

    // Register a telemetry packet.
    // The telemetry packet will be enabled by default;
    // it can be disabled with disableTelemetry().
    void addTelemetry(ESAT_CCSDSTelemetryPacketContents& telemetry);

    // Configure the subsystem data handler with the given application
    // parameters: application process identifier, major version
    // number, minor version number, patch version number.
    // Use the given clock to timestamp telemetry packets.
    // Listen to incoming packets and packet requests on the
    // given I2C interface.  The I2C interface must be already
    // started, but the subsystem data handler will start the
    // CCSDS-over-I2C slave protocol (call ESAT_I2CSlave.begin()).
    // Work with packets of the given packet data capacity.
    void begin(word applicationProcessIdentifier,
               byte majorVersionNumber,
               byte minorVersionNumber,
               byte patchVersionNumber,
               ESAT_Clock& clock,
               TwoWire& i2cInterface,
               unsigned long packetDataCapacity);

    // Disable the telemetry packet with the given identifier.
    void disableTelemetry(byte packetIdentifier);

    // Dispatch a telecommand packet.
    // Return true on success; otherwise return false.
    boolean dispatchTelecommand(ESAT_CCSDSPacket& telecommandPacket);

    // Enable the telemetry packet with the given identifier.
    void enableTelemetry(byte packetIdentifier);

    // Return true if there is a pending I2C packet request;
    // otherwise return false.
    boolean pendingI2CPacketRequest();

    // Prepare a new batch of telemetry packets from this subsystem.
    void prepareSubsystemsOwnTelemetry();

    // Queue a telecommand packet so that the I2C bus master can
    // request it later.
    // Return true on success; otherwise (on a full queue) return false.
    boolean queueTelecommandToI2C(ESAT_CCSDSPacket telecommandPacket);

    // Read a packet from the I2C interface.
    // Return true on success; otherwise return false.
    boolean readPacketFromI2C(ESAT_CCSDSPacket& packet);

    // Read a packet from the USB interface.
    // Return true on success; otherwise return false.
    boolean readPacketFromUSB(ESAT_CCSDSPacket& packet);

    // Read the next telemetry packet from this subsystem's current
    // batch of telemetry packets.
    // Fill the given packet with its contents.
    // Return true on a new packet; otherwise return false.
    boolean readSubsystemsOwnTelemetry(ESAT_CCSDSPacket& packet);

    // Respond to the pending (if any) I2C request.
    void respondToI2CPacketRequest();

    // Set the clock.
    void setTime(ESAT_Timestamp timestamp);

    // Write a packet to the USB interface.
    void writePacketToUSB(ESAT_CCSDSPacket packet);

  private:
    // Enabled telemetry list.
    ESAT_FlagContainer enabledTelemetry;

    // Telemetry packet for I2C requests.
    ESAT_CCSDSPacket i2cTelemetryPacket;

    // Telecommand packet for I2C requests.
    ESAT_CCSDSPacket i2cTelecommandPacket;

    // Pending telemetry list for I2C requests (active).
    ESAT_FlagContainer pendingI2CTelemetry;

    // Pending telemetry list for I2C requests
    // (buffer; copy this list into pendingI2CTelemetry
    // on pending telemetry list reset requests).
    ESAT_FlagContainer pendingI2CTelemetryBuffer;

    // Pending telemetry list for general output.
    ESAT_FlagContainer pendingTelemetry;

    // Telecommand packet dispatcher.
    ESAT_CCSDSTelecommandPacketDispatcher telecommandPacketDispatcher;

    // Clock used for telemetry packet timekeeping.
    ESAT_Clock* telemetryClock;

    // Telemetry packet builder.
    ESAT_CCSDSTelemetryPacketBuilder telemetryPacketBuilder;

    // Use this to read CCSDS packet from KISS frames coming from the
    // USB interface.
    ESAT_CCSDSPacketFromKISSFrameReader usbReader;

    // Use this to write CCSDS packets in KISS frames to the USB
    // interface.
    ESAT_CCSDSPacketToKISSFrameWriter usbWriter;

    // Respond to a named-packet telemetry request from the I2C master.
    void respondToNamedPacketTelemetryRequest(byte identifier);

    // Respond to a next-packet telecommand request from the I2C master.
    void respondToNextPacketTelecommandRequest();

    // Respond to a next-packt telemetry request from the I2C master.
    void respondToNextPacketTelemetryRequest();
};

// Global instance of the subsystem data handler library.
extern ESAT_SubsystemPacketHandlerClass ESAT_SubsystemPacketHandler;

// ESAT subsystem boards share the following traits:
// - Subsystems can generate CCSDS Space Packets with their own telemetry.
//   -- These telemetry packets come in batches; a batch can contain
//      up to one packet for each possible packet identifier (for
//      example, a batch could consist of one packet with identifier
//      zero, one packet with identifier twenty and one packet with
//      identifier two hundred).
//   -- To start a new batch of telemetry packets, call
//      ESAT_SubsystemPacketHandler.prepareSubsystemsOwnTelemetry().
//   -- To get the next telemetry packet from the batch, call
//      ESAT_SubsystemPacketHandler.readSubsystemsOwnTelemetry().
//   -- The subsystem packet handler uses ESAT_CCSDSTelemetryPacketContents
//      objects as recipes to build packets.  Register these recipes
//      (one per desired packet identifier) with
//      ESAT_SubsystemPacketHandler.addTelemetry().
//   -- Use ESAT_SubsystemPacketHandler.enableTelemetry() and
//      ESAT_SubsystemPacketHandler.disableTelemetry() to enable and
//      disable the generation of telemetry packets with specific
//      packet identifiers.
//   -- Telemetry packets obtained with
//      ESAT_SubsystemPacketHandler.readSubsystemsOwnTelemetry() often
//      go to the USB interface with
//      ESAT_SubsystemPacketHandler.writePacketToUSB().
//   -- The I2C bus master (the On-Board Computer) may request
//      telemetry packets.  Calling
//      ESAT_SubsystemPacketHandler.respondToI2CPacketRequest()
//      fulfills these requests automatically.
// - Subsystems can respond to CCSDS Space Packets carrying telecommands.
//   -- Call ESAT_SubsystemPacketHandler.dispatchTelecommand()
//      to respond to a telecommand.
//   -- The subsystem packet handler uses ESAT_CCSDSTelecommandPacketHandler
//      objects as recipes on how to respond to telecommands.  Register
//      these recipes (one per desired packet identifier and compatible
//      version number) with ESAT_SubsystemPacketHandler.addTelecommand().
//   -- Telecommand packets often come from the USB interface (read
//      with ESAT_SubsystemPacketHandler.readPacketFromUSB()) and the
//      I2C interface (read with
//      ESAT_SubsystemPacketHandler.readPacketFromI2C()).
// - Subsystems can receive CCSDS Space Packets (usually telecommands) from
//   the USB interface.
//   -- These packets come in KISS frames.
//   -- These incoming packets may be, for example, telecommands that
//      can be handled with
//      ESAT_SubsystemPacketHandler.dispatchTelecommand().
// - Subsystems can send CCSDS Space Packets (usually telemetry) through the
//   USB interface.
//   -- These packets go into KISS frames.
//   -- These outgoing packets may be, for example, telemetry packets
//      generated with
//      ESAT_SubsystemPacketHandler.readSubsystemsTelemetry().
// - Subsystems can receive from the I2C bus CCSDS Space Packets
//   (telemetry and telecommands) written by the I2C bus master (the
//   On-Board Computer).
//   -- Incoming telecommands directed to this subsystem are usually
//      handled with ESAT_SubsystemPacketHandler.dispatchTelecommand().
// - Subsystems can send through the I2C bus CCSDS Space Packets
//   (telemetry and telecommands) requested by the I2C bus master.
//   -- The subsystem packet handler uses
//      ESAT_CCSDSTelemetryPacketContents objects (registered with
//      ESAT_SubsystemPacketHandler.addTelemetry()) as recipes to
//      respond to telemery packet requests.
//   -- The subsystem packet handler carries a queue of telecommand
//      packets to respond to telecommand packet requests.  Queue
//      telecommands with
//      ESAT_SubsystemPacketHandler.queueTelecommandToI2C().

#endif /* ESAT_SubsystemPacketHandler_h */

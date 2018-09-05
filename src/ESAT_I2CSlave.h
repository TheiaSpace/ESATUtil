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

#ifndef ESAT_I2CSlave_h
#define ESAT_I2CSlave_h

#include <Arduino.h>
#include "ESAT_Buffer.h"
#include "ESAT_CCSDSPacket.h"
#include <Wire.h>

// ESAT I2C telecommand and telemetry protocol for I2C slave nodes.
// Use the global instance ESAT_I2CSlave.
class ESAT_I2CSlaveClass
{
  public:
    // Return value of requestedPacket() when there is no
    // pending packet read request.
    static const int NO_PACKET_REQUESTED = -1;

    // Deprecated constant; use NO_PACKET_REQUESTED instead.
    static const int NO_TELEMETRY_PACKET_REQUESTED __attribute__((deprecated("Use NO_PACKET_REQUESTED instead."))) = -1;

    // Return value of requestedPacket() when the request
    // is for a next-packet telemetry request.
    static const int NEXT_TELEMETRY_PACKET_REQUESTED = -2;

    // Return value of requestedPacket() when the request
    // is for a next-packet telecommand request.
    static const int NEXT_TELECOMMAND_PACKET_REQUESTED = -3;

    // Configure the I2C slave to listen on the given I2C interface
    // (register the I2C reception and request handlers).
    // The I2C interface must be already initiated.
    // The caller must provide the packet data buffers for telecommands
    // and telemetry.
    void begin(TwoWire& i2cInterface,
               byte masterWritePacketDataBuffer[],
               word masterWritePacketDataBufferLength,
               byte masterReadPacketDataBuffer[],
               word masterReadPacketDataBufferLength);

    // Return:
    // - NO_PACKET_REQUESTED if there isn't a pending packet read
    //   request.
    // - NEXT_TELEMETRY_PACKET_REQUESTED if there is a next-packet
    //   telemetry read request pending.
    // - NEXT_TELECOMMAND_PACKET_REQUESTED if there is a next-packet
    //   telecommand read request pending.
    // - The requested packet identifier if there is a pending
    //   named-packet telemetry read request pending.
    int requestedPacket();

    // Deprecated method; use requestedPacket() instead.
    // Same functionality as requestedPacket().
    int requestedTelemetryPacket() __attribute__((deprecated("Use requestedPacket() instead.")))
    {
      return requestedPacket();
    }

    // Read the next queued CCSDS packet and copy
    // its contents into the provided packet object.
    // Return true if there was a queued packet;
    // otherwise return false.
    boolean readPacket(ESAT_CCSDSPacket& packet);

    // Deprecated method; use readPacket() instead.
    // Same functionality as readPacket().
    inline boolean readTelecommand(ESAT_CCSDSPacket& packet) __attribute__((deprecated("Use readPacket() instead.")))
    {
      return readPacket(packet);
    }

    // Reject the requested packet read.
    // If there is a pending packet read request (PACKET_NOT_READY),
    // the next read of READ_STATE will be
    // PACKET_REJECTED.
    void rejectPacket();

    // Deprecated method; use rejectPacket() instead.
    // Same functionality as rejectPacket().
    inline void rejectTelemetryRequest() __attribute__((deprecated("Use rejectPacket() instead.")))
    {
      rejectPacket();
    }

    // Return true if the master requested a reset of the telemetry queue
    // since the last call to writePacket() with next-packet telemetry;
    // otherwise return false.
    boolean telemetryQueueResetReceived();

    // Queue a packet for reading by the master
    // if the following conditions are met:
    // - the packet is well-formed;
    // - the packet is a telemetry packet if the request is for
    //   a next-packet telemetry read;
    // - the packet is a telemetry packet if the request is for
    //   a named-packet telemetry read;
    // - the secondary header is present and the packet identifier
    //   matches the requested packet identifier if the request
    //   is for a named-packet telemetry read;
    // - the packet is a telecommand packet if the request is for
    //   a next-packet telecommand read.
    // If these conditions are met, the next read of READ_STATE
    // will be PACKET_READY.
    // If there is a pending packet read request, but the packet
    // doesn't meet the conditions stated above, the next read of
    // READ_STATE will be PACKET_INVALID.
    void writePacket(ESAT_CCSDSPacket& packet);

    // Deprecated method; use writePacket() instead.
    // Same functionality as writePacket().
    inline void writeTelemetry(ESAT_CCSDSPacket& packet) __attribute__((deprecated("Use writePacket() instead.")))
    {
      writePacket(packet);
    }

  private:
    // Register numbers of the ESAT CCSDS Space Packet-over-I2C
    // protocol.
    enum RegisterNumbers
    {
      WRITE_PRIMARY_HEADER = 0x00,
      WRITE_PACKET_DATA = 0x01,
      WRITE_STATE = 0x02,
      READ_TELEMETRY = 0x10,
      READ_STATE = 0x11,
      READ_PACKET = 0x12,
      READ_TELECOMMAND = 0x13,
      RESET_TELEMETRY_QUEUE = 0x14,
    };

    // Possible states of the low-level I2C slave state machine.
    enum I2CState
    {
      IDLE,
      REQUEST_READ_STATE,
      REQUEST_WRITE_STATE,
      REQUEST_READ_PACKET,
    };

    // Possible states of the high-level ESAT CCSDS Space
    // Packet-over-I2C master-write state machine.
    enum MasterWriteState
    {
      WRITE_BUFFER_EMPTY = 0,
      WRITE_BUFFER_FULL = 1,
      PACKET_DATA_WRITE_IN_PROGRESS = 2,
    };

    // Possible states of the high-level ESAT CCSDS Space
    // Packet-over-I2C master-read state machine.
    enum MasterReadState
    {
      PACKET_NOT_REQUESTED = 0,
      PACKET_NOT_READY = 1,
      PACKET_READY = 2,
      PACKET_REJECTED = 3,
      PACKET_INVALID = 4,
      PACKET_DATA_READ_IN_PROGRESS = 5,
    };

    // I2C messages will be sent in chunks of up to 16 bytes.
    static const byte I2C_CHUNK_LENGTH = 16;

    // I2C slave interface.
    TwoWire* bus;

    // Current state of the low-level I2C slave state machine.
    volatile I2CState i2cState;

    // Master-write packet buffer.
    ESAT_CCSDSPacket masterWritePacket;

    // Number of received master-written packet data bytes.
    unsigned long masterWritePacketDataBytesReceived;

    // Expected packet data length of the incoming master-write packet.
    unsigned long masterWritePacketDataLength;

    // State of the ESAT CCSDS Space Packet-over-I2C master-write
    // state machine.
    volatile MasterWriteState masterWriteState;

    // Master-read packet buffer.
    ESAT_CCSDSPacket masterReadPacket;

    // Master-read packet request type.
    volatile int masterReadRequestedPacket;

    // State of the ESAT CCSDS Space Packet-over-I2C master-read
    // state machine.
    volatile MasterReadState masterReadState;

    // Set this flag to true when the master requests a reset
    // of the telemetry queue.  Set this flag to false on calls
    // to writePacket() with next-packet telemetry.
    volatile boolean resetTelemetryQueue;

    // Handle a write to WRITE_PACKET_DATA.
    void handleWritePacketDataReception(ESAT_Buffer message);

    // Handle a write to WRITE_PRIMARY_HEADER.
    void handleWritePrimaryHeaderReception(ESAT_Buffer message);

    // Handle a write to WRITE_STATE.
    void handleWriteStateReception(ESAT_Buffer message);

    // Handle a write to READ_TELEMETRY.
    void handleReadTelemetryReception(ESAT_Buffer message);

    // Handle a write to READ_STATE.
    void handleReadStateReception(ESAT_Buffer message);

    // Handle a write to READ_PACKET.
    void handleReadPacketReception(ESAT_Buffer message);

    // Handle a write to READ_TELECOMMAND.
    void handleReadTelecommandReception(ESAT_Buffer message);

    // Handle a write to RESET_TELEMETRY_QUEUE.
    void handleResetTelemetryQueueReception(ESAT_Buffer message);

    // Handle a read from WRITE_STATE.
    void handleWriteStateRequest();

    // Handle a read from READ_STATE.
    void handleReadStateRequest();

    // Handle a read from READ_PACKET.
    void handleReadPacketRequest();

    // Handle a read from READ_PACKET (primary header).
    void handleReadPacketPrimaryHeaderRequest();

    // Handle a read from READ_PACKET (packet data).
    void handleReadPacketPacketDataRequest();

    // Return true if the provided packet matches the current read
    // request:
    // - the packet is well-formed;
    // - the packet is a telemetry packet if the request is for
    //   a next-packet telemetry read;
    // - the packet is a telemetry packet if the request is for
    //   a named-packet telemetry read;
    // - the secondary header is present and the packet identifier
    //   matches the requested packet identifier if the request
    //   is for a named-packet telemetry read;
    // - the packet is a telecommand packet if the request is for
    //   a next-packet telecommand read.
    // Return false otherwise.
    boolean packetMatchesReadRequest(ESAT_CCSDSPacket& packet);

    // Handle I2C writes from the master.
    static void receiveEvent(int numberOfBytes);

    // Handle I2C reads from the master.
    static void requestEvent();

    // Update the state of the telemetry queue.
    // This will disable the telemetry queue reset flag
    // if we are processing a next-packet telemetry read.
    void updateTelemetryQueueState();
};

// Global instance of the I2C slave library.
extern ESAT_I2CSlaveClass ESAT_I2CSlave;

#endif /* ESAT_I2CSlave_h */

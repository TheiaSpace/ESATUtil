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

#ifndef ESATI2CSlave_h
#define ESATI2CSlave_h

#include <Arduino.h>
#include <ESATCCSDSPacket.h>
#include <Wire.h>

// ESAT I2C telecommand and telemetry protocol for I2C slave nodes.
class ESATI2CSlave
{
  public:
    // Configure the I2C slave to listen on the given I2C interface
    // (register the I2C reception and request handlers).
    // The I2C interface must be already initiated.
    // The caller must provide the packet data buffers for telecommands
    // and telemetry.
    void begin(TwoWire& i2cInterface,
               byte telecommandPacketDataBuffer[],
               word telecommandPacketDataBufferLength,
               byte telemetryPacketDataBuffer[],
               word telemetryPacketDataBufferLength);

    // Return the number of the requested telemetry packet
    // or a negative number if no telemetry packet was requested.
    int requestedTelemetryPacket();

    // Read the next queued telecommand into a CCSDS packet.
    // Return true if there was a queued telecommand;
    // otherwise return false.
    boolean readTelecommand(ESATCCSDSPacket& packet);

    // Reject the last telemetry request.
    // If there is a pending telemetry request (TELEMETRY_NOT_READY),
    // the next read of TELEMETRY_STATUS will be
    // TELEMETRY_REQUEST_REJECTED.
    void rejectTelemetryRequest();

    // Queue a telemetry packet for reading by the master
    // if the following conditions are met:
    // - there is a pending telemetry request (TELEMETRY_NOT_READY);
    // - the packet is a well-formed telemetry packet;
    // - the packet identifier is the same as the requested
    //   packet identifier.
    // If these conditions are met, the next read of TELEMETRY_STATUS
    // will be TELEMETRY_READY.
    // If there is a pending telemetry request, but the packet
    // isn't a well-formed telemetry packet with the requested identifier,
    // the next read of TELEMETRY_STATUS will be TELEMETRY_INVALID.
    void writeTelemetry(ESATCCSDSPacket& packet);

  private:
    // Register numbers for telecommand and telemetry handling.
    enum RegisterNumbers
    {
      TELECOMMAND_PRIMARY_HEADER = 0,
      TELECOMMAND_PACKET_DATA = 1,
      TELECOMMAND_STATUS = 2,
      TELEMETRY_REQUEST = 3,
      TELEMETRY_STATUS = 4,
      TELEMETRY_VECTOR = 5,
    };

    // Possible states of the I2C slave state machine.
    enum State
    {
      IDLE,
      HANDLE_TELECOMMAND_PRIMARY_HEADER,
      HANDLE_TELECOMMAND_PACKET_DATA,
      HANDLE_TELECOMMAND_STATUS,
      HANDLE_TELEMETRY_REQUEST,
      HANDLE_TELEMETRY_STATUS,
      HANDLE_TELEMETRY_VECTOR_PRIMARY_HEADER,
      HANDLE_TELEMETRY_VECTOR_PACKET_DATA,
    };

    // Possible states of the telecommand reception state machine.
    enum TelecommandState
    {
      TELECOMMAND_NOT_PENDING = 0,
      TELECOMMAND_PENDING = 1,
    };

    // Possible states of the telemetry request state machine.
    enum TelemetryState
    {
      TELEMETRY_NOT_REQUESTED = 0,
      TELEMETRY_NOT_READY = 1,
      TELEMETRY_READY = 2,
      TELEMETRY_REQUEST_REJECTED = 3,
      TELEMETRY_INVALID = 4,
    };

    // I2C messages will be sent in chunks of up to 16 bytes.
    static const byte I2C_CHUNK_LENGTH = 16;

    // I2C slave interface.
    TwoWire* bus;

    // Current state of the I2C slave state machine.
    volatile State receiveState;
    volatile State requestState;

    // Telecommand packet buffer.
    ESATCCSDSPacket telecommand;

    // Number of received telecommand packet data bytes.
    long telecommandPacketDataBytesReceived;

    // State of the telecommand reception state machine.
    volatile TelecommandState telecommandState;

    // Telemetry packet buffer.
    ESATCCSDSPacket telemetry;

    // Identifier of the requested telemetry packet.
    volatile byte telemetryPacketIdentifier;

    // State of the telemetry request state machine.
    volatile TelemetryState telemetryState;

    // Handle a write to TELECOMMAND_PACKET_DATA.
    void handleTelecommandPacketDataReception(const byte message[],
                                              int messageLength);

    // Handle a write to TELECOMMAND_PRIMARY_HEADER.
    void handleTelecommandPrimaryHeaderReception(const byte message[],
                                                 int messageLength);

    // Handle a write to TELECOMMAND_STATUS.
    void handleTelecommandStatusReception(const byte message[],
                                          int messageLength);

    // Handle a write to TELEMETRY_REQUEST.
    void handleTelemetryRequestReception(const byte message[],
                                         int messageLength);

    // Handle a write to TELEMETRY_STATUS.
    void handleTelemetryStatusReception(const byte message[],
                                        int messageLength);

    // Handle a write to TELEMETRY_VECTOR.
    void handleTelemetryVectorReception(const byte message[],
                                        int messageLength);

    // Handle a read from TELECOMMAND_STATUS.
    void handleTelecommandStatusRequest();

    // Handle a read from TELEMETRY_STATUS.
    void handleTelemetryStatusRequest();

    // Handle a read from TELEMETRY_VECTOR (packet data).
    void handleTelemetryVectorPacketDataRequest();

    // Handle a read from TELEMETRY_VECTOR (primary header).
    void handleTelemetryVectorPrimaryHeaderRequest();

    // Handle I2C writes from the master.
    static void receiveEvent(int numberOfBytes);

    // Handle I2C reads from the master.
    static void requestEvent();
};

extern ESATI2CSlave I2CSlave;

#endif

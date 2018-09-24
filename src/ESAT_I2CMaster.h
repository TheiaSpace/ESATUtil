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

#ifndef ESAT_I2CMaster_h
#define ESAT_I2CMaster_h

#include <Arduino.h>
#include "ESAT_CCSDSPacket.h"
#include "ESAT_SemanticVersionNumber.h"
#include <Wire.h>

// ESAT I2C telecommand and telemetry protocol for I2C master nodes.
// Use the global instance ESAT_I2CMaster.
class ESAT_I2CMasterClass
{
  public:
    // Configure the I2C master node to work with the given I2C bus.
    // The slave will need then some time to be ready to serve
    // requests; ask if it is ready up to a given number of attempts
    // (attempts, by default, 32 attempts), waiting some time from one
    // attempt to the next: starting at a given number of milliseconds
    // (initialDelay, by default, 1 ms) and growing by a
    // multiplicative factor (growthFactor, by default, 1.2) on each new
    // attempt.
    void begin(TwoWire& bus,
               word attempts = 32,
               word initialDelay = 1,
               float growthFactor = 1.2);

    // Read a named-packet telemetry packet matching the given packet
    // identifier from the slave at the given address.
    // Return true on success; otherwise return false.
    boolean readNamedTelemetry(ESAT_CCSDSPacket& packet,
                               byte identifier,
                               byte address);

    // Read a next-packet telemetry packet from the slave at the given
    // address.
    // Call ESAT_I2CMaster.resetTelemetryQueue() before a series of
    // calls to ESAT_I2CMaster.readNextTelemetry() to get the
    // telemetry queue ready.
    // Return true on success; otherwise return false.
    boolean readNextTelemetry(ESAT_CCSDSPacket& packet,
                              byte address);

    // Return the protocol version number of the slave at the given address.
    // The protocol version number is 0.0.0 on error.
    ESAT_SemanticVersionNumber readProtocolVersionNumber(byte address);

    // Read a telecommand packet from the slave at the given address.
    // Return true on success; otherwise return false.
    boolean readTelecommand(ESAT_CCSDSPacket& packet,
                            byte address);

    // Deprecated method; use ESAT_I2CMaster.readNamedTelemetry(packet,
    // packetIdentifier, address) instead.
    // Read a named-packet telemetry (identified by packetIdentifier)
    // packet from a slave at the given address.
    // Communicate through the given TwoWire (I2C) bus. You must have
    // already called bus.begin() (just once) before making any calls
    // to ESAT_I2CMaster.readTelemetry().
    // Read a specific telemetry packet (identified by packetType)
    // from the device at the given address.
    // Fill packet with the read telemetry.
    // Wait some milliseconds (millisecondsAfterWrites) after each
    // write operation to give the slave time to process the request.
    // Retry several times (attempts), waiting several milliseconds
    // (millisecondsBetweenAttempts), if the telemetry is not ready.
    // Return true on success; otherwise return false.
    boolean readTelemetry(TwoWire& bus,
                          byte address,
                          byte packetIdentifier,
                          ESAT_CCSDSPacket& packet,
                          byte millisecondsAfterWrites,
                          byte attempts,
                          word millisecondsBetweenAttempts) __attribute__((deprecated("Use ESAT_I2CMaster.readNamedTelemetry(packet, identifier, address) instead.")));

    // Reset the telemetry packet queue for next-packet telemetry pf
    // the slave at the given address.
    // Call this method before a series of calls to
    // ESAT_I2CMaster.readNextTelemetry() to get the telemetry queue ready.
    // Reset the queue of the slave at the given address.
    // Return true on success; otherwise return false.
    boolean resetTelemetryQueue(byte address);

    // Write a packet to the slave at the given address.
    // Wait an optional number of microseconds between successive
    // 15-byte chunks written to the slave to give it some time
    // to process the received data.  Arduino's Wire library calls
    // the user data reception function after the stop condition,
    // so the slave cannot clock-stretch and it is necessary to give
    // it time manually.
    // Return true on success; otherwise return false.
    boolean writePacket(ESAT_CCSDSPacket& packet,
                        byte address,
                        word microsecondsBetweenChunks = 0);

    // Deprecated method; use ESAT_I2CMaster.writePacket(address,
    // packet) instead.
    // Write a telecommand packet to the device at the given address.
    // Communicate through the given TwoWire (I2C) bus. You must have
    // already called bus.begin() (just once) before any calls to
    // ESAT_I2CMaster.writeTelecommand().
    // The read/write pointer of the packet may move.
    // Wait some milliseconds (millisecondsAfterWrites) after each
    // write operation to give the slave time to process the request.
    // Retry several times (attempts), waiting several milliseconds
    // (millisecondsBetweenAttempts), if the telecommand queue is
    // full.
    // Return true on success; otherwise return false.
    boolean writeTelecommand(TwoWire& bus,
                             byte address,
                             ESAT_CCSDSPacket& packet,
                             byte millisecondsAfterWrites,
                             byte attempts,
                             word millisecondsBetweenAttempts) __attribute__((deprecated("Use ESAT_I2CMaster.writePacket(packet, address) instead.")));

  private:
    // Pass this constant as the requestedPacket argument
    // to writePacketRequest() to ask for next-packet telemetry.
    static const int NEXT_TELEMETRY_PACKET_REQUESTED = -2;

    // Pass this constant as the requestedPacket argument
    // to writePacketRequest() to ask for next-packet telecommand.
    static const int NEXT_TELECOMMAND_PACKET_REQUESTED = -3;

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
      PROTOCOL_VERSION_NUMBER = 0x20,
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

    // Possible states of the high-level ESAT CCSDS Space
    // Packet-over-I2C master-write state machine.
    enum MasterWriteState
    {
      WRITE_BUFFER_EMPTY = 0,
      WRITE_BUFFER_FULL = 1,
      PACKET_DATA_WRITE_IN_PROGRESS = 2,
    };

    // I2C messages will be sent in chunks of up to 16 bytes.
    static const byte I2C_CHUNK_LENGTH = 16;

    // Communicate through this bus.
    TwoWire* bus = nullptr;

    // Ask if the slave is ready to send or receive a packet up to
    // this number of times.
    word attempts;

    // When asking if the slave is ready to send or receive a packet,
    // multiply the delay time between attempts by this growth factor
    // on each new attempt.
    float growthFactor;

    // When asking if the slave is ready to send or receive a packet,
    // if the slave isn't ready at the first attempt, wait this number
    // of milliseconds between the first attempt and the second to
    // have the slave ready to send or receive a packet.  On each new
    // attempt, the delay time between attempts will grow by the
    // multiplicative growth factor.
    word initialDelay;

    // When communicating with a slave, wait this number of
    // milliseconds after each write operation to give the slave time
    // to process the request.  Deprecated methods methods
    // readTelemetry() and writeTelecommand() need this.
    word millisecondsAfterWrites;

    // See if we can read a packet now from the given address.
    // Return true if we can read a packet now; otherwise return false.
    boolean canReadPacket(byte address);


    // See if we can write packets now to the given address.
    // Return true if it is possible to write a packet now;
    // otherwise return false.
    boolean canWritePacket(byte address);

    // Return true if the received packet matches the packet request;
    // otherwise return false.
    boolean packetMatchesRequest(ESAT_CCSDSPacket& packet,
                                 int requestedPacket);

    // Read a packet from the device at the given address.
    // The requested packet is either:
    // - NEXT_TELEMETRY_PACKET_REQUESTED: next-packet telemetry;
    // - NEXT_TELECOMMAND_PACKET_REQUESTED: next-packet telecommand;
    // - from 0 to 255: named-packet telemetry with the given packet identifier.
    // Return true on success; otherwise return false.
    boolean readPacket(ESAT_CCSDSPacket& packet,
                       int requestedPacket,
                       byte address);

    // Read the packet data from the given address.
    // Return true on success; otherwise return false.
    boolean readPacketData(ESAT_CCSDSPacket& packet, byte address);

    // Read the packet primary header from the given address.
    // Return true on success; otherwise return false.
    boolean readPrimaryHeader(ESAT_CCSDSPacket& packet, byte address);

    // Write a packet request to the given address.
    // The requested packet is either:
    // - NEXT_TELEMETRY_PACKET_REQUESTED: next-packet telemetry;
    // - NEXT_TELECOMMAND_PACKET_REQUESTED: next-packet telecommand;
    // - from 0 to 255: named-packet telemetry with the given packet identifier.
    // Return true on success; otherwise return false.
    boolean requestPacket(int requestedPacket, byte address);

    // Write the packet data to the given address.
    // Wait a number of microseconds between chunks
    // to give the slave time to process the data.
    // Return true on success; otherwise return false.
    boolean writePacketData(ESAT_CCSDSPacket& packet,
                            byte address,
                            word microsecondsBetweenChunks);

    // Write the primary header of the packet to the given address.
    // Wait a number of microseconds between chunks
    // to give the slave time to process the data.
    // Return true on success; otherwise return false.
    boolean writePrimaryHeader(ESAT_CCSDSPacket& packet,
                               byte address,
                               word microsecondsBetweenChunks);
};

// Global instance of the I2C master library.
extern ESAT_I2CMasterClass ESAT_I2CMaster;

#endif /* ESAT_I2CMaster_h */

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

#ifndef ESAT_I2CMaster_h
#define ESAT_I2CMaster_h

#include <Arduino.h>
#include <ESAT_CCSDSPacket.h>
#include <Wire.h>

// ESAT I2C telecommand and telemetry protocol for I2C master nodes.
// Use the global instance ESAT_I2CMaster.
class ESAT_I2CMasterClass
{
  public:

    // Read an available telecommand packet from the device at the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times, waiting several milliseconds, if the
    // telecommand is not ready.
    // Return true on success; otherwise return false.
    boolean readTelecommand(TwoWire& bus,
                            byte address,
                            ESAT_CCSDSPacket& packet,
                            byte millisecondsAfterWrites,
                            byte attempts,
                            word millisecondsBetweenAttempts);

    // Read an specific telemetry packet from the device at the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times, waiting several milliseconds, if the
    // telemetry is not ready.
    // Return true on success; otherwise return false.
    boolean readTelemetry(TwoWire& bus,
                          byte address,
                          byte packetIdentifier,
                          ESAT_CCSDSPacket& packet,
                          byte millisecondsAfterWrites,
                          byte attempts,
                          word millisecondsBetweenAttempts);

    // Read an available telemetry packet from the device at the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times, waiting several milliseconds, if the
    // telemetry is not ready.
    // Return true on success; otherwise return false.
    boolean readTelemetry(TwoWire& bus,
                          byte address,
                          ESAT_CCSDSPacket& packet,
                          byte millisecondsAfterWrites,
                          byte attempts,
                          word millisecondsBetweenAttempts);

    // Reset the telemetry queue of the device at the given address.
    // Wait a given number of milliseconds after the write operation.
    // Return true on a successful I2C communication;
    // otherwise return false.
    boolean resetTelemetryQueue(TwoWire& bus,
                                byte address);

    // Write a packet to the device at the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times, waiting several milliseconds, if the
    // packet queue is full.
    // Return true on success; otherwise return false.
    boolean writePacket(TwoWire& bus,
                             byte address,
                             ESAT_CCSDSPacket& packet,
                             byte millisecondsAfterWrites,
                             byte attempts,
                             word millisecondsBetweenAttempts);

    // Deprecated method; use writePacket() instead.
    // Same functionality as writePacket().
    boolean writeTelecommand(TwoWire& bus,
                             byte address,
                             ESAT_CCSDSPacket& packet,
                             byte millisecondsAfterWrites,
                             byte attempts,
                             word millisecondsBetweenAttempts) __attribute__((deprecated("Use writePacket instead.")))
    {
      return writePacket(bus,
                         address,
                         packet,
                         millisecondsAfterWrites,
                         attempts,
                         millisecondsBetweenAttempts);
    }

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

    // See if we can read a packet now from the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times if the telemetry is not ready,
    // waiting several milliseconds between reattempts.
    // Return true if we can read a packet now; otherwise return false.
    boolean canReadPacket(TwoWire& bus,
                           byte address,
                           byte millisecondsAfterWrites,
                           byte attempts,
                           word millisecondsBetweenAttempts);

    // See if we can write packets now to the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times if the queue is not free,
    // waiting several milliseconds between reattempts.
    // Return true if it is possible to write a packet now;
    // otherwise return false.
    boolean canWritePacket(TwoWire& bus,
                           byte address,
                           byte millisecondsAfterWrites,
                           byte attempts,
                           word millisecondsBetweenAttempts);

    // Return true if the received packet matches the packet request;
    // otherwise return false.
    boolean packetMatchesRequest(ESAT_CCSDSPacket& packet,
                                 int requestedPacket);

    // Read a packet from the device at the given address.
    // The requested packet is either:
    // - NEXT_TELEMETRY_PACKET_REQUESTED: next-packet telemetry;
    // - NEXT_TELECOMMAND_PACKET_REQUESTED: next-packet telecommand;
    // - from 0 to 255: named-packet telemetry with the given packet identifier.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Retry several times, waiting several milliseconds, if the
    // telecommand is not ready.
    // Return true on success; otherwise return false.
    boolean readPacket(TwoWire& bus,
                       byte address,
                       int requestedPacket,
                       ESAT_CCSDSPacket& packet,
                       byte millisecondsAfterWrites,
                       byte attempts,
                       byte millisecondsBetweenAttempts);

    // Read the packet data from the given address.
    // Return true on success; otherwise return false.
    boolean readPacketData(TwoWire& bus,
                           byte address,
                           ESAT_CCSDSPacket& packet);

    // Read the packet primary header from the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Return true on success; otherwise return false.
    boolean readPrimaryHeader(TwoWire& bus,
                              byte address,
                              ESAT_CCSDSPacket& packet,
                              byte millisecondsAfterWrites);

    // Write a packet request for to the given address.
    // The requested packet is either:
    // - NEXT_TELEMETRY_PACKET_REQUESTED: next-packet telemetry;
    // - NEXT_TELECOMMAND_PACKET_REQUESTED: next-packet telecommand;
    // - from 0 to 255: named-packet telemetry with the given packet identifier.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Return true on success; otherwise return false.
    boolean requestPacket(TwoWire& bus,
                          byte address,
                          int requestedPacket,
                          byte millisecondsAfterWrites);

    // Write the packet data to the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Return true on success; otherwise return false.
    boolean writePacketData(TwoWire& bus,
                            byte address,
                            ESAT_CCSDSPacket& packet,
                            byte millisecondsAfterWrites);

    // Write the primary header to the given address.
    // Wait some milliseconds after each write operation to give
    // the slave time to process the request.
    // Return true on success; otherwise return false.
    boolean writePrimaryHeader(TwoWire& bus,
                               byte address,
                               ESAT_CCSDSPacket& packet,
                               byte millisecondsAfterWrites);
};

// Global instance of the I2C master library.
extern ESAT_I2CMasterClass ESAT_I2CMaster;

#endif /* ESAT_I2CMaster_h */

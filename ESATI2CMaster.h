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

#ifndef ESATI2CMaster_h
#define ESATI2CMaster_h

#include <Arduino.h>
#include <ESATCCSDSPacket.h>
#include <Wire.h>

// ESAT I2C telecommand and telemetry protocol for I2C master nodes.
class ESATI2CMaster
{
  public:
    // Read a telemetry packet from the device at the given address.
    // Retry several times, waiting several milliseconds, if the
    // telemetry is not ready.
    // Return true on success; otherwise return false.
    boolean readTelemetry(TwoWire& bus,
                          byte address,
                          byte packetIdentifier,
                          ESATCCSDSPacket& packet,
                          byte tries,
                          word millisecondsBetweenRetries);

    // Write a telecommand to the device at the given address.
    // Retry several times, waiting several milliseconds, if the
    // telecommand queue is full.
    // Return true on success; otherwise return false.
    boolean writeTelecommand(TwoWire& bus,
                             byte address,
                             ESATCCSDSPacket& packet,
                             byte tries,
                             word millisecondsBetweenRetries);

  private:
    // Register numbers for telecommand and telemetry handling.
    enum RegisterNumbers
    {
      TELECOMMAND_PRIMARY_HEADER = 0x00,
      TELECOMMAND_PACKET_DATA = 0x01,
      TELECOMMAND_STATUS = 0x02,
      TELEMETRY_REQUEST = 0x10,
      TELEMETRY_STATUS = 0x11,
      TELEMETRY_VECTOR = 0x12,
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

    // Read the telecommand status from the given address.
    // Retry several times if the queue is not free,
    // waiting several milliseconds between retries.
    // Return true if the telecommand queue is free; otherwise return false.
    boolean readTelecommandStatus(TwoWire& bus,
                                  byte address,
                                  byte tries,
                                  word millisecondsBetweenRetries);

    // Read the packet data from the given address.
    // Return true on success; otherwise return false.
    boolean readTelemetryPacketData(TwoWire& bus,
                                    byte address,
                                    ESATCCSDSPacket& packet);

    // Read the packet primary header from the given address.
    // Return true on success; otherwise return false.
    boolean readTelemetryPrimaryHeader(TwoWire& bus,
                                       byte address,
                                       ESATCCSDSPacket& packet);

    // Read the telemetry status from the given address.
    // Retry several times if the telemetry is not ready,
    // waiting several milliseconds between retries.
    // Return true if the telemetry is ready; otherwise return false.
    boolean readTelemetryStatus(TwoWire& bus,
                                byte address,
                                byte tries,
                                word millisecondsBetweenRetries);

    // Write the packet data to the given address.
    // Return true on success; otherwise return false.
    boolean writeTelecommandPacketData(TwoWire& bus,
                                       byte address,
                                       ESATCCSDSPacket& packet);

    // Write the primary header to the given address.
    // Return true on success; otherwise return false.
    boolean writeTelecommandPrimaryHeader(TwoWire& bus,
                                          byte address,
                                          ESATCCSDSPacket& packet);

    // Write a telemetry request for a packet identifier to the given
    // address.
    // Return true on success; otherwise return false.
    boolean writeTelemetryRequest(TwoWire& bus,
                                  byte address,
                                  byte packetIdentifier);
};

extern ESATI2CMaster I2CMaster;

#endif

Copyright (C) 2017, 2018 Theia Space, Universidad Politécnica de Madrid

This file is part of Theia Space's ESAT utility library.

Theia Space's ESAT utility library is free software: you can
redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either
version 3 of the License, or (at your option) any later version.

Theia Space's ESAT utility library is distributed in the hope that it
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Theia Space's ESAT utility library.  If not, see
<http://www.gnu.org/licenses/>.


ESAT utility library version 2.1.0.

Shared utility libraries for ESAT.

Use with Arduino 1.8.0 or newer.

The src/directory contains the utility library, which consists of the
following modules:


# ESAT_Buffer

Stream interface to byte buffers with bounds checking.


# ESAT_CCSDSPacket

Standard CCSDS space packets.


# ESAT_CCSDSPacketContents

ESAT-flavoured CCSDS space packet contents interface.


# ESAT_CCSDSPacketHandler

ESAT-flavoured CCSDS space packet handler interface.


# ESAT_CCSDSPacketFromKISSFrameReader

Read CCSDS space packets from KISS frames coming from a stream.


# ESAT_CCSDSPacketToKISSFrameWriter

Write CCSDS space packets to KISS frames going through a stream.


# ESAT_CCSDSPrimaryHeader

The primary header of CCSDS space packets.


# ESAT_CCSDSSecondaryHeader

The secondary header of CCSDS space packets used by ESAT subsystems.


# ESAT_CCSDSTelecommandPacketDispatcher

Dispatch ESAT-flavoured telecommand CCSDS space packets.


# ESAT_CCSDSTelemetryPacketBuilder

Build ESAT-flavoured telemetry CCSDS space packets.


# ESAT_Clock

Real-time clock interface.


# ESAT_CRC8

8-bit cyclic redundancy check.


# ESAT_FlagContainer

Collection of 256 boolean flags.


# ESAT_I2CMaster

Query other ESAT slave subsystem boards through the I2C bus.


# ESAT_I2CSlave

Respond to queries from the master ESAT subsystem through the I2C bus.


# ESAT_KISSStream

Stream interface to standard KISS frames.


# ESAT_SemanticVersionNumber

Version numbers in major.minor.patch format.


# ESAT_SoftwareClock

Software real-time clock implementing the ESAT_Clock interface.


# ESAT_Timer

Software timer for time-accurate periodic tasks.


# ESAT_Timestamp

Timestamp handling.


# ESAT_Util

Assorted utilities like type conversions and string handling.

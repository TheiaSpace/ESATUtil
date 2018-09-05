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


# ESAT_CCSDSPrimaryHeader

The primary header of CCSDS space packets.


# ESAT_CCSDSSecondaryHeader

The secondary header of CCSDS space packets used by ESAT subsystems.


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


# ESAT_SoftwareClock

Software real-time clock.


# ESAT_Timer

Software timer for time-accurate periodic tasks.


# ESAT_Timestamp

Timestamp handling.


# ESAT_Util

Assorted utilities like type conversions and string handling.

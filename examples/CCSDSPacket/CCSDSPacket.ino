/*
 * Copyright (C) 2018 Theia Space, Universidad Politécnica de Madrid
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ESAT_CCSDSPacket.h>

// ESAT_CCSDSPacket example program.
// CCSDS Space Packets.

// Work with this packet.
const byte capacity = ESAT_CCSDSSecondaryHeader::LENGTH + 7;
byte buffer[capacity];
ESAT_CCSDSPacket packet(buffer, capacity);

// Header contents.
const word applicationProcessIdentifier = 5;
const word packetSequenceCount = 1000;
const byte majorVersionNumber = 2;
const byte minorVersionNumber = 1;
const byte patchVersionNumber = 0;
const byte packetIdentifier = 8;
// Launch time of the Voyager 1 probe.
const ESAT_Timestamp timestamp(1977, 9, 5, 12, 56, 0);

// User data field contents.
const float floatingPointNumber = 3.1416;
const int integerNumber = -1234;
const byte firstByte = 9;
const byte secondByte = 3;

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Seed the random number generator.
  randomSeed(0);
}

void loop()
{
  (void) Serial.println(F("####################################"));
  (void) Serial.println(F("CCSDS Space Packets example program."));
  (void) Serial.println(F("####################################"));
  // Clear the packet.
  (void) Serial.println(F("Clearing (flushing) the packet..."));
  packet.flush();
  // Fill the headers of a telemetry packet.
  (void) Serial.println(F("Writing the headers of a telemetry packet..."));
  packet.writeTelemetryHeaders(applicationProcessIdentifier,
                               packetSequenceCount,
                               timestamp,
                               majorVersionNumber,
                               minorVersionNumber,
                               patchVersionNumber,
                               packetIdentifier);
  // Fill the user data field.
  (void) Serial.println(F("Filling the user data field..."));
  (void) Serial.print(F("Writing floating-point number "));
  (void) Serial.print(floatingPointNumber, 4);
  (void) Serial.println(F("..."));
  packet.writeFloat(floatingPointNumber);
  (void) Serial.print(F("Writing integer number "));
  (void) Serial.print(integerNumber, DEC);
  (void) Serial.println(F("..."));
  packet.writeInt(integerNumber);
  (void) Serial.print(F("Writing first byte "));
  (void) Serial.print(firstByte, DEC);
  (void) Serial.println(F("..."));
  packet.writeByte(firstByte);
  // This last write will fail.
  (void) Serial.print(F("Writing second byte "));
  (void) Serial.print(secondByte, DEC);
  (void) Serial.println(F(" (this will fail)..."));
  packet.writeByte(secondByte);
  if (packet.triedToWriteBeyondCapacity())
  {
    (void) Serial.println(F("Tried to write beyond packet capacity!"));
  }
  // Print the packet contents.
  (void) Serial.print(F("Packet contents: "));
  (void) Serial.println(packet);
  // Read the contents of the packet data field by field.
  (void) Serial.println(F("Reading the packet data field by field..."));
  packet.rewind();
  (void) Serial.print(F("Secondary header: "));
  (void) Serial.println(packet.readSecondaryHeader());
  (void) Serial.print(F("Floating-point number: "));
  (void) Serial.println(packet.readFloat(), 4);
  (void) Serial.print(F("Integer number: "));
  (void) Serial.println(packet.readInt(), DEC);
  (void) Serial.print(F("First byte: "));
  (void) Serial.println(packet.readByte(), DEC);
  (void) Serial.print(F("Second byte (this will fail and return dummy data): "));
  (void) Serial.println(packet.readByte(), DEC);
  if (packet.triedToReadBeyondLength())
  {
    (void) Serial.println(F("Tried to read beyond packet capacity!"));
  }
  // Print the raw packet contents.
  (void) Serial.print(F("Hexadecimal dump of the raw packet contents: "));
  byte rawPacketBytesArray[packet.length()];
  ESAT_Buffer rawPacketBytes(rawPacketBytesArray, packet.length());
  (void) packet.writeTo(rawPacketBytes);
  (void) Serial.println(rawPacketBytes);
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

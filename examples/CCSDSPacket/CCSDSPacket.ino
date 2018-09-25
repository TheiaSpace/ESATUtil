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
  (void) Serial.println(String("####################################"));
  (void) Serial.println(String("CCSDS Space Packets example program."));
  (void) Serial.println(String("####################################"));
  // Clear the packet.
  (void) Serial.println(String("Clearing (flushing) the packet..."));
  packet.flush();
  // Fill the headers of a telemetry packet.
  (void) Serial.println(String("Writing the headers of a telemetry packet..."));
  packet.writeTelemetryHeaders(applicationProcessIdentifier,
                               packetSequenceCount,
                               timestamp,
                               majorVersionNumber,
                               minorVersionNumber,
                               patchVersionNumber,
                               packetIdentifier);
  // Fill the user data field.
  (void) Serial.println(String("Filling the user data field..."));
  (void) Serial.print(String("Writing floating-point number "));
  (void) Serial.print(String(floatingPointNumber, 4));
  (void) Serial.println(String("..."));
  packet.writeFloat(floatingPointNumber);
  (void) Serial.print(String("Writing integer number "));
  (void) Serial.print(String(integerNumber, DEC));
  (void) Serial.println(String("..."));
  packet.writeInt(integerNumber);
  (void) Serial.print(String("Writing first byte "));
  (void) Serial.print(String(firstByte, DEC));
  (void) Serial.println(String("..."));
  packet.writeByte(firstByte);
  // This last write will fail.
  (void) Serial.print(String("Writing second byte "));
  (void) Serial.print(String(secondByte, DEC));
  (void) Serial.println(String(" (this will fail)..."));
  packet.writeByte(secondByte);
  if (packet.triedToWriteBeyondCapacity())
  {
    (void) Serial.println(String("Tried to write beyond packet capacity!"));
  }
  // Print the packet contents.
  (void) Serial.print(String("Packet contents: "));
  (void) Serial.println(packet);
  // Read the contents of the packet data field by field.
  (void) Serial.println(String("Reading the packet data field by field..."));
  packet.rewind();
  (void) Serial.print(String("Secondary header: "));
  (void) Serial.println(packet.readSecondaryHeader());
  (void) Serial.print(String("Floating-point number: "));
  (void) Serial.println(String(packet.readFloat(), 4));
  (void) Serial.print(String("Integer number: "));
  (void) Serial.println(String(packet.readInt(), DEC));
  (void) Serial.print(String("First byte: "));
  (void) Serial.println(String(packet.readByte(), DEC));
  (void) Serial.print(String("Second byte (this will fail and return dummy data): "));
  (void) Serial.println(String(packet.readByte(), DEC));
  if (packet.triedToReadBeyondLength())
  {
    (void) Serial.println(String("Tried to read beyond packet capacity!"));
  }
  // Print the raw packet contents.
  (void) Serial.print(String("Hexadecimal dump of the raw packet contents: "));
  byte rawPacketBytesArray[packet.length()];
  ESAT_Buffer rawPacketBytes(rawPacketBytesArray, packet.length());
  (void) packet.writeTo(rawPacketBytes);
  (void) Serial.println(rawPacketBytes);
  // End.
  (void) Serial.println(String("End."));
  (void) Serial.println(String(""));
  delay(1000);
}

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

#include <ESAT_CCSDSPacketFromKISSFrameReader.h>
#include <ESAT_CCSDSPacketToKISSFrameWriter.h>
#include <ESAT_KISSStream.h>

// ESAT_CCSDSPacketFromKISSFrameReader and
// ESAT_CCSDSPacketToKISSFrameWriter example program.
// Pass around CCSDS Space Packets in KISS frames.

// Store packets here.
const byte packetDataLength =
  ESAT_CCSDSSecondaryHeader::LENGTH;
const byte packetLength =
  ESAT_CCSDSPrimaryHeader::LENGTH + packetDataLength;
byte inputPacketData[packetDataLength];
ESAT_CCSDSPacket inputPacket(inputPacketData, packetDataLength);
byte outputPacketData[packetDataLength];
ESAT_CCSDSPacket outputPacket(outputPacketData, packetDataLength);

// Store frames here.
// Usually, you will want to pass frames through communication streams
// like Serial or SPI instead of storing them in an ESAT_Buffer.
const byte frameLength = ESAT_KISSStream::frameLength(packetLength);
byte frameData[frameLength];
ESAT_Buffer frame(frameData, frameLength);

// Pass packets in frames here.
byte readerData[packetLength];
ESAT_CCSDSPacketFromKISSFrameReader frameReader(frame,
                                                readerData,
                                                packetLength);
ESAT_CCSDSPacketToKISSFrameWriter frameWriter(frame);

// Header contents.
const word applicationProcessIdentifier = 5;
const unsigned long packetSequenceCount = 0;
const byte majorVersionNumber = 2;
const byte minorVersionNumber = 1;
const byte patchVersionNumber = 0;
const byte packetIdentifier = 3;
// Launch time of Venera 7 mission.
const ESAT_Timestamp timestamp(1970, 8, 17, 5, 38, 22);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
}

void loop()
{
  (void) Serial.println(String("###################################################"));
  (void) Serial.println(String("CCSDS Space Packets in KISS frames example program."));
  (void) Serial.println(String("###################################################"));
  // Build the input packet.
  (void) Serial.println(String("Building the input packet..."));
  inputPacket.flush();
  inputPacket.writeTelecommandHeaders(applicationProcessIdentifier,
                                      packetSequenceCount,
                                      timestamp,
                                      majorVersionNumber,
                                      minorVersionNumber,
                                      patchVersionNumber,
                                      packetIdentifier);
  (void) Serial.print(String("Input packet: "));
  (void) Serial.println(inputPacket);
  // Write the packet into a frame.
  (void) Serial.println(String("Writing the input packet to a frame..."));
  (void) frameWriter.unbufferedWrite(inputPacket);
  (void) Serial.print(String("Hexadecimal dump of the frame: "));
  (void) Serial.println(frame);
  // Read the packet from a frame.
  (void) Serial.println(String("Reading a packet from the frame..."));
  frame.rewind();
  (void) frameReader.read(outputPacket);
  (void) Serial.print(String("Output packet: "));
  (void) Serial.println(outputPacket);
  // End.
  (void) Serial.println(String("End."));
  (void) Serial.println(String(""));
  delay(1000);
}

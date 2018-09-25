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

#include <ESAT_Buffer.h>
#include <ESAT_KISSStream.h>

// ESAT_KISSStream example program.
// KISS frame streams.

// Input buffer.
// Use an ESAT_Buffer object for pretty-printing.
const byte inputCapacity = 4;
byte inputBuffer[inputCapacity];
ESAT_Buffer input(inputBuffer, inputCapacity);

// Frame buffer.
const byte frameCapacity = ESAT_KISSStream::frameLength(inputCapacity);
byte frameBuffer[frameCapacity];
ESAT_Buffer frame(frameBuffer, frameCapacity);

// Decoded contents buffer.
const byte decodedCapacity = inputCapacity;
byte decodedBuffer[decodedCapacity];
ESAT_Buffer decoded(decodedBuffer, decodedCapacity);

// KISS frame streams.
const byte readerCapacity = inputCapacity;
byte readerBuffer[readerCapacity];
ESAT_KISSStream kissReader(frame, readerBuffer, readerCapacity);
ESAT_KISSStream kissWriter(frame);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Fill the input buffer with regular bytes and special bytes
  // that must be escaped.
  input.write(0x01);
  input.write(0xC0);
  input.write(0xDB);
  input.write(0xFF);
}

void loop()
{
  (void) Serial.println("###################################");
  (void) Serial.println("KISS frame streams example program.");
  (void) Serial.println("###################################");
  // Show the contents of the input buffer.
  (void) Serial.print("Hexadecimal dump of the input: ");
  (void) Serial.println(input);
  // Write the contents of the input buffer to the KISS frame writer.
  // The KISS frame writer writes to ESAT_Buffer frame.
  (void) Serial.println("Starting a KISS frame...");
  (void) kissWriter.beginFrame();
  (void) Serial.println("Writing the input to the KISS frame...");
  input.rewind();
  while (input.available() > 0)
  {
    (void) kissWriter.write(input.read());
  }
  (void) Serial.println("Ending the KISS frame...");
  (void) kissWriter.endFrame();
  // Print the KISS frame.
  (void) Serial.print("Hexadecimal dump of the KISS frame: ");
  (void) Serial.println(frame);
  // Read the contents of the KISS frame reader.
  // The KISS frame reader reads from ESAT_Buffer frame,
  // so rewind frame to the KISS frame reader read from it.
  frame.rewind();
  (void) Serial.print("Receiving a frame...");
  const boolean gotFrame = kissReader.receiveFrame();
  if (gotFrame)
  {
    (void) Serial.println("Hexadecimal dump of the decoded frame contents:");
    while (kissReader.available() > 0)
    {
      (void) decoded.write(kissReader.read());
    }
    (void) Serial.println(decoded);
  }
  else
  {
    (void) Serial.println("Couldn't receive a frame.");
  }
  // End.
  (void) Serial.println("End.");
  (void) Serial.println("");
  delay(1000);
}

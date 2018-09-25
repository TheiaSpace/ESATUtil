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

// ESAT_Buffer example program.
// Stream interface for reading from and writing to buffers.

// Work with this buffer.
const byte capacity = 4;
byte array[capacity];
ESAT_Buffer buffer(array, capacity);

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
  (void) Serial.println("###############################");
  (void) Serial.println("Buffer streams example program.");
  (void) Serial.println("###############################");
  // Clear the buffer.
  (void) Serial.println("Clearing (flushing) the buffer...");
  buffer.flush();
  // Fill the buffer.
  (void) Serial.println("Filling the buffer with random data...");
  while (buffer.length() < buffer.capacity())
  {
    (void) buffer.write(random(0, 256));
  }
  // Print the buffer.
  (void) Serial.print("Hexadecimal dump of the buffer: ");
  (void) Serial.println(buffer);
  (void) Serial.print("Buffer length: ");
  (void) Serial.println(buffer.length(), DEC);
  // Read the buffer byte by byte.
  (void) Serial.println("Reading the buffer contents byte by byte...");
  buffer.rewind();
  while (buffer.available() > 0)
  {
    (void) Serial.print("Available bytes: ");
    (void) Serial.println(buffer.available(), DEC);
    (void) Serial.print("Current byte: 0x");
    (void) Serial.println(buffer.read(), HEX);
  }
  // Direct access to the buffer contents.
  (void) Serial.println("Accessing the backend array of the buffer...");
  for (unsigned long index = 0; index < buffer.length(); index++)
  {
    (void) Serial.print("array[");
    (void) Serial.print(index, DEC);
    (void) Serial.print("] = 0x");
    (void) Serial.println(array[index], HEX);
  }
  // End.
  (void) Serial.println("End.");
  (void) Serial.println("");
  delay(1000);
}

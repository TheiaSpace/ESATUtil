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

#include <ESAT_FlagContainer.h>

// ESAT_FlagContainer example program.
// Bit vectors.

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
  (void) Serial.println(String("######################"));
  (void) Serial.println(String("Flags example program."));
  (void) Serial.println(String("######################"));
  // Fill flags1 with powers of two.
  ESAT_FlagContainer flags1;
  flags1.set(1);
  flags1.set(2);
  flags1.set(4);
  flags1.set(8);
  flags1.set(16);
  flags1.set(32);
  flags1.set(64);
  flags1.set(128);
  (void) Serial.print(String("Active flags in flags1: "));
  (void) Serial.println(flags1);
  // Fill flags2 with Fibonacci numbers.
  ESAT_FlagContainer flags2;
  flags2.set(0);
  flags2.set(1);
  flags2.set(2);
  flags2.set(3);
  flags2.set(5);
  flags2.set(8);
  flags2.set(13);
  flags2.set(21);
  flags2.set(34);
  flags2.set(55);
  flags2.set(89);
  flags2.set(144);
  flags2.set(233);
  // Bitwise operations.
  (void) Serial.print(String("Active flags in flags2: "));
  (void) Serial.println(flags2);
  (void) Serial.print(String("Active flags in flags1 & flags2: "));
  (void) Serial.println(flags1 & flags2);
  (void) Serial.print(String("Active flags in flags1 | flags2: "));
  (void) Serial.println(flags1 | flags2);
  (void) Serial.print(String("Active flags in flags1 ^ flags2: "));
  (void) Serial.println(flags1 ^ flags2);
  (void) Serial.print(String("Active flags in ~flags1: "));
  (void) Serial.println(~flags1);
  // Iterate over flags1.
  (void) Serial.println("Iterating over flags1...");
  while (flags1.available() > 0)
  {
    // Print the number of available flags.
    (void) Serial.print(String("Available flags in flags1: "));
    (void) Serial.print(String(flags1.available(), DEC));
    (void) Serial.println(String("."));
    // Print the next true flag.
    (void) Serial.print(String("Next true flag in flags1: "));
    (void) Serial.println(String(flags1.readNext(), DEC));
    // Clear the next true flag.
    (void) Serial.print(String("Clearing flag number "));
    (void) Serial.print(String(flags1.readNext(), DEC));
    (void) Serial.println(String("."));
    flags1.clear(flags1.readNext());
  }
  // End.
  (void) Serial.println(String("End."));
  (void) Serial.println(String(""));
  delay(1000);
}

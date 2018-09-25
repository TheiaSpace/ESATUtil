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

#include <ESAT_CRC8.h>

// ESAT_CRC8 example program.
// Compute the CRC8 of some data.

const byte polynomial = 0b00000111;
ESAT_CRC8 crc(polynomial);

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
  // Print some introductory text.
  (void) Serial.println(String("#####################"));
  (void) Serial.println(String("CRC8 example program."));
  (void) Serial.println(String("#####################"));
  (void) Serial.print(String("Polynomial: "));
  (void) Serial.println(crc);
  // Ask the user for input text.
  String inputText;
  while (inputText.length() == 0)
  {
    (void) Serial.println(String("Type some text, please:"));
    inputText = Serial.readString();
  }
  // Print the input text.
  (void) Serial.print(String("Input text: \""));
  (void) Serial.print(inputText);
  (void) Serial.println(String("\""));
  // Compute the CRC8 remainder.
  (void) crc.print(inputText);
  const byte remainder = byte(crc.read());
  // Print the CRC8 remainder.
  (void) Serial.print(String("CRC8 (hexadecimal): "));
  (void) Serial.println(String(remainder, HEX));
  // End.
  (void) Serial.println(String("End."));
  (void) Serial.println(String(""));
  delay(1000);
}

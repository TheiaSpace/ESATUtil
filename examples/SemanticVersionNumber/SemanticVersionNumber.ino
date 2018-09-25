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

#include <ESAT_SemanticVersionNumber.h>

// ESAT_SemanticVersionNumber example program.
// Version numbers.

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
  (void) Serial.println(String("#########################################"));
  (void) Serial.println(String("Semantic version numbers example program."));
  (void) Serial.println(String("#########################################"));
  // Printing a version number.
  const ESAT_SemanticVersionNumber version(2, 1, 0);
  (void) Serial.print(String("Version number: "));
  (void) Serial.println(version);
  (void) Serial.print(String("Major version number: "));
  (void) Serial.println(String(version.majorVersionNumber, DEC));
  (void) Serial.print(String("Minor version number: "));
  (void) Serial.println(String(version.minorVersionNumber, DEC));
  (void) Serial.print(String("Patch version number: "));
  (void) Serial.println(String(version.patchVersionNumber, DEC));
  // Comparisons.
  if (version == ESAT_SemanticVersionNumber(1, 2, 3))
  {
    (void) Serial.print(version);
    (void) Serial.print(String(" == "));
    (void) Serial.println(ESAT_SemanticVersionNumber(1, 2, 3));
  }
  else
  {
    (void) Serial.print(version);
    (void) Serial.print(String(" != "));
    (void) Serial.println(ESAT_SemanticVersionNumber(1, 2, 3));
  }
  if (version == ESAT_SemanticVersionNumber(2, 1, 0))
  {
    (void) Serial.print(version);
    (void) Serial.print(String(" == "));
    (void) Serial.println(ESAT_SemanticVersionNumber(2, 1, 0));
  }
  else
  {
    (void) Serial.print(version);
    (void) Serial.print(String(" != "));
    (void) Serial.println(ESAT_SemanticVersionNumber(2, 1, 0));
  }
  // End.
  (void) Serial.println(String("End."));
  (void) Serial.println(String(""));
  delay(1000);
}

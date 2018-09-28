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
  // Seed the random number generator.
  randomSeed(0);
}

void loop()
{
  (void) Serial.println(F("#########################################"));
  (void) Serial.println(F("Semantic version numbers example program."));
  (void) Serial.println(F("#########################################"));
  // Printing a version number.
  const ESAT_SemanticVersionNumber firstVersion(random(1, 3),
                                                random(0, 2),
                                                random(0, 2));
  (void) Serial.print(F("First version number: "));
  (void) Serial.println(firstVersion);
  (void) Serial.print(F("First major version number: "));
  (void) Serial.println(firstVersion.majorVersionNumber, DEC);
  (void) Serial.print(F("First minor version number: "));
  (void) Serial.println(firstVersion.minorVersionNumber, DEC);
  (void) Serial.print(F("First patch version number: "));
  (void) Serial.println(firstVersion.patchVersionNumber, DEC);
  const ESAT_SemanticVersionNumber secondVersion(random(1, 3),
                                                 random(0, 2),
                                                 random(0, 2));
  (void) Serial.print(F("Second version number: "));
  (void) Serial.println(secondVersion);
  (void) Serial.print(F("Second major version number: "));
  (void) Serial.println(secondVersion.majorVersionNumber, DEC);
  (void) Serial.print(F("Second minor version number: "));
  (void) Serial.println(secondVersion.minorVersionNumber, DEC);
  (void) Serial.print(F("Second patch version number: "));
  (void) Serial.println(secondVersion.patchVersionNumber, DEC);
  // Comparisons.
  if (firstVersion == secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" == "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion != secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" != "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion < secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" < "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion <= secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" <= "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion > secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" > "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion >= secondVersion)
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" >= "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion.isBackwardCompatibleWith(secondVersion))
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" is backward compatible with "));
    (void) Serial.println(secondVersion);
  }
  if (firstVersion.isForwardCompatibleWith(secondVersion))
  {
    (void) Serial.print(firstVersion);
    (void) Serial.print(F(" is forward compatible with "));
    (void) Serial.println(secondVersion);
  }
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
  delay(1000);
}

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

#include <ESAT_SoftwareClock.h>

// ESAT_SoftwareCLock example program.
// Basic timekeeping.

ESAT_SoftwareClock clock;
// Start the clock at the time of launch of the first artificial
// satellite (Sputnik 1).
ESAT_Timestamp startTime(1957, 10, 4, 19, 28, 34);

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Set the clock.
  clock.write(startTime);
}

void loop()
{
  (void) Serial.println("###############################");
  (void) Serial.println("Software clock example program.");
  (void) Serial.println("###############################");
  // Print the time.
  (void) Serial.print("Start time: ");
  (void) Serial.println(startTime);
  (void) Serial.print("Current time: ");
  (void) Serial.println(clock.read());
  // End.
  (void) Serial.println("End");
  (void) Serial.println("");
  delay(1000);
}

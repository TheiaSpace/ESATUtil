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

#include <ESAT_Timer.h>

// ESAT_Timer example program.
// Periodic tasks.

// Period of 1000 milliseconds.
const word period = 1000;

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  // Set the timer.
  ESAT_Timer.begin(1000);
  // Seed the random number generator.
  randomSeed(0);
}

void loop()
{
  (void) Serial.println(F("######################"));
  (void) Serial.println(F("Timer example program."));
  (void) Serial.println(F("######################"));
  // Wait until the start of the next cycle.
  (void) Serial.println(F("Waiting until the next cycle..."));
  ESAT_Timer.waitUntilNextCycle();
  // Wake-up.
  (void) Serial.println(F("Starting a new cycle."));
  // Print the current processor time.
  (void) Serial.print(F("Current processor time: "));
  (void) Serial.print(millis(), DEC);
  (void) Serial.println(F(" milliseconds."));
  // Print the period.
  (void) Serial.print(F("Period: "));
  (void) Serial.print(ESAT_Timer.period, DEC);
  (void) Serial.println(F(" milliseconds."));
  // Print the processor load estimate.
  (void) Serial.print(F("Processor load: "));
  (void) Serial.print(ESAT_Timer.load());
  (void) Serial.println(F(" %."));
  // Stay busy for a random amount of time.
  const word busyMilliseconds = random(0, ESAT_Timer.period);
  (void) Serial.print(F("Staying busy until "));
  (void) Serial.print(busyMilliseconds, DEC);
  (void) Serial.println(F(" milliseconds into the cycle..."));
  while (ESAT_Timer.elapsedMilliseconds() < busyMilliseconds)
  {
  }
  // Print the elapsed cycle time.
  (void) Serial.print(F("Elapsed cycle time: "));
  (void) Serial.print(ESAT_Timer.elapsedMilliseconds(), DEC);
  (void) Serial.println(F(" milliseconds."));
  // End.
  (void) Serial.println(F("End."));
  (void) Serial.println();
}

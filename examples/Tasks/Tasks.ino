/*
 * Copyright (C) 2020 Theia Space, Universidad Politécnica de Madrid
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

#include <ESAT_Task.h>
#include <ESAT_TaskScheduler.h>

// ESAT_Task and ESAT_TaskScheduler example program.
// Periodic tasks.

// A task that runs ten times per second.
class HighFrequencyTaskClass: public ESAT_Task
{
  public:
    unsigned long period()
    {
      return 100;
    }

    void run()
    {
      (void) Serial.print(millis(), DEC);
      (void) Serial.println(F(": high frequency task."));
    }
};

HighFrequencyTaskClass HighFrequencyTask;

// A task that runs once every second.
class LowFrequencyTaskClass: public ESAT_Task
{
  public:
    unsigned long period()
    {
      return 1000;
    }

    void run()
    {
      (void) Serial.print(millis(), DEC);
      (void) Serial.println(F(": low frequency task."));
    }
};

LowFrequencyTaskClass LowFrequencyTask;


// Scheduler for periodic tasks.
ESAT_TaskScheduler scheduler;

void setup()
{
  // Configure the Serial interface.
  Serial.begin(9600);
  // Wait until Serial is ready.
  while (!Serial)
  {
  }
  scheduler.add(HighFrequencyTask);
  scheduler.add(LowFrequencyTask);
  (void) Serial.println(F("###############################"));
  (void) Serial.println(F("Periodic tasks example program."));
  (void) Serial.println(F("###############################"));
}

void loop()
{
  scheduler.run();
}

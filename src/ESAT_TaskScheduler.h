/*
 * Copyright (C) 2020 Theia Space, Universidad Politécnica de Madrid
 *
 * This file is part of Theia Space's ESAT Util library.
 *
 * Theia Space's ESAT Util library is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Theia Space's ESAT Util library is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Theia Space's ESAT Util library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <Arduino.h>
#include "ESAT_Task.h"

#ifndef ESAT_TaskScheduler_h
#define ESAT_TaskScheduler_h

// Periodic task scheduler.
class ESAT_TaskScheduler
{
  public:
    // Add a new task to the list of scheduled tasks.
    void add(ESAT_Task& task);

    // Run the scheduled tasks.
    void run();

  private:
    // List of scheduled tasks.
    ESAT_Task* tasks = nullptr;
};

#endif /* ESAT_TaskScheduler_h */

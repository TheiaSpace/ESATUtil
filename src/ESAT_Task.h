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

#ifndef ESAT_Task_h
#define ESAT_Task_h

// Periodic task interface.
// Use together with ESAT_TaskScheduler to run periodic tasks.
class ESAT_Task
{
  public:
    // Next task in the task list.
    // ESAT_TaskScheduler uses this to keep a linked list of
    // registered tasks: it can traverse the list by going from one
    // task object to the next object until reaching the end of the
    // list at nullptr.
    // Only ESAT_TaskScheduler should care about this.
    // Each task object should be added just one time to just one
    // ESAT_TaskScheduler object.
    ESAT_Task* next;

    // Last time (in milliseconds) this task was run.
    // ESAT_TaskScheduler uses this and period() to know when to run a
    // task.
    // Only ESAT_TaskScheduler should care about this.
    unsigned long lastExecutionTime;

    // Trivial destructor.
    // We need to define it because the C++ programming language
    // works this way.
    virtual ~ESAT_Task() {};

    // Return the milliseconds between successive executions of this task.
    virtual unsigned long period() = 0;

    // Run the task.
    virtual void run() = 0;
};

#endif /* ESAT_Task_h */

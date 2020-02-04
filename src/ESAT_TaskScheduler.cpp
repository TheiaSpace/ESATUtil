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

#include "ESAT_TaskScheduler.h"

void ESAT_TaskScheduler::add(ESAT_Task& task)
{
  task.next = tasks;
  task.lastExecutionTime = micros();
  tasks = &task;
}

void ESAT_TaskScheduler::run()
{
  for (ESAT_Task* task = tasks;
       task != nullptr;
       task = task->next)
  {
    const unsigned long period = task->period();
    if ((micros() - task->lastExecutionTime) >= period)
    {
      task->run();
      task->lastExecutionTime = task->lastExecutionTime + period;
    }
  }
}

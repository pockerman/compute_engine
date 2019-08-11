#ifndef TASK_UITILITIES_H
#define TASK_UITILITIES_H

#include "kernel/executors/task_base.h"
#include <vector>

namespace kernel
{
    namespace taskutils
    {
        template<typename TaskTp>
        bool tasks_finished(const std::vector<TaskTp>& tasks){

            for(const auto& task: tasks){

                if(!task->finished()){
                    return false;
                }
            }

            return true;
        }
    }
}

#endif // TASK_UITILITIES_H
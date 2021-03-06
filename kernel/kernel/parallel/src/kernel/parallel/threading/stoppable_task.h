#ifndef STOPPABLE_TASK_H
#define STOPPABLE_TASK_H

#include "kernel/base/config.h"

#ifdef USE_LOG
#include "kernel/utilities/logger.h"
#endif

#include "kernel/parallel/threading/task_base.h"

namespace kernel {


/// \brief Base class for a task that can stopped
/// The stop condition is described by the template
/// parameter. It must be default constructed.
///
template<typename StopCondition>
class StoppableTask: public TaskBase
{

public:

    /// \brief The condition type
    typedef StopCondition stop_condition_t;

    /// \brief Destructor
    ~StoppableTask();

    /// \brief Execute the task
    void operator()();

    /// \brief Returns true if the task should stop
    bool should_stop()const{return stop_cond_.stop();}

    /// \brief Returns true if the task is stopped
    bool is_stopped()const{return stop_cond_.stop();}

    /// \brief Access the stop condition object. since the conditionals may change
    /// allow this to be accessed externally.
    stop_condition_t& get_condition(){return stop_cond_;}

protected:

    /// \brief Constructor
    explicit StoppableTask(const stop_condition_t& condition);

    /// \brief The object responsible for providing
    /// the information if the task should be stopped or not
    stop_condition_t stop_cond_;
};

template<typename StopCondition>
StoppableTask<StopCondition>::StoppableTask(const stop_condition_t& condition)
    :
    TaskBase(),
    stop_cond_(condition)
{}

template<typename StopCondition>
StoppableTask<StopCondition>::~StoppableTask()
{}

template<typename StopCondition>
void
StoppableTask<StopCondition>::operator()(){

    try {

        this->set_state(TaskBase::TaskState::STARTED);
        this->run();
        if(!stop_cond_.stop()){
            this->set_state(TaskBase::TaskState::FINISHED);
        }
        else{
           this->set_state(TaskBase::TaskState::STOPPED);
        }

#ifdef USE_LOG
        std::ostringstream message;
        message<<"Task: "<<get_name()<<" finished successfully";
        Logger::log_info(message.str());
#endif
    }
    catch(std::logic_error& error){

#ifdef USE_LOG
        std::ostringstream message;
        message<<"A logic error occured whilst running task: "<<this->get_name();
        message<<" what() says: "<<error.what();
        Logger::log_error(message.str());
#endif

        // whatever caused this, we assume that the task was interrupted
        // by an exception
        set_state(TaskBase::TaskState::INTERRUPTED_BY_EXCEPTION);

    }
    catch (...) {

#ifdef USE_LOG
        std::ostringstream message;
        message<<"An exception occured whilst running task: "<<this->get_name();
        Logger::log_error(message.str());
#endif

        // whatever caused this, we assume that the task was interrupted
        // by an exception
        set_state(TaskBase::TaskState::INTERRUPTED_BY_EXCEPTION);
    }
}

}

#endif // STOPPABLE_TASK_H

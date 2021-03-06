#include "kernel/parallel/threading/thread_pool.h"
#include "kernel/parallel/threading/kernel_thread.h"
#include "kernel/parallel/threading/task_base.h"

#include <stdexcept> // for std::invalid_argument
#include <iostream>
namespace kernel
{

ThreadPool::ThreadPool(uint_t n_threads, bool start_)
    :
pool_(),
n_threads_(n_threads),
next_thread_available_ (kernel::KernelConsts::invalid_size_type()),
is_started_(false),
is_closed_(true)
{
    // TODO: perhaps we could request the system
    // using std::thread::hardware_concurrency()
    // or having a default number of threads?
    if(n_threads_ == 0){
        throw std::invalid_argument("Cannot start pool with no threads");
    }

    if(start_){
        start();
    }
}

ThreadPool::ThreadPool(const ThreadPoolOptions& options)
    :
pool_(),
n_threads_(),
next_thread_available_ (kernel::KernelConsts::invalid_size_type()),
options_(options),
is_started_(false),
is_closed_(true)
 {
          // TODO: perhaps we could request the system
          // using std::thread::hardware_concurrency()
          // or having a default number of threads?
          if(options_.n_threads == 0){
              throw std::invalid_argument("Cannot start pool with no threads");
          }

          if(options_.start_on_construction){
              start();
          }
}

ThreadPool::~ThreadPool(){
    close();
}


void
ThreadPool::start(){


    if(is_started_){
        throw std::logic_error("Pool is already running. You need to stop is first");
    }

    pool_.reserve(options_.n_threads);
    for(uint_t t=0; t < options_.n_threads; ++t){
        pool_.push_back( std::make_unique<detail::kernel_thread>(t) );
        pool_[t]->start();
    }

    is_started_ = true;
    is_closed_ = false;
    if(options_.msg_on_start_up){
        std::cout<<"MESSAGE:  Start up thread pool with "+std::to_string(options_.n_threads)<<" threads "<<std::endl;
    }

}

void
ThreadPool::add_task(TaskBase& task){


    if(!is_started_){
      throw std::logic_error("Thread pool is not started");
    }

    if(next_thread_available_ == kernel::KernelConsts::invalid_size_type() ||
       next_thread_available_ >= pool_.size()){
        next_thread_available_=0;
    }

    // pass the task
    pool_[next_thread_available_]->push_task(task);
    next_thread_available_++;

    if(options_.msg_when_adding_tasks){
        std::cout<<"MESSAGE:  Added task: "<<task.get_name()<<std::endl;
    }
}

void
ThreadPool::add_tasks(const std::vector<std::unique_ptr<task_t>>& tasks){

    if(!is_started_){
      throw std::logic_error("Thread pool is not started");
    }

    for(uint_t t=0; t<tasks.size(); ++t){
        if(!tasks[t]){
          throw std::invalid_argument("Attempt to add a null task pointer");
        }
        add_task(*(tasks[t].get()));
    }
}


void
ThreadPool::close(){

    if(!is_started_ || is_closed_){
        return;
    }

    for(uint_t t=0; t<pool_.size(); ++t){
        if(pool_[t]){
            pool_[t]->stop();
            pool_[t]->join();
        }
    }

    is_started_ = false;
    is_closed_ = true;
    if(options_.msg_on_shut_down){
        std::cout<<"MESSAGE:  Shut down  thread pool with "+std::to_string(options_.n_threads)<<" threads "<<std::endl;
    }
}
}

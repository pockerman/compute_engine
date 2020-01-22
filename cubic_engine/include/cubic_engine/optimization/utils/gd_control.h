#ifndef GD_CONTROL_H
#define GD_CONTROL_H

#include "kernel/utilities/iterative_algorithm_controller.h"
#include "kernel/base/kernel_consts.h"
#include "cubic_engine/base/cubic_engine_types.h"

namespace cengine
{

struct GDControl: public kernel::IterativeAlgorithmController
{
    constexpr static real_t DEFAULT_LEARNING_RATE = 0.01;

    /// \brief The learning rate
    real_t learning_rate;

    /// \brief Constructor
    GDControl( uint_t max_num_itrs,
               real_t tolerance=kernel::KernelConsts::tolerance(),
               real_t eta=GDControl::DEFAULT_LEARNING_RATE );

    /// \brief reset
    virtual void reset(const GDControl& control)final;

};


inline
GDControl::GDControl( uint_t max_num_itrs, real_t tolerance, real_t eta_ )
    :
kernel::IterativeAlgorithmController(max_num_itrs,  tolerance),
learning_rate(eta_)
{}

void
GDControl::reset(const GDControl& control){

    this->kernel::IterativeAlgorithmController::reset(control);
    learning_rate = control.learning_rate;
}

}

#endif // GD_CONTROL_H

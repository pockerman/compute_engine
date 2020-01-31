#include "kernel/dynamics/diff_drive_dynamics.h"
#include <cmath>
namespace kernel
{
namespace dynamics
{

DiffDriveDynamics::DiffDriveDynamics()
    :
  state_()
{
    state_.set(0, {"X", 0.0});
    state_.set(0, {"Y", 0.0});
    state_.set(0, {"Theta", 0.0});
}

DiffDriveDynamics::DiffDriveDynamics(DiffDriveDynamics::state_t&& state)
    :
      state_(std::move(state))
{}

void
DiffDriveDynamics::integrate(real_t distance, real_t orientation, const std::array<real_t, 2>& errors){

    auto values = state_.get_values();

    state_[0] += values[0] + (distance + errors[0])*std::cos(values[2] + orientation + errors[1]);
    state_[1] += values[1] + (distance + errors[0])*std::sin(values[2] + orientation + errors[1]);
    state_[2] += values[2] + orientation + errors[1];
}
}
}
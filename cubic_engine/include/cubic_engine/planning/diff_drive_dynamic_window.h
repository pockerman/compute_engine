#ifndef DIFF_DRIVE_DYNAMIC_WINDOW_H
#define DIFF_DRIVE_DYNAMIC_WINDOW_H

#include "cubic_engine/planning/dynamic_window.h"
#include "kernel/dynamics/diff_drive_dynamics.h"
#include <vector>
#include <array>
#include <limits>
#include <cmath>

namespace cengine
{
namespace planning {

///
/// \brief The DiffDriveDWConfig struct. Helper
/// struct to hold configuration properties
/// for the DiffDriveDW class
///
struct DiffDriveDWConfig
{

      real_t max_speed;
      real_t min_speed;
      real_t max_yaw_rate;
      real_t max_accel;
      real_t max_dyawrate;
      real_t dt;
      real_t min_cost;
      real_t v_reso;
      real_t yawrate_reso;
      real_t robot_radius;
      uint_t skip_n;
      real_t predict_time;
      real_t speed_cost_gain;
      real_t to_goal_cost_gain;
};

///
/// \brief The DiffDriveWindowProperties struct
///
struct DiffDriveWindowProperties
{
    real_t v_max;
    real_t v_min;
    real_t w_max;
    real_t w_min;
};

///
/// \brief DiffDriveDW class. Dynamic window approach for
/// differential drive systems. This class is an implementation
/// from: https://github.com/onlytailei/CppRobotics/blob/master/src/dynamic_window_approach.cpp
///
template<typename StateTp, typename GoalTp>
class DiffDriveDW: public DynamicWindowBase<StateTp, DiffDriveDWConfig, DiffDriveWindowProperties>
{
public:

    ///
    /// \brief state_t. The type of the state used by the window
    ///
    typedef typename DynamicWindowBase<StateTp, DiffDriveDWConfig, DiffDriveWindowProperties>::state_t state_t;


    ///
    /// \brief config_t. Congiguration type used by the window
    ///
    typedef typename DynamicWindowBase<StateTp, DiffDriveDWConfig, DiffDriveWindowProperties>::config_t config_t;

    ///
    /// \brief window_properties_t
    ///
    typedef typename DynamicWindowBase<StateTp, DiffDriveDWConfig, DiffDriveWindowProperties>::window_properties_t window_properties_t;

    ///
    /// \brief goal_t. The type of the goal
    ///
    typedef GoalTp goal_t;

    ///
    /// \brief control_t The type of the control input calculated by the
    /// the application of the window
    ///
    typedef std::vector<real_t> control_t;

    ///
    /// \brief Trajectory Helper type used to form trajectories
    ///
    typedef std::vector<std::array<real_t, 5>> trajectory_t;

    ///
    /// \brief DiffDriveDW. Constructor
    ///
    DiffDriveDW(state_t& state, const config_t& config, const goal_t& goal,
                const control_t& control, const window_properties_t& wproperties);

    ///
    /// \brief get_control. Access the control value
    ///
    const control_t& get_control()const{return control_;}

    ///
    /// \brief calculate_window. Calculate the window
    ///
    virtual window_properties_t& calculate_window()final override;

    ///
    /// \brief Given the obstacle type calculate the appropriate control
    ///
    template<typename ObstacleTp>
    trajectory_t dwa_control(const ObstacleTp& obstacle);


protected:

    ///
    /// \brief goal_ The goal of the robot
    ///
    goal_t goal_;

    ///
    /// \brief control_. The initial control input
    ///
    control_t control_;

    ///
    /// \brief dynamics_. The dynamics model
    ///
    kernel::dynamics::DiffDriveDynamics dynamics_;

    ///
    /// \brief calculate_control_input_ Calculate the control inputs
    ///
    template<typename ObstacleTp>
    trajectory_t calculate_control_input_(const ObstacleTp& obstacle);

    ///
    /// \brief calc_trajectory_
    ///
    trajectory_t calc_trajectory_();

    ///
    /// \brief calc_to_goal_cost_ Claculate the cost to the goal with the
    /// given trajectory
    ///
    real_t calc_to_goal_cost_(const trajectory_t& trajectory);

    ///
    /// \brief calc_obstacle_cost_ Calculate the cost to obstacle
    /// on the given trajectory. The cost is infinite if
    /// collision occurs and 0 if not.
    ///
    template<typename ObstacleTp>
    real_t calc_obstacle_cost_(const trajectory_t& trajectory, const ObstacleTp& obstacle);

    ///
    /// \brief update_state_from_dynamics_. Update the state from the dynamics
    ///
    void update_state_from_dynamics_();
};

template<typename StateTp, typename GoalTp>
DiffDriveDW<StateTp, GoalTp>::DiffDriveDW(state_t& state, const config_t& config, const goal_t& goal,
                                          const control_t& control, const window_properties_t& wproperties)
    :
    DynamicWindowBase<StateTp, DiffDriveDWConfig, DiffDriveWindowProperties>(state, config, wproperties),
    goal_(goal),
    control_(control)
{}

template<typename StateTp, typename GoalTp>
typename DiffDriveDW<StateTp, GoalTp>::window_properties_t&
DiffDriveDW<StateTp, GoalTp>::calculate_window(){

    this->w_properties_.v_min = std::max(this->state_->operator()("v") - this->config_.max_accel * this->config_.dt, this->config_.min_speed);
    this->w_properties_.v_max = std::min(this->state_->operator()("v") + this->config_.max_accel * this->config_.dt, this->config_.max_speed);
    this->w_properties_.w_min = std::max(this->state_->operator()("w") - this->config_.max_dyawrate * this->config_.dt, -this->config_.max_yaw_rate);
    this->w_properties_.w_max = std::min(this->state_->operator()("w") + this->config_.max_dyawrate * this->config_.dt, this->config_.max_yaw_rate);

    return this->w_properties_;
}

template<typename StateTp, typename GoalTp>
void
DiffDriveDW<StateTp, GoalTp>::update_state_from_dynamics_(){

    (*this->state_)[0] = dynamics_.get_x_position();
    (*this->state_)[1] = dynamics_.get_y_position();
    (*this->state_)[2] = dynamics_.get_orientation();
    (*this->state_)[3] = dynamics_.get_velocity();
    (*this->state_)[4] = dynamics_.get_angular_velocity();

}

template<typename StateTp, typename GoalTp>
typename DiffDriveDW<StateTp, GoalTp>::trajectory_t
DiffDriveDW<StateTp, GoalTp>::calc_trajectory_(){

    trajectory_t traj;
    traj.push_back(this->state_->get_values());

    typedef kernel::dynamics::DiffDriveDynamics::input_t input_t;
    input_t model_input;
    model_input.insert({"v", control_[0]});
    model_input.insert({"w", control_[1]});

    auto time = 0.0;
    while (time <= this->config_.predict_time){

        dynamics_.evaluate(model_input);
        update_state_from_dynamics_();
        traj.push_back(this->state_->get_values());
        time += this->config_.dt;
    }
    return traj;
}

template<typename StateTp, typename GoalTp>
real_t
DiffDriveDW<StateTp, GoalTp>::calc_to_goal_cost_(const trajectory_t& trajectory){

    auto goal_magnitude = std::sqrt(goal_[0]*goal_[0] + goal_[1]*goal_[1]);
    auto traj_magnitude = std::sqrt(std::pow(trajectory.back()[0], 2) + std::pow(trajectory.back()[1], 2));
    auto dot_product = (goal_[0] * trajectory.back()[0]) + (goal_[1] * trajectory.back()[1]);
    auto error = dot_product / (goal_magnitude * traj_magnitude);
    auto error_angle = std::acos(error);
    auto cost = this->config_.to_goal_cost_gain * error_angle;
    return cost;
}

template<typename StateTp, typename GoalTp>
template<typename ObstacleTp>
real_t
DiffDriveDW<StateTp, GoalTp>::calc_obstacle_cost_(const trajectory_t& trajectory, const ObstacleTp& obstacle){

    // calc obstacle cost inf: collistion, 0:free
    auto minr = std::numeric_limits<real_t>::max();

    for (auto ii=0; ii<trajectory.size(); ii+=this->config_.skip_n){
      for (auto i=0; i< obstacle.size(); i++){

        auto ox = obstacle[i][0];
        auto oy = obstacle[i][1];
        auto dx = trajectory[ii][0] - ox;
        auto dy = trajectory[ii][1] - oy;

        float r = std::sqrt(dx*dx + dy*dy);
        if (r <= this->config_.robot_radius){
            return std::numeric_limits<real_t>::max();
        }

        if (minr >= r){
            minr = r;
        }
      }
    }

    return 1.0 / minr;
}

template<typename StateTp, typename GoalTp>
template<typename ObstacleTp>
typename DiffDriveDW<StateTp, GoalTp>::trajectory_t
DiffDriveDW<StateTp, GoalTp>::calculate_control_input_(const ObstacleTp& obstacle){

    float min_cost = 10000.0;
    auto min_u = control_;
    min_u[0] = 0.0;

    auto v_min = this->w_properties_.v_min;
    auto v_max = this->w_properties_.v_max;
    auto w_min = this->w_properties_.w_min;
    auto w_max = this->w_properties_.w_max;

    // the best trajectory
    trajectory_t best_traj;

    // evalucate all trajectory with sampled input in dynamic window
    for (auto v=v_min; v <= v_max; v += this->config_.v_reso){
       for (auto y=w_min; y <= w_max; y += this->config_.yawrate_reso){

            trajectory_t traj = calc_trajectory_();

            // what's the cost reaching goal on this
            // trajectory
            auto to_goal_cost = calc_to_goal_cost_(traj);
            auto speed_cost = this->config_.speed_cost_gain * (this->config_.max_speed - traj.back()[3]);
            auto ob_cost = calc_obstacle_cost_(traj, obstacle);
            auto final_cost = to_goal_cost + speed_cost + ob_cost;

            if (min_cost >= final_cost){
              min_cost = final_cost;
              min_u = control_t{{v, y}};
              best_traj = traj;
            }
        }
    }

    control_ = min_u;
    return best_traj;
}

template<typename StateTp, typename GoalTp>
template<typename ObstacleTp>
typename DiffDriveDW<StateTp, GoalTp>::trajectory_t
DiffDriveDW<StateTp, GoalTp>::dwa_control(const ObstacleTp& obstacle){

    // set the time step for the dynamics
    dynamics_.set_time_step(this->config_.dt);

    // calculate the window
    auto& window = calculate_window();

    // calculate the control values based on the
    // updated window properties
    return calculate_control_input_(obstacle);
}

}
}

#endif // DIFF_DRIVE_DYNAMIC_WINDOW_H
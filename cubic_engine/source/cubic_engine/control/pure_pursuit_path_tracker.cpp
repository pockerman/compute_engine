#include "cubic_engine/control/pure_pursuit_path_tracker.h"
#include "kernel/discretization/utils/line_mesh_utils.h"
#include "kernel/base/kernel_consts.h"
namespace cengine{
namespace control{

PurePursuit2DPathTracker::PurePursuit2DPathTracker()
    :
      kernel::ObserverBase<kernel::numerics::LineMesh<2>*>(),
      boost::noncopyable(),
      lookahead_distance_(0.0),
      goal_radius_(0.0),
      goal_(),
      n_sampling_points_(kernel::KernelConsts::invalid_size_type())
{}

void
PurePursuit2DPathTracker::update(const path_t& resource){
    this->kernel::ObserverBase<kernel::numerics::LineMesh<2>*>::update(resource);
}

void
PurePursuit2DPathTracker::read(path_t& r)const{
  this->kernel::ObserverBase<kernel::numerics::LineMesh<2>*>::read(r);
}


void
PurePursuit2DPathTracker::execute(const kernel::dynamics::SysState<2>& state){

    /// 1. Find the path point closest to the position

    /// get the position coordinates
    real_t x = state.get("X");
    real_t y = state.get("Y");

    /// form the position
    kernel::GeomPoint<2> position({x,y});

    path_t path;
    this->read(path);

    /// find the closest point from the position to the
    /// path
    const kernel::GeomPoint<2> point = kernel::numerics::find_closest_point_to(path, position, n_sampling_points_);

    /// 2. Find the lookahead point

    //3. calculate the curvature and return the steering
    //   request based on that curvature
}


}

}

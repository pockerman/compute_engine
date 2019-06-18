/* 
 * File:   exe.h
 * Author: david
 *
 * Created on October 25, 2015, 10:26 AM
 */

#ifndef EXE_H
#define	EXE_H

#include "cubic_engine/base/cubic_engine_types.h"
#include "cubic_engine/estimation/extended_kalman_filter.h"

namespace exe2{

using cengine::DynVec;
using cengine::DynMat;
using cengine::real_t;
using cengine::uint_t;

const uint_t N_STEPS = 1000;
const  real_t DT = 0.1;


// The class that describes the dynamics of the robot
// The following model is useed
// $$ \dot{x} = vcos(\phi)$$
// $$ \dot{y} = vsin((\phi)$$
// $$ \dot{\phi} = \omega$$



/*struct MotionModel: public cengine::EKF_F_func
{
    virtual void operator()(DynVec<real_t>& x, const DynVec<real_t>& x_prev, const DynMat<real_t>& A,
                            const DynMat<real_t>& B, const DynVec<real_t>& u  )const override final;
};*/

/*struct ObservationModel: public cengine::EKF_H_func
{
   virtual DynVec<real_t> operator()(const DynVec<real_t>& x)const override final;
};*/

// The robot to simulate
class Robot
{
public:

    // constructor
    Robot();

    void simulate(DynVec<real_t>& u, const DynVec<real_t>& y);

    // initialize the robot
    void initialize();

private:

    // the state of the robot:
    // x, y are a 2D x-y position, $\phi$ is orientation, and v is velocity
    DynVec<real_t> state_;

    // matrix that describe the state of the robot
    DynMat<real_t> A_;
    DynMat<real_t> F_;

    // matrix that describes the input
    DynMat<real_t> B_;

    // The covariance matrix
    DynMat<real_t> P_;

    // The covariance matrix
    DynMat<real_t> Q_;

    DynMat<real_t> L_;
    DynMat<real_t> H_;
    DynMat<real_t> M_;
    DynMat<real_t> R_;

    // The externally applied input
    DynVec<real_t>* u_;

    // The state estimator of the robot
    cengine::ExtendedKalmanFilter state_estimator_;

    // The motion model the rovot is using
    typedef cengine::EKF_F_func MotionModel;
    MotionModel f_func_;

    // The observation model the robot is using
    typedef cengine::EKF_H_func ObservationModel;
    ObservationModel h_func_;

    void update_A_mat();
    void update_F_mat();
    void update_B_mat();
    void update_P_mat();
    void update_Q_mat();
    void update_L_mat();
    void update_H_mat();
    void update_M_mat();
    void update_R_mat();



};

}//exe2



#endif	/* EXE_H */


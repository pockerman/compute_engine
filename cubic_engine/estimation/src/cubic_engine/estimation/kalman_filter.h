#include "cubic_engine/base/cubic_engine_types.h"
#include "kernel/utilities/input_resolver.h"
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

#include <map>
#include <string>
#include <stdexcept> //for std::invalid_argument


namespace cengine{
namespace estimation{

template<typename MotionModelTp, typename ObservationModelTp>
struct KalmanFilterConfig
{
    typedef MotionModelTp motion_model_t;
    typedef ObservationModelTp observation_model_t;

    motion_model_t* motion_model;
    const observation_model_t* observation_model;

    DynMat<real_t> B;
    DynMat<real_t> P;
    DynMat<real_t> Q;
    DynMat<real_t> K;
    DynMat<real_t> R;
};

///
/// \brief Linear Kalman Filter implementation.
/// See: An Introduction to the Kalman Filter, TR 95-041
/// by Greg Welch1 and Gary Bishop
///
/// The algorithm is implemented as follows:
///
/// prediction step:
///
/// \f[\hat{\mathbf{x}}_{k} = F_k \mathbf{x}_{k-1} + B_k   \mathbf{u}_k + \mathbf{w}_k\f]
///
/// \f[\hat{P}_{k} = F_{k-1}  P_{k-1}  F_{k-1}^T +  Q_{k-1}\f]
///
/// update step:
///
/// \f[K_k = \hat{P}_{k}  H_{k}^T * (H_k  \hat{P}_{k} * H_{k}^T +  R_k )^{-1}\f]
///
/// \f[\mathbf{x}_k = \hat{\mathbf{x}}_{k} + K_k  (z_k - h( \hat{x}_{k}, 0))\f]
///
/// \f[P_k = (I - K_k  H_k)  \hat{P}_{k}\f]
///
/// where \f$w_k\f$ and \f$v_k\f$  represent process and measurement noise respectively.
/// They are assumed independent and normally distributed:
///
/// \f[p(w) \sim N(0,Q)\f]
///
/// \f[p(v) \sim N(0,R)\f]
///
/// The gain matrix K says how much the predictions should be corrected
/// The following matrices dimensions are assumed:
///
/// state vector:   x n x 1
/// control vector: u l x 1
/// meas. vector:   y m x 1
///
/// \f[ \mathbf{F} n \times n \f]
/// \f[ \mathfb{P} n \times n \f]
/// \f[ \mathbf{B} n \times l \f]
/// \f[ \mathbf{H} m \times n \f]
/// \f[ \mathbf{K} n \times m \f]
/// \f[ \mathbf{Q} n \times n \f]
/// \f[ \mathbf{R} m \times m \f]
///
template<typename MotionModelTp, typename ObservationModelTp>
class KalmanFilter: private boost::noncopyable
{

public:

    typedef MotionModelTp motion_model_t;
    typedef ObservationModelTp observation_model_t;
    typedef typename motion_model_t::input_t motion_model_input_t;
    typedef typename motion_model_t::matrix_t matrix_t;
    typedef typename motion_model_t::state_t state_t;
    typedef typename observation_model_t::input_t observation_model_input_t;
    typedef KalmanFilterConfig<motion_model_t, observation_model_t>  config_t;
    typedef std::map<std::string, boost::any> input_t;

    ///
    ///  \brief Constructor
    ///
    KalmanFilter();

    ///
    /// \brief Constructor
    ///
    KalmanFilter(motion_model_t& motion_model, const observation_model_t& observation_model);

    ///
    /// \brief KalmanFilter Constructor
    ///
    KalmanFilter(const config_t& config);

    ///
    /// \brief Destructor
    ///
    ~KalmanFilter();

    ///
    /// \brief Estimate the state. This function simply
    /// wraps the predict and update steps described by the
    /// functions below
    ///
    void estimate(const input_t& input );

    ///
    /// \brief Predicts the state vector x and the process covariance matrix P using
    /// the given input control u accroding to the following equations
    ///
    /// \f[\hat{x}_{k = F_k* x_{k-1} + B_k *  u_k + w_k\f]
    ///
    /// \f[\hat{P}_{k} = F_{k-1} * P_{k-1} * F_{k-1}^T +  Q_{k-1}\f]
    ///
    /// where \f$x_{k-1}\f$ is the state at the previous step, \f$u_{k}\f$
    /// is the control signal and w_k is the error associated with the
    /// control signal. In input argument passed to the function is meant
    /// to model in a tuple all the arguments needed. F, is the dynamics matrix
    /// and Q is the covariance matrix associate with the control signal
    ///
    /// The control input argument should supply both
    /// \f$u_k\f$ and \f$w_k\f$ vectors
    ///
    ///
    void predict(const input_t& input);

    ///
    /// \brief Updates the gain matrix \f$K\f$, the  state vector \f$x\f$ and covariance matrix P
    /// using the given measurement z_k according to the following equations
    ///
    /// K_k = \hat{P}_{k} * H_{k}^T * (H_k * \hat{P}_{k} * H_{k}^T +  R_k )^{-1}
    /// x_k = \hat{x}_{k} + K_k * (z_k - H * \hat{x}_{k}
    /// P_k = (I - K_k * H_k) * \hat{P}_{k}
    void update(const input_t& input);

    ///
    /// \brief Set the motion model
    ///
    void set_motion_model(motion_model_t& motion_model)
    {motion_model_ptr_ = &motion_model;}

    /// \brief Set the observation model
    void set_observation_model(const observation_model_t& observation_model)
    {observation_model_ptr_ = &observation_model;}

    ///
    /// \brief Set the matrix used by the filter
    ///
    void set_matrix(const std::string& name, const matrix_t& mat);

    ///
    /// \brief Returns true if the matrix with the given name exists
    ///
    bool has_matrix(const std::string& name)const;

    ///
    /// \brief Returns the state
    ///
    const state_t& get_state()const{return motion_model_ptr_->get_state();}

    ///
    /// \brief Returns the state
    ///
    state_t& get_state(){return motion_model_ptr_->get_state();}

    ///
    /// \brief Returns the state property with the given name
    ///
    real_t get(const std::string& name)const{return motion_model_ptr_->get(name);}

    ///
    /// \brief Returns the name-th matrix
    ///
    const DynMat<real_t>& operator[](const std::string& name)const;

    ///
    /// \brief Returns the name-th matrix
    ///
    DynMat<real_t>& operator[](const std::string& name);

protected:

    ///
    /// \brief pointer to the function that computes f
    ///
    motion_model_t* motion_model_ptr_;

    ///
    /// \brief pointer to the function that computes h
    ///
    const observation_model_t* observation_model_ptr_;

    ///
    /// \brief Matrices used by the filter internally
    ///
    std::map<std::string, matrix_t> matrices_;
};

template<typename MotionModelTp, typename ObservationModelTp>
KalmanFilter<MotionModelTp,ObservationModelTp>::KalmanFilter()
    :
    motion_model_ptr_(nullptr),
    observation_model_ptr_(nullptr),
    matrices_()
{}

template<typename MotionModelTp, typename ObservationModelTp>
KalmanFilter<MotionModelTp,
             ObservationModelTp>::KalmanFilter(motion_model_t& motion_model,
                                                               const observation_model_t& observation_model)
    :

    motion_model_ptr_(&motion_model),
    observation_model_ptr_(&observation_model)
{

}

template<typename MotionModelTp, typename ObservationModelTp>
KalmanFilter<MotionModelTp,
             ObservationModelTp>::KalmanFilter(const config_t& config)
    :
    motion_model_ptr_(config.motion_model),
    observation_model_ptr_(config.observation_model)
{
    // set the matrices
    set_matrix("B", config.B);
    set_matrix("P", config.P);
    set_matrix("Q", config.Q);
    set_matrix("K", config.K);
    set_matrix("R", config.R);
}

template<typename MotionModelTp, typename ObservationModelTp>
KalmanFilter<MotionModelTp,ObservationModelTp>::~KalmanFilter()
{}

template<typename MotionModelTp, typename ObservationModelTp>
const DynMat<real_t>&
KalmanFilter<MotionModelTp,ObservationModelTp>::operator[](const std::string& name)const{

    auto itr = matrices_.find(name);

    if(itr == matrices_.end()){
        throw std::invalid_argument("Matrix: "+name+" does not exist");
    }

    return itr->second;
}

template<typename MotionModelTp, typename ObservationModelTp>
DynMat<real_t>&
KalmanFilter<MotionModelTp,ObservationModelTp>::operator[](const std::string& name){

    auto itr = matrices_.find(name);

    if(itr == matrices_.end()){
        throw std::invalid_argument("Matrix: "+name+" does not exist");
    }

    return itr->second;
}

template<typename MotionModelTp, typename ObservationModelTp>
void
KalmanFilter<MotionModelTp,
             ObservationModelTp>::set_matrix(const std::string& name,
                                             const matrix_t& mat){

    if(name != "Q" && 
       name != "K" && 
       name != "R" && 
       name != "P" &&
       name != "B"){
        throw std::logic_error("Invalid matrix name. Name: "+
                               name+
                               " not in [Q, K, R, P, B]");
    }

    matrices_.insert_or_assign(name, mat);
}

template<typename MotionModelTp, typename ObservationModelTp>
bool
KalmanFilter<MotionModelTp,ObservationModelTp>::has_matrix(const std::string& name)const{

    auto itr = matrices_.find(name);
    return itr != matrices_.end();
}

template<typename MotionModelTp, typename ObservationModelTp>
void
KalmanFilter<MotionModelTp,
                     ObservationModelTp>::estimate(const input_t& input ){
    predict(input);
    update(input);
}

template<typename MotionModelTp, typename ObservationModelTp>
void
KalmanFilter<MotionModelTp,
             ObservationModelTp>::predict(const input_t& input ){

    if(!motion_model_ptr_){
        throw std::runtime_error("Motion model has not been set");
    }

    auto u = kernel::utils::InputResolver<input_t, DynVec<real_t>>::resolve("u", input);
    auto w = kernel::utils::InputResolver<input_t, DynVec<real_t>>::resolve("w", input);

    // make a state predicion using the
    // motion model
    auto& state = motion_model_ptr_->get_state();
    auto x = state.as_vector();

    // get the matrix that describes the dynamics
    // of the system
    auto& F = motion_model_ptr_->get_matrix("F");
    auto& B = (*this)["B"];

    x = F*x + B*u + w;
    state.set(x);

    // predict the covariance matrix
    auto& P = (*this)["P"];
    auto& Q = (*this)["Q"];
    auto F_T = trans( F );

    P = (F*P*F_T) + Q;
}

template<typename MotionModelTp, typename ObservationModelTp>
void
KalmanFilter<MotionModelTp,
             ObservationModelTp>::update(const input_t& input){

    if(!motion_model_ptr_){
        throw std::runtime_error("Motion model has not been set");
    }

    if(!observation_model_ptr_){
        throw std::runtime_error("Observation model has not been set");
    }

    auto& state = motion_model_ptr_->get_state();
    auto x = state.as_vector();
    auto& P = (*this)["P"];
    auto& R = (*this)["R"];

    auto& H = observation_model_ptr_->get_matrix("H");
    auto H_T = trans(H);

    try{

      auto S = H*P*H_T + R;
      auto S_inv = inv(S);

      if(has_matrix("K")){
          auto& K = matrices_["K"];
          K = P*H_T*S_inv;
      }
      else{
          auto K = P*H_T*S_inv;
          set_matrix("K", K);
      }

      auto& K = (*this)["K"];
      auto z = kernel::utils::InputResolver<input_t, DynVec<real_t>>::resolve("z", input);
      auto innovation = z - H*x;

      if(K.columns() != innovation.size()){
          throw std::runtime_error("Matrix columns: "+
                                    std::to_string(K.columns())+
                                    " not equal to vector size: "+
                                    std::to_string(innovation.size()));
      }

      x += K*innovation;
      state.set(x);

      IdentityMatrix<real_t> I(state.size());

      // update covariance matrix
      P = (I - K*H)*P;
    }
    catch(...){

        // this is a singular matrix what
        // should we do? Simply use the predicted
        // values and log the fact that there was a singular matrix

        throw;
    }
}

}
}

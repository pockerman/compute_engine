#ifndef UNSCENTED_KALMAN_FILTER_H
#define UNSCENTED_KALMAN_FILTER_H

#include "cubic_engine/base/cubic_engine_types.h"

#include <boost/noncopyable.hpp>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

namespace cengine {
namespace estimation {

template<typename MotionModelTp, typename ObservationModelTp>
class UnscentedKalmanFilter: private boost::noncopyable
{
public:

    typedef MotionModelTp motion_model_t;
    typedef ObservationModelTp observation_model_t;
    typedef typename motion_model_t::input_t motion_model_input_t;
    typedef typename motion_model_t::matrix_t matrix_t;
    typedef typename motion_model_t::state_t state_t;
    typedef typename observation_model_t::input_t observation_model_input_t;

    ///
    /// \brief Constructor
    ///
    UnscentedKalmanFilter();

    ///
    /// \brief Constructor
    ///
    UnscentedKalmanFilter(motion_model_t& motion_model,
                         const observation_model_t& observation_model);

    ///
    /// \brief Destructor
    ///
    ~UnscentedKalmanFilter();

    ///
    /// \brief Initialize the filter. The unscented Kalman filter
    /// uses so called sigma points for the prediction and the updates
    /// we have therefore to initialize the filter
    ///
    void initialize_sigma_points(real_t k);

    ///
    /// \brief Estimate the state. This function simply
    /// wraps the predict and update steps described by the
    /// functions below
    ///
    void estimate(const std::tuple<motion_model_input_t,
                                   observation_model_input_t>& input );

    ///
    /// \brief Predicts the state vector
    /// x and the process covariance matrix P using
    /// the given input control u
    /// accroding to the following equations
    ///
    void predict(const motion_model_input_t& input);

    ///
    /// \brief Updates the gain matrix K,
    /// the  state vector x and covariance matrix P
    /// using the given measurement z_k
    /// according to the following equations
    ///
    void update(const observation_model_input_t& z);

    ///
    /// \brief Set the motion model
    ///
    void set_motion_model(motion_model_t& motion_model)
    {motion_model_ptr_ = &motion_model;}

    ///
    /// \brief Set the observation model
    ///
    void set_observation_model(const observation_model_t& observation_model)
    {observation_model_ptr_ = &observation_model;}

    ///
    /// \brief Set the matrix used by the filter
    ///
    void set_matrix(const std::string& name, const matrix_t& mat);

    ///
    /// \brief Set the k parameter
    ///
    void set_k(real_t k){k_=k;}

    ///
    /// \brief Returns true if the matrix with the given name exists
    ///
    bool has_matrix(const std::string& name)const;

    /// \brief Returns the state
    const state_t& get_state()const
    {return motion_model_ptr_->get_state();}

    ///
    /// \brief Returns the state
    ///
    state_t& get_state(){return motion_model_ptr_->get_state();}

    ///
    /// \brief Returns the state property with the given name
    ///
    real_t get(const std::string& name)const
    {return motion_model_ptr_->get(name);}

    ///
    /// \brief Returns the name-th matrix
    ///
    const DynMat<real_t>& operator[](const std::string& name)const;

    ///
    /// \brief Returns the name-th matrix
    ///
    DynMat<real_t>& operator[](const std::string& name);

    ///
    /// \brief Helper for testing.
    /// Returns the number of sigma points
    ///
    uint_t n_sigma_points()const{return sigma_points_.size();}

    ///
    /// \brief Helper for testing.
    /// Returns the number of weights
    ///
    uint_t n_weights()const{return w_.size();}

    ///
    /// \brief update the sigma points
    ///
    void update_sigma_points();

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

    ///
    /// \brief An array that holds the sigma points
    ///
    std::vector<DynVec<real_t>> sigma_points_;

    ///
    /// \brief An array that holds the weights
    /// corresponding to each sigma point
    ///
    std::vector<real_t> w_;

    ///
    /// \brief The k parameter of the model
    ///
    real_t k_;

    ///
    /// \brief Check the state of the instance before
    /// doing any computations
    ///
    void check_sanity_()const;

};

template<typename MotionModelTp, typename ObservationModelTp>
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::UnscentedKalmanFilter()
    :
    motion_model_ptr_(nullptr),
    observation_model_ptr_(nullptr),
    matrices_(),
    sigma_points_(),
    w_(),
    k_(0.0)
{}

template<typename MotionModelTp, typename ObservationModelTp>
UnscentedKalmanFilter<MotionModelTp,
                     ObservationModelTp>::UnscentedKalmanFilter(motion_model_t& motion_model,
                                                                const observation_model_t& observation_model)
    :
    motion_model_ptr_(&motion_model),
    observation_model_ptr_(&observation_model),
    matrices_(),
    sigma_points_(),
    w_(),
    k_(0.0)
{}

template<typename MotionModelTp,
         typename ObservationModelTp>
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::~UnscentedKalmanFilter()
{}

template<typename MotionModelTp,
         typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
ObservationModelTp>::check_sanity_()const{

    if(!motion_model_ptr_){
      throw std::logic_error("Motion model has not been set");
    }

    if(!observation_model_ptr_){
      throw std::logic_error("Observation model has not been set");
    }

    if(sigma_points_.empty()){
        throw std::logic_error("Sigma points list is empty");
    }

    if(w_.empty()){
        throw std::logic_error("Weights list is empty");
    }

    if(w_.size() != sigma_points_.size()){
        throw std::logic_error("Weights and sigma poinst lists have incompatible sizes");
    }
}

template<typename MotionModelTp, typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
ObservationModelTp>::initialize_sigma_points(real_t k){

    // attempting to initialize without
    // the motion model is an error

    if(!motion_model_ptr_){
      throw std::logic_error("Motion model has not been set");
    }

    set_k(k);

    const uint_t n = motion_model_ptr_->get_state().dimension;

    sigma_points_.resize(2*n + 1, DynVec<real_t>());
    w_.resize(2*n + 1);

    w_[0] = k_/(n + k_);
    auto begin = w_.begin();
    begin++;
    std::for_each(begin, w_.end(),
                  [this, n](real_t& w){w = 0.5*(1/n + this->k_);});

    // update the sigma points
    update_sigma_points();
}

template<typename MotionModelTp,
         typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
ObservationModelTp>::update_sigma_points(){

    auto state_vec = motion_model_ptr_->get_state().as_vector();

    const uint_t n = state_vec.size();

    // the first sigma point is equal to the
    // current state vector
    sigma_points_[0] = state_vec;

    auto& P = (*this)["P"];
    // compute the Cholesky decomposition
    // of the current error covariance matrix
    DynMat<real_t> L;
    llh(P, L);

    // compute the remaining
    // sigma points;
    for(uint_t i=1; i<=n; ++i){
       auto col1 = column(L, i-1);
       sigma_points_[i] = state_vec + std::sqrt(n + k_)*col1;
       sigma_points_[i + n] = state_vec - std::sqrt(n+k_)*col1;

    }
}

template<typename MotionModelTp, typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::set_matrix(const std::string& name,
                                                      const matrix_t& mat){

    if(name != "Q" && name != "K" && name != "R" && name != "P"){
        throw std::logic_error("Invalid matrix name. Name: "+
                               name+
                               " not in [Q, K, R, P]");
    }

    matrices_.insert_or_assign(name, mat);
}

template<typename MotionModelTp, typename ObservationModelTp>
bool
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::has_matrix(const std::string& name)const{

    auto itr = matrices_.find(name);
    return itr != matrices_.end();
}

template<typename MotionModelTp, typename ObservationModelTp>
const DynMat<real_t>&
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::operator[](const std::string& name)const{

    auto itr = matrices_.find(name);

    if(itr == matrices_.end()){
        throw std::invalid_argument("Matrix: "+name+" does not exist");
    }

    return itr->second;
}

template<typename MotionModelTp, typename ObservationModelTp>
DynMat<real_t>&
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::operator[](const std::string& name){

    auto itr = matrices_.find(name);

    if(itr == matrices_.end()){
        throw std::invalid_argument("Matrix: "+name+" does not exist");
    }

    return itr->second;
}

template<typename MotionModelTp, typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
                     ObservationModelTp>::estimate(const std::tuple<motion_model_input_t,
                                                    observation_model_input_t>& input ){

    predict(input.template get<0>());
    update(input.template get<1>());
    update_sigma_points();
}

template<typename MotionModelTp, typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
                      ObservationModelTp>::predict(const motion_model_input_t& u){

    // check if we are sane
    check_sanity_();

    static auto point_propagator = [this, &u](DynVec<real_t>& point){
        point = motion_model_ptr_->evaluate(point, u);
    };

    // propagate the sigma points
    std::for_each(sigma_points_.begin(),
                  sigma_points_.end(),
                  point_propagator);

    DynVec<real_t> total_state(get_state().dimension, 0.0);

    // update the state
    for(uint_t p=0; p<sigma_points_.size(); ++p){
        total_state += w_[p]*sigma_points_[p];
    }

    /// state = sum w_i*p_i
    get_state().set(total_state);

    DynMat<real_t> total_cov(get_state().dimension,
                             get_state().dimension, 0.0);

    // predict the covariance matrix
    // update the state
    for(uint_t p=0; p<sigma_points_.size(); ++p){
        total_cov += w_[p]*(sigma_points_[p] - total_state)*trans(sigma_points_[p] - total_state);
    }

    auto& P = (*this)["P"];
    auto& Q = (*this)["Q"];

    P = total_cov + Q;
}

template<typename MotionModelTp, typename ObservationModelTp>
void
UnscentedKalmanFilter<MotionModelTp,
                     ObservationModelTp>::update(const observation_model_input_t&  z){

    check_sanity_();

    // predict the measurements from
    // the redrawn sigma points
    auto zpred_hat = observation_model_ptr_->evaluate(sigma_points_);

    // estimate mean and covariance
    // of predicted measurements
    DynVec<real_t> zpred(zpred_hat[0].size(), 0.0);

    for(uint_t p = 0; p<sigma_points_.size(); ++p){
        zpred += w_[p]*zpred_hat[p];
    }

    auto& R = (*this)["R"];
    DynMat<real_t> Py = R;

    for(uint_t p=0; p<sigma_points_.size(); ++p){
        Py += w_[p]*(zpred_hat[p] - zpred)*trans(zpred_hat[p] - zpred);
    }

    auto Py_invs = inv(Py);
    auto state_vec = motion_model_ptr_->get_state().as_vector();
    DynMat<real_t> Pxy(motion_model_ptr_->get_state().dimension,
                       zpred.size(), 0.0);

    for(uint_t p=0; p<sigma_points_.size(); ++p){
        Pxy += w_[p]*(sigma_points_[p] - state_vec)*trans(zpred_hat[p] - zpred);
    }

    if(has_matrix("K")){
        auto& K = (*this)["K"];
        K = Pxy*Py_invs;
    }
    else{
        auto K = Pxy*Py_invs;
        set_matrix("K", K);
    }

    auto& K = (*this)["K"];

    // update state
    auto innovation = z - zpred;
    motion_model_ptr_->get_state() += K*innovation;

    auto& P = (*this)["P"];
    P -= K*Py*trans(K);
}

}
}

#endif // UNSCENTED_KALMAN_FILTER_H

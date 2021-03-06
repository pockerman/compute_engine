#ifndef DISCRETE_WORLD_H
#define DISCRETE_WORLD_H

#include "cubic_engine/base/cubic_engine_types.h"
#include "cubic_engine/rl/world.h"
#include "kernel/base/kernel_consts.h"
#include "kernel/utilities/csv_file_writer.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <algorithm>
#include <any>
#include <tuple>
#include <initializer_list>

namespace cengine {
namespace rl {

///
/// \brief The DiscreteWorld class. Class for modeling discrete
/// worlds in reinforcement learning
///
template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
class DiscreteWorld: public World<ActionTp, StateTp, RewardTp>
{
public:

    ///
    /// \brief reward_t The type of the object responsible for
    /// delivering the rewards
    ///
    typedef typename World<ActionTp, StateTp, RewardTp>::reward_t reward_t;

    ///
    /// \brief action_t The type of the action
    ///
    typedef typename World<ActionTp, StateTp, RewardTp>::action_t action_t;

    ///
    /// \brief state_t The type of the state
    ///
    typedef typename World<ActionTp, StateTp, RewardTp>::state_t state_t;

    ///
    /// \brief reward_value_t The returned reward value type. Typically this will
    /// be kernel::real_t
    ///
    typedef typename World<ActionTp, StateTp, RewardTp>::reward_value_t reward_value_t;

    ///
    /// \brief dynamics_t The type responsible for handling the dynamics
    /// of the environment
    ///
    typedef DynamicsTp dynamics_t;

    ///
    /// \brief invalid_action Indicate an invalid action on the world
    /// Useful for uniform initialization when action is unknown
    ///
    static constexpr action_t INVALID_ACTION = ActionTp::INVALID_ACTION;

    ///
    /// \brief n_states. Returns the number of states
    ///
    uint_t n_states()const{return states_.size();}

    ///
    /// \brief n_actions. Max number of actions per state
    ///
    virtual uint_t n_actions()const=0;

    ///
    /// \brief Transition to a new state by
    /// performing the given action. It returns a tuple
    /// with the following information
    /// arg1: An observation of the environment.
    /// arg2: Amount of reward achieved by the previous action.
    /// arg3: Flag indicating whether it’s time to reset the environment again.
    /// Most (but not all) tasks are divided up into well-defined episodes,
    /// and done being True indicates the episode has terminated.
    /// (For example, perhaps the pole tipped too far, or you lost your last life.)
    /// arg4: The type depends on the subclass overriding this function
    /// diagnostic information useful for debugging. It can sometimes be useful for
    /// learning (for example, it might contain the raw probabilities behind the environment’s last state change).
    /// However, official evaluations of your agent are not allowed to use this for learning.
    ///
    virtual std::tuple<state_t, real_t, bool, std::any> step(const action_t&)=0;

    ///
    /// \brief sample_action. Sample an action from
    /// the allowed action space of the world
    ///
    virtual const action_t sample_action()const=0;

    ///
    /// \brief Returns true if the given state is a goal state
    ///
    bool is_goal_state(const state_t& state)const;

    ///
    /// \brief Returns the i-th state
    ///
    state_t& get_state(uint_t s);

    ///
    /// \brief Returns the i-th state
    ///
    const state_t& get_state(uint_t s)const;

    ///
    /// \brief returns the current state of the world
    ///
    const state_t& get_current_state()const{return *current_state_; }

    ///
    /// \brief Restart the world. This means that
    /// the state of the world will be that of start
    ///
    void restart(const state_t& start, const state_t& goal);

    ///
    /// \brief Restart the world with a list of goals
    ///
    void restart(const state_t& start, std::vector<state_t*>&& goal);

    ///
    /// \brief Restart the world but keep the goals
    ///
    void restart(const state_t& state);

    ///
    /// \brief restart. Restart the world and
    /// return the starting state
    ///
    state_t* restart();

    ///
    /// \brief Set up the cells map
    ///
    void set_states(std::vector<state_t>&& states){states_ = states;}

    ///
    /// \brief Set the state of the world
    ///
    void set_state(const state_t& state){current_state_ = &state;}

    ///
    /// \brief Returns true if the world is finished
    ///
    bool is_finished()const{return finished_;}

    ///
    /// \brief Append another goal
    ///
    void append_goal(const state_t& goal){goals_.push_back(&goal);}

    ///
    /// \brief Returns the reward for the given state
    /// and the given actions
    ///
    reward_value_t get_reward(const state_t& state, const action_t& action)const{
        return this->reward_.get_reward(state.get_id(), action);
    }

    ///
    /// \brief get_dynamics. Returns the probability at the given state
    /// to take action
    ///
    real_t get_dynamics(const state_t& state, const action_t& action)const;

    ///
    /// \brief get_dynamics_object. Returns read/write access to the
    /// object handling the dynamics of the environment
    ///
    dynamics_t& get_dynamics_object(){return dynamics_;}

    ///
    /// \brief save_world_as_csv. Save the world in csv format in the
    /// file specified by the given filename
    ///
    virtual void save_world_as_json(const std::string& filename)const;


    uint_t reset(){return 0;}

protected:

    ///
    /// \brief DiscreteWorld. Constructor
    ///
    DiscreteWorld();

    ///
    /// \brief The current state the world is in
    ///
    state_t* current_state_;

    ///
    /// \brief The goal states. More than one goal
    /// states are allowed
    ///
    std::vector<const state_t*> goals_;

    ///
    /// \brief A map that describes the possible state transitions
    /// from one state to another. This is simply a list of
    /// cells the agent can transition to
    ///
    std::vector<state_t> states_;

    ///
    /// \brief Flag indicating if the world
    /// has the current_state_ and goal_ state equal
    ///
    bool finished_;

    ///
    /// \brief dynamics_. Object describing the dynamics of the environment.
    /// that is how likely it is to transition to a state given an action
    ///
    dynamics_t dynamics_;

    ///
    /// \brief start_state_id_. The id of the state the
    /// world starts at
    ///
    uint_t start_state_id_;
};

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::DiscreteWorld()
    :
    World<ActionTp, StateTp, RewardTp>(),
    current_state_(nullptr),
    goals_(),
    states_(),
    finished_(false),
    start_state_id_(kernel::KernelConsts::invalid_size_type())
{}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
bool
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::is_goal_state(const state_t& state)const{

    return std::find_if(goals_.begin(),
                        goals_.end(),
                        [&](const auto* ptr){return ptr->get_id() == state.get_id();}) != goals_.end();
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
const typename DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::state_t&
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::get_state(uint_t id)const{

    if(id >= states_.size()){
        throw std::logic_error("Invalid state id: "+
                               std::to_string(id) +
                               " not in [0, "+
                               std::to_string(states_.size()));
    }

    return states_[id];
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
typename DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::state_t&
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::get_state(uint_t id){

    if(id >= states_.size()){
        throw std::logic_error("Invalid state id: "+
                               std::to_string(id) +
                               " not in [0, "+
                               std::to_string(states_.size()));
    }

    return states_[id];
}


template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
void
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::restart(const state_t& start,
                                                                const state_t& goal){

    current_state_ = &start;
    goals_ = std::vector<const state_t*>();
    goals_.push_back( &goal);
    finished_ = false;
    start_state_id_ = current_state_->get_id();

}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
void
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::restart(const state_t& start,
                             std::vector<state_t*>&& goals){

    current_state_ = &start;
    goals_ = std::vector<const state_t*>();
    goals_ = goals;
    finished_ = false;
    start_state_id_ = current_state_->get_id();
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
void
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::restart(const state_t& start){
    current_state_ = const_cast<state_t*>(&start);
    finished_ = false;
    start_state_id_ = current_state_->get_id();
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
typename DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::state_t*
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::restart(){

    if(states_.empty()){
        throw std::logic_error("Empty world states list. Have you called this->build?");
    }

    if(start_state_id_ >= states_.size()){
        throw std::logic_error("Invalid state index. Index=" + std::to_string(start_state_id_) +
                               " not in [0,"+std::to_string(states_.size()) + ")");
    }

    current_state_ = &states_[start_state_id_];
    finished_ = false;
    return current_state_;
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
real_t
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::get_dynamics(const state_t& state,
                                                                     const action_t& action)const{
    return dynamics_(state, action);
}

template<typename ActionTp, typename StateTp, typename RewardTp, typename DynamicsTp>
void
DiscreteWorld<ActionTp, StateTp, RewardTp, DynamicsTp>::save_world_as_json(const std::string& filename)const{

    using json = nlohmann::json;


    json json_data = {{"n_states", this->states_.size()}};

    for(uint_t c=0; c<this->states_.size(); ++c){
        auto& this_cell = this->states_[c];


        auto& state_transitions = this_cell.get_state_transitions();
        auto itr_begin = state_transitions.begin();
        auto itr_end = state_transitions.end();

        std::vector<uint_t> actions;
        actions.reserve(n_actions());

        std::vector<uint_t> states;
        states.reserve(n_actions());

        for(; itr_begin != itr_end; ++itr_begin){

            actions.push_back(static_cast<uint_t>(itr_begin->first));
            states.push_back(itr_begin->second->get_id());
        }

        json json_state_data;
        json_state_data[std::to_string(this_cell.get_id())]["actions"] = actions;
        json_state_data[std::to_string(this_cell.get_id())]["neighbors"] = states;
        json_data.insert(json_state_data.begin(), json_state_data.end());

    }

    std::ofstream file_stream(filename);
    file_stream << json_data <<std::endl;
    file_stream.close();
  }
}
}

#endif // DISCRETE_WORLD_H

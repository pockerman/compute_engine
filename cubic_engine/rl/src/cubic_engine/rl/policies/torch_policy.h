#ifndef TORCH_POLICY_H
#define TORCH_POLICY_H

#include "kernel/base/config.h"

#ifdef USE_PYTORCH

#include "cubic_engine/base/cubic_engine_types.h"
#include "cubic_engine/rl/actions/action_space.h"

#include "torch/torch.h"

#include <memory>
#include <vector>

namespace cengine {
namespace rl {
namespace policies {



///
/// \brief The TorchPolicyImpl class
///
class TorchPolicyImpl: public torch::nn::Module
{

public:

    ///
    /// \brief TorchPolicyImpl
    /// \param action_space
    /// \param base
    /// \param normalize_observations
    ///
    TorchPolicyImpl(actions::ActionSpace action_space,
                   std::shared_ptr<NNBase> base,
                   bool normalize_observations = false);

    ///
    /// \brief act
    /// \param inputs
    /// \param rnn_hxs
    /// \param masks
    /// \return
    ///
    std::vector<torch::Tensor> act(torch::Tensor inputs,
                                   torch::Tensor rnn_hxs,
                                   torch::Tensor masks) const;

    ///
    /// \brief evaluate_actions
    /// \param inputs
    /// \param rnn_hxs
    /// \param masks
    /// \param actions
    /// \return
    ///
    std::vector<torch::Tensor> evaluate_actions(torch::Tensor inputs,
                                                torch::Tensor rnn_hxs,
                                                torch::Tensor masks,
                                                torch::Tensor actions) const;

    ///
    /// \brief get_probs
    /// \param inputs
    /// \param rnn_hxs
    /// \param masks
    /// \return
    ///
    torch::Tensor get_probs(torch::Tensor inputs,
                            torch::Tensor rnn_hxs,
                            torch::Tensor masks) const;

    ///
    /// \brief get_values
    /// \param inputs
    /// \param rnn_hxs
    /// \param masks
    /// \return
    ///
    torch::Tensor get_values(torch::Tensor inputs,
                             torch::Tensor rnn_hxs,
                             torch::Tensor masks) const;

    ///
    /// \brief update_observation_normalizer
    /// \param observations
    ///
    void update_observation_normalizer(torch::Tensor observations);

    bool is_recurrent() const { return base->is_recurrent(); }
    uint_t get_hidden_size() const{return base->get_hidden_size();}
    bool using_observation_normalizer() const{return !observation_normalizer.is_empty();}

private:

    actions::ActionSpace action_space;
    std::shared_ptr<NNBase> base;
    ObservationNormalizer observation_normalizer;
    std::shared_ptr<OutputLayer> output_layer;

    std::vector<torch::Tensor> forward_gru(torch::Tensor x,
                                               torch::Tensor hxs,
                                               torch::Tensor masks);

};


//using TorchPolicy = Policy<torch_t>;

TORCH_MODULE(TorchPolicy);
}

}

}
#endif
#endif // TORCH_POLICY_H
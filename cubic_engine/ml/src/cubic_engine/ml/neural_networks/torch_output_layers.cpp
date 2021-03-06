#include "cubic_engine/ml/neural_networks/torch_output_layers.h"

#ifdef USE_PYTORCH

#include "cubic_engine/ml/statistics/torch_normal.h"
#include "cubic_engine/ml/statistics/torch_bernoulli.h"
#include "cubic_engine/ml/statistics/torch_categorical.h"

namespace cengine {
namespace ml {
namespace nets {

LinearOutputLayer::LinearOutputLayer(unsigned int num_inputs, unsigned int num_outputs)
    :
      linear(num_inputs, num_outputs)
{
    register_module("linear", linear);
}

BernoulliOutput::BernoulliOutput(unsigned int num_inputs, unsigned int num_outputs)
    :
    LinearOutputLayer(num_inputs, num_outputs)
{
    torch::nn::init::uniform_(linear->named_parameters()["weight"], 0.01, 0); //, 1, 0);

    //torch::init_weights(linear->named_parameters(), 0.01, 0);
}

std::unique_ptr<ml::stats::TorchDistribution>
BernoulliOutput::forward(torch::Tensor x){

    x = linear(x);
    return std::make_unique<ml::stats::TorchBernoulli>(nullptr, &x);

}

CategoricalOutput::CategoricalOutput(uint_t num_inputs, uint_t num_outputs)
    :
     LinearOutputLayer(num_inputs, num_outputs)
{
     torch::nn::init::uniform_(linear->named_parameters()["weight"], 0.01, 0); //, 1, 0);
     //torch::nn::init_weights(linear->named_parameters(), 0.01, 0);
}


std::unique_ptr<ml::stats::TorchDistribution>
CategoricalOutput::forward(torch::Tensor x){

    x = linear(x);
    return std::make_unique<ml::stats::TorchCategorical>(nullptr, &x);

}

NormalOutput::NormalOutput(uint_t num_inputs, uint_t num_outputs)
    :
     LinearOutputLayer(num_inputs, num_outputs),
     scale_log()
{
    scale_log = register_parameter("scale_log", torch::zeros({static_cast<long int>(num_outputs)}));

     torch::nn::init::uniform_(linear->named_parameters()["weight"], 1, 0); //, 1, 0);
     //torch::nn::init_weights(linear_loc->named_parameters(), 1, 0);
}


std::unique_ptr<ml::stats::TorchDistribution>
NormalOutput::forward(torch::Tensor x){

    auto loc = linear(x);
    auto scale = scale_log.exp();
    return std::make_unique<ml::stats::TorchNormalDistribution>(loc, scale);

}

}
}
}
#endif

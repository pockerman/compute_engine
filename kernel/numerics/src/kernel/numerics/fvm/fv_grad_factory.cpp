#include "kernel/base/config.h"

#ifdef USE_FVM

#include "kernel/numerics/fvm/fv_grad_factory.h"
#include "kernel/numerics/fvm/fv_gauss_grad.h"

namespace kernel{
namespace numerics{

template<int dim>
std::shared_ptr<FVGradBase<dim>>
FVGradFactory<dim>::build(FVGradType type){

    std::shared_ptr<FVGradBase<dim>> ptr;
    switch(type)
    {
        case FVGradType::GAUSS:
        {
            ptr.reset(new FVGaussGrad<dim>());
            break;
        }
        default:
        {
            throw std::logic_error("Invalid FV Grad type");
        }
    }

    return ptr;
}

template class FVGradFactory<1>;
template class FVGradFactory<2>;
template class FVGradFactory<3>;

}
}
#endif

#include "kernel/base/config.h"

#ifdef USE_FVM

#include "kernel/numerics/fvm/fv_grad_base.h"

namespace kernel{
namespace numerics{

template<int dim>
FVGradBase<dim>::FVGradBase(FVGradType type)
    :
      type_(type)
{}

template<int dim>
FVGradBase<dim>::~FVGradBase()
{}

template class FVGradBase<1>;
template class FVGradBase<2>;
template class FVGradBase<3>;

}
}

#endif

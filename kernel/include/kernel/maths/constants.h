#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "kernel/base/types.h"
#include "boost/noncopyable.hpp"

namespace kernel
{

class MathConsts: private boost::noncopyable
{
public:

    /// \brief The mathematical constant PI
    constexpr static real_t PI = 3.14159265359;


private:

    MathConsts()
    {}
};

}

#endif // CONSTANTS_H

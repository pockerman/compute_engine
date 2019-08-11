#ifndef TYPES_H
#define TYPES_H

#include "kernel/base/config.h"

#include <blaze/Math.h>
#include <cstddef>
#include <string>


namespace kernel
{
    /// \brief Expose various shortcuts
    typedef std::size_t uint_t;

    /// \brief Configure the double precision type
    #ifdef KERNEL_REAL_TYPE_FLOAT
        typedef float real_t;
    #else
        typedef double real_t;
    #endif

    /// \brief General matrix type
    template<typename T>
    using DynMat = blaze::DynamicMatrix<T>;

    /// \brief General vector type
    template<typename T>
    using DynVec = blaze::DynamicVector<T>;

}

#endif
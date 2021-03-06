#ifndef ITERATOR_HELPERS_H
#define ITERATOR_HELPERS_H

#include "kernel/base/types.h"
#include <iterator>

namespace kernel{

template<typename IteratorTp>
struct iterator_value_accessor
{
    typedef typename std::iterator_traits<IteratorTp>::value_type value_type;
    static value_type get( IteratorTp itr ){return *itr;}
    static value_type get( const value_type& itr){return itr;}
};

template<typename T>
struct iterator_value_accessor<blaze::DenseIterator<T, blaze::AlignmentFlag::aligned>>
{
    typedef typename blaze::DenseIterator<T, blaze::AlignmentFlag::aligned> IteratorTp;
    typedef T value_type;
    static value_type get( const IteratorTp& itr){return itr->value();}
    static value_type get( const T& itr){return itr;}
};

}

#endif // ITERATOR_HELPERS_H

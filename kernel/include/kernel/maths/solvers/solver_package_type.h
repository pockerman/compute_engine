#ifndef SOLVER_PACKAGE_TYPE_H
#define SOLVER_PACKAGE_TYPE_H

#include <string>

namespace kernel{
namespace maths{
namespace solvers{

///
/// \brief The SolverPackageType enum Enumeration of the various
/// solution packages
///
enum class SolverPackageType{TRI_AMESOS2, TRI_AZTECOO, INVALID_TYPE};

///
/// \brief solver_package_type_to_string convert SolverPackageType to std::string
///
std::string solver_package_type_to_string(SolverPackageType type);
}
}
}

#endif // SOLVER_PACKAGE_TYPE_H

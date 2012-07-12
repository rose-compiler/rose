/**
 * \file include/scoplib/Candl.hpp
 * \brief Enable use of Candl to compute dependencies.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _CANDL_HPP_
#define _CANDL_HPP_

#include "common/PolyhedricContainer.hpp"
#include "common/PolyhedricDependency.hpp"
#include "scoplib/ScopLib.hpp"
#include "scoplib/Exception-ScopLib.hpp"

#include "candl/candl.h"

namespace ScopLib {

template <class Function, class Expression, class VariableLBL>
std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> * ComputeDependenciesWithCandl(
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program
);

#include "scoplib/Candl.tpp"

}

#endif /* _CANDL_HPP_ */


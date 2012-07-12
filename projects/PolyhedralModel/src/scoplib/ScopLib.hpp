/**
 * \file include/scoplib/ScopLib.hpp
 * \brief Enable translation from PolyhedricAnnotation format to ScopLib format
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _SCOPLIB_HPP_
#define _SCOPLIB_HPP_

#include "common/PolyhedricContainer.hpp"
#include "scoplib/Exception-ScopLib.hpp"

#include "scoplib/scop.h"

namespace ScopLib {

template <class Function, class Expression, class VariableLBL>
scoplib_scop_p PolyhedricAnnotationToScopLib(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program);

#include "scoplib/ScopLib.tpp"

}

#endif /* _SCOPLIB_HPP_ */


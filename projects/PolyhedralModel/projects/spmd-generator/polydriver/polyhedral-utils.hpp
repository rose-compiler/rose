
#ifndef __POLYHEDRAL_UTILS_HPP__
#define __POLYHEDRAL_UTILS_HPP__

#include "rose/rose-utils.hpp"

// Needed printers
std::ostream & operator << (std::ostream & out, SgStatement & arg);

std::ostream & operator << (std::ostream & out, const SgStatement & arg);

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj);

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj);

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj);

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj);

/*
namespace PolyhedricAnnotation {

template <>
PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & 
    getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(const SgStatement * arg);

template <>
Domain<SgStatement, SgExprStatement, RoseVariable> &
    getDomain<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
Scattering<SgStatement, SgExprStatement, RoseVariable> &
    getScattering<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
DataAccess<SgStatement, SgExprStatement, RoseVariable> &
    getDataAccess<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

}
*/
#endif /* __POLYHEDRAL_UTILS_HPP__ */


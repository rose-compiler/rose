
#ifndef __POLYHEDRAL_KERNEL_HPP__
#  define __POLYHEDRAL_KERNEL_HPP__

#include "rose/rose-utils.hpp"
#include "common/PolyhedricContainer.hpp"
#include "common/PolyhedricDependency.hpp"
#include "common/Schedule.hpp"
#include "common/ScheduleSpace.hpp"

namespace PolyhedricAnnotation {

/***************/
/* Entry Point */
/***************/

bool parse(SgFunctionDeclaration * function_decl, std::string prefix) throw (Exception::ExceptionBase);

/**************/
/* Containers */
/**************/

  /* FunctionPolyhedralProgram */

typedef PolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable> FunctionPolyhedralProgram;

template <>
FunctionPolyhedralProgram & getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgFunctionDeclaration * arg);

template <>
void setPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(SgFunctionDeclaration * arg);

  /* FunctionDomain */

typedef Domain<SgFunctionDeclaration, SgExprStatement, RoseVariable> FunctionDomain;

template <>
FunctionDomain & getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

  /* FunctionScattering */

typedef Scattering<SgFunctionDeclaration, SgExprStatement, RoseVariable> FunctionScattering;

template <>
FunctionScattering & getScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

  /* FunctionDataAccess */

typedef DataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable> FunctionDataAccess;

template <>
FunctionDataAccess & getDataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setDataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

}

/* FunctionDependency */

namespace PolyhedricDependency {

typedef Dependency<SgFunctionDeclaration, SgExprStatement, RoseVariable> FunctionDependency;

}

// Needed printers
std::ostream & operator << (std::ostream & out, SgFunctionDeclaration & arg);

std::ostream & operator << (std::ostream & out, const SgFunctionDeclaration & arg);

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj);

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj);

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj);

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj);

#endif /* __POLYHEDRAL_KERNEL_HPP__ */


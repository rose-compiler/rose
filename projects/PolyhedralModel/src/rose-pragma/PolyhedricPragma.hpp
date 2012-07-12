
#ifndef _POLYHEDRIC_PRAGMA_HPP_
#define _POLYHEDRIC_PRAGMA_HPP_

#include "rose/rose-utils.hpp"
#include "common/PolyhedricContainer.hpp"
#include "common/PolyhedricDependency.hpp"
#include "common/Schedule.hpp"
#include "common/ScheduleSpace.hpp"
#include "rose-pragma/Exception-rose-pragma.hpp"

namespace PolyhedricAnnotation {

/***************/
/* Entry Point */
/***************/

void parse(SgPragmaDeclaration * pragma_decl) throw (Exception::ExceptionBase);

/**************/
/* Containers */
/**************/

  /* PragmaPolyhedralProgram */

typedef PolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaPolyhedralProgram;

template <>
PragmaPolyhedralProgram & getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgPragmaDeclaration * arg);

template <>
void setPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(SgPragmaDeclaration * arg);

  /* PragmaDomain */

typedef Domain<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaDomain;

template <>
PragmaDomain & getDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

  /* PragmaScattering */

typedef Scattering<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaScattering;

template <>
PragmaScattering & getScattering<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setScattering<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

  /* PragmaDataAccess */

typedef DataAccess<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaDataAccess;

template <>
PragmaDataAccess & getDataAccess<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg);

template <>
void setDataAccess<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
);

}

namespace PolyhedricDependency {

/**************/
/* Dependency */
/**************/

typedef Dependency<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaDependency;

}

namespace Scheduling {

/**************/
/* Scheduling */
/**************/

typedef Schedule<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaSchedule;

typedef ScheduleSpace<SgPragmaDeclaration, SgExprStatement, RoseVariable> PragmaScheduleSpace;

}

#ifdef ROSE_USE_CLOOG

namespace CodeGeneration {

/******************/
/* CodeGeneration */
/******************/

SgBasicBlock * generateBasicBlockFor(Scheduling::PragmaSchedule &);

void generateAndReplace(Scheduling::PragmaSchedule &);

}

#endif

// Needed printers

// std::ostream & operator << (std::ostream & out, SgPragmaDeclaration & arg);

std::ostream & operator << (std::ostream & out, const SgPragmaDeclaration & arg);

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj);

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj);

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj);

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj);

#endif /* _POLYHEDRIC_PRAGMA_HPP_ */

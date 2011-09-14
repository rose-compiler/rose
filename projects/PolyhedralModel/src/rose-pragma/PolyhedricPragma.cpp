
#include "rose-pragma/PolyhedricPragma.hpp"
#include "rose/Attribute.hpp"
#include "rose/Parser.hpp"
#ifdef ROSE_USE_CLOOG
#include "rose/CodeGeneration.hpp"
#endif
#include "rose-pragma/PolyhedricPragmaParser.hpp"

namespace PolyhedricAnnotation {

/***************/
/* Entry Point */
/***************/

void parse(SgPragmaDeclaration * pragma_decl) throw (Exception::ExceptionBase) {
  SgScopeStatement * scope = isSgScopeStatement(SageInterface::getNextStatement(pragma_decl));

  if (scope == NULL)
    throw Exception::InvalidPolyhedricPragmaException("The statement following the pragma is not valid.");

  PolyhedricPragmaParser pragma_parser(scope);

  std::string pragma = pragma_decl->get_pragma()->get_pragma();

  if (!pragma_parser.parse(pragma, pragma_parser))
    throw Exception::NotPolyhedricPragmaException(pragma);

  PolyhedricAnnotation::Parse<SgPragmaDeclaration>(pragma_decl, scope);
}

/**************/
/* Containers */
/**************/

  /* PragmaPolyhedralProgram */

template <>
PragmaPolyhedralProgram & getPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgPragmaDeclaration * arg) {
  PragmaPolyhedralProgram * res = dynamic_cast<PolyhedralProgramAttribute<SgPragmaDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::PolyhedralProgram"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::PolyhedralProgram", arg);
  return *res;
}

template <>
void setPolyhedralProgram<SgPragmaDeclaration, SgExprStatement, RoseVariable>(SgPragmaDeclaration * arg) {
  new PolyhedralProgramAttribute<SgPragmaDeclaration>(arg);
}

  /* PragmaDomain */

template <>
PragmaDomain & getDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  PragmaDomain * res = dynamic_cast<DomainAttribute<SgPragmaDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::Domain"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Domain", arg);
  return *res;
}

template <>
void setDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DomainAttribute<SgPragmaDeclaration>(polyhedral_program, expression, nbr_iterators);
}

  /* PragmaScattering */

template <>
PragmaScattering & getScattering<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  PragmaScattering * res = dynamic_cast<ScatteringAttribute<SgPragmaDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::Scattering"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Scattering", arg);
  return *res;
}

template <>
void setScattering<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new ScatteringAttribute<SgPragmaDeclaration>(polyhedral_program, expression, nbr_iterators);
}

  /* PragmaDataAccess */

template <>
PragmaDataAccess & getDataAccess<SgPragmaDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  PragmaDataAccess * res = dynamic_cast<DataAccessAttribute<SgPragmaDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::DataAccess"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::DataAccess", arg);
  return *res;
}

template <>
void setDataAccess<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  PragmaPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DataAccessAttribute<SgPragmaDeclaration>(polyhedral_program, expression, nbr_iterators);
}

// A template specialization for access parsing

template <>
void makeAccessAnnotation<SgPragmaDeclaration, SgExprStatement, RoseVariable>(
  SgExprStatement * expression,
  PolyhedricAnnotation::PragmaDataAccess & data_access
) {
  makeAccessAnnotationSage<SgPragmaDeclaration>(expression, data_access);
}

}

#ifdef ROSE_USE_CLOOG

namespace CodeGeneration {

/******************/
/* CodeGeneration */
/******************/

SgBasicBlock * generateBasicBlockFor(Scheduling::PragmaSchedule & schedule) {
  PolyhedricAnnotation::PragmaPolyhedralProgram & polyhedral_program = schedule.getPolyhedralProgram();
  SgScopeStatement * scope = isSgScopeStatement(polyhedral_program.getBase()->get_parent());

  if (scope == NULL) {
    std::cerr << "The parent of \"" << *(polyhedral_program.getBase()) << "\" is not a SgScopeStatement." << std::endl;
    ROSE_ASSERT(false);
  }

  return generateBasicBlockFromSchedule(polyhedral_program, schedule, scope);
}

void generateAndReplace(Scheduling::PragmaSchedule & schedule) {
  SgBasicBlock * generated_block = generateBasicBlockFor(schedule);
  SgPragmaDeclaration * pragma_decl = schedule.getPolyhedralProgram().getBase();
  SgStatement * next_statement = SageInterface::getNextStatement(pragma_decl);

  SageInterface::removeStatement(next_statement);
  SageInterface::replaceStatement(pragma_decl, generated_block);
}

}

#endif

// Needed printers
/*
std::ostream & operator << (std::ostream & out, SgPragmaDeclaration & arg) {
  out << "Polyhedric Pragma " << &arg;
  return out;
}
*/

std::ostream & operator << (std::ostream & out, const SgPragmaDeclaration & arg) {
  out << "Polyhedric Pragma " << &arg;
  return out;
}

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(obj.first).getDomainPolyhedron();
}

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getScattering<SgPragmaDeclaration, SgExprStatement, RoseVariable>(obj.first).getScattering();
}

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(obj.first).getNumberOfIterators();
}

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgPragmaDeclaration, SgExprStatement, RoseVariable>(obj.first).getNumberOfGlobals();
}


#include "PolyhedralKernel.hpp"

#include "rose/Attribute.hpp"
#include "rose/Parser.hpp"
#include "rose/Exception-rose.hpp"

namespace PolyhedricAnnotation {

/***************/
/* Entry Point */
/***************/

// TODO remove bool to fit the exception pattern...
bool parse(SgFunctionDeclaration * function_decl, std::string prefix) throw (Exception::ExceptionBase) {
  if (function_decl->isForward()) return false; // TODO exception

  std::string name = function_decl->get_name().getString();

  if (name.compare(0, prefix.length(), prefix) != 0) return false; // TODO exception

  SgFunctionDefinition * func_def = function_decl->get_definition();

  if (func_def == NULL) return false; // TODO exception

  SgBasicBlock * func_body = func_def->get_body(); // TODO

  if (func_body == NULL) return false; // TODO exception
//    throw Exception::InvalidPolyhedricPragmaException("The statement following the pragma is not valid.");

  PolyhedricAnnotation::Parse<SgFunctionDeclaration>(function_decl, func_body);

  return true;
}

/**************/
/* Containers */
/**************/

  /* FunctionPolyhedralProgram */

template <>
FunctionPolyhedralProgram & getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgFunctionDeclaration * arg) {
  FunctionPolyhedralProgram * res = dynamic_cast<PolyhedralProgramAttribute<SgFunctionDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::PolyhedralProgram"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::PolyhedralProgram", arg);
  return *res;
}

template <>
void setPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(SgFunctionDeclaration * arg) {
  new PolyhedralProgramAttribute<SgFunctionDeclaration>(arg);
}

  /* FunctionDomain */

template <>
FunctionDomain & getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  FunctionDomain * res = dynamic_cast<DomainAttribute<SgFunctionDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::Domain"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Domain", arg);
  }
  return *res;
}

template <>
void setDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DomainAttribute<SgFunctionDeclaration>(polyhedral_program, expression, nbr_iterators);
}

  /* FunctionScattering */

template <>
FunctionScattering & getScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  FunctionScattering * res = dynamic_cast<ScatteringAttribute<SgFunctionDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::Scattering"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Scattering", arg);
  }
  return *res;
}

template <>
void setScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new ScatteringAttribute<SgFunctionDeclaration>(polyhedral_program, expression, nbr_iterators);
}

  /* FunctionDataAccess */

template <>
FunctionDataAccess & getDataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  FunctionDataAccess * res = dynamic_cast<DataAccessAttribute<SgFunctionDeclaration> *>(arg->getAttribute("PolyhedricAnnotation::DataAccess"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::DataAccess", arg);
  }
  return *res;
}

template <>
void setDataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  FunctionPolyhedralProgram & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DataAccessAttribute<SgFunctionDeclaration>(polyhedral_program, expression, nbr_iterators);
}

template <>
void makeAccessAnnotation<SgFunctionDeclaration, SgExprStatement, RoseVariable>(
  SgExprStatement * expression,
  PolyhedricAnnotation::FunctionDataAccess & data_access
) {
  makeAccessAnnotationSage<SgFunctionDeclaration>(expression, data_access);
}

}

// Needed printers
std::ostream & operator << (std::ostream & out, SgFunctionDeclaration & arg) {
  out << "Polyhedric FUnction " << &arg;
  return out;
}

std::ostream & operator << (std::ostream & out, const SgFunctionDeclaration & arg) {
  out << "Polyhedric Function " << &arg;
  return out;
}

// For generic QUAST usage.

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(obj.first).getDomainPolyhedron();
}

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getScattering<SgFunctionDeclaration, SgExprStatement, RoseVariable>(obj.first).getScattering();
}

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(obj.first).getNumberOfIterators();
}

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(obj.first).getNumberOfGlobals();
}


#include "polydriver/polyhedral-utils.hpp"

#include "rose/Attribute.hpp"
#include "rose/Parser.hpp"
#include "rose/Exception-rose.hpp"

namespace PolyhedricAnnotation {

/**************/
/* Containers */
/**************/

  /* PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> */

template <>
PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(const SgStatement * arg) {
  PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> * res = dynamic_cast<PolyhedralProgramAttribute<SgStatement> *>(arg->getAttribute("PolyhedricAnnotation::PolyhedralProgram"));
  if (!res)
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::PolyhedralProgram", arg);
  return *res;
}

template <>
void setPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(SgStatement * arg) {
  new PolyhedralProgramAttribute<SgStatement>(arg);
}

  /* Domain<SgStatement, SgExprStatement, RoseVariable> */

template <>
Domain<SgStatement, SgExprStatement, RoseVariable> & getDomain<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  Domain<SgStatement, SgExprStatement, RoseVariable> * res = dynamic_cast<DomainAttribute<SgStatement> *>(arg->getAttribute("PolyhedricAnnotation::Domain"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Domain", arg);
  }
  return *res;
}

template <>
void setDomain<SgStatement, SgExprStatement, RoseVariable>(
  PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DomainAttribute<SgStatement>(polyhedral_program, expression, nbr_iterators);
}

  /* Scattering<SgStatement, SgExprStatement, RoseVariable> */

template <>
Scattering<SgStatement, SgExprStatement, RoseVariable> & getScattering<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  Scattering<SgStatement, SgExprStatement, RoseVariable> * res = dynamic_cast<ScatteringAttribute<SgStatement> *>(arg->getAttribute("PolyhedricAnnotation::Scattering"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::Scattering", arg);
  }
  return *res;
}

template <>
void setScattering<SgStatement, SgExprStatement, RoseVariable>(
  PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new ScatteringAttribute<SgStatement>(polyhedral_program, expression, nbr_iterators);
}

  /* DataAccess<SgStatement, SgExprStatement, RoseVariable> */

template <>
DataAccess<SgStatement, SgExprStatement, RoseVariable> & getDataAccess<SgStatement, SgExprStatement, RoseVariable>(const SgExprStatement * arg) {
  DataAccess<SgStatement, SgExprStatement, RoseVariable> * res = dynamic_cast<DataAccessAttribute<SgStatement> *>(arg->getAttribute("PolyhedricAnnotation::DataAccess"));
  if (!res) {
    throw  Exception::RoseAttributeMissing("PolyhedricAnnotation::DataAccess", arg);
  }
  return *res;
}

template <>
void setDataAccess<SgStatement, SgExprStatement, RoseVariable>(
  PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program,
  SgExprStatement * expression,
  size_t nbr_iterators
) {
  new DataAccessAttribute<SgStatement>(polyhedral_program, expression, nbr_iterators);
}

template <>
void makeAccessAnnotation<SgStatement, SgExprStatement, RoseVariable>(
  SgExprStatement * expression,
  PolyhedricAnnotation::DataAccess<SgStatement, SgExprStatement, RoseVariable> & data_access
) {
  makeAccessAnnotationSage<SgStatement>(expression, data_access);
}

}

std::ostream & operator << (std::ostream & out, SgStatement & arg) {
        out << arg.unparseToString();
        return out;
}

std::ostream & operator << (std::ostream & out, const SgStatement & arg) {
        out << arg.unparseToString();
        return out;
}

const Polyhedron & getDomain(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgStatement, SgExprStatement, RoseVariable>(obj.first).getDomainPolyhedron();
}

const std::vector<LinearExpression_ppl> & getScattering(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getScattering<SgStatement, SgExprStatement, RoseVariable>(obj.first).getScattering();
}

size_t getDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgStatement, SgExprStatement, RoseVariable>(obj.first).getNumberOfIterators();
}

size_t getExtraDimension(const std::pair<SgExprStatement *, size_t> obj) {
  return PolyhedricAnnotation::getDomain<SgStatement, SgExprStatement, RoseVariable>(obj.first).getNumberOfGlobals();
}


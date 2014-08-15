
#include "DLX/Tooling/language.hpp"

namespace DLX {

namespace Directives {

template <>
generic_clause_t<Tooling::language_t> * buildClause<Tooling::language_t>(Tooling::language_t::clause_kinds_e kind) {
  switch (kind) {
    case Tooling::language_t::e_tooling_clause_var:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var>();
    case Tooling::language_t::e_tooling_clause_value:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value>();
    case Tooling::language_t::e_tooling_clause_file:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file>();
    case Tooling::language_t::e_tooling_clause_format:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format>();
    case Tooling::language_t::e_tooling_clause_tag:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag>();
    case Tooling::language_t::e_tooling_clause_pack:
      return new clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack>();
    case Tooling::language_t::e_clause_last:
      assert(false);
    default:
     assert(false);
  }
}
 
template <>
bool parseClauseParameters<Tooling::language_t>(
  std::string & directive_str,
  SgLocatedNode * directive_node,
  generic_clause_t<Tooling::language_t> * clause
) {
  switch (clause->kind) {
    case Tooling::language_t::e_tooling_clause_var:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_var>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> *)clause
      );
    case Tooling::language_t::e_tooling_clause_value:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_value>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> *)clause
      );
    case Tooling::language_t::e_tooling_clause_file:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_file>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> *)clause
      );
    case Tooling::language_t::e_tooling_clause_format:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_format>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> *)clause
      );
    case Tooling::language_t::e_tooling_clause_tag:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_tag>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> *)clause
      );
    case Tooling::language_t::e_tooling_clause_pack:
      return Frontend::Frontend<Tooling::language_t>::parseClauseParameters<Tooling::language_t::e_tooling_clause_pack>(
        directive_str, directive_node, (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> *)clause
      );
    case Tooling::language_t::e_clause_last:
      assert(false);
    default:
      assert(false);
  }
}

/*

// We could have this in our header file (+ forcing template instantiation for each enum element).
//    ??? pros/cons ??? : any idea?

template <clause_kinds_e kind>
clause_t<Tooling::language_t, kind> * generic_clause_t<Tooling::language_t>::is<kind>() {
  return (this->kind == kind) ? (clause_t<Tooling::language_t, kind> *)this : NULL;
}
*/

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_var>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_var) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> *)this : NULL;
}

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_value>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_value) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> *)this : NULL;
}

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_file>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_file) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> *)this : NULL;
}

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_format>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_format) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> *)this : NULL;
}

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_tag>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_tag) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> *)this : NULL;
}

template <>
template <>
clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> * generic_clause_t<Tooling::language_t>::is<Tooling::language_t::e_tooling_clause_pack>() {
  return (this->kind == Tooling::language_t::e_tooling_clause_pack) ? (clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> *)this : NULL;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_var> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_var>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_var);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_var> *)this)->parameters;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_value> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_value>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_value);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_value> *)this)->parameters;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_file> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_file>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_file);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_file> *)this)->parameters;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_format> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_format>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_format);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_format> *)this)->parameters;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_tag> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_tag>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_tag);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_tag> *)this)->parameters;
}

template <>
template <>
generic_clause_t<Tooling::language_t>::parameters_t<Tooling::language_t::e_tooling_clause_pack> & generic_clause_t<Tooling::language_t>::getParameters<Tooling::language_t::e_tooling_clause_pack>() {
  assert(this->kind == Tooling::language_t::e_tooling_clause_pack);
  return ((clause_t<Tooling::language_t, Tooling::language_t::e_tooling_clause_pack> *)this)->parameters;
}

}

}


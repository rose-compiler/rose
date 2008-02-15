
#ifndef BDWY_DIAGNOSTIC_CHAIN_H
#define BDWY_DIAGNOSTIC_CHAIN_H

#include "c_breeze.h"
#include "pointers.h"
#include "broadway.h"


/** @brief A data structure used for tracing in error disgnostic.
 *
 */

class Diagnostic {
private:
  typedef map<memoryDef*, memoryuse_set> def2uses;

  /** @brief The property to trace. */
  REF  enumPropertyAnn        *_property;
  /** @brief The set of values in the property to trace. */
  TREE enumvalue_set           _diagnostic_values;
  /** @brief The definitions and uses at a location. */
  TREE map<Location*,def2uses> _def2uses;

public:
  /** @brief Create a new Diagnostic */
  Diagnostic(enumPropertyAnn *property) : _property(property) {}

  /** @brief Check if a value is traced by this Diagnostic */
  inline bool is_diagnostic_value(enumValueAnn *v) {
    return v && _diagnostic_values.test(v->id());
  }

  /** @brief Add a value to be traced by this Diagnostic */
  inline void add_diagnostic_value(enumValueAnn *v) {
    if(v && ! is_diagnostic_value(v)) _diagnostic_values.insert(v);
  }

  /** @brief Check if any value in a given set is traced by this Diagnostic. */
  inline bool contain_diagnostic_value(enumvalue_set values) const {
    return (_diagnostic_values & values).any();
  }

  /** @brief Remember a new definition/use pair for a location. */
  inline void add_defuse(Location *loc, memoryDef *lhs, memoryUse *rhs) {
    if(loc && lhs && rhs) _def2uses[loc][lhs].insert(rhs);
  }

  /** @brief Erase all definitions and uses for a location. */
  inline void reset(Location *loc) { _def2uses.erase(loc); }
  inline void reset() { _def2uses.clear();  }

  /** @brief Performs tracing for the error diagnostic on the pointer value at
   *  a location. */
  void trace(ostream & out, Location *loc, pointerValue &pv);

private:
  /** Tracing is performed via pairs of Locations. For convenience, define type
   *  for such pairs. */
  typedef pair<Location*,memoryDef*>  Location_pair;

  /** The result of trace on each pair is an ordered list of Location.
   *  Define a data structure to hold such result. It also holds pointers to
   *  successor Segment's, so that each chain of Segment's constitute a path
   *  which is a desired trace result. */
  class                              Segment; // holds an ordered list of Location

  /** For convenience, define ordered list of Segment's. */
  typedef list<Segment*>             Segments;

  /** Stores all segments computed. */
  Segments  segments;

  /** Each path (trace) starts with a Segment. Store the start of each path here. */
  Segments  starts;

  /** Stores the Segment computed for each Location pair. */
  map<Location_pair,Segment*>  segment_path;

  /** @brief Helper functions for the other trace function. */
  Segment *trace(Location *from, memoryDef *def);

  /** @brief Helper functions for the other trace function. */
  Segment *trace(Location *from, Location *to);

  int detect_cycle(Segment *cur, Segments & visited, bool specific=false);

  void find_shortest(Segment *cur, Segments path, Segments &shortest);
};

#endif

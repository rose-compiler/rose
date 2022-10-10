#ifndef LATTICE_H
#define LATTICE_H

#include "sage3basic.h"
#include <iostream>
#include <string>
#include "VariableIdMapping.h"

namespace CodeThorn {

/// Defines a lattice and its operations for the dataflow analysis
class Lattice {
 public:
  /// tests if @ref other over-approximates this lattice
  virtual bool approximatedBy(Lattice& other) const = 0;
  
  /// merges @ref other into this 
  virtual void combine(Lattice& other) = 0;
  
  /// tests if this lattice represents bot
  virtual bool isBot() const = 0;
  
  /// prints a representation of this lattice on stream @ref os
  /// \param os  the output stream
  /// \param vim a variable ID mapping that gives names to Variable-ids
  virtual void toStream(std::ostream& os, VariableIdMapping* vim);
  
  /// returns a string representing this lattice
  /// \param vim a variable ID mapping that gives names to Variable-ids
  virtual std::string toString(VariableIdMapping* vim=0);

  // schroder3 (2016-08-05): Asymmetric variant of approximatedBy. Default implementation
  //  calls the "normal" approximatedBy. The asymmetric variant is for example used to check
  //  whether the result of a transfer function of a backward edge is approximated by the
  //  the previous state.
  //~ virtual bool approximatedByAsymmetric(Lattice& other) const;
  
  // schroder3 (2016-08-05): Asymmetric variant of combine. Default implementation
  //  calls the "normal" combine. The asymmetric variant is for example used if the
  //  result of a transfer function of a backward edge is combined with the previous
  //  state.
  //~ virtual void combineAsymmetric(Lattice& other);
  
  virtual ~Lattice();
};

}

#endif
   

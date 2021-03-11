#ifndef BOOL_LATTICE_H
#define BOOL_LATTICE_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <climits>
#include <string>
#include <sstream>

using std::string;
using std::istream;
using std::ostream;

namespace CodeThorn {
/*!
  \brief Several abstract types implementing abstract semantics.
  \details These types are used by the analyzer for computing abstract values according the semantics of the respective domain.
  \date 2012
  \author Markus Schordan
 */

  /*!
    \brief Serves as top value in analysis.
    \date 2012
    \author Markus Schordan
  */
  class Top {
  };

/*!
  \brief Serves as bot value in analysis.
  \date 2012
  \author Markus Schordan
 */
  class Bot {
  };

/*!
  \brief Implements semantic functions of a boolean lattice.
  \date 2012
  \author Markus Schordan, Adrian Prantl
  \details All lattice domain operators are implemented as overloaded functions.
 */
class BoolLattice {
 public:
  enum ValueType { FALSE, TRUE, BOT, TOP};
  BoolLattice();
  BoolLattice(bool val); // type conversion
  BoolLattice(Top e); // type conversion
  BoolLattice(Bot e); // type conversion
  BoolLattice(int x); // type conversion
  bool isTop() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isBot() const;
  BoolLattice operator!();
  BoolLattice operator||(BoolLattice other);
  BoolLattice operator&&(BoolLattice other);
  /// least upper bound
  BoolLattice lub(BoolLattice other);
  /// greatest lower bound
  BoolLattice glb(BoolLattice other);
  /// operator= : C++ default used
  bool operator==(BoolLattice other) const;
  bool operator!=(BoolLattice other) const;
  /// for sorting only!
  bool operator<(BoolLattice other) const;
  string toString() const;
  inline ValueType val() const { return value; }
  friend ostream& operator<<(ostream& os, const BoolLattice& value);
 private:
  ValueType value;
};

 ostream& operator<<(ostream& os, const BoolLattice& value);

} // end of namespace CodeThorn


#endif

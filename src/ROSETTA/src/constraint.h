#ifndef __CONSTRAINT_H__
#define __CONSTRAINT_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ROSETTA_macros.h"
#include <string>

class Constraint
{
  // An Algebra for the specification of containts for the specialization of grammars
  // This class specifies contraints as part of an algebraic system of contraints for 
  // the definition of grammars from previously defined grammars.  Terminals contain a 
  // list of contraints which are use to qualify a terminal in a lower level grammar.

           // What types of constraints are there (which we care about):
           //      value constraints (the value of a scalar would include the matching of a string)
           //           equality
           //           inequality
           //           less than
           //           greater than
           //      type constraints (e.g. all functions with specific return_type)
           //
           //


           // Is every data member used to define ternimals and nonterminals within a grammar a type of contraint?
           // so we could have: 
           //      a return_type constraint for a function type
           //      a name constraint for a classType (since it has a name)
           //      an expression constraint for the test in an IF_STATEMENT
           //      etc.

// So where a terminal contains data we have an opportunity to apply a constraint
// For example the X_Grammar_WHILE_STATEMENT has two pieces of data
//          X_Grammar_Statement* p_condition;
//          X_Grammar_BasicBlock* p_body;
//
//      Thus we can apply contraints to either or both of these data members
//      So the generation of the terminals implementation should generate two
//      contraint lists (one for each data member).
//           ContraintList cl_condition;
//           ContraintList cl_body;

// Case of a ClassType:
//           X_Grammar_DeclarationStatement* p_declaration;
//      so the we would generate a 
//           ContraintList cl_declaration;

// Hierarchies of Constraints -- further contraints :-).

 public:
  enum constraintTypeEnum
  {
    value,
    type
  };

  // Store the name of the member data variable for which this constraint applies
  std::string name;

  std::string constraintString;

                // Store the string that represents code in the lower level grammar 
                // list<char*> nameList;

  ~Constraint ();
  Constraint ();
  Constraint ( const std::string& name, const std::string& constraintString );
  Constraint ( const Constraint & X );
  Constraint & operator= ( const Constraint & X );

  void addConstraint ( const std::string& constraintString );
  void setName( const std::string& inputName );
  const std::string& getName() const;
  const std::string& getConstraintString() const;

  void consistencyCheck() const;

};


#endif

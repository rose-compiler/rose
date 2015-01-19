
// This defines the design of the finite state machine for the discretization abstraction in 
// the Maple DSL.
// The finite machine is required because we cannot execute the code that we see 
// at compile time, and so must emulate it.  We can do this because the operations
// defined in the stencil specification have clear semantics and the stencil specification
// uses a restricted subset of C++ which does not require runtime evaluation.  
// This is a critical point in the design and use of the stencil specification and one
// leveraged in the interpretation of it at compile time.  More analysis at compile-time
// might permit additional flexability in stencil specificaion in the future (if it
// is required).

// There are specific events defined for this finte state machine:
//    1) the creation of a first-order discretization 
//    2) the operation to add a first-order discretization to an existing operator
//    3) operations on discretization operators
//       a. union
//       b. convolution


class DiscretizationFSM
   {
  // This abstracts the concept of a first-order discrete operator Maple Discretization DSL.
  // We store runtime data to distinquish forward and backward discretizations (D_plus and D_minus).

     public:

       // This enum supports the specification of forward and backward discretizations.
          enum DiscreticationKind_enum 
             {
               e_error,
               e_plus  /* forward discretization */, 
               e_minus /* backward discretization */,
               e_last
             };

          int dimension;
          DiscreticationKind_enum discreticationKind;

       // Need to represent specific events that coorespond to associated member function calls on D_plus and D_minus.
       // D_plus D_plus::operator()(D d)
       // D_minus D_minus::operator()(D d)
          bool builtFromOperatorParens;

       // Note that we might have to save the name of the varable we are modeling so that we can generate the correct code in Maple (which will use variable names).
       // Note that this pointer should not be shared, else the state of the finite state machine could be modified externally to the finite state machine.
       // So when this pointer is intialized to a non-null value we need to make a copy of the DiscretizationFSM.
          DiscretizationFSM* operatorParensArgument;
          SgName argumentName;

       // This is the name of the operator used for Maple code generation.
          std::string discretizationName;

       // This cooresponds to the D_plus and D_minus operator().
          DiscretizationFSM operator()(DiscretizationFSM*);


          DiscretizationFSM(int dim, DiscreticationKind_enum discreticationKind);

       // DiscretizationFSM operator*=(int x);

          void display(const std::string & label);
   };

class OperatorFSM
   {
  // This finite state machine is mostly a container for the 
  // DiscretizationFSM objects.  As a wrapper it holds the 
  // list of DiscretizationFSM.

  // This finite state machine operates via specific events and
  // records the state transitions so that the result is a data
  // struction defining the discretization operator and which we 
  // can interogate to generate code for the discretization at 
  // compile time.  The code we generate is Maple language code
  // and this is fed to the Maple compiler/interpreter and the
  // output is either a data structure (from which we translate 
  // the data structure to out Stencil DSL FSM data structure,
  // or the output could be code (which we either treat as text
  // or we read in to ROSE so that we can build ASTs which we
  // can use as a basis for further transformations and optimizations.

  // This Maple DSL is a bit more complex since we required both
  // FSM and a small IR to support the structure of the AST so
  // that we can reproduce the structure in the code generation 
  // phase (generating maple code).  It is because this is in 
  // places similar to an AST and not a FSM that I expect to 
  // require names.

  // As an AST we could just preform variable substitution to 
  // generate the AST of objects representing discretizations 
  // and operators.  But I think that more complex useage may 
  // better demonstrate why we might need to mix the concepts 
  // of the FSM with that of the AST to hold the structure.

     public:

       // Note that within the Operator class all inputs have already been constructed as Operators
       // except for the OperatorFSM(DiscretizationFSM x) constructor. So we need a lists of
       // both DiscretizationFSM and OperatorFSM objects.  Note also that for the case of 
       // a constructed OperatorFSM from a DiscretizationFSM, we may only need one DiscretizationFSM
       // instead of a list.  I think the discretizationList can only ever be a singleton entry. 
       // Where as the operatorList can have multiple entries.
          std::vector<DiscretizationFSM> discretizationList;

       // It might be the this list is not required since everything is in terms of the list of 
       // DiscretizationFSM which can hold many DiscretizationFSM objects (by value to avoid 
       // side-effects).
          std::vector<OperatorFSM> operatorList;

       // This is the name of the operator used for Maple code generation.
          std::string operatorName;

     public:
       // Default constructor.
          OperatorFSM();

       // This is the simplest stencil (a single point).
          OperatorFSM(DiscretizationFSM x);

       // operator addition (union)
          OperatorFSM operator+(const OperatorFSM & X);

       // Convolution operator
       // StencilFSM operator*(const StencilFSM & X);

       // Assignment operator (operator=).
          OperatorFSM operator=(const OperatorFSM & X);

       // Copy constructor.
          OperatorFSM (const OperatorFSM & X);

          void display(const std::string & label);
   };



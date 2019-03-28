
#ifndef _CSHARP_BUILDER_H
#define _CSHARP_BUILDER_H

struct SgNode;

// AST builder functions
namespace csharp_translator
{
  enum SeqKind
  {
    GLOBALSCOPESEQ = 2,
    CLASSMEMBERSEQ = 3,
    PARAMETERSEQ   = 4,
    METHODBODYSEQ  = 5,
    EXPRLISTSEQ    = 6,
    IFSTMT         = 7,
    FORSTMT        = 8,
    WHILESTMT      = 9,
  };

  /// the simplest function
  void helloFromCxx();

  /// opens a Sequence
  void beginSeq(SeqKind);

  /// closes the last sequence on the operand stack
  ///   and incorporates all findings
  void closeSeq();

  /// creates representation for a C# built-in type
  void predefinedType(const char*);

  /// creates the representation for a name
  void name(const char*);

  /// \brief creates a using directive with unique C# identifier uid.
  /// \param uid a unique key representing the C# declaration
  /// \param levels number of components of the name
  /// \details
  ///   pops of levels names from the name stack, assembles them
  ///   to a single name and creates a new SgUsingDeclaration.
  ///   The new IR node will get uid as its unique ID.
  /// \example
  ///   namestack->Text->System
  ///   usingDirective(x, 2)  => "System.Text"
  /// \note
  ///   currently a hidden namespace with the name is created
  /// \todo
  ///   use namespace declaration uids instead of names
  ///   - can we get these from the roselyn parser?
  void usingDirective(int uid, int levels);

  /// creates the representation for a variable declaration
  /// \param uid a unique key representing the C# declaration
  /// \param number of arguments
  ///        - 2 name+type
  ///        - 3 name+type+initializer
  void varDecl(int uid);

  /// creates the representation for a variable declaration
  /// \param uid a unique key representing the C# declaration
  /// \param number of arguments
  ///        - 2 name+type
  ///        - 3 name+type+initializer
  void paramDecl(int uid);

  /// creates a class declaration
  /// \param uid a unique key representing the C# declaration
  void classDecl(int uid);

  /// creates a method declaration
  /// \param uid a unique key representing the C# declaration
  void methodDecl(int uid);

  /// looks up the method declaration with unique key uid and pushes it
  ///   on the node stack for further processing (e.g., fill in body).
  /// \param uid a unique key representing the C# declaration
  void stageMethodDecl(int uid);

  /// looks up the variable declaration with unique key uid and pushes
  ///   reference of it on the node stack for further processing
  ///   (e.g., fill in body).
  /// \param uid a unique key representing the C# declaration
  void refVarParamDecl(int uid);

  /// looks up the function declaration with unique key uid and pushes
  ///   reference of it on the node stack for further processing
  ///   (e.g., fill in body).
  /// \param uid a unique key representing the C# declaration
  void refFunDecl(int uid);


  /// looks up the variable declaration with unique key uid and sets
  ///   its initializer to an expression popped from the node stack.
  /// \param uid a unique key representing the C# declaration
  void initVarParamDecl(int uid);


  //
  // statement builders

  void returnStmt(int args);

  //
  // expression builders

  /// wraps a popped expression from the nodes stack
  ///   and pushes a new SgAssignInitializer
  void valueInitializer();

  /// pops a type from the type stack and pushes a new literal node for
  ///   lit on the nodes stack.
  /// \param lit a string representation of the literal
  void literal(const char* lit);

  /// pops two binary expressions from the node stack, one type from
  ///   the type stack and pushes back a combined binary expression.
  /// \param rep a string representation the operation
  void binary(const char* rep);

  /// invoked at the end when everything is set and done
  /// \details
  ///   performs some basic sanity checking
  void basicFinalChecks();
}

SgNode* popBuiltAST();

#endif /* _CSHARP_BUILDER_H */

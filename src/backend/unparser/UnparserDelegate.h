#ifndef UNPARSER_DELEGATE_H
#define UNPARSER_DELEGATE_H

// This is the base class for the support or alternative code generation mechanisms (by Qing Yi)
// and is the basis of the copy based unparsing that unparses the code by copying parts of the
// AST not transformed directly from the source file (character by character, to preserve
// absolutely ALL formatting).  Patch files can then be generated from such files, where the
// patches represent only the transformations introduced.
class UnparseFormat;
class UnparseDelegate {
 public:
  virtual ~UnparseDelegate() {};

  // This class need only support the unparsing of statements since all other IR nodes are
  // unparsed by the functions that unparse statements.
 virtual bool unparse_statement(SgStatement* stmt, SgUnparse_Info& info, UnparseFormat& out) = 0;
};

#endif


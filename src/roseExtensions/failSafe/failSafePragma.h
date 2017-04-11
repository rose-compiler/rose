#ifndef __FAIL_SAFE_PRAGMA_H__
#define __FAIL_SAFE_PRAGMA_H__

/**
 * Pragma parsing for FAIL-SAFE project
 * 
 * By Liao, Jan. 27th, 2014
 * 
 */
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
class SgNode;

namespace FailSafe
{
  class Attribute;
  // Entry point for all FAIL-SAFE related handling, including parsing, AST construction and translation.
  void process_fail_safe_directives (SgSourceFile *sageFilePtr);

  //! Parse failsafe directives and generate/attach attributes
  void parse_directives_to_attributes(SgSourceFile* fptr);

  //! Parse a single directive and return the generated attribute 
   Attribute* parse_fail_safe_directive(SgPragmaDeclaration*);

  // FAIL-SAFE construct name list
  // All directive and clause types are listed so they are accessible uniformly
  enum fail_safe_enum
  {
    e_unknown = 0,

    //1. Directives
    e_region, 
    e_status_predicate,
    e_data_predicate,
    e_tolerance,
    e_double_redundancy,
    e_triple_redundancy,
    e_save,
   
   // 2. Clauses
   e_assert, 
   e_specifier, // have to use this clause to preserve the order of clauses being parsed
   e_region_reference, 
   e_error,
   e_violation,
   e_recover,

   // specifier implemented as special clauses, existence indicate value
   e_pre, // default for most pragmas
   e_post,

   // 3. Values of some clauses
   
   // error types
   // TODO: define a real set of error types
   e_type_ET1,
   e_type_ET2,
   
   // violation types
   e_violation_NaN, 
   e_violation_SECDED,
   e_violation_SEGFAULT,
   e_violation_ANY,
   // TODO mantissa ?
   // TODO : bits
   
   e_not_failsafe // not a FAIL-SAFE construct
  }; // end fail_safe_enum


  //! Help convert FailSafe construct to string 
  std::string toString (fail_safe_enum fs_type);

  //! Check if a construct is a directive or a clause
  ROSE_DLL_API bool isDirective (fail_safe_enum input);
  ROSE_DLL_API bool isClause (fail_safe_enum input);

  // follow the example OmpAttribute declared in OmpAttribute.h
  class ROSE_DLL_API Attribute
  {
    private:
      //! The associated SgNode for this attribute, could be SgPragmaDeclaration or other nodes
      SgNode*  mNode;
      //! Associated PreprocessingInfo for Fortran only
      //PreprocessingInfo* pinfo;

      //!Directive information: type 
      enum fail_safe_enum  fs_type;

      //! Clause information 
      // vector is used to preserve the order of clauses in the directive
      // map is used to fast query if a clause exists or not
      // Some clauses are allowed to appear more than once, merge the content into the first occurrence in our implementation
      std::vector<fail_safe_enum> clauses;
      std::map<fail_safe_enum,bool> clause_map; 

      //! Variable list appearing within some directives and clauses
      // We use a pair of (variable_name, SgNode) for each variable
      // TODO Can a variable show up in more than more clauses?
      std::map<fail_safe_enum, std::vector<std::pair<std::string,SgNode* > > > variable_lists;
      // A reverse map from a variable to the clauses the variable appears
      std::map<std::string, std::vector<fail_safe_enum> > var_clauses;

      // Expressions 
      // e.g. assert (exp)
      std::map<fail_safe_enum, std::pair<std::string, SgExpression*> > expressions;

      // Values for some clauses
      fail_safe_enum specifier;  // pre or post
      fail_safe_enum error_type; // error_type for error()
      fail_safe_enum violation_type; // tolerated violation type  for tolerate ()

      SgNode* recovery_func; // reference to the recovery_func, should be SgFunctionRefExp
      SgNode* arg_list; // argument list passed to the recovery function, should be SgExprListExp

      // prepare for named assertion regions so we don't use label
      bool bName;
      std::string name;

      void init(); // initialized internal data

      //! Convert individual directives and clauses associated with this attribute to string
      // Called internally by the external public toFailSafeString() function
      std::string toFailSafeString(fail_safe_enum fs_type);
      //! Convert a variable list to x,y,z ,without parenthesis.
      std::string toFailSafeString(std::vector<std::pair<std::string,SgNode* > >);

      //! Private constructors to enforce the use of a builder function instead
      Attribute()
      {
        mNode = NULL; 
        fs_type = e_unknown;
        init();
      }
      Attribute (fail_safe_enum fstype, SgNode* mynode): mNode(mynode),fs_type (fstype)
    {
      init();  
    } 

    public:
      //!--------------AST connection------------------
      //! Get the associated SgPragmaDeclaration for C/C++, if any
      SgPragmaDeclaration* getPragmaDeclaration();

      //! Get the associated SgNode, can be SgPragmaDeclaration or others( for fortran nodes)
      SgNode* getNode(){return mNode;};
      void setNode(SgNode* n) { mNode= n;};

      //!------------directive type-------
      void setDirectiveType(fail_safe_enum fstype){ assert (FailSafe::isDirective(fstype)); fs_type = fstype;}
      fail_safe_enum getDirectiveType() {return fs_type;}

      //!-----------clauses -------------
      void addClause(fail_safe_enum clause_type);
      //! Check if a directive has a clause of the specified type
      bool hasClause(fail_safe_enum clause_type);

      //! Get all existing clauses
      std::vector<fail_safe_enum> getClauses() { return clauses;};

      //!-------------variable list --------------
      //! Add a variable into a variable list of a construct ,return the symbol of the variable added, if possible   
      SgVariableSymbol* addVariable(fail_safe_enum targetConstruct, const std::string& varString, SgInitializedName* sgvar=NULL);
      //! Check if a variable list is associated with a construct
      bool hasVariableList(fail_safe_enum);
      //! Get the variable list associated with a construct
      std::vector<std::pair<std::string,SgNode* > > getVariableList(fail_safe_enum);

      //! Find the relevant clauses for a variable
      std::vector<enum fail_safe_enum> get_clauses(const std::string& variable);

      //!----------expressions -------------------
      //! Add an expression to a clause
      void addExpression(fail_safe_enum targetConstruct, const std::string& expString, SgExpression*    sgexp=NULL);
      //! Get expression of a clause
      std::pair<std::string, SgExpression*> getExpression(fail_safe_enum targetConstruct);

      //!--------values for some clauses ----------
      void setSpecifierValue (fail_safe_enum value);
      fail_safe_enum getSpecifierValue () {return specifier; };

      void setErrorType (fail_safe_enum value);
      fail_safe_enum getErrorType() { return error_type; };

      void setViolationType(fail_safe_enum value);
      fail_safe_enum getViolationType() {return violation_type;};

      void setRecoveryFunc(SgNode* f) {recovery_func = f;};
      SgNode* getRecoveryFunc () {return recovery_func;} ;

      void setRecoveryArgList (SgNode* l) {arg_list = l;};
      SgNode* getRecoveryArgList () {return arg_list; }

      //! Check if a variable is inside a variable list of a clause/directive.
      bool isInConstruct(const std::string & variable, enum fail_safe_enum);

      void setName (const std::string & name);
      std::string getName() {return name; };
      bool isNamed () {return bName; };

      //! Convert back to a legal pragma string
      std::string toFailSafeString();
      //! Just another name of toFailSafeString();
      std::string toString() {return toFailSafeString(); };

      friend Attribute * buildAttribute (fail_safe_enum fstype, SgNode* node);
  }; // end class Attribute
  
  // A builder function
  Attribute * buildAttribute (fail_safe_enum fstype, SgNode* node);

  class ROSE_DLL_API AttributeList : public AstAttribute
  {
    public:
      std::vector<Attribute*> attriList;
      // Restore to legal failsafe directive strings
      std::string toFailSafeString();
      //! another name to toFailSafeString()
      std::string toString() { return toFailSafeString();} ;
      ~AttributeList();
  };
 

   // Add attribute to a SgNode
   ROSE_DLL_API void addAttribute(Attribute* attribute, SgNode* node);

   //! Get Attribute from a SgNode, return NULL if not found
   ROSE_DLL_API AttributeList* getAttributeList(SgNode* node);

   //! Get the first Attribute from a SgNode, return NULL if not found
   Attribute* getAttribute(SgNode* node);
    
   //! Remove OmpAttribute from a SgNode
   ROSE_DLL_API void removeAttribute(Attribute* attribute, SgNode* node);

   //! Get enum from a pragma attached with Attribute
   fail_safe_enum getConstructEnum(SgPragmaDeclaration* decl);


} //end namespace FailSafe

#endif /* __FAIL_SAFE_PRAGMA_H__ */

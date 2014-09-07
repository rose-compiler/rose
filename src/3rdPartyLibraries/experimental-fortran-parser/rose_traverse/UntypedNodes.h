#ifndef UNTYPED_NODES_H
#define UNTYPED_NODES_H

#include "assert.h"
#include <vector>
#include <string>

#define ROSE_DEPRECATED_FUNCTION
#define Rose_STL_Container std::vector
#define ROSE_DLL_API

#define SgCopyHelp int
#define RTIReturnType int
#define ROSE_VisitorPattern int
#define ROSE_VisitTraversal int
#define ReferenceToPointerHandler int
#define AstRegExAttribute int


class SgUntypedInitializedName;
typedef Rose_STL_Container<SgUntypedInitializedName*> SgUntypedInitializedNamePtrList;
typedef SgUntypedInitializedNamePtrList*              SgUntypedInitializedNamePtrListPtr;

class SgUntypedStatement;
typedef Rose_STL_Container<SgUntypedStatement*> SgUntypedStatementPtrList;
typedef SgUntypedStatementPtrList*              SgUntypedStatementPtrListPtr;

class SgUntypedDeclarationStatement;
typedef Rose_STL_Container<SgUntypedDeclarationStatement*> SgUntypedDeclarationStatementPtrList;
typedef SgUntypedDeclarationStatementPtrList*              SgUntypedDeclarationStatementPtrListPtr;

class SgUntypedFunctionDeclaration;
typedef Rose_STL_Container<SgUntypedFunctionDeclaration*> SgUntypedFunctionDeclarationPtrList;
typedef SgUntypedFunctionDeclarationPtrList*              SgUntypedFunctionDeclarationPtrListPtr;


class Sg_File_Info
   {
   };

class SgNode
  {
     public: 
         SgNode()          {}
         virtual ~SgNode() {}  // need to allow deletion of lists
  };

class SgLocatedNode : public SgNode
   {
     public: 
         Sg_File_Info* get_startOfConstruct() const;
         void set_startOfConstruct(Sg_File_Info* startOfConstruct);

     public: 
         virtual ~SgLocatedNode();

     public: 
         SgLocatedNode(Sg_File_Info* startOfConstruct = NULL); 

     protected:
         Sg_File_Info* p_startOfConstruct;

   };

class SgLocatedNodeSupport: public SgLocatedNode
   {
     public:
         SgLocatedNodeSupport(Sg_File_Info* startOfConstruct ); 
   };

class SgToken : public SgLocatedNodeSupport
   {
     public: 

enum ROSE_Fortran_Keywords
   {
      FORTRAN_ABSTRACT = 0 + 0,
      FORTRAN_ACCESS = 1 + 0,
      FORTRAN_ACTION = 2 + 0,
      FORTRAN_ALLOCATE = 3 + 0,
      FORTRAN_ALLOCATABLE = 4 + 0,
      FORTRAN_ASSIGN = 5 + 0,
      FORTRAN_ASSOCIATE = 6 + 0,
      FORTRAN_ASYNCHRONOUS = 7 + 0,
      FORTRAN_BACKSPACE = 8 + 0,
      FORTRAN_BIND = 9 + 0,
      FORTRAN_BLANK = 10 + 0,
      FORTRAN_BLOCK_DATA = 11 + 0,
      FORTRAN_CALL = 12 + 0,
      FORTRAN_CHARACTER = 13 + 0,
      FORTRAN_CLASS = 14 + 0,
      FORTRAN_CLOSE = 15 + 0,
      FORTRAN_CONTINUE = 16 + 0,
      FORTRAN_CYCLE = 17 + 0,
      FORTRAN_CASE = 18 + 0,
      FORTRAN_COMMON = 19 + 0,
      FORTRAN_COMPLEX = 20 + 0,
      FORTRAN_CONTAINS = 21 + 0,
      FORTRAN_DEALLOCATE = 22 + 0,
      FORTRAN_DATA = 23 + 0,
      FORTRAN_DEFERRED = 24 + 0,
      FORTRAN_DELIM = 25 + 0,
      FORTRAN_DIMENSION = 26 + 0,
      FORTRAN_DO = 27 + 0,
      FORTRAN_DT = 28 + 0,
      FORTRAN_DOUBLEPRECISION = 29 + 0,
      FORTRAN_ENCODING = 30 + 0,
      FORTRAN_END_CASE = 31 + 0,
      FORTRAN_ENDDO = 32 + 0,
      FORTRAN_END_FILE = 33 + 0,
      FORTRAN_END_ENUM = 34 + 0,
      FORTRAN_END_INTERFACE = 35 + 0,
      FORTRAN_END_TYPE = 36 + 0,
      FORTRAN_ERR = 37 + 0,
      FORTRAN_ERRMSG = 38 + 0,
      FORTRAN_EXIT = 39 + 0,
      FORTRAN_ELSE = 40 + 0,
      FORTRAN_ELSEWHERE = 41 + 0,
      FORTRAN_ELSEIF = 42 + 0,
      FORTRAN_ENDIF = 43 + 0,
      FORTRAN_ENTRY = 44 + 0,
      FORTRAN_END = 45 + 0,
      FORTRAN_ENUM = 46 + 0,
      FORTRAN_ENUMERATOR = 47 + 0,
      FORTRAN_EQUIVALENCE = 48 + 0,
      FORTRAN_EXTERNAL = 49 + 0,
      FORTRAN_EXTENDS = 50 + 0,
      FORTRAN_FILE = 51 + 0,
      FORTRAN_FINAL = 52 + 0,
      FORTRAN_FMT = 53 + 0,
      FORTRAN_FORALL = 54 + 0,
      FORTRAN_FORM = 55 + 0,
      FORTRAN_FORMATTED = 56 + 0,
      FORTRAN_FORMAT = 57 + 0,
      FORTRAN_FLUSH = 58 + 0,
      FORTRAN_FUNCTION = 59 + 0,
      FORTRAN_GENERIC = 60 + 0,
      FORTRAN_GOTO = 61 + 0,
      FORTRAN_ID = 62 + 0,
      FORTRAN_IF = 63 + 0,
      FORTRAN_INQUIRE = 64 + 0,
      FORTRAN_INTEGER = 65 + 0,
      FORTRAN_IOMSG = 66 + 0,
      FORTRAN_IOSTAT = 67 + 0,
      FORTRAN_IMPLICIT = 68 + 0,
      FORTRAN_IMPLICIT_NONE = 69 + 0,
      FORTRAN_IMPORT = 70 + 0,
      FORTRAN_INTERFACE = 71 + 0,
      FORTRAN_INTENT = 72 + 0,
      FORTRAN_INTRINSIC = 73 + 0,
      FORTRAN_LEN = 74 + 0,
      FORTRAN_LOGICAL = 75 + 0,
      FORTRAN_KIND = 76 + 0,
      FORTRAN_MODULE_PROC = 77 + 0,
      FORTRAN_MODULE = 78 + 0,
      FORTRAN_NON_INTRINSIC = 79 + 0,
      FORTRAN_NON_OVERRIDABLE = 80 + 0,
      FORTRAN_NULL = 81 + 0,
      FORTRAN_NULLIFY = 82 + 0,
      FORTRAN_NAMELIST = 83 + 0,
      FORTRAN_NML = 84 + 0,
      FORTRAN_NONE = 85 + 0,
      FORTRAN_NOPASS = 86 + 0,
      FORTRAN_ONLY = 87 + 0,
      FORTRAN_OPEN = 88 + 0,
      FORTRAN_OPTIONAL = 89 + 0,
      FORTRAN_PARAMETER = 90 + 0,
      FORTRAN_PASS = 91 + 0,
      FORTRAN_PAUSE = 92 + 0,
      FORTRAN_POINTER = 93 + 0,
      FORTRAN_PRINT = 94 + 0,
      FORTRAN_PRIVATE = 95 + 0,
      FORTRAN_PROCEDURE = 96 + 0,
      FORTRAN_PROGRAM = 97 + 0,
      FORTRAN_PROTECTED = 98 + 0,
      FORTRAN_READ = 99 + 0,
      FORTRAN_REAL = 100 + 0,
      FORTRAN_RETURN = 101 + 0,
      FORTRAN_REWIND = 102 + 0,
      FORTRAN_ROUND = 103 + 0,
      FORTRAN_SELECTCASE = 104 + 0,
      FORTRAN_SELECTTYPE = 105 + 0,
      FORTRAN_SEQUENCE = 106 + 0,
      FORTRAN_SAVE = 107 + 0,
      FORTRAN_SIGN = 108 + 0,
      FORTRAN_SIZE = 109 + 0,
      FORTRAN_SOURCE = 110 + 0,
      FORTRAN_STAT = 111 + 0,
      FORTRAN_STOP = 112 + 0,
      FORTRAN_SUBROUTINE = 113 + 0,
      FORTRAN_TARGET = 114 + 0,
      FORTRAN_THEN = 115 + 0,
      FORTRAN_DERIVED_DECL = 116 + 0,
      FORTRAN_TYPEIS = 117 + 0,
      FORTRAN_UNFORMATTED = 118 + 0,
      FORTRAN_UNIT = 119 + 0,
      FORTRAN_USE = 120 + 0,
      FORTRAN_VALUE = 121 + 0,
      FORTRAN_VOLATILE = 122 + 0,
      FORTRAN_WAIT = 123 + 0,
      FORTRAN_WHERE = 124 + 0,
      FORTRAN_WRITE = 125 + 0,
   // DQ (11/27/2013): Added missing entries.
      FORTRAN_END_PROGRAM = 126 + 0,
      FORTRAN_END_FUNCTION = 127 + 0,
      FORTRAN_END_SUBROUTINE = 128 + 0,
      FORTRAN_END_MODULE = 129 + 0,
      FORTRAN_DOUBLE_COMPLEX = 130 + 0,
      FORTRAN_TYPE = 131 + 0,
      FORTRAN_UNKNOWN = 132 + 0
   };

// Langauge specific token enums for Fortran intrinsic operators
enum ROSE_Fortran_Operators
   {
      FORTRAN_INTRINSIC_PLUS = 0 + 10000,
      FORTRAN_INTRINSIC_MINUS = 1 + 10000,
      FORTRAN_INTRINSIC_POWER = 2 + 10000,
      FORTRAN_INTRINSIC_CONCAT = 3 + 10000,
      FORTRAN_INTRINSIC_TIMES = 4 + 10000,
      FORTRAN_INTRINSIC_DIVIDE = 5 + 10000,
      FORTRAN_INTRINSIC_AND = 6 + 10000,
      FORTRAN_INTRINSIC_OR = 7 + 10000,
      FORTRAN_INTRINSIC_EQV = 8 + 10000,
      FORTRAN_INTRINSIC_NEQV = 9 + 10000,
      FORTRAN_INTRINSIC_EQ = 10 + 10000,
      FORTRAN_INTRINSIC_NE = 11 + 10000,
      FORTRAN_INTRINSIC_GE = 12 + 10000,
      FORTRAN_INTRINSIC_LE = 13 + 10000,
      FORTRAN_INTRINSIC_LT = 14 + 10000,
      FORTRAN_INTRINSIC_GT = 15 + 10000,
      FORTRAN_INTRINSIC_NOT = 16 + 10000,
      FORTRAN_INTRINSIC_OLDEQ = 17 + 10000,
      FORTRAN_INTRINSIC_OLDNE = 18 + 10000,
      FORTRAN_INTRINSIC_OLDGE = 19 + 10000,
      FORTRAN_INTRINSIC_OLDLE = 20 + 10000,
      FORTRAN_INTRINSIC_OLDLT = 21 + 10000,
      FORTRAN_INTRINSIC_OLDGT = 22 + 10000
   }; // enum ROSE_Fortran_Operators
   }; // class SgToken

#endif // COMPILED_WITH_ROSE

#include "Cxx_Grammar.h"
//------------------------------------------------------------------------------------------------------
// from Cxx_Gramman.h
//
//------------------------------------------------------------------------------------------------------

#ifndef   COMPILED_WITH_ROSE

//TODO-CER- remove?
//typedef Rose_STL_Container<SgUntypedDeclaration*> SgUntypedDeclarationPtrList;
typedef Rose_STL_Container<SgUntypedFunctionDeclaration*> SgUntypedFunctionDeclarationPtrList;
// end 2014.3.6

class SgUntypedInitializedName;
typedef Rose_STL_Container<SgUntypedInitializedName*> SgUntypedInitializedNamePtrList;

class SgUntypedStatement;
typedef Rose_STL_Container<SgUntypedStatement*> SgUntypedStatementPtrList;

class SgUntypedDeclarationStatement;
typedef Rose_STL_Container<SgUntypedDeclarationStatement*> SgUntypedDeclarationStatementPtrList;

 
#ifdef OBSOLETE_2014_3_7

// 2014.3.6
class SgUntypedStatementList : public SgUntypedNode
   {
     public: 
         const SgUntypedStatementPtrList&  get_stmt_list() const;
         SgUntypedStatementPtrList& get_stmt_list(); 


     public: 
         virtual ~SgUntypedStatementList();


     public: 
         SgUntypedStatementList(Sg_File_Info* startOfConstruct ); 
         SgUntypedStatementList(); 

    protected:
         SgUntypedStatementPtrList p_stmt_list;

   };


#ifdef OBSOLETE
class SgUntypedStatementList : public SgUntypedNode
   {
     public: 
       SgUntypedStatementList(std::vector<SgUntypedStatement*>* list) : p_stmt_list(list) {}

       virtual ~SgUntypedStatementList()
          {
             // TODO - what about content of list
             if (p_stmt_list) delete p_stmt_list;
          }

       std::vector<SgUntypedStatement*>* get_statement_list() {return p_stmt_list;}
       std::vector<SgUntypedStatement*>* give_statement_list()
          {
             std::vector<SgUntypedStatement*>* stmt_list = p_stmt_list;
             p_stmt_list = NULL;
             return stmt_list;
          }
       //TODO delete void set_statement_list(std::vector<SgUntypedStatement*>* list) {p_stmt_list = list;}

     private: 
       std::vector<SgUntypedStatement*>* p_stmt_list;
   };
#endif


// 2014.3.6
class SgUntypedExpression : public SgUntypedNode
   {
     public: 
         virtual ~SgUntypedExpression();

     public: 
         SgUntypedExpression(Sg_File_Info* startOfConstruct ); 
         SgUntypedExpression() {}

    protected:
   };


class SgUntypedUnaryOperator : public SgUntypedExpression
   {
     public: 
          SgToken::ROSE_Fortran_Operators get_operator_enum() const;
          void set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum);

          std::string get_operator_name() const;
          void set_operator_name(std::string operator_name);

          SgUntypedExpression* get_operand() const;
          void set_operand(SgUntypedExpression* operand);

          virtual ~SgUntypedUnaryOperator() {}

     public: 
          SgUntypedUnaryOperator(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Operators operator_enum = SgToken::FORTRAN_INTRINSIC_PLUS, std::string operator_name = "", SgUntypedExpression* operand = NULL); 
          SgUntypedUnaryOperator(SgToken::ROSE_Fortran_Operators operator_enum, std::string operator_name, SgUntypedExpression* operand); 

    protected:
          SgToken::ROSE_Fortran_Operators p_operator_enum;
          std::string p_operator_name;
          SgUntypedExpression* p_operand;
   };

class SgUntypedBinaryOperator  : public SgUntypedExpression
   {
     public: 
         SgToken::ROSE_Fortran_Operators get_operator_enum() const {return p_operator_enum;}
         void set_operator_enum(SgToken::ROSE_Fortran_Operators operator_enum);

         std::string get_operator_name() const;
         void set_operator_name(std::string operator_name);

         SgUntypedExpression* get_lhs_operand() const {return p_lhs_operand;}
         void set_lhs_operand(SgUntypedExpression* lhs_operand);

         SgUntypedExpression* get_rhs_operand() const {return p_rhs_operand;}
         void set_rhs_operand(SgUntypedExpression* rhs_operand);

         virtual ~SgUntypedBinaryOperator() {}

         SgUntypedBinaryOperator(Sg_File_Info* startOfConstruct , SgToken::ROSE_Fortran_Operators operator_enum = SgToken::FORTRAN_INTRINSIC_PLUS, std::string operator_name = "", SgUntypedExpression* lhs_operand = NULL, SgUntypedExpression* rhs_operand = NULL); 

         SgUntypedBinaryOperator(SgToken::ROSE_Fortran_Operators operator_enum, std::string operator_name, SgUntypedExpression* lhs_operand, SgUntypedExpression* rhs_operand)
            {
               p_operator_enum = operator_enum;
               p_operator_name = operator_name;
               p_lhs_operand = lhs_operand;
               p_rhs_operand = rhs_operand;
            }

    protected:
          SgToken::ROSE_Fortran_Operators p_operator_enum;
          std::string p_operator_name;
          SgUntypedExpression* p_lhs_operand;
          SgUntypedExpression* p_rhs_operand;
   };


// 2014.3.6
class SgUntypedValueExpression : public SgUntypedExpression
   {
     public: 
         std::string get_value_string() const;
         void set_value_string(std::string value_string);

     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type) {p_type = type;}

     public: 
         virtual ~SgUntypedValueExpression();

     public: 
         SgUntypedValueExpression(Sg_File_Info* startOfConstruct , std::string value_string = "", SgUntypedType* type = NULL); 
         SgUntypedValueExpression(std::string value_string, SgUntypedType* type); 

    protected:
         std::string p_value_string;
         SgUntypedType* p_type;

         //TODO-DQ-2014.3.6 consider these member variables (perhaps are all part of type)
         //         std::string kind;                               // should be part of type
         //         SgToken::ROSE_Fortran_Keywords type;            // e.g., FORTRAN_INTEGER
         //         bool has_kind;
         //         bool is_literal;
   };


#ifdef OBSOLETE
class SgUntypedValueExpression  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedValueExpression() {}
         SgUntypedValueExpression(Sg_File_Info* startOfConstruct ); 
         SgUntypedValueExpression();

         SgUntypedValueExpression(const char* val) : value(val), has_kind(false),
                                                     type(SgToken::FORTRAN_UNKNOWN_TYPE),
                                                     is_constant(false), is_literal(false) {}

         std::string get_value() const     {return value;}
         void set_value(std::string& val)  {value = val;}

         std::string get_kind() const      {return kind;}
         void set_kind(std::string& k)     {kind = k;  has_kind=true;}
         bool hasKind()                    {return has_kind;}

         bool isConstant()                 {return is_constant;}
         void set_constant_flag(bool flag) {is_constant = flag;}

         bool isLiteral()                  {return is_literal;}
         void set_literal_flag(bool flag)  {is_literal  = flag;}

         SgToken::ROSE_Fortran_Keywords get_type()             {return type;}
         void set_type(SgToken::ROSE_Fortran_Keywords t)       {type = t;}

    protected:
         std::string value;
         std::string kind;
         SgToken::ROSE_Fortran_Keywords type;            // e.g., FORTRAN_INTEGER
         bool has_kind;
         bool is_constant;
         bool is_literal;
   };
#endif

class SgUntypedRefExpression  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedRefExpression() {}
         SgUntypedRefExpression();

         SgUntypedRefExpression(const char* str) : name(str), has_kind(false),
                                                   is_variable(false), is_constant(false)
            {
            }

         // TODO - need component and array elements

         std::string get_name() const      {return name;}
         void set_name(std::string& str)   {name = str;}

         std::string get_kind() const      {return kind;}
         void set_kind(std::string& k)     {kind = k;  has_kind=true;}
         bool hasKind()                    {return has_kind;}

         bool isConstant()                 {return is_constant;}
         void set_constant_flag(bool flag) {is_constant = flag;}

         bool isVariable()                 {return is_variable;}
         void set_variable_flag(bool flag) {is_variable = flag;}

    protected:
         std::string name;
         std::string kind;
         bool has_kind;
         bool is_variable;
         bool is_constant;
   };

class SgUntypedArrayReferenceExpression  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedArrayReferenceExpression() {}
         SgUntypedArrayReferenceExpression(Sg_File_Info* startOfConstruct ); 
         SgUntypedArrayReferenceExpression();
   };

class SgUntypedOtherExpression  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedOtherExpression() {}
         SgUntypedOtherExpression(Sg_File_Info* startOfConstruct ); 
         SgUntypedOtherExpression();
         SgUntypedOtherExpression(std::string expression_name, enum SgToken::ROSE_Fortran_Keywords expression_enum) {}
   };

class SgUntypedFunctionCallOrArrayReferenceExpression  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedFunctionCallOrArrayReferenceExpression() {}
         SgUntypedFunctionCallOrArrayReferenceExpression(Sg_File_Info* startOfConstruct ); 
         SgUntypedFunctionCallOrArrayReferenceExpression(); 

   };


// 2014.3.6
class SgUntypedStatement : public SgUntypedNode
   {
     public: 
         std::string get_label_string() const;
         void set_label_string(std::string label_string);

     public: 
         SgToken::ROSE_Fortran_Keywords get_statement_enum() const;
         void set_statement_enum(SgToken::ROSE_Fortran_Keywords statement_enum);

     public: 
         virtual ~SgUntypedStatement();

     public: 
         SgUntypedStatement(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

    protected:
         std::string p_label_string;
         SgToken::ROSE_Fortran_Keywords p_statement_enum;

   };


#ifdef OBSOLETE
class SgUntypedStatement : public SgUntypedNode
   {
     public: 
         unsigned int get_numeric_label() const;
         void set_numeric_label(unsigned int numeric_label);
         // CER
         std::string & get_string_label() {return p_string_label;}
         SgToken::ROSE_Fortran_Keywords get_statement_enum() const {return p_statement_enum;};
         void set_statement_enum(SgToken::ROSE_Fortran_Keywords statement_enum);

         // CER
         void set_statement_name(std::string name) {p_statement_name = name;}
         std::string & get_statement_name() {return p_statement_name;}

         virtual ~SgUntypedStatement() {}
         // CER
         SgUntypedStatement() {}

         SgUntypedStatement(Sg_File_Info* startOfConstruct , std::string label_string = "",
                            SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT)
            : SgUntypedNode(startOfConstruct)
            {
               p_string_label = label_string;
               p_statement_enum = statement_enum;
            }

    protected:
         unsigned int p_numeric_label;
         std::string  p_string_label;
         std::string  p_statement_name;
         SgToken::ROSE_Fortran_Keywords p_statement_enum;
   };
#endif



// 2014.3.6
class SgUntypedNamedStatement : public SgUntypedStatement
{
     public: 
         std::string get_statement_name() const;
         void set_statement_name(std::string statement_name);

     public: 
         virtual ~SgUntypedNamedStatement();

     public: 
         SgUntypedNamedStatement(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT, std::string statement_name = ""); 

    protected:
         std::string p_statement_name;
   };


// 2014.3.6
class SgUntypedDeclarationStatement : public SgUntypedStatement
   {
     public: 
         virtual ~SgUntypedDeclarationStatement();

     public: 
         SgUntypedDeclarationStatement(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
         SgUntypedDeclarationStatement(std::string label_string, SgToken::ROSE_Fortran_Keywords statement_enum); 

    protected:

   };



#ifdef OBSOLETE
class SgUntypedDeclarationStatement : public SgUntypedStatement
{
     public: 
         virtual ~SgUntypedDeclarationStatement();

     public: 
         SgUntypedDeclarationStatement(Sg_File_Info* startOfConstruct , std::string label_string = "",
                                       SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT)
            : SgUntypedStatement(start, label_string, statement_enum)
            {
            }

    protected:

   };
#endif


// 2014.3.6
class SgUntypedVariableDeclaration : public SgUntypedDeclarationStatement
{
     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         const SgUntypedInitializedNamePtrList&  get_variables() const;
         SgUntypedInitializedNamePtrList& get_variables(); 

     public: 
         virtual ~SgUntypedVariableDeclaration();

     public: 
         SgUntypedVariableDeclaration(Sg_File_Info* startOfConstruct , std::string label_string = "",
                                      SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT,
                                      SgUntypedType* type = NULL); 

    protected:
         SgUntypedType* p_type;
         SgUntypedInitializedNamePtrList p_variables;

   };


#ifdef OBSOLETE
class SgUntypedVariableDeclaration : public SgUntypedDeclarationStatement
   {
     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         const SgUntypedInitializedNamePtrList&  get_variables() const;
         SgUntypedInitializedNamePtrList& get_variables(); 


     public: 
         virtual ~SgUntypedVariableDeclaration();


     public: 
         SgUntypedVariableDeclaration(Sg_File_Info* startOfConstruct, std::string label_string = "",
                                      SgToken::ROSE_Fortran_Keywords stmt_enum = SgToken::FORTRAN_ABSTRACT,
                                      SgUntypedType* type = NULL)
            : SgUntypedDeclarationStatement(startOfConstruct, label_string, stmt_enum)
            {
               p_type = type;
            }

    protected:
// Start of memberFunctionString
SgUntypedType* p_type;
          
// End of memberFunctionString
// Start of memberFunctionString
SgUntypedInitializedNamePtrList p_variables;
   };
#endif


// 2014.3.6
class SgUntypedFunctionDeclaration : public SgUntypedDeclarationStatement
   {
     public: 

         //TODO-DQ- need this for function name and type
         std::string get_name() {return p_name;}
         void        set_name(std::string name) {p_name = name;}

         SgUntypedFunctionScope* get_scope() const;
         void set_scope(SgUntypedFunctionScope* scope);

         //TODO-DQ-2014.3.6 add end_statement
         SgUntypedStatement* get_end_statement() const {return p_end_statement;}
         void                set_end_statement(SgUntypedStatement* stmt) {p_end_statement = stmt;}

     public: 
         virtual ~SgUntypedFunctionDeclaration();

     public: 
         SgUntypedFunctionDeclaration(Sg_File_Info* startOfConstruct , std::string label_string = "",
                                      SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

    protected:
         SgUntypedFunctionScope* p_scope;
         //TODO-DQ-2014.3.6 add end_statement
         SgUntypedStatement* p_end_statement;
         //TODO-DQ- need this for function name and type
         std::string p_name;
         //TODO-DQ- parameter list
         //......
   };


// 2014.3.6
class SgUntypedScope : public SgUntypedStatement
   {
     public: 
         SgUntypedDeclarationList* get_declaration_list() const;
         void set_declaration_list(SgUntypedDeclarationList* declaration_list);

     public: 
         SgUntypedStatementList* get_statement_list() const;
         void set_statement_list(SgUntypedStatementList* statement_list);

     public: 
         SgUntypedFunctionDeclarationList* get_function_list() const;
         void set_function_list(SgUntypedFunctionDeclarationList* function_list);

     public: 
         virtual ~SgUntypedScope();

     public: 
         SgUntypedScope(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

    protected:
         SgUntypedDeclarationList* p_declaration_list;
         SgUntypedStatementList* p_statement_list;
         SgUntypedFunctionDeclarationList* p_function_list;
   };


// 2014.3.6
class SgUntypedFunctionScope : public SgUntypedScope
   {
     public: 
         virtual ~SgUntypedFunctionScope();

     public: 
         SgUntypedFunctionScope(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

    protected:

   };


// 2014.3.6
class SgUntypedAssignmentStatement  : public SgUntypedStatement
   {
     public: 
         SgUntypedExpression* get_lhs_operand() const;
         void set_lhs_operand(SgUntypedExpression* lhs_operand);

     public: 
         SgUntypedExpression* get_rhs_operand() const;
         void set_rhs_operand(SgUntypedExpression* rhs_operand);

     public: 
         virtual ~SgUntypedAssignmentStatement();

     public: 
         SgUntypedAssignmentStatement(Sg_File_Info* startOfConstruct , std::string label_string = "",
                                      SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT,
                                      SgUntypedExpression* lhs_operand = NULL, SgUntypedExpression* rhs_operand = NULL); 

    protected:
         SgUntypedExpression* p_lhs_operand;
         SgUntypedExpression* p_rhs_operand;
   };


#ifdef OBSOLETE
class SgUntypedAssignmentStatement  : public SgUntypedStatement
   {
     public: 
         virtual ~SgUntypedAssignmentStatement() {}
         SgUntypedAssignmentStatement(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
         SgUntypedAssignmentStatement(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum); 
         // TODO - CER
         SgUntypedAssignmentStatement(std::string str_label, SgUntypedExpression* lhs, SgUntypedExpression* rhs)
            {
               // TODO - what about statement_enum?
               p_string_label = str_label;
               p_lhs = lhs;
               p_rhs = rhs;
            }

         // TODO - CER
         SgUntypedExpression* get_lhs() {return p_lhs;}
         SgUntypedExpression* get_rhs() {return p_rhs;}

     // TODO - CER
     protected:
         SgUntypedExpression* p_lhs;
         SgUntypedExpression* p_rhs;
   };
#endif


// 2014.3.6
class SgUntypedReferenceExpression : public SgUntypedExpression
   {
     public: 
         //TODO-DQ-2014.3.6 I think these can go away
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

         //TODO-DQ-2014.3.6 I think name should replace type
         SgUntypedInitializedName* get_name() const;
         void set_name(SgUntypedInitializedName* initializedName);

     public: 
         virtual ~SgUntypedReferenceExpression();

     public: 
         SgUntypedReferenceExpression(Sg_File_Info* startOfConstruct , SgUntypedType* type = NULL); 
         SgUntypedReferenceExpression(SgUntypedType* type); 

    protected:
         //TODO-DQ-2014.3.6 I think this should go away
         SgUntypedType* p_type;
         //TODO-DQ-2014.3.6 I think this should replace type
         SgUntypedInitializedName* p_name;

   };



class SgUntypedFunctionCallStatement  : public SgUntypedStatement
   {
     public: 
         virtual ~SgUntypedFunctionCallStatement();
         SgUntypedFunctionCallStatement(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
         SgUntypedFunctionCallStatement(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum); 
   };


// 2014.3.6
class SgUntypedInitializedName : public SgUntypedNode
   {
     public: 
         SgUntypedType* get_type() const;
         void set_type(SgUntypedType* type);

     public: 
         std::string get_name() const;
         void set_name(std::string name);

     public: 
         virtual ~SgUntypedInitializedName();

     public: 
         SgUntypedInitializedName(Sg_File_Info* startOfConstruct , SgUntypedType* type = NULL, std::string name = ""); 
         SgUntypedInitializedName(SgUntypedType* type, std::string name); 

    protected:
         SgUntypedType* p_type;
         std::string p_name;
   };


#ifdef OBSOLETE
class SgUntypedInitializedName  : public SgUntypedExpression
   {
     public: 
         virtual ~SgUntypedInitializedName() {}
         SgUntypedInitializedName(Sg_File_Info* startOfConstruct ); 
         SgUntypedInitializedName();
         SgUntypedInitializedName(Sg_File_Info* startOfConstruct , SgUntypedType* type = NULL, std::string name = "")
            : p_name(name), p_type(type)
            {
            }

         const std::string& getName() {return p_name;}

    protected:
         std::string p_name;
         SgUntypedType* p_type;
   };
#endif


// CER
#ifdef NOT_YET
class SgUntypedBlockStatement : public SgUntypedStatement
   {
     public: 
        virtual ~SgUntypedBlockStatement();
        SgUntypedBlockStatement(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
        // CER  SgUntypedBlockStatement(Sg_File_Info* startOfConstruct , std::string numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT, std::vector<SgUntypedStatement*> statement_list );
        SgUntypedBlockStatement(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum); 
        SgUntypedBlockStatement(std::string numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum, std::vector<SgUntypedStatement*> statement_list);

        std::vector<SgUntypedStatement*> get_statement_list();
        void set_statement_list (std::vector<SgUntypedStatement*> s);

     protected:
         std::vector<SgUntypedStatement*> statement_list;

   };
#endif


// 2014.3.6
class SgUntypedType : public SgUntypedNode
   {
     public:
         bool get_is_constant() const;
         void set_is_constant(bool is_constant);

         //TODO-DQ-2014-3-6
         std::string get_name() {return p_name;}
         void set_name(std::string name) {p_name = name;}

         bool has_kind() {return p_has_kind;}
         SgUntypedExpression* get_kind() {return p_kind;}

         void set_literal_flag(bool flag) {p_is_literal = flag;}
         void set_constant_flag(bool flag) {p_is_constant = flag;}
         void set_keyword(SgToken::ROSE_Fortran_Keywords keyword) {p_keyword = keyword;}

     public: 
         virtual ~SgUntypedType() {}

     public: 
         //TODO-DQ-2014-3-6 change constructor
         SgUntypedType(Sg_File_Info* startOfConstruct, SgToken::ROSE_Fortran_Keywords keyword)
            {
               p_keyword = keyword;
            }
         SgUntypedType() {}

    protected:
         bool p_is_constant;
         //TODO-DQ-2014.3.6 need following member variables
         std::string p_name;

         //TODO-DQ-2014.3.6 consider these member variables (perhaps are all part of type)
         //         std::string kind;                               // should be part of type
         //         SgToken::ROSE_Fortran_Keywords type;            // e.g., FORTRAN_INTEGER
         //         bool has_kind;
         //         bool is_literal;
         //         bool is_class;
         //         bool is_intrinsic;
         //         bool is_user_defined;

         SgUntypedExpression* p_kind;
         SgToken::ROSE_Fortran_Keywords p_keyword;            // e.g., FORTRAN_INTEGER
         bool p_has_kind;
         bool p_is_literal;
         bool p_is_class;
         bool p_is_intrinsic;
         bool p_is_user_defined;

   };


// 2014.3.6
class SgUntypedDeclarationList : public SgUntypedNode
   {
     public: 
         const SgUntypedDeclarationStatementPtrList&  get_decl_list() const;
         SgUntypedDeclarationStatementPtrList& get_decl_list(); 

     public: 
         virtual ~SgUntypedDeclarationList();

     public: 
         SgUntypedDeclarationList(Sg_File_Info* startOfConstruct ); 
         SgUntypedDeclarationList(); 

    protected:
         SgUntypedDeclarationStatementPtrList p_decl_list;
   };


#ifdef DELETE_ME
class SgUntypedDeclarationStatement  : public SgUntypedStatement
   {
     public: 
         virtual ~SgUntypedDeclarationStatement() {}
         SgUntypedDeclarationStatement(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
         SgUntypedDeclarationStatement(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum);
         // CER
         SgUntypedDeclarationStatement(Sg_File_Info* start, std::string string_label, SgToken::ROSE_Fortran_Keywords stmt_enum,
                                       SgUntypedType* type)
            : SgUntypedStatement(start, string_label, stmt_enum)
            {
            }
         SgUntypedDeclarationStatement() {}

         void appendExecPart(SgUntypedStatement* stmt) {exec_part_list.push_back(stmt);}

     protected:
         std::vector<SgUntypedStatement*> spec_part_list;
         std::vector<SgUntypedStatement*> exec_part_list;
   };
#endif


// 2014.3.6
class SgUntypedImplicitDeclaration : public SgUntypedDeclarationStatement
   {
     public: 
         virtual ~SgUntypedImplicitDeclaration();

     public: 
         SgUntypedImplicitDeclaration(Sg_File_Info* startOfConstruct , std::string label_string = "", 
                                      SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT)
            : SgUntypedDeclarationStatement(start, label_string, statement_enum)
            {
            }

    protected:

   };


#ifdef DELETE_ME
class SgUntypedVariableDeclaration : public SgUntypedDeclarationStatement
   {
     public: 
         virtual ~SgUntypedVariableDeclaration() {}
         SgUntypedVariableDeclaration(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
         SgUntypedVariableDeclaration(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum); 

         SgUntypedVariableDeclaration(Sg_File_Info* start, std::string label_str, SgToken::ROSE_Fortran_Keywords stmt_enum,
                                      SgUntypedType* type)
            : SgUntypedDeclarationStatement(start, label_str, stmt_enum, type)
            {
            }

         SgToken::ROSE_Fortran_Keywords get_type_name() {return p_type_name;}
         void set_type_name(SgToken::ROSE_Fortran_Keywords type_name) {p_type_name = type_name;}

         const std::vector<SgUntypedInitializedName*> & get_variable_name_list() {return p_variable_name_list;}
         void append_variable_name(SgUntypedInitializedName* name) {p_variable_name_list.push_back(name);}

     private: 
         SgToken::ROSE_Fortran_Keywords p_type_name;
         std::vector<SgUntypedInitializedName*> p_variable_name_list;
   };
#endif


#ifdef OBSOLETE
//TODO-CER- update
class SgUntypedScopeStatement  : public SgUntypedStatement
   {
     public: 

         virtual void append_declaration ( SgUntypedDeclarationStatement* decl );
         virtual void append_statement   ( SgUntypedStatement* stmt );
         virtual void append_isub_part   ( SgUntypedFunctionDefinition* fdef );

     public: 
         virtual ~SgUntypedScopeStatement();

    protected:

         SgUntypedDeclarationPtrList* p_decl_list;
         SgUntypedStatementPtrList*   p_exec_list;
         SgUntypedStatementPtrList*   p_isub_list;

   };
#endif


#ifdef DELETE
class SgUntypedBasicBlock  : public SgUntypedScopeStatement
   {
     public: 
         const SgUntypedStatementPtrList&  get_statements() const;
         SgUntypedStatementPtrList& get_statements(); 

     public: 
         virtual ~SgUntypedBasicBlock();

    protected:
         SgUntypedStatementPtrList p_statements;
   };
#endif

// 2014.3.6
class SgUntypedFunctionDeclarationList : public SgUntypedNode
   {
     public: 
         const SgUntypedFunctionDeclarationPtrList&  get_func_list() const;
         SgUntypedFunctionDeclarationPtrList& get_func_list(); 

     public: 
         virtual ~SgUntypedFunctionDeclarationList();

     public: 
         SgUntypedFunctionDeclarationList(Sg_File_Info* startOfConstruct ); 
         SgUntypedFunctionDeclarationList(); 

    protected:
         SgUntypedFunctionDeclarationPtrList p_func_list;

   };


// 2014.3.6
//TODO-DQ- change parent class?  //class SgUntypedProgramHeaderDeclaration : public SgUntypedDeclarationStatement
class SgUntypedProgramHeaderDeclaration : public SgUntypedFunctionDeclaration
   {
     public: 
         virtual ~SgUntypedProgramHeaderDeclaration();

     public: 
         SgUntypedProgramHeaderDeclaration(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

   };



// 2014.3.6
//TODO-DQ- change parent class?
//class SgUntypedSubroutineDeclaration : public SgUntypedDeclarationStatement
class SgUntypedSubroutineDeclaration : public SgUntypedFunctionDeclaration
   {
     public: 
      //TODO-DQ-2014.3.6 - removed scope
         SgUntypedFunctionScope* get_scope() const;
         void set_scope(SgUntypedFunctionScope* scope);

     public: 
         virtual ~SgUntypedSubroutineDeclaration();

     public: 
         SgUntypedSubroutineDeclaration(Sg_File_Info* startOfConstruct , std::string label_string = "", SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 

    protected:
      //TODO-DQ-2014.3.6 - removed scope
         SgUntypedFunctionScope* p_scope;

   };



#ifdef OBSOLETE
class SgUntypedSubroutineDeclaration : public SgUntypedDeclarationStatement
   {
     public: 
      virtual ~SgUntypedSubroutineDeclaration() {}
      SgUntypedSubroutineDeclaration(Sg_File_Info* startOfConstruct , unsigned int numeric_label = 0, SgToken::ROSE_Fortran_Keywords statement_enum = SgToken::FORTRAN_ABSTRACT); 
      SgUntypedSubroutineDeclaration(unsigned int numeric_label, SgToken::ROSE_Fortran_Keywords statement_enum); 
      // CER
      SgUntypedSubroutineDeclaration(Sg_File_Info* start, std::string str_label, SgToken::ROSE_Fortran_Keywords statement_enum,
                                     SgUntypedType* type)
         : SgUntypedDeclarationStatement(start, str_label, statement_enum, type)
         {
            p_begin_stmt = NULL;
            p_end_stmt = NULL;
            p_spec_part_list = NULL;
            p_exec_part_list = NULL;
            p_subprogram_part_list = NULL;
         }

      // CER
      SgUntypedStatement* get_begin_statement() {return p_begin_stmt;}
      SgUntypedStatement* get_end_statement  () {return p_end_stmt;}

      void set_begin_statement(SgUntypedStatement* stmt) {p_begin_stmt = stmt;}
      void set_end_statement  (SgUntypedStatement* stmt) {p_end_stmt   = stmt;}

      std::vector<SgUntypedStatement*>* get_spec_part_list() {return p_spec_part_list;}
      std::vector<SgUntypedStatement*>* get_exec_part_list() {return p_exec_part_list;}

      std::vector<SgUntypedStatement*>* get_subprogram_part_list() {return p_subprogram_part_list;}

      void set_spec_part_list (std::vector<SgUntypedStatement*>* list) {p_spec_part_list = list;}
      void set_exec_part_list (std::vector<SgUntypedStatement*>* list) {p_exec_part_list = list;}
      // CER - TODO - don't know what type this is (statement list?)
      void set_subprogram_part(std::vector<SgUntypedStatement*>* list) {p_subprogram_part_list = list;}

     protected:
      SgUntypedStatement* p_begin_stmt;
      SgUntypedStatement* p_end_stmt;
      std::vector<SgUntypedStatement*>* p_spec_part_list;
      std::vector<SgUntypedStatement*>* p_exec_part_list;
      std::vector<SgUntypedStatement*>* p_subprogram_part_list;
   };
#endif

#endif // OBSOLETE_2014_3_7

//------------------------------------------------------------------------------------------------------

#endif // UNTYPED_NODES_H

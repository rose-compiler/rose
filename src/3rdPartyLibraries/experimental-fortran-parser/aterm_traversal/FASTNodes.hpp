#ifndef FAST_NODES_H
#define FAST_NODES_H

#include <map>
#include <string>
#include <vector>

namespace FAST {

class PosInfo
{
 public:
   PosInfo() : pStartLine(0), pStartCol(0), pEndLine(0), pEndCol(0)
    {
    }

   PosInfo(int strtLine, int strtCol, int endLine, int endCol)
     {
        pStartLine = strtLine;
        pStartCol  = strtCol;
        pEndLine   = endLine;
        pEndCol    = endCol;
     }

   int  getStartLine()             { return pStartLine; }
   int  getStartCol()              { return pStartCol;  }
   int  getEndLine()               { return pEndLine;   }
   int  getEndCol()                { return pEndCol;    }

   void setStartLine ( int line )  { pStartLine = line; }
   void setStartCol  ( int col  )  { pStartCol  = col;  }
   void setEndLine   ( int line )  { pEndLine   = line; }
   void setEndCol    ( int col  )  { pEndCol    = col;  }

 protected:
   int pStartLine, pStartCol;  // location (line,col) of first character ( 1 based)
   int pEndLine,   pEndCol;    // location (line,col) of last  character (+1 col)
};

class FASTNode
{
 public:

   FASTNode()
     {
     }

   FASTNode(PosInfo position)
     {
        pPosInfo = position;
     }

   virtual ~FASTNode() { }

   PosInfo & getPosInfo()                 {return pPosInfo;}
   void      setPosInfo(PosInfo pos)      {pPosInfo = pos;}

 protected:
   PosInfo pPosInfo;           // location of node
};

class Statement;

class Scope : public FASTNode
{
 public:
   Scope()
     {
     }

   std::vector<Statement*> & get_declaration_list()   {return pDecls;}
   std::vector<Statement*> & get_statement_list()     {return pStmts;}
   std::vector<Statement*> & get_function_list()      {return pFuncs;}

 protected:
   std::vector<Statement*> pDecls;
   std::vector<Statement*> pStmts;
   std::vector<Statement*> pFuncs;

};


/* Type Specifications
 */
class TypeSpec : public FASTNode
{
 public:
   enum TypeEnum
     {
        Unknown = 0,
        Void,
        Integer,
        Real,
        Double,
        Complex,
        DoubleComplex,
        Boolean,
        Character,
        Derived  //TODO-SgUntyped
     };

   TypeSpec(TypeEnum type_id) : pTypeEnum(type_id)
     {
     }

   TypeSpec(TypeEnum type_id, PosInfo pos) : FASTNode(pos), pTypeEnum(type_id)
     {
     }

   TypeEnum  getTypeEnum()                    {return pTypeEnum;}
   void      setTypeEnum(TypeEnum type_enum)  {pTypeEnum = type_enum;}

 protected:
   TypeEnum pTypeEnum;
};

class IntrinsicTypeSpec : public TypeSpec
{
 public:
   IntrinsicTypeSpec() : TypeSpec(TypeSpec::Unknown)
     {
     }
};


/* Statements
 */
class Statement : public FASTNode
{
 public:
   Statement(std::string label, std::string eos, PosInfo pos)
      :  FASTNode(pos), pLabel(label), pEOS(eos)
     {
     }

   std::string & getLabel()                   {return pLabel;}
   void          setLabel(std::string label)  {pLabel = label;}

   std::string & getEOS()                     {return pEOS;}
   void          setEOS(std::string eos)      {pEOS = eos;}

 protected:
   std::string pLabel;
   std::string pEOS;
};

class ProgramStmt : public Statement
{
 public:
   ProgramStmt(std::string label, std::string name, std::string eos, PosInfo pos)
      : Statement(label,eos,pos), pName(name)
     {
     }

   std::string & getName()                   {return pName;}
   void          setName(std::string name)   {pName = name;}

 protected:
   std::string pName;
};

class EndProgramStmt : public Statement
{
 public:
   EndProgramStmt(std::string label, std::string name, std::string eos, PosInfo pos)
      : Statement(label,eos,pos), pName(name)
     {
     }

   std::string & getName()                   {return pName;}
   void          setName(std::string name)   {pName = name;}

 protected:
   std::string pName;
};

class ContainsStmt : public Statement
{
};

class UseStmt : public Statement
{
 public:

   enum ModuleNature
     {
        None = 0,
        Intrinsic,
        NonIntrinsic
     };

   UseStmt(std::string label, std::string name, ModuleNature nature, std::string eos, PosInfo pos)
      : Statement(label,eos,pos), pName(name), pNature(nature)
     {
     }

   std::string & getName()                  {return pName;}
   void          setName(std::string name)  {pName = name;}

   ModuleNature  getModuleNature()                     {return pNature;}
   void          setModuleNature(ModuleNature nature)  {pNature = nature;}

   std::vector<std::string> & getOnlyList()            {return pOnlyList;}
   void setOnlyList(std::vector<std::string> list)     {pOnlyList = list;}

   std::map<std::string, std::string> & getRenameMap() {return pRenameMap;}
   void setRenameMap(std::map<std::string, std::string> map) {pRenameMap = map;}

 protected:
   std::string pName;
   ModuleNature pNature;
   std::vector<std::string> pOnlyList;
   std::map<std::string, std::string> pRenameMap;
};

class LetterSpec : public FASTNode
{
 public:
   LetterSpec() : pLetterBegin(0), pLetterEnd(0)
     {
     }
   LetterSpec(char begin, PosInfo pos)
      : FASTNode(pos), pLetterBegin(begin), pLetterEnd(0)
     {
     }
   LetterSpec(char begin, char end, PosInfo pos)
      : FASTNode(pos), pLetterBegin(begin), pLetterEnd(end)
     {
     }

   char getLetterBegin()             {return pLetterBegin;}
   void setLetterBegin(char letter)  {pLetterBegin = letter;}

   char getLetterEnd()             {return pLetterEnd;}
   void setLetterEnd(char letter)  {pLetterEnd = letter;}

 protected:
   char pLetterBegin;
   char pLetterEnd;
};

class ImplicitSpec : public FASTNode
{
 public:
   ImplicitSpec(TypeSpec spec, PosInfo pos) : FASTNode(pos), pTypeSpec(spec)
     {
     }

   std::vector<LetterSpec> & getLetterSpecList()             {return pLetterSpecList;}
   void setLetterSpecList(std::vector<LetterSpec> list)      {pLetterSpecList = list;}

 protected:
   TypeSpec pTypeSpec;
   std::vector<LetterSpec> pLetterSpecList;
};

class ImplicitStmt : public Statement
{
 public:

   ImplicitStmt(std::string label, std::vector<ImplicitSpec> spec_list, std::string eos, PosInfo pos)
      : Statement(label,eos,pos), pImplicitSpecList(spec_list)
     {
     }

   std::vector<ImplicitSpec> & getImplicitSpecList()             {return pImplicitSpecList;}
   void setImplicitSpecList(std::vector<ImplicitSpec> list)      {pImplicitSpecList = list;}

 protected:
   std::vector<ImplicitSpec> pImplicitSpecList;
};

/* Procedures
 */
class Procedure : public FASTNode
{
 public:
   Procedure(PosInfo pos) : FASTNode(pos)
   {
   }
};

class MainProgram : public Procedure
{
 public:
   MainProgram(ProgramStmt* program, Scope* scope, ContainsStmt* contains, EndProgramStmt* end, PosInfo pos)
      : Procedure(pos)
     {
        pProgramStmt = program;
        pScope = scope;
        pContainsStmt = contains;
        pEndProgramStmt = end;
     }
   virtual ~MainProgram()
     {
        printf("--- DESTROYING MainProgram \n");
        if (pProgramStmt) delete pProgramStmt;
        delete pScope;
        if (pContainsStmt) delete pContainsStmt;
        delete pEndProgramStmt;
     }

   ProgramStmt*    getProgramStmt()                       { return pProgramStmt; }
   EndProgramStmt* getEndProgramStmt()                    { return pEndProgramStmt; }
   ContainsStmt*   getContainsStmt()                      { return pContainsStmt; }
   Scope*          getScope()                             { return pScope; }

 protected:
   ProgramStmt* pProgramStmt;
   Scope* pScope;
   ContainsStmt* pContainsStmt;
   EndProgramStmt* pEndProgramStmt;
};

class Converter
{
 public:
   virtual void convert_MainProgram(MainProgram* main_program) = 0;
};

} // namespace FAST

#endif

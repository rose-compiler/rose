#ifndef FAST_NODES_H
#define FAST_NODES_H

#include <string>
#include <vector>

namespace FAST {

class PosInfo
{
 public:
   PosInfo() : pStartLine(0), pStartCol(0), pEndLine(0), pEndCol(0)
    {
    }

   PosInfo(int strtLine, int strtCol, int endLine,  int endCol)
     {
        pStartLine = strtLine;
        pStartCol  = strtCol;
        pEndLine   = endLine;
        pEndCol    = endCol;
     }

   int  getStartLine()          { return pStartLine; }
   int  getStartCol()           { return pStartCol; }
   int  getEndLine()            { return pEndLine; }
   int  getEndCol()             { return pEndCol; }

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

   PosInfo     & getPosInfo()                 {return pPosInfo;}
   void          setPosInfo(PosInfo pos)      {pPosInfo = pos;}

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
   UseStmt(std::string label, std::string name, std::string eos, PosInfo pos)
      : Statement(label,eos,pos), pName(name)
     {
     }

   std::string & getName()                   {return pName;}
   void          setName(std::string name)   {pName = name;}

 protected:
   std::string pName;
// TODO -> UseStmt(OptLabel,OptModuleNature,Name,ListStarOfRename)
// OptModuleNature, ListStarOfRename
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

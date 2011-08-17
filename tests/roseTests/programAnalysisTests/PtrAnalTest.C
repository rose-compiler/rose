
#include <sage3.h>

#include <StmtInfoCollect.h>
#include <AstInterface_ROSE.h>

#include <PtrAnal.h>
#include <string>
#include <iostream>
#include <CommandOptions.h>
#include <GraphIO.h>

void PrintUsage( char* name)
{
  std::cerr << name << "<program name>" << "\n";
}

class PrintPtrAnalMap : public ProcessAstNode
{
   PtrAnal& m;
  public:
   PrintPtrAnalMap(PtrAnal& _m) : m(_m) {}
   virtual bool Traverse( AstInterface &fa, const AstNodePtr& _n,
                             AstInterface::TraversalVisitType t) 
  {
       AstNodePtr n = fa.IsExpression(_n); 
       if (n != AST_NULL) {
          PtrAnal::VarRef p = m.translate_exp(n);
          if (p.name != "") {
            std::cout << AstToString(n) << ":" << 
             ((long) p.stmt) << p.name << "\n"; 
          }
      }
      else if (fa.IsStatement(_n)) {
          PtrAnal::StmtRef p = m.translate_stmt(_n);
          if (p.size()) {
            std::cout << AstToString(_n) << ":" << 
             ((long) p.front()) << "->" << ((long)p.back()) << "\n"; 
          }
      }
       return true;
  }
};



class TestPtrAnal : public PtrAnal
{
 private:
  long stmtIndex;
  void outputlist(const std::list<std::string>& r) 
    {
       std::list<std::string>::const_iterator p = r.begin(); 
       if (p == r.end()) return;
       std::cout << *p;
       for (++p; p != r.end(); ++p) {
          std::cout << "," << *p;
       }
    }
  virtual bool may_alias(const std::string& x, const std::string& y) { return true; }
  virtual void contrl_flow(Stmt s1, Stmt s2, CFGConfig::EdgeType t) 
    {
      std::cout << "ctrl: " << ((long)s1) << "->" << ((long)s2) << " " << CFGConfig::EdgeType2String(t) << "\n";
    }
  virtual Stmt x_eq_y(const std::string& x, const std::string& y)
      { 
        if (x == "" || y == "") {
            std::cerr << "x=" << x << "; y=" << y <<"\n";
            assert(0);
        } 
        std::cout << stmtIndex << ":" << x << "=" << y << ";\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt x_eq_addr_y(const std::string& x, const std::string& y)
      { std::cout << stmtIndex << ":" << x << "=" << "&" << y << ";\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt x_eq_field_y(const std::string& x, const std::string& field,
                            const std::string& y)
      { std::cout << stmtIndex << ":" << x << "=" << y;
        if (field != "") 
            std::cout << "." << field;
        std::cout << "\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt x_eq_deref_y(const std::string& x, const std::string& field,
                            const std::string& y)
      { std::cout << stmtIndex << ":" << x << "=" << "(*" << y << ")";
        if (field != "") 
            std::cout << "." << field;
        std::cout << "\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt field_x_eq_y(const std::string& x, 
              const std::list<std::string>& fields, const std::string& y)
      { std::cout << stmtIndex << ":" << x ;
        for (std::list<std::string>::const_iterator p = fields.begin();
             p != fields.end(); ++p) {
           std::cout << "." << (*p);
        }
        std::cout << "=" << y << "\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt deref_x_eq_y(const std::string& x, 
              const std::list<std::string>& fields, const std::string& y)
      { std::cout << stmtIndex << ":" << "(*" << x << ")";
        for (std::list<std::string>::const_iterator p = fields.begin();
             p != fields.end(); ++p) {
           std::cout << "." << (*p);
        }
        std::cout << "=" << y << "\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt allocate_x(const std::string& x)
      { std::cout << stmtIndex << ":" << "allocate " << x << ";\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt x_eq_op_y(OpType op, const std::string& x, const std::list<std::string>& y) 
      { 
            std::cout << stmtIndex << ":" << x << "=" << "op(";
            for (std::list<std::string>::const_iterator p = y.begin();
                 p != y.end(); ++p) {
               std::cout << (*p) << " ";
            }
            std::cout << ");\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt funcexit_x( const std::string& x)
      {  std::cout << stmtIndex << ":" << "return-" << x << ";\n"; 
        return (void*)stmtIndex++; 
      }
  virtual Stmt funcdef_x(const std::string& x, 
                          const std::list<std::string>& params,
                          const std::list<std::string>& results) 
      { 
          std::cout << stmtIndex << ":" << "func " << x;
          std::cout << "(";
          outputlist( params); 
          std::cout << ") => (";
          outputlist( results); 
          std::cout << ");\n";
        return (void*)stmtIndex++; 
      }
  virtual Stmt funccall_x ( const std::string& p,
                         const std::list<std::string>& args,
                          const std::list<std::string>& x)
      { 
        std::cout << stmtIndex << ":" << "(";
        outputlist(x);
        std::cout << ") =" << p << "(";
        outputlist( args); 
        std::cout << ");\n";
        return (void*)stmtIndex++; 
       }

 public:
  TestPtrAnal() : stmtIndex(1) {}
  void operator()(AstInterface& fa, SgNode* head)
  {
    PtrAnal::operator()(fa, AstNodePtrImpl(head));
    std::cout << "\n Mapping " << "\n";
    PrintPtrAnalMap pr(*this);
    AstNodePtrImpl cur (head);
    ReadAstTraverse(fa, cur, pr);
    std::cout << "END Mapping \n\n";
  }
};

int
main ( int argc,  char * argv[] )
   {

     if (argc <= 1) {
         PrintUsage(argv[0]);
         return -1;
     }

    SgProject sageProject ( argc,argv);
    SageInterface::changeAllLoopBodiesToBlocks(&sageProject);
    CmdOptions::GetInstance()->SetOptions(argc, argv);


   TestPtrAnal op;
   int filenum = sageProject.numberOfFiles();
   for (int i = 0; i < filenum; ++i) {

     SgSourceFile* sageFile = isSgSourceFile(sageProject.get_fileList()[i]);
     std::string filename = sageFile->get_file_info()->get_filename();
     ROSE_ASSERT(sageFile != NULL);
     SgGlobal *root = sageFile->get_globalScope();

     AstInterfaceImpl scope(root);
     AstInterface fa(&scope);
     SgDeclarationStatementPtrList& declList = root->get_declarations ();
     for (SgDeclarationStatementPtrList::iterator p = declList.begin(); p != declList.end(); ++p) {
          SgFunctionDeclaration *func = isSgFunctionDeclaration(*p);
          if (func == 0)
             continue;
          SgFunctionDefinition *defn = func->get_definition();
          if (defn == 0)
             continue;
          if (defn->get_file_info()->get_filename() != filename)
             continue;
          op(fa, defn);
     }
   }
  return 0;
}


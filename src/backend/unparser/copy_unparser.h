/* copy_unparser.h */
#ifndef COPY_UNPARSER_H
#define COPY_UNPARSER_H
                                                                                                         


#include <list>
#include <string>

class CopyUnit {

// a statement's start and end character position in the oroginal source file
  unsigned long start, end;
 public:
  CopyUnit( unsigned long s= 0, unsigned long e =0) : start(s), end(e) {}
  unsigned long get_start() const { return start; }
  unsigned long get_end() const { return end; }
};

class CopyUnparser : public UnparseDelegate
{
// This class is a delegate class.  The unparser was modified to date an optional 
// UnparseDelegate object, if the pointer to UnparseDelegate is non-null then the 
// unparser queries the UnparseDelegate object before unparsing statements (only 
// applied to SgStatement) look for a delegate before proceeding to unparse.  In 
// this way alternative unparsing can be introduced on top of the original 
// unparser.
      std::ostream* os; //! the directed output for the current file
     
 const char* filename;
 std::ifstream ifs;

 void put(int c); 
// character offset for the start of each line in the original input file
 std::vector <unsigned long> lineMap;

// Set of modified IR nodes
 std::set <SgNode*> modset;

// STL map of SgStatement to their position in the source code (CopyUnit)
 std::map <SgStatement*, CopyUnit> copylist;

 void Traverse (SgStatement *stmt);
public:
 CopyUnparser(SgFile& file);

// DQ (5/26/2005): Made destructor virtual to match use with virtual member function
 virtual ~CopyUnparser();

 bool is_modified( SgNode* n) ;
 virtual bool unparse_statement(SgStatement* stmt, SgUnparse_Info& info, UnparseFormat& out);


};

// void CopyUnparseProject(SgProject& project);
void CopyUnparseProject ( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL );
#endif

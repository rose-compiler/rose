/* copy_unparser.h */
#ifndef COPY_UNPARSER_H
#define COPY_UNPARSER_H
                                                                                                         
#include "rose.h"

#include <list>
#include <string>

class CopyUnit {
  unsigned long start, end;
 public:
  CopyUnit( unsigned long s= 0, unsigned long e =0) : start(s), end(e) {}
  unsigned long get_start() const { return start; }
  unsigned long get_end() const { return end; }
};
                                                                                                         
class CopyUnparser : public UnparseDelegate
{
 char* filename;
 ifstream ifs;
 vector <unsigned long> lineMap;
 set <SgNode*> modset;
 map <SgStatement*, CopyUnit> copylist;

 void Traverse (SgStatement *stmt);
public:
 CopyUnparser(SgFile& file);
 ~CopyUnparser();

 bool is_modified( SgNode* n) ;
 virtual bool unparse_statement(SgStatement* stmt, SgUnparse_Info& info, UnparseFormatBase& out);
};

#endif

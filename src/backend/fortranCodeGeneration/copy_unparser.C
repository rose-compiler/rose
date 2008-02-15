/* copy_unparser.C */
                                                                                                         
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
                                                                                                         
#include "rose.h"
                                                                                                         
#include "unparser.h"
#include <copy_unparser.h>

#include <list>
#include <string>
#include <support/CommandOptions.h>

bool DebugCopyUnparse()
{
  static int doit = 0;
  if (doit == 0) 
    doit = (CmdOptions::GetInstance()->HasOption("debugcpup"))? 2 : 1;
  return doit == 2;
}


bool CopyUnparser:: is_modified( SgNode* n) 
  {
    if (modset.find(n) != modset.end())
       return true;
/*
    Sg_File_Info* f = n->get_file_info();
    if (f != 0  && f->get_isPartOfTransformation())
        return true;
*/
    if (n->get_isModified())
        return true;

    vector <SgNode*> children = n->get_traversalSuccessorContainer();
    for (vector<SgNode*>::const_iterator cp = children.begin(); 
         cp != children.end(); ++cp) {
       SgNode* cur = *cp;
       if (cur != 0 && is_modified( cur))  {
            modset.insert(n);
            return true;
       }
    }
    return false;
  }

CopyUnparser:: CopyUnparser(SgFile& file)
{
    filename = file.getFileName(); 
    ifs.open(filename, ios::binary); 
    lineMap.push_back(0);
    while (ifs.good()) {
      char c; 
      do { 
         c = ifs.get(); 
        if (DebugCopyUnparse()) 
             cerr << c;
      } while (ifs.good() && c != '\n');
      unsigned long pos = ifs.tellg();
      lineMap.push_back(pos);
      if (ifs.good() && DebugCopyUnparse()) 
         cerr << "\n ending position: " << pos;
    }
    Traverse(file.get_root());
}

CopyUnparser::~CopyUnparser()
{
    ifs.close(); 
}

void  CopyUnparser :: Traverse (SgStatement *stmt)
  {
    Sg_File_Info* start = stmt->get_startOfConstruct(), *end = stmt->get_endOfConstruct();
    assert( start != 0);
    if (strcmp(start->get_filename(),filename)) 
       return;

    //QY: temporary fix. line and colume should start from 1, not 0
    if (start->get_line() == 0)
        start->set_line(1);
    if (start->get_col() == 0)
       start->set_col(1);

    if (DebugCopyUnparse() && end == 0) 
            cerr << "statement has no ending info " << start->get_line() << " : " << start->get_col()  << " => ? " << endl;

    if (!is_modified(stmt) && start != 0 && end != 0) { 
        if (DebugCopyUnparse()) 
            cerr << "statement is non-modified: " << start->get_line() << " : " << start->get_col()  << " => " << end->get_line() << " : " << end->get_col() +1 << endl;
        
        copylist[stmt] = CopyUnit(lineMap[start->get_line()-1]+start->get_col()-1, 
                                    lineMap[end->get_line()-1]+end->get_col()+1); 
    }
    else if (start->get_isPartOfTransformation()) {
        if (DebugCopyUnparse()) 
              cerr << "statement is new \n";
    }
    else {
        if (DebugCopyUnparse() && start != 0 && end != 0)  
            cerr << "statement is modified: " << start->get_line() << " : " << start->get_col()  << " => " << end->get_line() << " : " << end->get_col() << endl;
        vector <SgNode*> children = stmt->get_traversalSuccessorContainer();
        for (vector<SgNode*>::const_iterator cp = children.begin(); 
             cp != children.end(); ++cp) {
           SgStatement* cur = isSgStatement(*cp);
           if (cur != 0)  
               Traverse(cur);
           else
              cerr << "not statement : " << (*cp)->sage_class_name() << endl;
        }
    }
  }

bool CopyUnparser::unparse_statement(SgStatement* stmt, SgUnparse_Info& info, UnparseFormatBase& out)
{

     map <SgStatement*, CopyUnit>::const_iterator cp = copylist.find(stmt);
     if (cp != copylist.end()) {
        CopyUnit cur = (*cp).second;
        if (DebugCopyUnparse()) 
           cerr << "copying from " << cur.get_start() << " until " << cur.get_end() << endl;
        ifs.clear();
        ifs.seekg(cur.get_start(), ios_base::beg);
        for (unsigned i = 0; i < cur.get_end() - cur.get_start(); ++i) {
            assert(ifs.good());
            int c = ifs.get();
            if (DebugCopyUnparse()) 
                 cerr << (char)c;
            out.put(c);
        }
        return true;
     }
     return false;
}

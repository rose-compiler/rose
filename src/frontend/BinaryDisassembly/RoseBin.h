/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/

#ifndef __RoseBin__
#define __RoseBin__


//#include <mysql.h>
#include "RoseBin_abstract.h"
#include <stdio.h>
#include <iostream>
// #include "rose.h"
#include "RoseBin_DB_IDAPRO.h"
// #include "RoseBin_support.h"
// #include "RoseBin_unparse.h"


class RoseBin : public RoseBin_abstract {
 private:
  MYSQL_RES *res_set;
  // the DB connection
  RoseBin_DB_IDAPRO* idaDB; 
  MYSQL* conn;
  //  RoseBin_unparse* unparser;
  //SgAsmNode* globalNode;
  //std::list<std::string> functionNames;

  void loadAST(std::string filename);
  void saveAST(std::string filename);


 public:

  RoseBin(char* host, 
          char* user, 
          char* passw, 
          char* dbase) {
    RoseBin_support::setDebugMode(true);    
    //    RoseBin_support::setDebugMode(false);    
        RoseBin_support::setDataBaseSupport(true);    
    idaDB = new RoseBin_DB_IDAPRO(host, user, passw, dbase);
    //unparser = new RoseBin_unparse();
    //RoseBin_support::setUnparseVisitor(unparser->getVisitor());
    res_set = 0;
    conn = 0;
    //    globalNode = 0;
    //functionNames.clear();
    //num_inst=0;
    //num_func=0;
    //RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::none;
    
  }
  
  ~RoseBin() {
    delete idaDB;
    //delete unparser;

    idaDB = NULL;
    //unparser = NULL;

    //if (globalNode)
    //  delete globalNode;
  }


  // allow filtering of functions
  void setFunctionFilter(std::list<std::string> functionName);

  void visit(SgNode* node);

  // connect to the DB
  void connect_DB(const char* socket);

  // query the DB to retrieve all data
  SgAsmNode* retrieve_DB_IDAPRO();

  // close the DB
  void close_DB();

  // unparse the AST to assembly
  void unparse(char* fileName);

  void test();
};

#endif


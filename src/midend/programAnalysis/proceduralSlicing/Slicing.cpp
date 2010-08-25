#include "rose.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Slicing.h"

/*
The function that initializes the slicing criterion and finds the function declaration in which the slicing will be done.
 */
void Slicing::init(SgProject* sgproject, set<SgStatement*>& slicing_criterion,
                   SgFunctionDefinition*& func_defn){
  SgFile &sageFile = sgproject->get_file(0);
  SgGlobal *root = sageFile.get_root();

  // Finding the set of statements (slicing criterion) between the #pragma declarations
  FindSlicingCriterion(root, slicing_criterion);
  
  // Find the function declaration in which the slicing criterion is:
  // Using the first statement in the slicing criterion should be sufficient because all of the
  // statements in the slicing criterion should all be in the same basic block, thus also in the
  // same function.
  SgNode* node = isSgNode(*slicing_criterion.begin());
  SgFunctionDeclaration* func_decl =  TransformationSupport::getFunctionDeclaration(node);
  // This function gives seg fault when empty between pragmas and interface 2 or 3? why??
  
  func_defn = func_decl->get_definition(); 
  //writeStatements(stmts, "What is between the pragmas:");
}


void Slicing::completeSlice(SgProject* sgproject){
  set<SgStatement*> slicing_criterion;
  SgFunctionDefinition* func_defn;
  init(sgproject, slicing_criterion, func_defn);
  set<SgNode*> stmt_in_slice;  
  onlyStmts(slicing_criterion, func_defn, stmt_in_slice); 
  
  MarkingNodes v;
  addControlStmts(sgproject, v, func_defn,stmt_in_slice);
  
  keepNeededFunc(sgproject, v, func_defn);

  /*
  // Checking!
  CheckWhichAreMarked check;
  check.traverse(sgproject,preorder);
  */

  // Traversal of the AST to remove all nodes which haven't been marked 
  RemovalOfNodes r;
  r.traverse(sgproject,preorder);

 // Unparsing; that is generating outputfile with prefix rose_
  sgproject->unparse();
 
}

void Slicing::keepNeededFunc(SgProject* sgproject, MarkingNodes v, SgFunctionDefinition* func_defn){

  // get the function calls that are already in the slice
  list<SgFunctionDeclaration*> funclist = v.get_funclist();
  funclist.sort();
  funclist.unique(); // To remove duplicates
  list<SgFunctionDeclaration*> keepTheseFunc = funclist;  // save a copy
  
#ifdef DEBUG_SLICING
  cout << "funclist contains" << endl;
  for(list<SgFunctionDeclaration*>::const_iterator ss=funclist.begin(); ss!=funclist.end(); ++ss){
    cout <<"    - " << (*ss)->unparseToString() << endl;
  }
#endif
  
  // now we need to find other functions these functions in the funclist might call:
  // In the new list "keepTheseFunc" all functions which we want to keep are stored,
  // while in "funclist" they are removed as we "traverse" them to find addtional function calls.
  while(!funclist.empty()){
    SgFunctionDeclaration* funcdecl = *(funclist.begin()); // get the first element of the list
    funclist.pop_front();
     
    // to include forward declarations???
    funcdecl->get_forwardDefinition();
    if(funcdecl->get_forwardDefinition()!=NULL){
      cout << "Should happen some times..." << endl;
      //keepTheseFunc.push_front(funcdecl->get_forwardDefinition()->get_declaration());
    }

    // Finding all SgFunctionRefExp in the subtree starting from the function declaration funcdecl:
    list<SgNode*> l = NodeQuery::querySubTree(isSgNode(funcdecl),V_SgFunctionRefExp);
    
    // For all SgFunctionRefExp add them to both the list storing the needed function for later use
    // (keepTheseFunc) and to the list we traverse (funclist).
    for(list<SgNode*>::const_iterator s=l.begin(); s!=l.end(); ++s){
      SgFunctionDeclaration* d = isSgFunctionRefExp(*s)->get_symbol()->get_declaration();
      keepTheseFunc.push_front(d);
      funclist.push_front(d); // to handle "nested" function calls.
      cout <<"    - " << (*s)->unparseToString() << endl;
   }
  }
  
  // see to that all variables within the functions are declared properly - to handle global variables!
  for(list<SgFunctionDeclaration*>::const_iterator k = keepTheseFunc.begin(); k!=keepTheseFunc.end(); ++k){
    list<SgNode*> var = NodeQuery::querySubTree(isSgNode(*k), V_SgVarRefExp);
    for(list<SgNode*>::const_iterator it = var.begin(); it!=var.end(); ++it){
      MarkingNodes::checkIfDeclared(*it,true);
    } 
  }
  
  // Must also remember to keep the function we slice...
  // (in this function we have already checked if all variables are declared properly...
  keepTheseFunc.push_front(func_defn->get_declaration());
  //func_defn->get_declaration()->get_name();  // THis sometimes fails. Why??
  
  // get prototypes of the functions


  // for those function declarations that are not in the list: Remove the keep attribute from
  // these SgFunctionDeclaration nodes:
  list<SgNode*> allFuncDecl = NodeQuery::querySubTree(sgproject,V_SgFunctionDeclaration);
  for(list<SgNode*>::const_iterator it=allFuncDecl.begin(); it!=allFuncDecl.end(); ++it){
 if(isSgClassDeclaration(*it)!=NULL){cout << "tell me!" << endl; }
     // if the functiondeclaration is not in the list "keepTheseFunc" then we remove the "keep"
    // attribute. This removes the whole subtree constituting the function in the AST.
    if(!isInList((*it), keepTheseFunc) && isSgFunctionDeclaration(*it)!=NULL){ 
     if((*it)->attribute.exists("keep")){      
       (*it)->attribute.remove("keep");
      }   
    }
  }
  
}

void Slicing::sliceOnlyStmtWithControl(SgProject* sgproject, set<SgNode*>& stmt_in_slice){
  set<SgStatement*> slicing_criterion;
  SgFunctionDefinition* func_defn;
  init(sgproject, slicing_criterion, func_defn);
  onlyStmts(slicing_criterion, func_defn, stmt_in_slice); 

  MarkingNodes v;
  addControlStmts(sgproject,v, func_defn,stmt_in_slice);
  // Traversing the AST to mark which nodes are to be kept for the slice/ marking nodes that are in the slice set. At the same time in this traversal, we collect new statements from the control constructions around the statements in the slice, which we will need to find the data flow of.
#ifdef DEBUG_SLICING 
  writeNodes(stmt_in_slice, "What stmt_in_slice contains:");
#endif
} 

void Slicing::addControlStmts(SgProject* sgproject, MarkingNodes& v,SgFunctionDefinition* func_defn,
                              set<SgNode*>& stmt_in_slice){
  v.initialize(stmt_in_slice, func_defn);  // copying these to the marking class, and we continue adding statements to this list in the following traversals????
  v.traverse(sgproject,false);
  set<SgNode*> new_stmt = v.get_newStatementList();
  
#ifdef DEBUG_SLICING 
  writeNodes(new_stmt, "What newstmt contains:");
#endif 
  
  // To handle nested control statements: "newstmt" contains additional statements necessary for the slice. For these statements we need to find both the statements affect them and mark the AST accordingly.
  while(!new_stmt.empty()){
    for(set<SgNode*>::const_iterator it=new_stmt.begin();it!=new_stmt.end();++it){
      //For all these new statements,find the statements affecting these statements and collect in stmt_in_slice.
      FindStatements::FindStatementsForSlice(func_defn, (*it), stmt_in_slice, 0);
    }
    new_stmt.clear(); // clear out, so the while loop will terminate.
    
#ifdef DEBUG_SLICING  
    writeNodes(stmt_in_slice, "What stmt_in_slice contains:");  
#endif    
    
    // We traverse the AST one more time to include the new statements affecting the control statements.
    v.initialize(stmt_in_slice, func_defn);// update with whole slice.
    v.traverse(sgproject,false);
    new_stmt =  v.get_newStatementList();  // When no more additional statements we exit the while loop.
  }
  // Have to save somewhere all new statements... to be able to return them...
  
  
}



void Slicing::sliceOnlyStmts(SgProject* sgproject, set<SgNode*>& stmt_in_slice){
   // Finding the set of statements (slicing criterion) between the #pragma declarations
  set<SgStatement*> slicing_criterion;
  SgFunctionDefinition* func_defn;

  init(sgproject, slicing_criterion, func_defn);
  onlyStmts(slicing_criterion, func_defn, stmt_in_slice);

}
void Slicing::onlyStmts(set<SgStatement*> slicing_criterion, SgFunctionDefinition* func_defn,
			set<SgNode*>& stmt_in_slice){
  
  // set<SgNode*> stmt_in_slice will contain the statements relevant for the program slice. However, without control statements which do not directly include any reference to the slicing statement.
  //set<SgNode*> stmt_in_slice; 
  set<SgStatement*>::const_iterator s;
  for(s=slicing_criterion.begin(); s!=slicing_criterion.end(); ++s){
    // For all statmenets in the slicing criterion, we find the statements
    // affecting these slicing criterion statements (using def use chain graph...) and
    // collect them in the set "stmt_in_slice".
    FindStatements::FindStatementsForSlice(func_defn, isSgNode(*s), stmt_in_slice, 0);
  }
#ifdef DEBUG_SLICING  
  writeNodes(stmt_in_slice, "What stmt_in_slice contains");
#endif
}



void Slicing::FindSlicingCriterion(SgGlobal* root,set<SgStatement*>& stmts){
  // If no slicing criterion present, exit
  list<SgNode*> pragma = NodeQuery::querySubTree(root, V_SgPragmaDeclaration);
  if(pragma.empty()){
    cout << "The sgproject has no pragma declarations, that is no slicing criterion.\n" 
         << "Slicing program ends..." << endl;
    exit(-1);
  }
  // Finding the set of statements between two pragma declarations.
  // The pragmas must be declared within a basic block.
  SlicingCriterion sc;
  sc.set_bool(); // initialize bool variable in traversal class
  sc.traverse(root,preorder);
  stmts = sc.get_stmts_between_pragma();
}


// Output functions, useful when testing and debugging.
void Slicing::writeStatements(set<SgStatement*> stmts, string heading){
  cout<< heading << endl;
  set<SgStatement*>::const_iterator s;
  for(s=stmts.begin();s!=stmts.end();++s){
    cout <<"    - " << (*s)->unparseToString() << endl;
  }  
}

void Slicing::writeNodes(set<SgNode*> stmts, string heading){
  cout<< heading << endl;
  set<SgNode*>::const_iterator s;
  for(s=stmts.begin();s!=stmts.end();++s){
    cout <<"    - " << (*s)->unparseToString() << endl;
  } 
}

// To find whether node is in the funclist or not.

bool Slicing::isInList(SgNode* node, list<SgFunctionDeclaration*> funclist){
  bool isThere = false;
  if(isSgFunctionDeclaration(node)!=NULL){
    SgFunctionDeclaration* funcdecl = isSgFunctionDeclaration(node);
    for(list<SgFunctionDeclaration*>::const_iterator it=funclist.begin(); it!=funclist.end(); ++it){
      if(funcdecl->get_name() == (*it)->get_name())
	//if(funcdecl == *it)  // doesn't include function prototypes
	isThere = true;
    }
  }
  return isThere;
}

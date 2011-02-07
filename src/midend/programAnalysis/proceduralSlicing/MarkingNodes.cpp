#include "rose.h" 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "MarkingNodes.h"

/* * * * * * * * * * * * * * *
 * evaluateInheritedAttribute
 * * * * * * * * * * * * * * */
bool MarkingNodes::evaluateInheritedAttribute(SgNode* astNode, bool inherited){
  
  bool keep=false;  // default = false

  // the parent node is to be kept, then keep this child node as well
  if(inherited){
    keepNode(astNode);
    keep = true; 
  }
  // otherwice check if the node corresponds to a statement in the list of statements we want to keep
  else{
    set<SgNode*>::const_iterator s;
    for(s=stmtlist.begin();s!=stmtlist.end();++s){ 
      if(astNode==(*s)){ 
        keepNode(astNode);
        keep = true; 
      } 
    } 
  }

  // Do special things for special AST nodes:
  
  // If Basic block, cannot unconditionally inherit information
  if(isSgBasicBlock(astNode)!=NULL){
    keep = false;
  }
  
  // We always want to keep the return statement of a function
  if(isSgReturnStmt(astNode)!=NULL){
    keepNode(astNode);
    // to include any changes that might have affected the return statement.
    SgStatement *s = isSgReturnStmt(astNode)->get_expression_root()->get_statement();
    insertInList(isSgNode(s));
    keep = true;
  }
  
  // We want to keep all break and continue statements _before_ the pragma declaration
  if(isSgBreakStmt(astNode)!=NULL || isSgContinueStmt(astNode)!=NULL){
    if(beforePragma){ 
      keepNode(astNode);  
      keep = true;
    }
  }
  // Update bool variable saying if we are before or after the slicing criterion
  if(isSgPragmaDeclaration(astNode)!=NULL){
    beforePragma = false; 
  }

  // When finding a function declaration which we are not doing slicing on, then keep as is.
  if(isSgFunctionDeclaration(astNode)!=NULL){
    if(isSgFunctionDeclaration(astNode)->get_definition()!=sliceThisFunc){
      keepAllNodes(astNode);
    }
    // always keep the main function, but cannot unconditionally pass on information
    else {
      keepNode(astNode);
      keep = false;
    }
  }
  
  // check if it is a variable on the right hand side, and in case it is,
  // check if the declaration of the variable is marked!        
  checkIfDeclared(astNode,keep);
  
  // Make sure SgGlobal is always marked
  if(isSgGlobal(astNode)!=NULL){
    keepNode(astNode);
    keep = false;
  }

  return keep;  
}

/* * * * * * * * * * * * * * *
 * evaluateSynthesizedAttribute
 * * * * * * * * * * * * * * */
bool MarkingNodes::evaluateSynthesizedAttribute(SgNode* astNode,
                                                  bool inherited, vector<bool> synattri){
  bool keep = false;  // default

  // If inherited info to keep, then keep.
  if(inherited){
    keepNode(astNode);
    keep = true;
  }
  // else check if any of the childer pass up the syntehsized attribute that tells us to keep it
  else{
    vector<bool>::iterator it;
    for(it=synattri.begin(); it!=synattri.end(); ++it){
      if(*it){
        keepNode(astNode);
        // Now in the case of a SgBasicBlock we want to pass on information to keep
        // the node up in the AST.
        if(isSgBasicBlock(astNode) != NULL){
          keep = true;
        }
      }
    }
  }

  // To include statements in control statements: If a control structure is marked to be kept, then extract the statements within the control structure itself (not within the basic block...)

  // For statements
  if(isSgForStatement(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      // init statements
      list<SgStatement*> init = isSgForStatement(astNode)->get_init_stmt();
      for(list<SgStatement*>::const_iterator it=init.begin(); it!=init.end(); ++it){
        insertInList(isSgNode(*it));
        keepNode(isSgNode(*it));
        keep = true;
      }
      // test expression
      SgExpression* test = isSgForStatement(astNode)->get_test_expr();
      // When inserting in this list, do not need to explicitly mark node by keepNode because these statements are marked in a later traversal, when we also have found all statements that affect this one as well
      insertInList(isSgNode(test));
      
      // increment expression
      SgExpression* increment = isSgForStatement(astNode)->get_increment_expr();
      insertInList(isSgNode(increment));
    }
    // not explicitly marking these nodes to be kept, they seem to be included anyway.
  }
  // If Statement
  if(isSgIfStmt(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      SgStatement *cond = isSgIfStmt(astNode)->get_conditional();
      insertInList(isSgNode(cond));
    }
  }
  // While statement
  if(isSgWhileStmt(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      SgStatement *cond = isSgWhileStmt(astNode)->get_condition();
      insertInList(isSgNode(cond));
    }
  }
  // Do-while statement
  if(isSgDoWhileStmt(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      SgStatement *cond = isSgDoWhileStmt(astNode)->get_condition();
      insertInList(isSgNode(cond));
    }
  }
  // Switch statement
  if(isSgCaseOptionStmt(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      keep = true;   // I need this
    }
  }
  if(isSgSwitchStatement(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      SgExpression *selector = isSgSwitchStatement(astNode)->get_item_selector();
      insertInList(isSgNode(selector));
    }
  }
  // Function Reference
  if(isSgFunctionRefExp(astNode)!=NULL){
    if(astNode->attribute.exists("keep")){
      SgFunctionDeclaration* funcdecl = TransformationSupport::getFunctionDeclaration(astNode);
      if(funcdecl->get_definition()==sliceThisFunc){
        //If we find a function reference in the function in which the slicing criterion is, then add these function references to a list: They have to be included in the slice.
        SgFunctionSymbol* symbol = isSgFunctionRefExp(astNode)->get_symbol();
        SgFunctionDeclaration* decl = symbol->get_declaration();
        funclist.push_front(decl);
      }
    }
  }
     
  return keep;
}

bool MarkingNodes::defaultSynthesizedAttribute(){   return false; }


void MarkingNodes::checkIfDeclared(SgNode* node,bool keep){
  // This function checks if an AST node included in a slice is declared properly (i.e. that
  // the declaration of a variable is included in the slice.
  if(keep){
    if(isSgVarRefExp(node)!=NULL){
      SgVarRefExp* varrefexp  = isSgVarRefExp(node);
      SgVariableSymbol* symbol = varrefexp->get_symbol();
      SgInitializedName* declaration = symbol->get_declaration();
      SgDeclarationStatement* stmt = declaration->get_declaration();
      if(!stmt->attribute.exists("keep")){ 
        stmt->attribute.add("keep", new KeepAttribute(true));
        // must also make sure that subtree from this statement is marked with true: That
        // is taken care of in evaluateInherited - in the if-branch if(inherited), because the
        // bool keep will be true for the children.
      }
      // Including the class definition within a class
      if(isSgClassDefinition(stmt->get_parent())!=NULL){
        SgClassDefinition* parent = isSgClassDefinition(stmt->get_parent());
        if(!parent->attribute.exists("keep")){ 
          parent->attribute.add("keep", new KeepAttribute(true));
        }
        if(isSgClassDeclaration(parent->get_parent())!=NULL){
          SgClassDeclaration* classdecl = isSgClassDeclaration(parent->get_parent());
          if(!classdecl->attribute.exists("keep")){ 
            classdecl->attribute.add("keep", new KeepAttribute(true));
          }
        }
      }
    }
  }
}
// initialize before marking traversal.
void MarkingNodes::initialize(set<SgNode*> s, SgFunctionDefinition* func){
  stmtlist=s;
  sliceThisFunc = func;
  beforePragma = true;
  newStmtlist.clear();
}

bool MarkingNodes::isAlreadyInSet(SgNode* node){
  // checks if node is already in the output set of statements
  bool isThere = false;
  set<SgNode*>::const_iterator it;
  for(it=newStmtlist.begin(); it!=newStmtlist.end();++it){
    if((*it)==node)
      isThere = true;
  }
  for(it=allreturnedstmts.begin(); it!=allreturnedstmts.end();++it){
    if((*it)==node)
      isThere = true;
  }
  return isThere;
}

void MarkingNodes::keepNode(SgNode* astNode){
  // marking the node to be kept, using an AstAttribute of type bool.
  if(!astNode->attribute.exists("keep")){ 
    astNode->attribute.add("keep", new KeepAttribute(true));
  } 
}

void MarkingNodes::keepAllNodes(SgNode* astNode){
  MarkAll all;
  all.traverse(astNode,preorder);

}

// When calling this function, we do not need to call keepNode because we will traverse the AST one more time and then this node will be marked to be kept.
void MarkingNodes::insertInList(SgNode* node){
  if(!isAlreadyInSet(node)){
    newStmtlist.insert(node); 
    allreturnedstmts.insert(node); 
  }
}



/*
A tool to move declarations into innermost scopes when applicable.

by Chunhua "Leo" Liao, 9/3/2014
*/
#include "rose.h"
#include "transformationTracking.h"
#include <iostream>
#include <queue> // used for a worklist of declarations to be moved 
using namespace std;
bool debug = false;

//! An internal flag to control SageInterface::moveDeclarationToInnermostScope
// Users want to see the tool working by default
extern bool tool_keep_going;
// Users want to see conservative and aggressive moving
extern bool decl_mover_conservative;

//! Move a declaration to a scope which is the closest to the declaration's use places. It may generate new declarations to be considered later on so worklist is used.
bool moveDeclarationToInnermostScope(SgDeclarationStatement* decl, std::queue<SgVariableDeclaration *> &worklist, bool debug/*= false */);

//By default ASSERT should block the execution to find issues.
//But some users want to keep the tool going even when some assertion fails. 
//This is only supported by SageInterface::moveDeclarationToInnermostScope() and associated functions for now
bool tool_keep_going = false;

// Like any other compiler-based tools. We do things conservatively by default.
// Declarations with initializers will not be moved
// if it is set to false.  Declaration with initializers will be moved, 
// sends out warning if it crosses a loop boundaries in between. 
bool decl_mover_conservative = true;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    void virtual visit (SgNode* n)
    {
      if (isSgFunctionDeclaration(n)!=NULL){
        SgFunctionDeclaration* func = isSgFunctionDeclaration(n);  
        ROSE_ASSERT(func != NULL);
        if (func->get_definition() == NULL) return;

        // TODO: skip things from headers. 
        // skip compiler generated codes, mostly from template headers
        if (func->get_file_info()->isCompilerGenerated())
        {
          return;
        }

        SgBasicBlock* body = func->get_definition()->get_body();
        if (body == NULL) return; 
        // Prepare the function body: ensure body basic block for while, for, if, etc.
         SageInterface::changeAllBodiesToBlocks (body, false);

        Rose_STL_Container<SgNode*> var_decls= NodeQuery::querySubTree(body,V_SgVariableDeclaration);
        if (debug )
          cout<<"Number of declarations to be considered = "<<var_decls.size()<<endl;

        std::queue<SgVariableDeclaration* > worklist;

        for (size_t i=0; i< var_decls.size(); i++)
        {
          SgVariableDeclaration* decl = isSgVariableDeclaration(var_decls[i]);
          ROSE_ASSERT(decl!= NULL);
          worklist.push(decl);
        }

       // using a worklist instead of a fixed vector, since we will iteratively consider declarations added in the process.
       // e.g. we insert a declaration into the true/false bodies of if statement if a target scope is a if statement.
       // These two inserted declarations will be further considered.
         while (!worklist.empty())
        {    
          SgVariableDeclaration* decl = isSgVariableDeclaration(worklist.front());
          ROSE_ASSERT(decl!= NULL);
          worklist.pop();

          bool result=false;
          if (SageInterface::isStatic(decl))
          {
            if (debug)
              cout<<"skipping a static variable declaration .."<<endl;
          }
          else
          {
            bool null_or_literal_initializer = false;
            SgInitializedName* init_name = SageInterface::getFirstInitializedName (decl);
            ROSE_ASSERT (init_name!= NULL);
            SgInitializer * initor =  init_name->get_initptr();
            if (initor == NULL) 
              null_or_literal_initializer = true;
            else
             {
               SgAssignInitializer* assign_initor = isSgAssignInitializer (initor);
               if (assign_initor != NULL)
               {
                 if (isSgValueExp(assign_initor->get_operand()))
                   null_or_literal_initializer = true;
               }
             } 

            // conservative mode:  Only move declarations with no or value(literal) initializer
            if (decl_mover_conservative)
            {
              if (debug)
                 cout<<"Using conservative moving for decl .."<<endl;
              if (null_or_literal_initializer)   
                result = moveDeclarationToInnermostScope(decl, worklist, debug);
              else
              {
                if (debug)
                  cout<<"Skipping a declaration since it has complex initializer .."<<endl;
              }
            }
            else // aggressive move
            {

              if (debug)
                 cout<<"Using aggressive moving for decl .."<<endl;
              result = moveDeclarationToInnermostScope(decl, worklist, debug);
            }
          }
        }
      } // end if
    } // end visit()
};

int main(int argc, char * argv[])

{
  vector <string> argvList (argv, argv + argc);
  // pass -rose:debug to turn on debugging mode
  if (CommandlineProcessing::isOption (argvList,"-rose:debug","",true))
  {
    debug = true;
    cout<<"Turing on debugging model..."<<endl;
  }
  // We don't remove this option since it is used later by other logic
  if (CommandlineProcessing::isOption (argvList,"-rose:keep_going","",false))
  {
    tool_keep_going = true;
    cout<<"Turing on the keep going model, ignore assertions as much as possible..."<<endl;
  }

  // ROSE base does not use this option. remove it after use.
  if (CommandlineProcessing::isOption (argvList,"-rose:aggressive","",true)) 
  {
    decl_mover_conservative = false;
    cout<<"Turing on the aggressive model, allowing moving declarations with initializers and cross loop boundaries, but will send out warnings..."<<endl;
  }


  SgProject *project = frontend (argvList);

  SgFilePtrList file_ptr_list = project->get_fileList();
  visitorTraversal exampleTraversal;
  for (size_t i = 0; i<file_ptr_list.size(); i++)
  {
    SgFile* cur_file = file_ptr_list[i];
    SgSourceFile* s_file = isSgSourceFile(cur_file);
    if (s_file != NULL)
    {
      //exampleTraversal.traverseInputFiles(project,preorder);
       exampleTraversal.traverseWithinFile(s_file, preorder);
    }
  }
 // run all tests
  AstTests::runAllTests(project);
  return backend(project);
}



//! Move a declaration to a scope which is the closest to the declaration's use places
/*  On the request from Jeff Keasler, we provide this transformation:
 *  For a declaration, find the innermost scope we can move it into, without breaking the code's original semantics
 *  For a single use place, move to the innermost scope.
 *  For the case of multiple uses, we may need to duplicate the declarations and move to two scopes if there is no variable reuse in between, 
 *    otherwise, we move the declaration into the innermost common scope of the multiple uses. 
 *  
 *  Data structure: we maintain a scope tree, in which each node is 
 *     a scope 1) defining or 2) use the variable, or 3) is live in between .
 *  Several implementation choices for storing the tree
 *  1) The scope tree reuses the AST. AST attribute is used to store extra information  
 *  2) A dedicated scope tree independent from AST. 
 *  3) Storing individual scope chains (paths) in the tree
 *     hard to maintain consistency if we trim paths, hard to debug.
 *
 *  For efficiency, we save only a single scope node  if there are multiple uses in the same scope. 
 *  Also for two use scopes with enclosing relationship, we only store the outer scope in the scope tree and trim the rest. 
 *
 *  Algorithm:
 *    Save the scope of the declaration int DS
 *    Step 1: create a scope tree first, with trimming 
 *    Pre-order traversal to find all references to the declaration
 *    For each reference place
 *    {
 *       back track all its scopes until we reach DS
 *       if DS == US, nothing can be done to move the declaration, stop. 
 *       all scopes tracked are formed into a scope chain (a vector: US .. DS).
 *
 *       create a scope node of use type, called US.
 *       For all scopes in between, create nodes named intermediate scope or IS. 
 *       Add Ds-IS1-IS2..-US (reverse order of the scope chain) into the scope tree, consider consolidating overlapped scope chains;
 *          1) if an intermediate scope is equal to a use scope of another scope chain; stop add the rest of this chain. 
 *          2) if we are adding a use scope into the scope tree, but the same scope is already added by a intermediate scope from anther chain
 *             we mark the  existing scope node as a use scope, and remove its children from the scope tree.
 *    }
 *
 *    Step 2: find the scopes to move the declaration into
 *    find the innermost scope containing all paths to leaves: innerscope: single parent, multiple children
 *    count the number of children of innerscope: 
 *      if this is only one leaf: move the declaration to the innermost scope
 *      if there are two scopes: 
 *          not liveout for the variable in question?  duplicate the declaration and move to each scope chain. 
 *          if yes liveout in between two scopes.  no duplication, move the declaration to innerscope
 *
 *  Iterative moving process: 
 *
 *   For duplicated declarations inserted into inner scopes, we have to consider if they can be moved further.
 *     So we use a worklist to store all variable declarations to be considered.
 *     The worklist is initialized with original declarations in the code.
 *     New declarations are added into the worklist during the duplication/insertion process.
 *     The entire process terminate when the worklist becomes empty.      
 *
 *  //TODO optimize efficiency for multiple declarations
 * //TODO move to a separated source file or even namespace
 * By Liao, 9/3/2014
*/

enum ScopeType {s_decl, s_intermediate, s_use};
class Scope_Node {
  public: 
    Scope_Node (SgScopeStatement* s, ScopeType t):scope(s), s_type(t) {};
    //TODO: protect members behind access functions
    // scope information
    SgScopeStatement* scope; 
    ScopeType s_type;
    int depth; // the depth in the tree (path or chain) starting from 0

    // for tree information
    Scope_Node* parent; // point to the parent scope in the chain
    std::vector < Scope_Node* > children ; // point to children scopes 
  
    //print the subtree rooted at this node to a dot file named as filename.dot
    void printToDot(std::string filename);
    std::string prettyPrint();
    //! Depth-first traverse to get all nodes within the current subtree
    void traverse_node(std::vector<Scope_Node* >& allnodes);
    //! Delete all children, grandchildren, etc.
    void deep_delete_children();
    //! Starting from the root, find the first node which has more than one children.
    // This is useful to identify the innermost common scope of all leaf scopes.
    Scope_Node * findFirstBranchNode();
    //~Scope_Node () {  deep_delete_children(); }

   private: 
      //! Recursive traverse the current subtree and write dot file information
      void traverse_write (Scope_Node* n, std::ofstream & dotfile);
      std::string getLineNumberStr(){ int lineno = scope->get_file_info()->get_line(); return StringUtility::numberToString(lineno); } ;
      // Dot graph Node Id: unique memory address, prepend with "n_".
      std::string getDotNodeId() { return "n_"+StringUtility::numberToString(scope); };
      std::string getScopeTypeStr() 
      { string rt;  
        if (s_type == s_decl) rt = "s_decl"; 
        else if (s_type == s_intermediate) rt = "s_intermediate";
        else if (s_type == s_use) rt = "s_use";
        else ROSE_ASSERT (false);
        return rt; }
  };

// Topdown traverse a tree to find the first node with multiple children
// Intuitively, the innermost common scope for a variable.
// However, we have to adjust a few special cases: 
// For example: if-stmt case
//  A variable is used in both true and false body.
 // Naive analysis will find if-stmt is the inner-most common scope.
//  But we cannot really move a declaration to the if-stmt scope (condition)
Scope_Node*  Scope_Node::findFirstBranchNode()
{
  Scope_Node* first_branch_node = this; 
  // three cases: 0 children, 1 children, >1 children
  while (first_branch_node->children.size()==1)
    first_branch_node = first_branch_node->children[0];

#if 0 // this adjustment should not be done until we figure out if the variable can be
      // moved downward into the two branch scopes or not.
      // We only adjust if we cannot move downward further,but trying to move var decl to if-stmt.
      // With this consideration, we do this adjust later after considering liveness between mutliple scopes.
      //
  // Adjust for if-stmt: special adjustment
  // switch stmt needs not to be adjusted since there is a middle scope as the innermost scope
  // if a variable is used in multiple case: scopes. 
   if (isSgIfStmt (first_branch_node->scope))
      first_branch_node = first_branch_node->parent;
#endif
  // now the node must has either 0 or >1 children.
  return first_branch_node; 
}
  
//TODO make this a member of Scope_Node.  
std::map <SgScopeStatement* , Scope_Node*>  ScopeTreeMap; // quick query if a scope is in the scope tree, temp variable shared by all trees.
void Scope_Node::deep_delete_children()
{
  std::vector <Scope_Node*> allnodes;
  // TODO better way is to use width-first traversal and delete from the bottom to the top
  this->traverse_node (allnodes);
  // allnodes[0] is the root node itself, we keep it.
  // reverse order of preorder to delete things?
  //for (size_t i =1; i<allnodes.size(); i++) 
  for (size_t i =allnodes.size()-1 ; i>0; i--) 
  {
    Scope_Node* child = allnodes[i];
    // mark the associated scope as not in the scope tree
    ScopeTreeMap[child->scope] = NULL;
    // TODO: currently a workaround for multiple delete due to recursive call issue. 
    if (child != NULL)
    {
      delete child;
      allnodes[i] = NULL;
    }
  }
  // no children for the current node
  children.clear(); 
}

void Scope_Node::traverse_node(std::vector<Scope_Node* >& allnodes)
{
  // action on current node, save to the vector
  ROSE_ASSERT (this != NULL); // we should not push NULL pointers
  allnodes.push_back(this);
  std::vector < Scope_Node* > children = this->children;
  for (size_t i=0; i<children.size(); i++) 
  {
     Scope_Node* child = children[i];
     if (child != NULL) // we may delete child nodes and have NULL pointers
       child->traverse_node(allnodes);
  }
}

std::string Scope_Node::prettyPrint()
{
  int lineno = scope->get_file_info()->get_line();
  return StringUtility::numberToString(scope)+"@"+StringUtility::numberToString(lineno);
}

void Scope_Node::printToDot (std::string filename)
{
  string full_filename = filename + ".dot";
  ofstream dotfile (full_filename.c_str());
  dotfile <<"digraph scopetree{"<<endl;
  traverse_write (this, dotfile);
  dotfile <<"}"<<endl;
}

void Scope_Node::traverse_write(Scope_Node* n,std::ofstream & dotfile)
{
  std::vector < Scope_Node* > children = n->children;
  // must output both node and edge lines: here is the node
  dotfile<<n->getDotNodeId()<<"[label=\""<< StringUtility::numberToString(n->scope)
         <<"\n"<<n->scope->class_name()
         <<"\nLine="<<n->getLineNumberStr()
         <<" Depth="<<StringUtility::numberToString(n->depth)
         <<"\nType="<<n->getScopeTypeStr()<<"\"];"<<endl;
  for (size_t i=0; i<children.size(); i++)
  {
    // Here is the edge
    dotfile<<n->getDotNodeId()<<"->"<<children[i]->getDotNodeId()<<";"<<endl;
    traverse_write (children[i], dotfile);
  }
}
 
 
//! A helper function to skip some scopes, such as while stmt scope: special adjustment.
// used for a variable showing up in condition expression of some statement. 
// We return a grand parent scope for those variables. 
/*
 *We don't try to merge a variable decl into the conditional of a while statement
 * The reason is that often the declaration has an initializer , which must be preserved.
 * The conditional contain the use of the declared variable, which usually cannot be merged with the declaration.
 * e.g.  int yy = 10; while (yy< 100) { yy ++; ... }
 * Another tricky thing is even though yy shows up in both condition and while-body, 
 * the scope tree will only show while-stmt as the single scope node since the body scope is shadowed. 
 * Note: the scope of the condition is considered to be the while-stmt.
 * TODO in a rare case like   int yy = 10; while (yy) { } we can merge the declaration and condition together.
 * */
static SgScopeStatement * getAdjustedScope(SgNode* n)
{
  ROSE_ASSERT (n!= NULL); 
  SgScopeStatement* result =  SageInterface::getScope (n);
  if (isSgWhileStmt (result) || isSgIfStmt (result) || isSgDoWhileStmt (result) || isSgSwitchStatement(result) )
    result = SageInterface::getEnclosingScope(result, false);

  // TODO: can recursive while-stmt scope happen?
  ROSE_ASSERT  (isSgWhileStmt (result) == NULL);
  ROSE_ASSERT  (isSgIfStmt (result) == NULL);
  return result; 
}
//! Generate a scope tree for a declaration: the tree is trimmed. 
//  To trim the tree , the inner scopes using the variable are removed if there is a use scope which enclosing the inner scopes. 
//  Return the tree, can not be a NULL pointer. At least we return a node for the scope of the declaration
//  Also return the leaf scopes represented by the tree
//  TODO: make it a SageInterface function ?
Scope_Node* generateScopeTree(SgDeclarationStatement* decl, bool debug = false)//std::map <SgScopeStatement*, Scope_Node*>& ScopeTreeMap)
{
  ScopeTreeMap.clear();
//  bool debug = true; // debugging flag
  ROSE_ASSERT (decl != NULL);
  SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl);
  ROSE_ASSERT (var_decl != NULL);
  SgScopeStatement * decl_scope = decl->get_scope();
  ROSE_ASSERT (decl_scope != NULL);

  SgVariableSymbol* var_sym = isSgVariableSymbol(SageInterface::getFirstVarSym(var_decl));
  ROSE_ASSERT (var_sym != NULL);

  // the root of the scope tree
  Scope_Node* scope_tree =new Scope_Node (decl_scope, s_decl);
  scope_tree->depth = 0;
  scope_tree->parent= NULL;
  ScopeTreeMap[decl_scope] = scope_tree; 

  // Step 1. Find all variable references to the declared variable.
  // Note: querySubTree uses pre-order traversal. 
  // This is essential since we want to preserve the order of the scopes within a parent scope.
  // The order of found variable references reflects the order of their scopes in AST. 
  //TODO: optimize for multiple declarations, avoid redundant query
  //   We can batch-generate scope trees for all declarations within a function
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(decl_scope, V_SgVarRefExp);
  std::vector  <SgVarRefExp*> var_refs; 
  bool usedInSameScope = false; // if the declared variable is also used within the same scope
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp(*i);
    if (vRef->get_symbol() == var_sym )
    {
      //if (SageInterface::getScope(vRef) == decl_scope) 
      if (getAdjustedScope(vRef) == decl_scope) 
      {
        usedInSameScope = true; 
        break;
      } // same scope
      var_refs.push_back(vRef);
    } // match symbol
  }
  // If there is a single variable use which happens within the declaration scope,
  // we immediately return.
  if (usedInSameScope) 
  { 
    if (debug)
      cout<<"Found a declaration with a varRefExp in the same scope, return a single node scope tree."<<endl;
    return scope_tree; 
  }

  // Step 2: For each reference, generate a scope chain, stored in a temp stack. 
  // Each scope can be either where the variable is declared, used, or a scope in between (intermediate)
  /*
   *    For each reference place
   *    {
   *       back track all its scopes until we reach DS
   *       if DS == US, nothing can be done to move the declaration, stop. 
   *       all scopes tracked are formed into a scope chain (a vector: US .. DS).
   *
   *       create a scope node of use type, called US.
   *       For all scopes in between, create nodes named intermediate scope or IS. 
   *       Add Ds-IS1-IS2..-US (reverse order of the scope chain) into the scope tree, consider consolidating overlapped scope chains;
   *          1) if an intermediate scope is equal to a use scope of another scope chain; stop add the rest of this chain. 
   *          2) if we are adding a use scope into the scope tree, but the same scope is already added by a intermediate scope from anther chain
   *             we mark the  existing scope node as a use scope, and remove its children from the scope tree.
   *             This is hard to do if we store paths explicitly (we have to change all other paths containing the scope being added) 
   *    }
   */
  // There may be multiple variable references within a same scope, 
  // we use a set to record processed scope in order to avoid repetitively considering the same use scopes.
  std::set<SgScopeStatement*> processedUseScopes;
  // For each variable reference, we backtrack its scopes, and add the scope chain into the scope tree of this variable.
  for (size_t i =0; i< var_refs.size(); i++)
  {
    std::stack <SgScopeStatement*> temp_scope_stack;
    SgVarRefExp *vRef = var_refs[i];
    //SgScopeStatement* var_scope = SageInterface::getScope (vRef);
    SgScopeStatement* var_scope = getAdjustedScope(vRef);
    SgScopeStatement * current_scope = var_scope;
    ROSE_ASSERT (current_scope != decl_scope); // we should have excluded this situation already
    temp_scope_stack.push (current_scope) ;  // push the very first bottom scope
    do {
      // this won't work since getScope () will return the input scope as it is!!
      //current_scope = getScope (current_scope);
      current_scope = current_scope->get_scope(); // get parent scope and push
      temp_scope_stack.push (current_scope) ; 
    }
    while (current_scope != decl_scope) ;
    // exit condition is current_scope == decl_scope, as a result  
    // at this point , the declaration scope is already pushed into the stack  

    if (debug)
    {
      cout<<"scope stack size="<<temp_scope_stack.size()<<endl;
    }

    // Step 3: add nodes into the scope tree from the stack.
    //if the current use scope is not yet considered
    // add nodes into the scope tree, avoid duplicated add the scope node containing multiple var references.
    if (processedUseScopes.find(var_scope) == processedUseScopes.end())
    {
      // add each scope into the scope tree
      Scope_Node* current_parent = scope_tree;
      int depth_counter = 0;
      while (!temp_scope_stack.empty())
      { // TODO: verify that the scope tree preserves the original order of children scopes.
        current_scope = temp_scope_stack.top();
        Scope_Node * current_node = NULL; 
        ScopeType s_t; 
        if (current_scope == var_scope) 
          s_t = s_use;
        else 
          s_t = s_intermediate; 
        // avoid add duplicated node into the tree, the first one, root node, is duplicate.
        if (ScopeTreeMap[current_scope] == NULL )
        {
          current_node = new Scope_Node(current_scope, s_t);
          (current_parent->children).push_back(current_node);
          current_node->parent = current_parent;
          current_node->depth = depth_counter;

          ScopeTreeMap[current_scope] = current_node;
        }
        else
        {
          //TODO: optimize 3: we only push scopes which are not yet in the scope tree in to the temp_scope_stack for a new variable use!!
          // This will save overhead of pushing duplicated scopes into stack and later adding them into the scope tree!!
          // Downside: need to calculate depth separately, not using the stack depth. 
          //
          //handle possible overlapped paths, we only want to store the shortest path reaching a s_use node.
          // Further path to deeper use of a variable does not need to be considered. 
          // This is called trimmed path or trimmed tree. 
          //
          // Three conditions
          //1. Reuse the existing scope node, continue next scope in the stack
          //  e.g. the root node and the intermediate nodes shared by multiple scope chains
          // no special handling at all here. 

          //2. Reuse the existing scope node, stop going further
          //  e.g. the existing scope node is already a leaf node (s_use type)
          //  There is no need to go deeper for the current scope chain.
          Scope_Node* existing_node = ScopeTreeMap[current_scope]; 
          if (existing_node->s_type == s_use)
          {
            break; // jump out the while loop for the stack. 
          }  
          //3. Modify the existing scope node's type to be decl, stop g
          //  e.g We are reaching the leaf node of the current scope chain (s_use type)
          //  The existing scope node is s_intermediate. 
          //  we should delete the existing subtree and add the new leaf node
          //  To simplify the implementation, we change type of the existing node to be s_use type and delete all its children nodes. 
          else if (s_t == s_use) 
          {
            ROSE_ASSERT (existing_node->s_type == s_intermediate);
            existing_node->s_type = s_use; 
            existing_node->deep_delete_children(); 
          }  
        } 
        temp_scope_stack.pop();
        // must use the one in the tree, not necessary current_node from the stack
        current_parent = ScopeTreeMap[current_scope]; 
        //         if (current_node != ScopeTreeMap[current_scope])
        //            delete current_node; // delete redundant Scope Node.
        depth_counter++;
      } // end while pop scope stack  

      // mark the current leaf scope as processed.
      processedUseScopes.insert (var_scope);
    } // end if not processed var scope  
  } // end of adding a scope stack into the  scope tree

  //------------- debug the scope tree---------
  if (debug)
  {
    SgFunctionDeclaration* func_decl = SageInterface::getEnclosingFunctionDeclaration (decl);
    std::string func_name = (func_decl->get_name()).getString();
    int decl_lineno = decl->get_file_info()->get_line();
    std::string filename = "scope_tree_"+func_name+"_"+(var_sym->get_name()).getString()+ "_at_"+ StringUtility::numberToString(decl_lineno);
    // report the number of nodes in the scope tree
    std::vector <Scope_Node*> allnodes; 
    scope_tree->traverse_node (allnodes);
    cout<<"Scope tree node count:"<<allnodes.size()<<endl;
    // write the tree into a dot file
    scope_tree->printToDot(filename);
  }
  return scope_tree; 
}  



#if 0
static bool hasLoopInBetween (SgScopeStatement* top_scope, SgScopeStatement* bottom_scope)
{
  bool rt = false;
  ROSE_ASSERT (top_scope && bottom_scope);
  if (SageInterface::isAncestor (top_scope, bottom_scope))
  {
    do {
      bottom_scope = bottom_scope->get_scope(); // current bottom does not cout, we may move to the loop header
      if (isSgForStatement(bottom_scope)||isSgDoWhileStmt(bottom_scope) || isSgWhileStmt(bottom_scope))
      {
        rt = true;
//        if (debug)
        {
          cout<<"Found a loop boundary at line "<< bottom_scope->get_file_info()->get_line()<<endl;
//          cout<<"The declaration in question has the following file info:"<<endl;
        }
//        decl->get_file_info()->display();
      }
    } while (top_scope!=bottom_scope);
  }
  else
  {
    if (!SageInterface::tool_keep_going)
    {
      cerr<<"Error. declaration scope is not an ancestor scope of the target scope"<<endl;
      ROSE_ASSERT (false);
    }
  }
  return rt;
}

#endif

// A helper function to process target scopes
// if a target scope is a if statement, we grab the true and false body scopes as the new target scopes and remove the if-stmt scope from the original scope vector.
std::vector <SgScopeStatement*> processTargetScopes(std::vector <SgScopeStatement*> scopes)
{
  std::vector <SgScopeStatement*> processed_scopes; 
  for (size_t i = 0; i< scopes.size(); i++)
  {
    SgScopeStatement* target_scope = scopes[i];
    if (SgIfStmt* if_stmt = isSgIfStmt (target_scope))
    {
        if (if_stmt->get_true_body())     
        {
          SageInterface::ensureBasicBlockAsTrueBodyOfIf (if_stmt);
          SgScopeStatement* true_body = isSgScopeStatement(if_stmt->get_true_body());
          processed_scopes.push_back (true_body);
        }

        if (if_stmt->get_false_body())
        {
          SageInterface::ensureBasicBlockAsFalseBodyOfIf (if_stmt);
          SgScopeStatement* false_body = isSgScopeStatement(if_stmt->get_false_body());
          processed_scopes.push_back (false_body);
        }
    }
    else
    {
      processed_scopes.push_back(target_scope);
    }
  }
  return processed_scopes;
}

// Move a single declaration into multiple scopes. 
// For each target scope:
// 1. Copy the decl to be local decl , 
// 2. Insert into the target_scopes, 
// 3. Replace variable references to the new copies
// Finally, erase the original decl 
//
// if the target scope is a For loop && the variable is index variable,  merge the decl to be for( int i=.., ...).
void copyMoveVariableDeclaration(SgVariableDeclaration* decl, std::vector <SgScopeStatement*> scopes, std::queue<SgVariableDeclaration*> &worklist)
{
  ROSE_ASSERT (decl!= NULL);
  ROSE_ASSERT (scopes.size() != 0);
  SgInitializedName* i_name = SageInterface::getFirstInitializedName (decl);
  ROSE_ASSERT (i_name != NULL);
  SgVariableSymbol * sym = SageInterface::getFirstVarSym(decl);
  ROSE_ASSERT (sym != NULL);
  SgScopeStatement* orig_scope = sym->get_scope();

  // when we adjust first branch node  (if-stmt with both true and false body )in the scope tree, we may backtrack to the decl's scope 
  // We don't move anything in this case.
  if ((scopes.size()==1) && (scopes[0] == decl->get_scope()))
  {
     return;
  }

  scopes = processTargetScopes(scopes);

#if 0 //TODO: this is tricky, we have to modify the scope tree to backtrack this. 
  // For aggressive mode, skip moving if the move will cross some boundaries
  for (size_t i = 0; i< scopes.size(); i++)
  {
    SgScopeStatement* target_scope = scopes[i]; 
    ROSE_ASSERT (target_scope != decl->get_scope());
  } 
#endif

  for (size_t i = 0; i< scopes.size(); i++)
  {
    SgScopeStatement* target_scope = scopes[i]; 
    ROSE_ASSERT (target_scope != decl->get_scope());

    SgScopeStatement* adjusted_scope = target_scope; 
    SgVariableDeclaration * decl_copy = SageInterface::deepCopy(decl);

    //bool skip = false; // in some rare case, we skip a target scope, no move to that scope (like while-stmt)
    //This won't work. The move must happen to all scopes or not at all, or dangling variable use without a declaration.
    //We must skip scopes when generating scope tree, not wait until now.

    switch (target_scope->variantT())
    {
      case V_SgBasicBlock:
        {
          SageInterface::prependStatement (decl_copy, adjusted_scope);
          break;
        }
      case V_SgForStatement:
        {
          SgForStatement* stmt = isSgForStatement (target_scope);
          ROSE_ASSERT (stmt != NULL);
          // target scope is a for loop and the declaration declares its index variable.
          if (i_name == SageInterface::getLoopIndexVariable (stmt))
          {
            if (i_name == NULL)
            {
              cerr<<"Warning: in moveVariableDeclaration(): target_scope is a for loop with unrecognized index variable. Skipping it ..."<<endl;
              break;
            }
            // we move int i; to be for (int i=0; ...);
            SgStatementPtrList& stmt_list = stmt->get_init_stmt();
            // Try to match a pattern like for (i=0; ...) here
            // assuming there is only one assignment like i=0
            // We don't yet handle more complex cases
            if (stmt_list.size() !=1)
            {
              // TODO, how to handle two variable declaration?  I think this cannot happen in this context
              // find the inner most used scope of variable a.  It cannot be declaration of a. 
              cerr<<"Error in moveVariableDeclaration(): only single init statement is handled for SgForStatement now."<<endl;
              ROSE_ASSERT (stmt_list.size() ==1);
            }
            SgExprStatement* exp_stmt = isSgExprStatement(stmt_list[0]);
            ROSE_ASSERT (exp_stmt != NULL);
            SgAssignOp* assign_op = isSgAssignOp(exp_stmt->get_expression());
            if (assign_op != NULL)
            {
              ROSE_ASSERT (assign_op != NULL);
              stmt_list.clear();

              SageInterface::mergeDeclarationAndAssignment (decl_copy, exp_stmt);
              // insert the merged decl into the list, TODO preserve the order in the list
              stmt_list.insert (stmt_list.begin(),  decl_copy);
              decl_copy->set_parent(stmt->get_for_init_stmt());
              ROSE_ASSERT (decl_copy->get_parent() != NULL); 
            }
            // TODO: it can be SgCommanOpExp
            if (isSgCommaOpExp (exp_stmt->get_expression()) )
            {
               cerr<<"Error in moveVariableDeclaration(), multiple expressions in for-condition is not supported now. "<<endl;
               if (tool_keep_going) 
                 break;
               else  
                 ROSE_ASSERT (assign_op != NULL);
            } 
         } //
          else 
          {
            SgBasicBlock* loop_body = SageInterface::ensureBasicBlockAsBodyOfFor (stmt);
            adjusted_scope = loop_body;
            SageInterface::prependStatement (decl_copy, adjusted_scope);
          }
          break;
        }
      // we duplicate and insert the declaration into true (and false) body, if the body exists
      // The two duplicated declarations are added into the  work list to be processed later on
      case V_SgIfStmt: 
        {
          cerr<<"if statement @ line"<< target_scope->get_file_info()->get_line()<< " should not show up in target scope vector after processTargetScopes()"<<endl;
          ROSE_ASSERT (false);
          break;
        }
      /*
       *We don't try to merge a variable decl into the conditional of a while statement
       * The reason is that often the declaration has an initializer , which must be preserved.
       * The conditional contain the use of the declared variable, which cannot be merged with the declaration.
       * e.g.  int yy = 10; while (yy< 100) { yy ++; ... }
       * Another tricky thing is even though yy shows up in both condition and while-body, 
       * the scope tree will only show while-stmt as the single scope node since the body scope is shadowed. 
       * Note: the scope of the condition is considered to be the while-stmt.
       * */  
      case V_SgWhileStmt: 
          cerr<<"while statement @ line"<< target_scope->get_file_info()->get_line()<< " should not show up in scope tree"<<endl;
          ROSE_ASSERT (false);
          break;
      default:
        {
          cerr<<"Error. Unhandled target scope type:"<<target_scope->class_name()
            << " when moving decl at:"<< decl->get_file_info()->get_line()
            << " for variable "<< sym->get_name()
            << " to scope at:"<< target_scope->get_file_info()->get_line()<<endl;
          ROSE_ASSERT  (false);
        }
    }
#if 1 
    // check what is exactly copied:
    // Symbol is not copies. It is shared instead
    SgVariableSymbol * new_sym = SageInterface::getFirstVarSym (decl_copy);

    // init name is copied, but its scope is not changed!
    // but the symbol cannot be find by calling init_name->get_symbol_from_symbol_table ()
    SgInitializedName* init_name_copy = SageInterface::getFirstInitializedName (decl_copy);
    init_name_copy->set_scope (adjusted_scope);

    //ROSE_ASSERT (false);
    if (orig_scope != adjusted_scope)
    {
      // SageInterface::fixVariableDeclaration() cannot switch the scope for init name.
      // it somehow always reuses previously associated scope.
      ROSE_ASSERT (i_name != init_name_copy);
      // we have to manually copy the symbol and insert it
      SgName sname = sym->get_name();
      adjusted_scope->insert_symbol(sname, new SgVariableSymbol (init_name_copy));
    }
    new_sym = SageInterface::getFirstVarSym (decl_copy);
    ROSE_ASSERT (sym!=new_sym);
    // This is difficult since C++ variables have namespaces
    // Details are in SageInterface::fixVariableDeclaration()
    ROSE_ASSERT (adjusted_scope->symbol_exists(new_sym));
#endif     
   

#if 1 
    // replace variable references
    SageInterface::replaceVariableReferences  (sym, new_sym, adjusted_scope);
#endif 

// No need to check how the target scopes are identified.
// I treat them all the same. All newly inserted variables will be added into the worklist for further consideration!
    // add declarations into the worklist if the target scope is a marked true/false body of a if-stmt.
    // Note: not all bodies should be added. Only consider the marked scopes!!
//   if (todo_scopes.find(target_scope) != todo_scopes.end()) 
   {
     worklist.push(decl_copy);
   }

   //SageInterface::setSourcePositionForTransformation (decl_copy);
#if 1
// send out warning info if there is a for loop between declaration's scope and the target scope
// a declaration is moved across the loop boundary.
   SgScopeStatement* top_scope = orig_scope;
   SgScopeStatement* bottom_scope = target_scope;
   if (SageInterface::isAncestor (top_scope, bottom_scope))
   {
     do {
       bottom_scope = bottom_scope->get_scope();
       if (isSgForStatement(bottom_scope)||isSgDoWhileStmt(bottom_scope) || isSgWhileStmt(bottom_scope))
       {
         cout<<"Warning: aggressive declaration moving across a loop boundary at line "<< bottom_scope->get_file_info()->get_line()<<endl;
         cout<<"The declaration in question has the following file info:"<<endl;
         decl->get_file_info()->display();
       }

     } while (top_scope!=bottom_scope);
   }
   else
   {
     if (!tool_keep_going)
     {
       cerr<<"Error. declaration scope is not an ancestor scope of the target scope"<<endl;
       ROSE_ASSERT (false);
     }
   }
#endif

  } //end for all scopes

  // remove the original declaration , must use false to turn of auto-relocate comments
  // TODO: investigate why default behavior adds two extra comments in the end of the code
  SageInterface::removeStatement(decl, false);
  //TODO deepDelete is problematic
  //SageInterface::deepDelete(decl);  // symbol is not deleted?
  //orig_scope->remove_symbol(sym);
  //delete i_name;
}

//! Check if a variable (symbol) is live in for a scope
// TODO A real solution is to check liveness analysis result here. However, the analysis is not ready for production use.
// The current workaround is to use a syntactic analysis: 
//
//    The opposite of dead:  !dead, not first defined
//    check if the first reference (in a pre-order traversal) to the variable is a definition (write access) only, exclude a reference which is both read and write, like a+=1; 
//    first defined means the variable is the lhs of a= rhs_exp; and it does not show up on rhs_exp; 
static bool isLiveIn(SgVariableSymbol* var_sym, SgScopeStatement* scope)
{
  SgVarRefExp * first_ref = NULL; 
  Rose_STL_Container <SgNode*> testList = NodeQuery::querySubTree (scope, V_SgVarRefExp);
  for (size_t i=0; i< testList.size(); i++)
  {
    SgVarRefExp * current_exp = isSgVarRefExp (testList[i]);
    ROSE_ASSERT (current_exp != NULL);
    if (current_exp->get_symbol() == var_sym)
    {
      first_ref = current_exp; 
      break;
    }
  } // end for search

  // No reference at all?  Not liveIn
  if (first_ref == NULL) return false; 

  // Now go through common cases for finding definitions. 
  // 1. We don't care about declarations since the variables will be different from var_sym!
  // 2. For unary operations: ++ or --,  they must read the variables first then change the values
  // 3. Binary operations: only SgAssignOp does not read lhs oprand (define only )
  // 4. Function call parameters: addressOf(a): cannot move, is live

  // if a = rhs_exp;  and a does not show up in rhs_exp; 
  // then it is not live in (return false).
// TODO : handle casting
// TODO: handle rare case like (t=i)=8; 
  SgNode* parent = first_ref ->get_parent();
  ROSE_ASSERT (parent != NULL);
  if ( (isSgAssignOp(parent) && isSgAssignOp(parent)->get_lhs_operand()==first_ref) )
  {
    // check if it shows up on the rhs
    bool onRhs = false; 
    Rose_STL_Container <SgNode*> rhs_vars = NodeQuery::querySubTree (isSgAssignOp(parent)->get_rhs_operand(), V_SgVarRefExp);
    for (size_t i=0; i< rhs_vars.size(); i++)
    {
      SgVarRefExp * current_var = isSgVarRefExp (rhs_vars[i]);
      ROSE_ASSERT (current_var != NULL);
      if (current_var->get_symbol() == var_sym)
      {
        onRhs = true;
        break;
      }
    } // end for search
    if (!onRhs ) return false; 
  }

  //All other cases, we conservatively assume the variable is live in for the scope
  return true; 
}

//! A helper function to check if there is a target scope which is a for loop with complex init_stmt list
static SgForStatement* hasALoopWithComplexInitStmt( std::vector <SgScopeStatement *> &target_scopes)
{

   for (size_t i= 0; i< target_scopes.size(); i++)
   {
     SgScopeStatement* current_scope = target_scopes[i];
     if (SgForStatement* for_loop = isSgForStatement (current_scope))
       if (SageInterface::hasMultipleInitStatmentsOrExpressions (for_loop))
         return for_loop;
    }
  return NULL;
}


bool moveDeclarationToInnermostScope(SgDeclarationStatement* declaration, std::queue<SgVariableDeclaration*> &worklist, bool debug = false)
{
  SgVariableDeclaration * decl = isSgVariableDeclaration(declaration);
  ROSE_ASSERT (decl != NULL);
  // Step 1: generate a scope tree for the declaration
  // -----------------------------------------------------
  Scope_Node* scope_tree = generateScopeTree (decl, debug);

  // single node scope tree, nowhere to move into. 
  if ((scope_tree->children).size() == 0 )
  {
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return false; 
  }

  // for a scope tree with two or more nodes  
  Scope_Node* first_branch_node = scope_tree->findFirstBranchNode();

  // the scopes to be moved into
  std::vector <SgScopeStatement *> target_scopes; 
  // Step 2: simplest case, only a single use place
  // -----------------------------------------------------
  // the first branch node is also the bottom node
  if ((first_branch_node->children).size() ==0)
  {
    SgScopeStatement* bottom_scope = first_branch_node->scope;
    target_scopes.push_back(bottom_scope);
  } // end the single decl-use path case
  else 
  { 
    //Step 3: multiple scopes
    // -----------------------------------------------------
    // there are multiple (0 to n - 1 )child scopes in which the variable is used. 
    // if for all scope 1, 2, .., n-1
    //  the variable is defined before being used (not live)
    //  Then we can move the variable into each child scope
    // Conversely, if any of scope has liveIn () for the declared variable, we cannot move
    bool moveToMultipleScopes= true ; 

    for (size_t i =1; i< (first_branch_node->children).size(); i++)
    {
      SgVariableSymbol * var_sym = SageInterface::getFirstVarSym (decl); 
      ROSE_ASSERT (var_sym != NULL);
      SgScopeStatement * current_child_scope = (first_branch_node->children[i])->scope;
      ROSE_ASSERT (current_child_scope != NULL); 
      if (isLiveIn (var_sym, current_child_scope))
        moveToMultipleScopes = false;
    }  // end for all scopes

    if (moveToMultipleScopes)
    {
      if (debug)
        cout<<"Found a movable declaration for multiple child scopes"<<endl;
      for (size_t i =0; i< (first_branch_node->children).size(); i++)
      {
        // we try to get the bottom for each branch, not just the upper scope
        // This is good for the case like: "if () { for (i=0;..) {}}" and if-stmt's scope is a child of the first branch scope node
        // TODO: one branch may fork multiple branches. Should we move further down on each grandchildren branch?
        //       Not really, we then find the common inner most scope of that branch. just simply move decl there!
        // Another thought: A better fix: we collect all leaf nodes of the scope tree! It has nothing to do with the first branch node!
        //       this won't work. First branch node still matters. 
        Scope_Node* current_child_scope = first_branch_node->children[i];     
        Scope_Node* bottom_node = current_child_scope -> findFirstBranchNode ();
        SgScopeStatement * bottom_scope = bottom_node->scope;
        ROSE_ASSERT (bottom_scope!= NULL);
        target_scopes.push_back (bottom_scope);
      }
    }
    else // we still have to move it to the innermost common scope
    {
      SgScopeStatement* bottom_scope = first_branch_node->scope;
#if 0 // We no longer do the adjustment here. We delay the logic in the insertion logic. And generate two additional declarations to be considered later on
      // special adjustment here for if-stmt as the single bottom scope  to be inserted into the var decl.
      // we should adjust here, not any other places !!
      // TODO may have to include other special stmt like while?
      if (isSgIfStmt(bottom_scope))
        bottom_scope = SageInterface::getEnclosingScope(bottom_scope, false);
#endif 
      if (decl->get_scope() != bottom_scope)
      {
        target_scopes.push_back(bottom_scope);
      }
    } // end else
  } // end else multiple scopes

  if (target_scopes.size()>0)
  {
    // ignore complex for init stmt for now 
    SgForStatement* bad_loop = hasALoopWithComplexInitStmt (target_scopes);
    if (bad_loop != NULL)
    {
      cerr<<"Error: SageInterface::moveDeclarationToInnermostScope() gives up moving a variable decl due to a complex target loop scope"<<endl;
      cerr<<"Variable declaration in question is:"<<endl;
      decl->get_file_info()->display();
      cerr<<"Loop scope with complex init stmt is:"<<endl;
      bad_loop->get_file_info()->display();
      if (!tool_keep_going )
        ROSE_ASSERT (false);
    }
    else
      copyMoveVariableDeclaration (decl, target_scopes, worklist);
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return true;
  }
  else 
  {
    scope_tree->deep_delete_children ();
    delete scope_tree;
    return false;  
  }
}


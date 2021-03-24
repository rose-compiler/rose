// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "abstract_handle.h"
#include "roseAdapter.h"
#include <vector>

using namespace std;
//using namespace AbstractHandle;
using namespace SageInterface;

namespace AbstractHandle
{
  // A helper function to convert SageType string to its enumerate type.
  // V_SgNumVariants is the last enum value of VariantT, which means no match is found.
  static VariantT getVariantT(string type_str)
  {
    int i=0;
    string temp;
    //Assume the simplest conversion: adding 'Sg' is enough
    //Be compatible with both SgStatement and Statement
    string::size_type pos = type_str.find ("Sg",0);
    if (pos!=0)
      temp = "Sg"+type_str;
    else
      temp = type_str; 
    while (Cxx_GrammarTerminalNames[i].name!=temp && i != V_SgNumVariants)
      i++;
    return (VariantT)i;  
  }
  // test LDADD dependency
  roseNode* buildroseNode(SgNode* snode)
  {
    static std::map<SgNode*, roseNode*> sgNodeMap;
    assert (snode !=NULL);
    roseNode* result = sgNodeMap[snode];
    if (result == NULL)
    {
      // this should be the only place to call the constructor
      result = new roseNode(snode);
      assert (result != NULL);
      assert (result->getNode()!=NULL);
      sgNodeMap[snode] = result;
    }
    assert (result->getNode()!=NULL);
    return result;
  }

  // the major constructor
  roseNode::roseNode(SgNode* snode)
  {
    assert(snode != NULL);
    mNode=snode;
  }

  /* Remove 'Sg' prefix will get a construct type name for now.
   * More serious implementation will have a conversion from 
   * ROSE type names to the abstract handle construct type names.
   * */
  string roseNode::getConstructTypeName() const
  {
    char result[256];
    //ROSE_ASSERT(mNode!=NULL);
    if (mNode==NULL) return "";
    istringstream buffer(mNode->sage_class_name());
    buffer.ignore (2);
    buffer.getline(result,256);
    return string(result);
  }

  //return name for various named constructs
  // return empty string if somehow we cannot get a good name.
  // for a node without a name, we will use numbering or label as its specifier.
  string roseNode::getName() const
  {
    string result;
    ROSE_ASSERT(mNode!=NULL);
    // only declarations with symbols in ROSE have user-level names
    // need to double check this
    if (isSgFile(mNode)) 
    {
      return isSgFile(mNode)->get_file_info()->get_filenameString ();
    } 
    else  if (isSgProject(mNode))
    { // No name field for rose projects
      return "";
    }
    else if (isSgFunctionDefinition(mNode))
    {
      SgFunctionDefinition* def = isSgFunctionDefinition(mNode);
      return (def->get_declaration()->search_for_symbol_from_symbol_table()->get_name()).getString();
    }  

    SgDeclarationStatement* decl = isSgDeclarationStatement(mNode); 
    if (decl)
    {
      switch (decl->variantT())
      { 
        case V_SgVariableDeclaration:
          {
            SgVariableSymbol * symbol=SageInterface::getFirstVarSym(isSgVariableDeclaration(decl));
            result = symbol->get_name();
            break;
          }
        case V_SgClassDeclaration:
        case V_SgTypedefDeclaration:
        case V_SgNamespaceDeclarationStatement:
        case V_SgFunctionDeclaration:
        case V_SgTemplateDeclaration:
        case V_SgMemberFunctionDeclaration:
        case V_SgTemplateMemberFunctionDeclaration:
          {
            result = (decl->search_for_symbol_from_symbol_table()->get_name()).getString();
            ROSE_ASSERT(result.length()!=0);
            break;
          }

        // support template instantiations: they have names
        // symbol name is not sufficient to differentiate them: must have parameters followed.
        // Try mangled names are too long!!
        case V_SgTemplateInstantiationDecl:
        case V_SgTemplateInstantiationMemberFunctionDecl:  
        case V_SgTemplateInstantiationFunctionDecl:
          // No explicit name available
        case V_SgCtorInitializerList:
        case V_SgPragmaDeclaration:
        case V_SgFunctionParameterList:
        case V_SgUsingDirectiveStatement:
        case V_SgStmtDeclarationStatement:
          {
            break;
          }
        default:
          {
            cerr<<"Warning, unhandled declaration type in roseNode::getName(), use default empty name for "<<mNode->class_name()<<endl;
            //ROSE_ASSERT(false);
            break;
          }
      }// end switch
    }
    return result ;
  }

  bool roseNode::hasSourcePos() const
  {
    source_position pos = getStartPos();
    //0 line number means no source position information for ROSE AST
    if (pos.line==0)
      return false;
    else
      return true;
  }

  bool roseNode::hasName() const
  {
    string name = getName();
    // 0 sized name means no name for ROSE AST
    return (name.size()>0);
  }

  string roseNode::getFileName() const
  {
    string result;
    ROSE_ASSERT(mNode!=NULL);
    // no uniform way to get file name in ROSE
    SgLocatedNode* lnode = isSgLocatedNode(mNode);
    SgSourceFile* sfile = isSgSourceFile(mNode);
    if (lnode)
      result = lnode->getFilenameString();
    else if (sfile)
      result = sfile->getFileName();
    else
    {
      cerr<<"error: unrecognized sage node to get a file name:"
        <<mNode->class_name()<<endl;
      ROSE_ABORT();
    }

    return result;
  }

  abstract_node* roseNode::getParent() const
  {
    ROSE_ASSERT(mNode!=NULL); // NULL roseNode allowed?
    //if (mNode==NULL) return NULL;
    abstract_node* result = NULL;
    if (mNode->get_parent()!=NULL)
      result = buildroseNode(mNode->get_parent());
    return result;
  }

  /* we need a file handle, not just the file name string
   * so a node for file is needed to generate the file handle legally.
   */
  abstract_node* roseNode::getFileNode() const
  {
    ROSE_ASSERT(mNode!=NULL);
    if (isSgProject(mNode)) // project node has no single file associated. 
      return NULL;
    SgFile* filenode = getEnclosingFileNode(mNode);

    abstract_node* result = buildroseNode(filenode);
    return result;
  }

  source_position roseNode::getStartPos() const
  {
    source_position pos; 
    ROSE_ASSERT(mNode!=NULL);
    SgLocatedNode* lnode = isSgLocatedNode(mNode);
    if (lnode != NULL)
    {
      pos.line = lnode->get_file_info()->get_line();
      pos.column= lnode->get_file_info()->get_col();
    }
    else
    {
      pos.line=0;
      pos.column=0;
    }
    return pos;
  }

  source_position roseNode::getEndPos() const
  {
    source_position pos; 
    ROSE_ASSERT(mNode!=NULL);
    SgLocatedNode* lnode = isSgLocatedNode(mNode);
    if (lnode != NULL)
    {  
      pos.line = lnode->get_endOfConstruct()->get_line();
      pos.column= lnode->get_endOfConstruct()->get_col(); 
    }else
    {
      pos.line=0;
      pos.column=0;
    }
    return pos;
  }

  // return the numbering within a scope /parent node
  // start from 1
  size_t roseNode::getNumbering(const abstract_node * another_node) const
  {
    size_t number = 1;
    // self is counted as number 1 if no parent node exists
    if (another_node==NULL)
      return 1;

    // find nodes of the same type within the parent/scope node
    // using preorder traversal to order the nodes.
    SgNode* root = (SgNode*) ((dynamic_cast<const roseNode*> (another_node))->getNode());
    ROSE_ASSERT(root !=NULL);
    Rose_STL_Container <SgNode*> nodeArray = NodeQuery::querySubTree(root,mNode->variantT());
    for (Rose_STL_Container<SgNode *>::iterator i=nodeArray.begin();
        i!=nodeArray.end(); i++)
    {
      if (*i == mNode) 
        break;
      else 
        // should consider the node within the same file only!!
        if ((*i)->get_file_info()->get_filenameString() == 
            mNode->get_file_info()->get_filenameString())
          number++;
    }  
    return number;
  }

  std::string roseNode::toString() const
  {
    std::string result;
    result= mNode->unparseToString();
    return result;
  }

  //Find a node from a string for an abstract handle
  // eg. find a file node from a string like SgSourceFile<name,/home/liao6/names.cpp>
  abstract_node* roseNode::findNode(std::string construct_type_str, specifier mspecifier) const  
  {
    abstract_node* result=NULL;
    VariantT vt = getVariantT(construct_type_str); 
    // somehow the construct type str does not match any node types. 
    if (vt == V_SgNumVariants)
      return NULL; 

    //Get all matched nodes according to node type
    // search within current node as root for a subtree
    Rose_STL_Container<SgNode*> nodelist =  NodeQuery::querySubTree((SgNode*)(getNode()),vt);

    for (Rose_STL_Container<SgNode *>::iterator i=nodelist.begin();i!=nodelist.end();i++)
    {
      abstract_node* cnode = buildroseNode(*i);
      if (mspecifier.get_type()==e_position)
      {
        if (isEqual(mspecifier.get_value().positions, cnode->getSourcePos()))
        {
          result = cnode;
          break;
        }
      }
      else if (mspecifier.get_type()==e_name)
      {
        if (mspecifier.get_value().str_v == cnode->getName())
        {
          result = cnode;
          break;
        }
      } 
      else if (mspecifier.get_type()==e_numbering)
      {
        if (mspecifier.get_value().int_v == cnode->getNumbering(this))
        {
          result = cnode;
          break;
        }
      } 
      else
      {
        cerr<<"error: unhandled specifier type in roseNode::findNode()"<<endl;
        ROSE_ABORT();
      }
    }//end for

    return result;
  }

  // A simplest implementation here, for now
  bool roseNode::operator==(const abstract_node & x) const
  {
    SgNode* other_node = (SgNode*) ( (dynamic_cast<const roseNode&> (x)).getNode());

    return (mNode ==other_node);
  }


  //! A helper function to build a single abstract handle item
  static abstract_handle * buildSingleAbstractHandle(SgNode* snode, abstract_handle * p_handle = NULL)
  {
    abstract_handle *  result = NULL; 
    ROSE_ASSERT (snode != NULL);
    abstract_node * anode = buildroseNode(snode);
    ROSE_ASSERT (anode != NULL);

    // look up first
    result = handle_map[anode];
    if (result != NULL)
      return result;

    //Create the single handle item 
    if (isSgSourceFile (snode) || isSgProject(snode) ||isSgGlobal(snode))
      result = new abstract_handle (anode); // default name or file location specifiers are used
    else 
    {
      // any other types of AST node, use numbering or name within the parent handle
      ROSE_ASSERT (p_handle != NULL);
      if (isSgFunctionDefinition(snode))
        result = new abstract_handle  (anode, e_name, p_handle);
      else  
        result = new abstract_handle  (anode, e_numbering, p_handle);
    }  

    ROSE_ASSERT (result != NULL);
    // cache the result
    handle_map[anode] = result;   

    return result;
  }

  //! A default builder function handles all details: file use name, others use numbering  
  //  Algorithm:
  //   Go through all parent scopes until reach SgSourceFile. store them into a list
  //   iterate on the list to generate all abstract handles, with optional the parent handle information
  abstract_handle * buildAbstractHandle(SgNode* snode)
  {
    ROSE_ASSERT (snode != NULL);
    // simple nodes
    if  (isSgSourceFile (snode) || isSgProject(snode) ||isSgGlobal(snode))
      return buildSingleAbstractHandle (snode);

    // all other nodes, trace back to SgGlobal, store all intermediate scope nodes
    std::vector<SgNode*> scope_list; 
    SgScopeStatement* p_scope = SageInterface::getEnclosingScope(snode);
    while (!isSgGlobal(p_scope))
    {
      scope_list.push_back(p_scope);
      p_scope =  SageInterface::getEnclosingScope(p_scope);
    }

    ROSE_ASSERT (isSgGlobal(p_scope));
    abstract_handle * p_handle = buildSingleAbstractHandle (p_scope);

    // Now go through the list to generate numbering handles
    std::vector<SgNode*>::reverse_iterator riter;
    for (riter = scope_list.rbegin(); riter!= scope_list.rend(); riter++)
    {
      SgNode* c_node = *riter;
      p_handle = buildSingleAbstractHandle (c_node, p_handle);
    }

    ROSE_ASSERT (p_handle != NULL);

    return buildSingleAbstractHandle (snode, p_handle);
  }

  //! Convert an abstract handle string to a located node in AST
  SgLocatedNode* convertHandleToNode(const std::string& cur_handle)
  {
    SgLocatedNode* lnode=NULL;

    SgProject* project = getProject();
    if (project == NULL)
      return NULL; 

    SgFilePtrList & filelist = project->get_fileList();
    SgFilePtrList::iterator iter= filelist.begin();
    for (;iter!=filelist.end();iter++)
    {
      SgSourceFile* sfile = isSgSourceFile(*iter);
      if (sfile != NULL)
      { 
        // prepare a file handle first
        abstract_node * file_node = buildroseNode(sfile);
        ROSE_ASSERT (file_node);
        abstract_handle* fhandle = new abstract_handle(file_node);
        ROSE_ASSERT (fhandle);
        // try to match the string and get the statement handle
        abstract_handle * shandle = new abstract_handle (fhandle,cur_handle);
        // it is possible that a handle is created but no matching IR node is found
        if (shandle != NULL)
        { 
          if (shandle->getNode() != NULL)
          { // get SgNode from the handle
#ifdef _MSC_VER  // abstract_node::getNode() using covariant type is not supported on Windows MSVC
          lnode = NULL; 
#else          
            SgNode* target_node = (SgNode*) (shandle->getNode()->getNode());
            ROSE_ASSERT(isSgStatement(target_node));
            lnode= isSgLocatedNode(target_node);
#endif            
            break;
          }
        }
      } //end if sfile
    } // end for
   return lnode;
  } // end convertHandleToNode
} // end of namespace







#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "abstract_handle.h"
#include "loopAdapter.h"
#include <assert.h>

using namespace std;
using namespace AbstractHandle;

  // an internal map to avoid duplicated nodes for a same file
  map<string, abstract_node*> file_node_map;

   /* Only handle for loops
    * */
   string loopNode::getConstructTypeName() const
   {
      return string("ForStatement");
   }

   /* All loops are not named */
   string loopNode::getName() const
   {
     return string("");
   }

   /* source position is the required info. for MyLoop */
   bool loopNode::hasSourcePos() const
   {
     return true;
   }

  /* MyLoop is not named */
   bool loopNode::hasName() const
   {
     string name = getName();
     // 0 sized name means no name for ROSE AST
     return (name.size()>0);
   }

   string loopNode::getFileName() const
   {
     return getNode()->sourceFileName;
   }

  /* MyLoop only works for list of loops, no parent-child relation */
  abstract_node* loopNode::getParent() const
  {

    return NULL;
  }

/* 
 * Create the fileNode on the fly when needed
 */
   abstract_node* loopNode::getFileNode() const
   {
     abstract_node* filenode = file_node_map[getNode()->sourceFileName];
     if (filenode==NULL)
       filenode = new fileNode (getNode()->sourceFileName);
     return filenode;
   }

/* MyLoop only keeps line number of loops */
   source_position loopNode::getStartPos() const
   {
     source_position pos; 
     assert(mNode!=NULL);
     
     pos.line=getNode()->line_number;
     pos.column=0;
     return pos;
   }

/* MyLoop does not keep end source position information at all*/
   source_position loopNode::getEndPos() const
   {
     source_position pos; 
     pos.line=0;
     pos.column=0;
     return pos;
   }

   source_position_pair loopNode::getSourcePos() const
   {
     source_position_pair result;
     result.first = getStartPos();
     result.second= getEndPos();
     return result;
   }

  // return the numbering within a scope 
  // MyLoop only cares about numbering inside its source file
  // The parameter has to be a fileNode
  size_t loopNode::getNumbering(const abstract_node * another_node) const
  {
    int number=1;
    const fileNode* file_node = dynamic_cast<const fileNode*> (another_node);
    assert(file_node!=NULL);
    vector<MyLoop*> *loops = file_node->getMLoops();
   for (vector<MyLoop *>::iterator i=(*loops).begin();
        i!=(*loops).end(); i++)
    {
      if ((*i)==getNode())
        break;
      else
        number++;
    }  
    return number;
  }

  // A simplest implementation here, for now
   bool loopNode::operator==(const abstract_node & x) const
   {
     MyLoop* other_node =  (dynamic_cast<const loopNode&> (x)).getNode();

     return (mNode ==other_node);
   }

  // Not labeled  
  size_t loopNode::getIntLabel() const
  {
    return 0;
  }
  // Not labeled 
  std::string loopNode::getStringLabel() const
  {
    return string("");
  }
std::string loopNode::toString() const
{
  std::string result;
  result= getNode()->loop_code;
  return result;
}

// MyLoop does not allow nested loops, so no child handle can be find from a loop node
abstract_node* loopNode::findNode(std::string input) const
{
  return NULL;
}

abstract_node* loopNode::findNode(std::string construct_type_str, specifier mspecifier) const  
{
  return NULL;
}

//-----------------fileNode -----------------------------
std::string fileNode::getConstructTypeName() const
{
  return string("SourceFile");
}

/*A file itself does not have line, column information */
bool fileNode::hasSourcePos() const
{
  return false;
}
bool fileNode::hasName() const
{
  return true;
}

std::string fileNode::getName() const
{
  return mfileName;
}

AbstractHandle::abstract_node* fileNode::getFileNode() const
{
  //return this;
}

/*No further parent for files */
AbstractHandle::abstract_node* fileNode::getParent() const
{ 
  return NULL;
}

//Find a node from a string of an abstract handle within a file
//Assume MyLoop only supports source position based specifier
// eg. find a loop node from a string like ForStatement<position,15>
AbstractHandle::abstract_node* fileNode::findNode(std::string input) const
{
   assert(input.size()>0);
   istringstream buffer(input);
   char type_str[256], specifier_str[PATH_MAX+512];
   AbstractHandle::specifier mspecifier;

   buffer.getline(type_str,256,'<');

   buffer.unget(); // put back '<'
   buffer.getline(specifier_str,PATH_MAX+512,'>');

  fromString(mspecifier,specifier_str);

  return findNode(type_str,mspecifier);
}

AbstractHandle::abstract_node* fileNode::findNode(std::string construct_type_str, AbstractHandle::specifier mspecifier) const
{
  abstract_node* result=NULL;

  //Get all matched nodes according to node type
  vector<MyLoop*> loops = *mLoops;  

  for (vector<MyLoop *>::iterator i=loops.begin();i!=loops.end();i++)
  {
    abstract_node* cnode = new loopNode(*i);
    if (mspecifier.get_type()==e_position)
    {
      if (isEqual(mspecifier.get_value().positions, cnode->getSourcePos()))
      {
        result = cnode;
        break;
      }
    }
    else 
    {
      cerr<<"error: unhandled specifier type in loopNode::findNode()"<<endl;
      assert(false);
    }
  }//end for
  return result;  
}

std::string fileNode::getFileName() const
{
  return mfileName;
  }

  AbstractHandle::source_position_pair fileNode::getSourcePos() const
  {
    source_position_pair result;
    return result;
  }
  AbstractHandle::source_position fileNode::getStartPos() const
  {
  }

  AbstractHandle::source_position fileNode::getEndPos() const
  {
  }

  size_t fileNode::getNumbering (const AbstractHandle::abstract_node* another_node) const
  {
    return 1;
  }

  size_t fileNode::getIntLabel() const
  {}
  std::string fileNode::getStringLabel() const
  {}
  std::string fileNode::toString() const
  {
    return mfileName;
  }

  bool fileNode::operator == (const abstract_node & x) const
  {
    return (toString()==x.toString()); 
  }






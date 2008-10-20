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

/* source position is the required info. for MyLoop */
bool loopNode::hasSourcePos() const
{
 return true;
}

string loopNode::getFileName() const
{
 return getNode()->sourceFileName;
}

/* Assume MyLoop only works for flat list of loops, no real parent-child relation 
 * but file node can be treated as their parent here
 * */
abstract_node* loopNode::getParent() const
{
  return getFileNode();
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
 source_position result;
 result.line=getNode()->line_number;
 result.column=0;
 return result;
}

// return the numbering within a scope (file)
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

std::string loopNode::toString() const
{
  std::string result;
  result= getNode()->loop_code;
  return result;
}

// A simplest implementation here, for now
 bool loopNode::operator==(const abstract_node & x) const
 {
   MyLoop* other_node =  (dynamic_cast<const loopNode&> (x)).getNode();

   return (mNode ==other_node);
 }

//-----------------fileNode -----------------------------
std::string fileNode::getConstructTypeName() const
{
  return string("SourceFile");
}

bool fileNode::hasName() const
{
  return true;
}

std::string fileNode::getName() const
{
  return mfileName;
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

  std::string fileNode::toString() const
  {
    return mfileName;
  }

  bool fileNode::operator == (const abstract_node & x) const
  {
    return (toString()==x.toString()); 
  }

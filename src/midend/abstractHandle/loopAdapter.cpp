

#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "abstract_handle.h"
#include "loopAdapter.h"
#include <assert.h>

using namespace std;
using namespace AbstractHandle;

// an internal map to avoid duplicated nodes 
map<string, abstract_node*> file_node_map;
map<MyLoop*, abstract_node*> loop_node_map;

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

/* 
 * */
abstract_node* loopNode::getParent() const
{
  if (getNode()->parent!=NULL)
  {
    abstract_node* result = loop_node_map[getNode()->parent];
    if (result == NULL)
      return new loopNode(getNode()->parent);
    else
      return result;
  }
  else
    return getFileNode();
}

/* 
* Create the fileNode on the fly when needed
*/
abstract_node* loopNode::getFileNode() const
{
 abstract_node* filenode = file_node_map[getNode()->sourceFileName];
 if (filenode==NULL)
 {
   filenode = new fileNode (getNode()->sourceFileName);
   file_node_map[getNode()->sourceFileName]=filenode;
 }
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
// MyLoop can be attached to a file node or another loop node.
// The parameter must be either of a fileNode or a loopNode
size_t loopNode::getNumbering(const abstract_node * another_node) const
{
  int number=1;
  const fileNode* file_node = dynamic_cast<const fileNode*> (another_node);
  const loopNode* p_loop_node = dynamic_cast<const loopNode*> (another_node);

  vector<MyLoop*> loops;

  if (file_node)
    loops = file_node->getMLoops();
  else if (p_loop_node)  
    loops = p_loop_node->getChildren();
  else
    ROSE_ABORT();
 for (vector<MyLoop*>::iterator i=loops.begin();
      i!=loops.end(); i++)
  {
    if ((*i)==getNode())
      break;
    else
      number++;
  }  
  return number;
}

AbstractHandle::abstract_node* loopNode::findNode(std::string construct_type_str, AbstractHandle::specifier mspecifier) const
{
   abstract_node* result=NULL;
    //Get all matched nodes according to node type
  vector<MyLoop*> loops = getChildren();  
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
      cerr<<"error: unhandled specifier type in loopNode::findNode()"<<endl;
      ROSE_ABORT();
    }
  }//end for
  return result;

}

std::string loopNode::toString() const
{
  std::string result;
  //We ignore this for simplicity
  //result= getNode()->loop_code;
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
  vector<MyLoop*> loops = mLoops;  

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
    } else if (mspecifier.get_type()==e_numbering)
    {
      if (mspecifier.get_value().int_v == cnode->getNumbering(this))
      {
        result = cnode;
        break;
      }
    }
    else 
    {
      cerr<<"error: unhandled specifier type in loopNode::findNode()"<<endl;
      ROSE_ABORT();
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

/* A thin client example:
 *   A simplest loop tool maintaining a list of loops and source position information
 * */
#ifndef loop_adapter_INCLUDED
#define loop_adapter_INCLUDED

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "myloop.h" // customer's own loop representation
#include "abstract_handle.h"

class loopNode: public AbstractHandle::abstract_node 
{
 public:
   loopNode(MyLoop* snode):mNode(snode){};

   virtual ~loopNode(){};

   MyLoop* getNode() const {return mNode; }

   // must provide non-pure virtual function prototypes here
   virtual std::string getConstructTypeName() const;
   virtual bool hasSourcePos() const;
   virtual bool hasName() const;
   virtual std::string getName() const;
   virtual AbstractHandle::abstract_node* getFileNode() const;
   virtual AbstractHandle::abstract_node* getParent() const;
   virtual AbstractHandle::abstract_node* findNode(std::string) const;
   virtual AbstractHandle::abstract_node* findNode(std::string construct_type_str, AbstractHandle::specifier mspecifier) const;
   virtual std::string getFileName() const;
   virtual AbstractHandle::source_position_pair getSourcePos() const;
   virtual AbstractHandle::source_position getStartPos() const;
   virtual AbstractHandle::source_position getEndPos() const;

  virtual size_t getNumbering (const AbstractHandle::abstract_node* another_node) const;
  virtual size_t getIntLabel() const; 
  virtual std::string getStringLabel() const;
  virtual std::string toString() const;
  virtual bool operator == (const abstract_node & x) const;
protected:
  MyLoop* mNode;
};

// an internal map to avoid duplicated nodes for a same file
extern  std::map<std::string, AbstractHandle::abstract_node*> file_node_map;

/* Implement fileNode , no direct MyLoop node is mapped */
class fileNode: public AbstractHandle::abstract_node 
{
 public:
   fileNode(std::string name):mfileName(name){ file_node_map[name]=this;};

   virtual ~fileNode(){};
   /*No corresponding user node available */
   fileNode* getNode() const {return NULL; }

   // must provide non-pure virtual function prototypes here
   virtual std::string getConstructTypeName() const;
   virtual bool hasSourcePos() const;
   virtual bool hasName() const;
   virtual std::string getName() const;
   virtual AbstractHandle::abstract_node* getFileNode() const;
   virtual AbstractHandle::abstract_node* getParent() const;
   virtual AbstractHandle::abstract_node* findNode(std::string) const;
   virtual AbstractHandle::abstract_node* findNode(std::string construct_type_str, AbstractHandle::specifier mspecifier) const;
   virtual std::string getFileName() const;
   virtual AbstractHandle::source_position_pair getSourcePos() const;
   virtual AbstractHandle::source_position getStartPos() const;
   virtual AbstractHandle::source_position getEndPos() const;

  virtual size_t getNumbering (const AbstractHandle::abstract_node* another_node) const;
  virtual size_t getIntLabel() const; 
  virtual std::string getStringLabel() const;
  virtual std::string toString() const;
  virtual bool operator == (const abstract_node & x) const;
  //Additional stuff
  void setMLoops(std::vector<MyLoop*>* myloops ){mLoops = myloops;}
  std::vector<MyLoop*>* getMLoops() const {return mLoops;}

protected:
  std::string mfileName;
  std::vector<MyLoop*>* mLoops;
};


#endif

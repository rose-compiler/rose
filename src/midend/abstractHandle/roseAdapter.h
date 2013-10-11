#ifndef rose_adapter_INCLUDED
#define rose_adapter_INCLUDED


#include <iostream>
#include <string>

#include "abstract_handle.h"
namespace AbstractHandle{
class roseNode: public AbstractHandle::abstract_node 
{
 public:
   virtual ~roseNode(){};
   void* getNode() const {return (void*) mNode; }
   virtual std::string getConstructTypeName() const;
   virtual bool hasSourcePos() const;
   virtual bool hasName() const;
   virtual std::string getName() const;
   virtual AbstractHandle::abstract_node* getFileNode() const;
   virtual AbstractHandle::abstract_node* getParent() const;
   virtual AbstractHandle::abstract_node* findNode(std::string construct_type_str, AbstractHandle::specifier mspecifier) const;
   virtual std::string getFileName() const;
   virtual AbstractHandle::source_position getStartPos() const;
   virtual AbstractHandle::source_position getEndPos() const;

  virtual size_t getNumbering (const AbstractHandle::abstract_node* another_node) const;
  virtual std::string toString() const;
  virtual bool operator == (const abstract_node & x) const;
  ROSE_DLL_API friend roseNode* buildroseNode(SgNode* snode);
protected:
   // hide constructors to enforce using the builder function instead
    roseNode(){mNode =NULL;};
    roseNode(SgNode* snode);
    SgNode* mNode;
};

  // Use a build function to avoid duplicated building
    ROSE_DLL_API roseNode* buildroseNode(SgNode* snode);
}

#endif

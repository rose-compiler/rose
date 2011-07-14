
#ifndef MULTI_GRAPH_CREATE
#define MULTI_GRAPH_CREATE

#include <stdlib.h>
#include <assert.h>

class MultiGraphCreate;
class MultiGraphElem 
{
  MultiGraphCreate *gc;
  int count;
 public:
  MultiGraphElem( MultiGraphCreate *c) : gc(c) { count = 0; }
  virtual ~MultiGraphElem() { assert( count == 0); }

  bool UnlinkGraphCreate( const MultiGraphCreate *c)
       { if (gc != c) 
            count--;
         else
            gc = 0;
         if (gc == 0 && count == 0) {
             delete this;
            return true;
         }
         return false;
       }
  int LinkGraphCreate(const MultiGraphCreate* c)
    { 
      if (gc != c) ++count;
      return count; 
    }
  MultiGraphCreate* GetGraphCreate() const { return gc; }
  virtual std::string toString() const { return ""; }
 friend class MultiGraphCreate;
};

class MultiGraphCreate 
{
 protected:
  virtual ~MultiGraphCreate() {}
  bool UnlinkElem( MultiGraphElem *n) { return n->UnlinkGraphCreate(this); }
  bool LinkElem( MultiGraphElem *n) { return n->LinkGraphCreate(this); }
  bool UnlinkElem( void* n) {  return false; }
  bool LinkElem(void* n) { return false; }
 public:
  bool ContainElem( const MultiGraphElem* e) const
          { return e->GetGraphCreate() == this; }
};

template <class Info>
class MultiGraphElemTemplate : public MultiGraphElem
{
  Info info;
 protected:
  virtual ~MultiGraphElemTemplate() {}
 public:
  MultiGraphElemTemplate(MultiGraphCreate *c, const Info&  _info)
    : MultiGraphElem(c), info(_info) {}
  Info&  GetInfo() { return info; }
  Info GetInfo() const { return info; }
  // Liao, 2/22/2008, some instantiated objects do not have this member function
 // std::string toString() const { return info.toString(); }
};

#endif

#ifndef TRANSFORM_LOOP_TREE_H
#define TRANSFORM_LOOP_TREE_H

#include <vector>
#include <LoopTree.h>
#include <LoopTreeHoldNode.h>
#include <DepRel.h>

class ObserveTransform;
class LoopTreeTransform
{
  protected:
    void UnlinkNode( LoopTreeNode* n) { n->Unlink(); }
    void ReplaceChildren( LoopTreeNode *on, LoopTreeNode *nn, int pos);
    void InsertNode( LoopTreeNode *node, LoopTreeNode *pos, int opt);
    void RemoveNode( LoopTreeNode* node) { node->RemoveSelf(); }
  public:
    LoopTreeNode* InsertHandle(LoopTreeNode *pos, int opt); // opt == -1 as parent; 1 : as child
    LoopTreeNode* InsertLoop( LoopTreeNode* l,LoopTreeNode *pos, int opt);
};

class LoopTreeDistributeNode : public LoopTreeTransform
{
// DQ (11/25/2009): Changed name from SelectObject to SelectObjectBase to avoid SelectObject function ambiguity using Microsoft Visual Studio
  typedef SelectObjectBase<LoopTreeNode*>& SelectLoopTreeNode;
  LoopTreeNode* Distribute( LoopTreeNode *n, SelectLoopTreeNode sel, 
                            ObserveTransform &ob);
 public:
  // distribute n and put the distributed node before n; 
  LoopTreeNode* operator () (LoopTreeNode *n, SelectLoopTreeNode sel, bool before = true);

  //distribute the parent of nodes and put distributed nodes before the others
  LoopTreeNode* operator () (LoopTreeNodeIterator nodes);
};

class LoopTreeSplitStmt : public LoopTreeTransform
{ 
  LoopTreeNode* operator() (LoopTreeNode *stmt, LoopTreeNode* restr1, LoopTreeNode* restr2);
 public: 
  LoopTreeNode* operator() ( LoopTreeNode *stmt, LoopTreeNode* loop1, 
                            LoopTreeNode* loop2, DepRel r);
  LoopTreeNode* operator() ( LoopTreeNode *stmt, LoopTreeNode* loop, const SymbolicVal& split);
};

class LoopTreeMergeLoop : public LoopTreeTransform
{ public: void operator () (LoopTreeNode *ancs,LoopTreeNode *desc, int align);
};

class LoopTreeMergeStmtLoop : public LoopTreeTransform
{ public: void operator () (LoopTreeNode *ancs, LoopTreeNode *desc,
                              LoopTreeNode *stmt, int align);
};

class LoopTreeBlockLoop : public LoopTreeTransform
{ public: 
    LoopTreeNode* operator() ( LoopTreeNode *n, SymbolicVar bvar, SymbolicVal b);
};

class LoopTreeSwapNodePos : public LoopTreeTransform
{ public: void operator () ( LoopTreeNode *n1, LoopTreeNode *n2);
};

class LoopTreeEmbedStmt :  public LoopTreeTransform
{ public: void operator () (LoopTreeNode *loop, LoopTreeNode* stmt, SymbolicVal selIter);
};

class SelectArray
{
 private:
  struct ArrayDim { 
    SymbolicVal incr, size;
    int dim;
    ArrayDim( int d, const SymbolicVal& in, const SymbolicVal& _size)
     : incr(in), size(_size), dim(d) {}
  };
  std::vector<SymbolicVal> selstart;
  std::list<ArrayDim> selinfo;
 public:
  SelectArray(int arrdim) 
   {  for (int i = 0; i < arrdim; ++i) selstart.push_back(SymbolicVal()); }
  std::string toString() const;

  unsigned arr_dim() const { return selstart.size(); }
  unsigned sel_dim() const { return selinfo.size(); }

  const SymbolicVal& sel_start( int dim) const { return selstart[dim]; }
  SymbolicVal& sel_start( int dim) { return selstart[dim]; }

  class const_iterator   
  {
     std::list<ArrayDim>::const_iterator impl;
    public:
      const_iterator(const std::list<ArrayDim>::const_iterator& that) 
         : impl(that) {}
      const_iterator(const const_iterator& that) : impl(that.impl) {}
      bool operator == (const const_iterator& that) const
          { return impl == that.impl; }
      void operator ++() { ++impl;}
      void operator ++(int) { impl++;}
      SymbolicVal cur_incr() const { return (*impl).incr; }
      SymbolicVal cur_size() const { return (*impl).size; }
      int cur_dim() const { return (*impl).dim; }
  };
  class iterator 
  {
     std::list<ArrayDim>::iterator impl;
    public:
      iterator(const std::list<ArrayDim>::iterator& that) : impl(that) {}
      iterator(const iterator& that) : impl(that.impl) {}
      bool operator == (const iterator& that) const
          { return impl == that.impl; }
      void operator ++() { ++impl;}
      void operator ++(int) { impl++;}
      SymbolicVal& cur_incr() { return (*impl).incr; }
      SymbolicVal& cur_size() { return (*impl).size; }
      int cur_dim() const { return (*impl).dim; }
  };
  const_iterator begin() const { return selinfo.begin(); }
  const_iterator end() const { return selinfo.end(); }
  iterator begin() { return selinfo.begin(); }
  iterator end() { return selinfo.end(); }

  bool select(LoopTransformInterface& la, LoopTreeNode* stmt, LoopTreeNode* cproot,
                AstInterface::AstNodeList& index) ;

  SymbolicVal sel_index( AstInterface& fa, AstInterface::AstNodeList& arrindex) const;
  friend class SelectArray::const_iterator;
};

class CopyArrayConfig
{
 public:
  typedef enum {NONE = 0, INIT_COPY = 1, SAVE_COPY = 2, 
                ALLOC_COPY = 4, DELETE_COPY = 8, SHIFT_COPY = 16} CopyOpt;
  static std::string CopyOpt2String( int opt);

  CopyArrayConfig( AstInterface& fa, const std::string& arr, const AstNodeType& base,
                   const SelectArray& sel, LoopTreeNode* shift = 0);
  std::string toString() const;


  AstNodePtr copy_codegen(LoopTransformInterface& fa, CopyOpt opt) const;
  AstNodePtr allocate_codegen(AstInterface& fa) const;
  AstNodePtr delete_codegen(AstInterface& fa) const;

  bool need_allocate_buffer() const;
  bool need_delete_buffer() const;
  bool shift_buffer() const { return shift != 0; }
  bool scalar_repl() const;

  SymbolicVal buffer_size() const { return bufsize[bufsize.size()-1]; }
  SymbolicVal buf_offset(AstInterface& fa, std::vector<SymbolicVal>& arroffset) const;
  AstNodePtr buf_codegen(AstInterface& fa, const SymbolicVal& index) const;
  AstNodePtr buf_codegen(AstInterface& fa, AstInterface::AstNodeList& arrindex) const;
 private:
  void set_bufname( AstInterface& fa);
  void set_bufsize( AstInterface& fa);


  void copy_scalar_codegen(LoopTransformInterface& la, 
                          SelectArray::const_iterator selp, 
                           std::vector<SymbolicVal>& arrindex, CopyOpt opt,
                          AstNodePtr& r, int& bufoffset) const ;
  void copy_loop_codegen(LoopTransformInterface& la, 
                          SelectArray::const_iterator selp, 
                          std::vector<SymbolicVal>& arrindex, CopyOpt opt,
                          AstNodePtr& r, const AstNodePtr& bufoffset) const ;

  std::vector<SymbolicVal> bufsize, arrshift;
  LoopInfo* shift;
  SymbolicVal bufshift;

  AstNodePtr prep;
  std::string arrname, bufname;
  AstNodeType basetype;
  SelectArray sel;
};

class LoopTreeCopyArrayToBuffer :  public LoopTreeTransform
{ 
 public: 
  typedef CopyArrayConfig::CopyOpt CopyOpt;
  void operator()(LoopTransformInterface& la, LoopTreeNode* init, 
                           LoopTreeNode* save,
                           const CopyArrayConfig& c, int opt);
};

class LoopTreeReplaceAst :  public LoopTreeTransform
{
 public:
  LoopTreeNode* operator()(LoopTransformInterface& la, LoopTreeNode* h,
                           const AstNodePtr& orig, const AstNodePtr& repl);
};
                                                                                                
void ApplyLoopSplitting(LoopTreeNode *r);
void OptimizeLoopTree( LoopTreeNode *r);

#endif

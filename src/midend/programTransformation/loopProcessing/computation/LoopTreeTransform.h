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
  typedef RoseSelectObject<LoopTreeNode*>& SelectLoopTreeNode;
  LoopTreeNode* Distribute( LoopTreeNode *n, SelectLoopTreeNode sel, 
                            ObserveTransform &ob, LoopTreeNode** loc=0);
  /* QY: distribute the parent of loc to separate all children before loc */
  LoopTreeNode* DistributeBefore(LoopTreeNode* parent, LoopTreeNode* loc);

 public:
  typedef enum {BEFORE, AFTER, ORIG} Location;
  /* QY: distribute n; put the distributed node before/after n or based on
     on original locations of split children */
  LoopTreeNode* operator () (LoopTreeNode *n, SelectLoopTreeNode sel, Location config=BEFORE);
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

typedef enum {NONE = 0, INIT_COPY = 1, SAVE_COPY = 2, INIT_SAVE_COPY=3,
                ALLOC_COPY = 4, ALLOC_INIT_COPY=5, ALLOC_SAVE_COPY=6,
                ALLOC_INIT_SAVE_COPY=7, DELETE_COPY = 8, 
                ALLOC_DELETE_COPY=12, ALLOC_INIT_DELETE_COPY=13,
                ALLOC_SAVE_DELETE_COPY=14, ALLOC_INIT_SAVE_DELETE_COPY=15,
                SHIFT_COPY = 16, COPY_INSIDE=32} CopyArrayOpt;
std::string CopyArrayOpt2String( CopyArrayOpt opt);


/*QY: descriptor for copying array elements to a buffer space */
class SelectArray
{
 public:
  /* QY: the array dimension to copy; support only rectangle copy regions */
  class ArrayDim 
  { 
    SymbolicVal incr, size; /*QY: the increment and size of the copy dim */
    int dim;                /*QY: dim# (for multi-dimentional arrays) */
    std::pair<int,int> accLoops;  /*QY: the loops that access the copied dim*/
  public:
    ArrayDim( int _dim, const SymbolicVal& _incr, const SymbolicVal& _size,
              int minLoopLevel, int maxLoopLevel)
     : incr(_incr), size(_size), dim(_dim), accLoops(minLoopLevel,maxLoopLevel)
       {}
    const SymbolicVal& get_incr() const { return incr; }
    const SymbolicVal& get_size() const { return size; }
    int get_arrDim() const { return dim; }
    int get_minLoopLevel() const { return accLoops.first; }
    int get_maxLoopLevel() const { return accLoops.second; }
    friend class SelectArray;
  };

 private:
  std::vector<SymbolicVal> selstart;/*QY: the array starting address to copy*/
  std::list<ArrayDim> selinfo;      /*QY: elements to copy after selstart*/
  std::string arrname, bufname;     /*QY: array name and buffer name */
  AstNodeType basetype;             /*QY: type of array elements*/
  std::vector<SymbolicVal> bufsize; /*QY: buffer stride for each copy dim*/
  AstNodePtr prep;                  /*QY: AST to be inserted before copying */

 public:
  SelectArray(const AstNodeType& base, const std::string& arr, int arrdim) 
    : arrname(arr), basetype(base)
   {  for (int i = 0; i < arrdim; ++i) selstart.push_back(SymbolicVal()); }
  std::string toString() const;

  /*QY: insert new select info (selincr,selsize,startdiff) to selinfo. 
         return whether the internal is modified */
  bool insert_selinfo (LoopTreeGetVarBound &context,
                 int arrDim, SymbolicVal& selincr, SymbolicVal& selsize, 
                 int minLevel, int maxLevel, const SymbolicVal& startdiff);

  /*QY: select copy dimension */
  bool select( LoopTreeNode* stmt, LoopTreeNode* cproot,
                AstInterface::AstNodeList& index) ; 
  void set_bufname( AstInterface& fa); /*QY: done before doing xform */
  void set_bufsize( AstInterface& fa); /*QY: done after selecting dimensions*/

  std::string arr_name() const { return arrname; }
  AstNodeType elem_type() const { return basetype; }
  unsigned arr_dim() const { return selstart.size(); }
  unsigned sel_dim() const { return selinfo.size(); }

  const SymbolicVal& sel_start(unsigned dim) const 
        { assert(dim < selstart.size()); return selstart[dim]; }
  SymbolicVal& sel_start(unsigned dim) 
        { assert(dim < selstart.size()); return selstart[dim]; }
  bool scalar_repl() const;
  bool need_allocate_buffer() const;
  bool need_delete_buffer() const;

  AstNodePtr initcopy_codegen( LoopInfo* shift = 0) const;
  AstNodePtr savecopy_codegen() const;
  AstNodePtr shiftcopy_codegen( LoopInfo& shift, 
                          const std::vector<SymbolicVal>& arrshift,
                          const SymbolicVal& bufshift) const;
  AstNodePtr allocate_codegen(AstInterface& fa) const;
  AstNodePtr delete_codegen(AstInterface& fa) const;


  SymbolicVal buf_size() const { return bufsize[bufsize.size()-1]; }
  SymbolicVal buf_offset(AstInterface& fa, std::vector<SymbolicVal>& arroffset) const;

  /*QY: return scalar repl buffer reference for the given buffer offset */
  AstNodePtr buf_codegen(AstInterface& fa, int offset) const;

  /*QY: return array copy buffer reference for the given buffer offset */
  AstNodePtr buf_codegen(AstInterface& fa, const SymbolicVal& offset) const;

  /*QY: return array copy buffer reference for the given array ref*/
  AstNodePtr buf_codegen(AstInterface& fa, AstInterface::AstNodeList& arrindex) const;

  class const_iterator   
  {
     std::list<ArrayDim>::const_iterator impl;
    public:
      const_iterator(const std::list<ArrayDim>::const_iterator& that) 
         : impl(that) {}
      const_iterator(const const_iterator& that) : impl(that.impl) {}
      bool operator == (const const_iterator& that) const
          { return impl == that.impl; }
      bool operator != (const const_iterator& that) const
          { return impl != that.impl; }
      void operator ++() { ++impl;}
      void operator ++(int) { impl++;}
      const ArrayDim& operator *() const { return *impl; }
  };
  class iterator 
  {
     std::list<ArrayDim>::iterator impl;
    public:
      iterator(const std::list<ArrayDim>::iterator& that) : impl(that) {}
      iterator(const iterator& that) : impl(that.impl) {}
      bool operator == (const iterator& that) const
          { return impl == that.impl; }
      bool operator != (const iterator& that) const
          { return impl != that.impl; }
      void operator ++() { ++impl;}
      void operator ++(int) { impl++;}
      ArrayDim& operator *() { return *impl; }
     friend class SelectArray;
  };
  const_iterator begin() const { return selinfo.begin(); }
  const_iterator end() const { return selinfo.end(); }
  iterator begin() { return selinfo.begin(); }
  iterator end() { return selinfo.end(); }
  void erase(iterator& p) { selinfo.erase(p.impl); }
  friend class SelectArray::const_iterator;
};

class CopyArrayConfig
{
 public:
  CopyArrayConfig(AstInterface& fa, const SelectArray& sel, 
            CopyArrayOpt opt, LoopTreeNode* shift = 0);
  std::string toString() const;

  bool shift_buffer() const { return shift != 0; }
  CopyArrayOpt get_opt() const { return opt; }
  void set_opt(CopyArrayOpt _opt) { opt = _opt; }
  AstNodePtr CodeGen( const AstNodePtr& c) const; 
  SelectArray& get_arr() { return sel; }
  const SelectArray& get_arr() const { return sel; }

 private:
  std::vector<SymbolicVal> arrshift;
  LoopInfo* shift;
  SymbolicVal bufshift;

  SelectArray sel;
  CopyArrayOpt opt;
};

class LoopTreeCopyArrayToBuffer :  public LoopTreeTransform
{ 
 public: 
  void operator()( LoopTreeNode* repl,
                  LoopTreeNode* init, LoopTreeNode* save, 
                  CopyArrayConfig& c);
};

class LoopTreeReplaceAst :  public LoopTreeTransform
{
 public:
  LoopTreeNode* operator()( LoopTreeNode* h,
                           const AstNodePtr& orig, const AstNodePtr& repl);
};
                                                                                                
void ApplyLoopSplitting(LoopTreeNode *r);
void OptimizeLoopTree( LoopTreeNode *r);

#endif

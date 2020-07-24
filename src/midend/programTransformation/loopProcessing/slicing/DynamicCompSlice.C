#include <DynamicCompSlice.h>
#include <CompSliceImpl.h>
#include <LoopTreeTransform.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */
#include <assert.h>
#include <stdio.h>

class CondSliceStmt : public CompSliceStmt
{
  int groupIndex;
 protected:
  virtual LoopTreeShadowNode* CloneNode( LoopTreeNode *n) const
    { return new CondSliceStmt( n, *this); }
 public:
  CondSliceStmt( LoopTreeNode *s,  CompSliceImpl *tc, int a = 0, int gi = 0)
     : CompSliceStmt(s, tc, a), groupIndex(gi) {}
  CondSliceStmt( LoopTreeNode *n, const CondSliceStmt &that)
     : CompSliceStmt(n, that), groupIndex(that.groupIndex) {}
  ~CondSliceStmt() {}

  unsigned GetSliceGroupIndex() const { return groupIndex; }
  void Dump() const
    {
      CompSliceStmt::Dump();
      std::cerr << "slicing group: " <<  groupIndex << "\n";
    }
  virtual std::string GetClassName() const { return "CondSliceStmt"; }
};


class DynamicCompSliceImpl : public CompSliceImpl
{
   unsigned groupIndex, groupNum;
 protected:
   virtual CompSliceStmt* CreateSliceStmtNode( LoopTreeNode *n, CompSliceStmt *_that = 0)
   {
      CondSliceStmt *that = (_that == 0 || _that->GetClassName() != "CondSliceStmt")? 0
                            :  static_cast<CondSliceStmt*>(_that);
      size_t gi = (that == 0)? groupIndex : groupIndex + that->GetSliceGroupIndex();
      if (gi > groupNum)
          groupNum = gi;
      return new CondSliceStmt( n, this, 0, gi );
   }
 public:
  DynamicCompSliceImpl( int looplevel, int gi = 0 )
     : CompSliceImpl(looplevel), groupIndex(gi+1), groupNum(gi) {}
  ~DynamicCompSliceImpl() {}
  virtual CompSliceImpl* CloneImpl() const
    { return new DynamicCompSliceImpl( QuerySliceLevel(), groupNum); }
  unsigned GetGroupNum() { return groupNum; }
  void NextGroup() { if (groupIndex == groupNum) ++groupIndex; }
  void Append( const CompSliceImpl& that)
  {
   CompSliceImpl::Append(that);
   groupIndex = groupNum;
  }
};

DynamicCompSlice :: DynamicCompSlice( int level)
  : CompSlice(new DynamicCompSliceImpl(level) )
  { }

DynamicCompSlice :: DynamicCompSlice( const DynamicCompSlice& that)
  : CompSlice(that) {}

DynamicCompSlice :: ~DynamicCompSlice()
  {}

unsigned DynamicCompSlice:: QuerySliceStmtGroupIndex( const LoopTreeNode *n) const
{
  CondSliceStmt* sliceStmt = static_cast<CondSliceStmt*>( GetImpl()->QuerySliceStmt(n));
  assert(sliceStmt != NULL);
  return sliceStmt->GetSliceGroupIndex();
}

unsigned DynamicCompSlice :: QuerySliceGroupNumber() const
{
  DynamicCompSliceImpl* p = static_cast<DynamicCompSliceImpl*>(GetImpl());
  return p->GetGroupNum();
}


LoopTreeNode* DynamicSlicing::
Transform( LoopTreeDepComp& c, const CompSlice *_slice, LoopTreeNode *root)
{
  AstInterface& fa = LoopTransformInterface::getAstInterface();
  const DynamicCompSlice* slice = static_cast<const DynamicCompSlice*>(_slice);
  int num = slice->QuerySliceGroupNumber();
  LoopTreeNode *nr = root;
  if (num > 1) {
    std::string groupVar = fa.NewVar(fa.GetType("int")), groupVarN = groupVar + "N";
    fa.NewVar( fa.GetType("int"), groupVarN);
    LoopTreeCreate *tc = c.GetLoopTreeCreate();
    nr = tc->CreateLoopNode( SymbolicVar(groupVar, AST_NULL), 1, SymbolicVar(groupVarN, AST_NULL), 1);
    LoopTreeTransform().InsertLoop( nr, root, -1);

    AstInterface::AstNodeList args;
    char buf[11];
    for (int i = 1; i <= num; ++i) {
       sprintf(buf, "%1d", i);
       std::string name = groupVar + buf;
       fa.NewVar(fa.GetType("int"), name);
       args.push_back( fa.CreateVarRef( name).get_ptr() );
    }
    int id;
    AstNodePtr config = LoopTransformInterface::CreateDynamicFusionConfig( fa.CreateVarRef(groupVarN), args, id);
    LoopTreeNode *configNode = tc->CreateStmtNode(config);
    configNode->Link( nr, LoopTreeNode::AsPrevSibling);
    AstNodePtr configEnd = LoopTransformInterface::CreateDynamicFusionEnd( id);
    LoopTreeNode *endNode = tc->CreateStmtNode(configEnd);
    endNode->Link( nr, LoopTreeNode::AsNextSibling);

    for (CompSlice::ConstStmtIterator p = slice->GetConstStmtIterator();
         !p.ReachEnd(); ++p) {
        LoopTreeNode* stmt = p.Current();
        sprintf(buf, "%1d", slice->QuerySliceStmtGroupIndex(stmt));
        LoopTreeEmbedStmt()( nr, stmt, SymbolicVar(groupVar + buf, AST_NULL) );
    }
    DependenceHoisting::Transform(c, slice, root);
  }
  else
    nr = DependenceHoisting::Transform(c, slice, root);

  return nr;
}



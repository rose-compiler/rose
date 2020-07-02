
#include <CompSliceDepGraph.h>
#include <CompSliceLocality.h>
#include <FusionAnal.h>
#include <CommandOptions.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

//#define DEBUG 1

bool DebugFusion()
{
  static int r = 0;
  if (r == 0)
     r = (CmdOptions::GetInstance()->HasOption("-debugfusion")?2 : 1);
  return r == 2;
}

void FuseSliceNests( CompSliceNest& g1, CompSliceNest& g2, const DepInfo& info)
{
#ifdef DEBUG
std::cerr << "FuseSliceNests\n";
#endif
  int num1 = g1.NumberOfEntries(), num2 = g2.NumberOfEntries();

  int j;
  for (j = 0; j < num1; j++) {
    FusionInfo fuse = GetFusionInfo(info, j, j);
    if (!fuse.succ)
       break;
    if (fuse.align != 0)
      g2.AlignEntry(j, -fuse.align);
  }
  const CompSlice* slice1=g1[0];
  for (int k1 = num1-1; k1 >= j; k1--)
     g1.DeleteEntry(k1,!g1[k1]->SliceCommonLoop(slice1));
  const CompSlice* slice2=g2[0];
  for ( int k2 = num2-1; k2 >= j; k2--)
     g2.DeleteEntry(k2,!g2[k2]->SliceCommonLoop(slice2));
  g1.AppendNest(g2);
}

FusionInfo GetFusionInfo(const DepInfo& e, int index1, int index2)
{
#ifdef DEBUG
std::cerr << "GetFusionInfo\n";
#endif
   if (e.IsTop())
       return true;
   DepRel r = e.Entry(index1,index2);
   DepDirType t = r.GetDirType() ;
   if (t == DEPDIR_EQ || t == DEPDIR_LE) {
       if (DebugFusion()) {
         std::cerr << "fusion align is " << r.GetMaxAlign() << " from dep" << r.toString() << std::endl;
       }
       return FusionInfo(true, r.GetMaxAlign());
   }
   else if (t == DEPDIR_NONE)
       return true;
   else
       return false;
}

FusionInfo LoopFusionAnal::
operator()( CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2,
             int j, int k, const DepInfo& e)
{
#ifdef DEBUG
std::cerr << "LoopFusionAnal\n";
#endif
  FusionInfo result = GetFusionInfo(e, j, k);
  if (result) {
     LoopStepInfo step1 = SliceLoopStep(n1[j]), step2 = SliceLoopStep(n2[k]);
     if ( step1.step == step2.step ||
          (step1.step == -step2.step &&
              (step1.reversible || step2.reversible)) ) {
         return result;
     }
  }
  result.succ = false;
  return result;
}

FusionInfo AnyReuseFusionAnal ::
operator()( CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2, int j, int k, const DepInfo& e)
{
#ifdef DEBUG
std::cerr << "AnyReuseFusionAnal\n";
#endif
  if ( reg->TemporaryReuses(n1[j],n2[k]) || reg->SpatialReuses(n1[j],n2[k])) {
    return LoopFusionAnal:: operator()( reg, n1,n2,j,k,e);
  }
  return false;
}


FusionInfo BetterReuseFusionAnal ::
operator()( CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2, int j, int k, const DepInfo& e)
{
#ifdef DEBUG
std::cerr << "BetterReuseFusionAnal\n";
#endif
  if (index > j || index > k)
     index = 0;
  float diff1 = 0, diff2 = 0;
  if (j != index)
      diff1 = reg->TemporaryReuses(n1[j]) - reg->TemporaryReuses(n1[index])
            + reg->SpatialReuses(n1[j]) - reg->SpatialReuses(n1[index]);
  if (k != index)
      diff2 = reg->TemporaryReuses(n2[k]) - reg->TemporaryReuses(n2[index])
            + reg->SpatialReuses(n2[k]) - reg->SpatialReuses(n2[index]);
  if ( reg->TemporaryReuses(n1[j],n2[k]) - diff1 - diff2 > 0) {
     index++;
     return LoopFusionAnal::operator()( reg, n1,n2,j,k,e);
  }
  return false;
}

FusionInfo OrigLoopFusionAnal::
operator()(CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2, int j, int k, const DepInfo& e)
{
#ifdef DEBUG
std::cerr << "OrigLoopFusionAnal\n";
#endif
    CompSlice::ConstLoopIterator iter1 = n1[j]->GetConstLoopIterator(),
                                 iter2 = n2[k]->GetConstLoopIterator();
    if (iter1.ReachEnd() || iter2.ReachEnd())
       return false;
    AstNodePtr ast = iter1.Current()->GetOrigStmt();
    for (iter1++; !iter1.ReachEnd(); ++iter1) {
       if (ast != iter1.Current()->GetOrigStmt())
           return false;
    }
    for ( ; !iter2.ReachEnd(); ++iter2) {
       if (ast != iter2.Current()->GetOrigStmt())
           return false;
    }
    return GetFusionInfo(e, j, k);
}

FusionInfo InnermostLoopFission::
operator()(CompSliceLocalityRegistry *anal, CompSliceNest& n1, CompSliceNest& n2, int j, int k, const DepInfo& e)
{
#ifdef DEBUG
std::cerr << "InnermostLoopFission\n";
#endif
   FusionInfo result = OrigLoopFusionAnal::operator()(anal, n1, n2, j, k, e);
   if (result) {
      CompSlice::ConstLoopIterator iter1 = n1[j]->GetConstLoopIterator();
      LoopTreeNode* l = iter1.Current();
      LoopTreeTraverse p(l);
      for (p.Advance(); !p.ReachEnd(); p.Advance()) {
         if (LoopTreeInterface().IsLoop(p.Current()))
            return result;
      }
      return false;
   }
   return false;
}

bool MultiLevelFusion ::
Fusible( CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2, const DepInfo& e) const
{
#ifdef DEBUG
std::cerr << "MultiLevelFusion:Fusible\n";
#endif
  for (int j = 0; j < n1.NumberOfEntries(); j++) {
    for (int k = 0; k < n2.NumberOfEntries(); k++) {
      if ( (*anal)(reg, n1,n2,j,k,e))
         return true;
    }
  }
  return false;
}

void MultiLevelFusion ::
Fuse( CompSliceLocalityRegistry *reg,CompSliceNest& n1, CompSliceNest& n2, DepInfo& e) const
{
#ifdef DEBUG
std::cerr << "MultiLevelFusion:Fuse\n";
#endif
  int num1 = n1.NumberOfEntries(), num2 = n2.NumberOfEntries();
  int num = (num1 > num2)? num2 : num1;
  DepInfo fuse = DepInfoGenerator::GetBottomDepInfo(num,num);
  for (int j = 0; j < num1; ++j) {
    int k;
    for ( k = j; k < num2; ++k) {
      if (FusionInfo info = (*anal)(reg, n1, n2, j, k, e)) {

         fuse.Entry(j,j) = DepRel(DEPDIR_EQ, info.align);
         if (k != j) {
            n2.SwapEntry(j,k);
            e.SwapLoop( j, k, DEP_SINK);
         }
         break;
      }
    }
    if (k == num2) {
      n1.DeleteEntry(j);
      e.RemoveLoop(j, DEP_SRC);
      --j;
      --num1;
    }
  }
  FuseSliceNests(n1,n2,fuse);
}

bool SameLevelFusion ::
Fusible( CompSliceLocalityRegistry *reg, CompSliceNest &n1, CompSliceNest &n2, const DepInfo &e) const
{
#ifdef DEBUG
std::cerr << "SameLevelFusion:Fusible\n";
#endif
  return n1.NumberOfEntries() && n2.NumberOfEntries()
         && (*anal)(reg, n1,n2,0,0,e);
}

void SameLevelFusion ::
Fuse( CompSliceLocalityRegistry *reg, CompSliceNest& n1, CompSliceNest& n2, DepInfo& e) const
{
#ifdef DEBUG
std::cerr << "SameLevelFusion:Fuse\n";
#endif
  int num = n1.NumberOfEntries();
  if (num > n2.NumberOfEntries())
     num = n2.NumberOfEntries();
  DepInfo fuse = DepInfoGenerator::GetBottomDepInfo(num,num);
  int j;
  for ( j = 0; j < num; j++) {
    FusionInfo info = (*anal)( reg, n1, n2, j, j, e);
    if (info)
       fuse.Entry(j,j) = DepRel(DEPDIR_EQ, info.align);
    else
       break;
  }
  if (j > 0)
    FuseSliceNests(n1,n2,fuse);
}


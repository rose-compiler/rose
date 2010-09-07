

#include <BlockingAnal.h>

static int SliceNestReuseLevel(CompSliceLocalityRegistry *anal, CompSliceNest& n)
     { 
       unsigned num = n.NumberOfEntries();
       size_t reuseLevel = 0;
       for (; reuseLevel < num; ++reuseLevel) {
          if (anal->TemporaryReuses(n[reuseLevel]) > 0 || anal->SpatialReuses(n[reuseLevel]) > 0)
             break;
       }
       return reuseLevel;
    }

static SymbolicVal GetDefaultBlockSize(AstInterface& fa, const CompSlice* slice) 
    {

       LoopTransformOptions* opt = LoopTransformOptions::GetInstance();
       if (!opt->DoDynamicTuning()) {
	    return opt->GetDefaultBlockSize();
       }
       else {
	   int dt = opt->GetDynamicTuningIndex();
           AstInterface::AstNodeList l;
           l.push_back(fa.CreateConstInt(dt));

           CompSlice::ConstLoopIterator iter = slice->GetConstLoopIterator();
           LoopTreeNode *loop = iter.Current();
           SymbolicBound b = loop->GetLoopInfo()->GetBound();
           SymbolicVal size = b.ub - b.lb + 1;
 
           l.push_back(fa.CreateConstInt(1));
           l.push_back(size.CodeGen(fa));
           AstNodePtr init = fa.CreateFunctionCall("getTuningValue", l);
           return SymbolicVar(fa.NewVar(fa.GetType("int"), "",true,AST_NULL, init),AST_NULL); 
       }
    }

void LoopNoBlocking::
SetBlocking(AstInterface& fa,  CompSliceLocalityRegistry *anal, CompSliceNest& n) 
   {
       blocksize.clear();
       for (int i = 0; i < n.NumberOfEntries(); ++i) 
          blocksize.push_back(1);
   }

void OuterLoopReuseBlocking ::
SetBlocking( AstInterface& fa, CompSliceLocalityRegistry *anal, CompSliceNest& n)
     { 
         blocksize.clear();
         unsigned num = n.NumberOfEntries();
         int reuseLevel = SliceNestReuseLevel(anal, n); 
	 for (int i = 0; i < reuseLevel; ++i)
             blocksize.push_back(1);
         size_t index;
         for ( index = reuseLevel; index < num-spill; ++index)  {
             blocksize.push_back(GetDefaultBlockSize(fa, n[index]));
         }
	 for (; index < num; ++index)
             blocksize.push_back(1);
   }

void InnerLoopReuseBlocking ::
SetBlocking( AstInterface& fa, CompSliceLocalityRegistry *anal, CompSliceNest& n)
{ 
         blocksize.clear();
         unsigned num = n.NumberOfEntries();
         int reuseLevel = SliceNestReuseLevel(anal, n); 
         blocksize.resize(reuseLevel + 1, 1);
         for ( size_t index = reuseLevel+1; index < num; ++index) 
             blocksize.push_back(GetDefaultBlockSize(fa,n[index]));
}

void AllLoopReuseBlocking ::
SetBlocking( AstInterface& fa,CompSliceLocalityRegistry *anal, CompSliceNest& n)
     { 
         blocksize.clear();
         unsigned num = n.NumberOfEntries();
         if (num == 1) {
            blocksize.push_back(1);
            return;
         }
         int reuseLevel = SliceNestReuseLevel(anal, n); 
	 for (int i = 0; i < reuseLevel; ++i)
             blocksize.push_back(1);
         for ( size_t index = reuseLevel; index < num; ++index) 
             blocksize.push_back(GetDefaultBlockSize(fa,n[index]));
   }


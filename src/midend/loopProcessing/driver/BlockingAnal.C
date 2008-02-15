#include <general.h>
#include <BlockingAnal.h>

static int SliceNestReuseLevel(CompSliceLocalityRegistry *anal, CompSliceNest& n)
     { 
       unsigned num = n.NumberOfEntries();
       unsigned int reuseLevel = 0;
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
           AstInterface::AstNodeList l = AstInterface::CreateList();
           AstInterface::ListAppend(l,AstInterface::CreateConstInt(dt));

           CompSlice::ConstLoopIterator iter = slice->GetConstLoopIterator();
           LoopTreeNode *loop = iter.Current();
           SymbolicBound b = loop->GetLoopInfo()->GetBound();
           SymbolicVal size = b.ub - b.lb + 1;
 
           AstInterface::ListAppend(l,AstInterface::CreateConstInt(1));
           AstInterface::ListAppend(l,size.CodeGen(fa));
           AstNodePtr init = fa.CreateFunctionCall("getTuningValue",
		                                          "int getTuningValue(int,int,int);",l);
           return SymbolicVar(fa.NewVar(fa.GetType("int"), "",true,0, init),0); 
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
         unsigned int reuseLevel = SliceNestReuseLevel(anal, n); 
	 for (unsigned int i = 0; i < reuseLevel; ++i)
             blocksize.push_back(1);
         unsigned int index;
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
         unsigned int reuseLevel = SliceNestReuseLevel(anal, n); 
	 for (unsigned int i = 0; i <= reuseLevel; ++i)
             blocksize.push_back(1);
         for ( unsigned int index = reuseLevel+1; index < num; ++index) 
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
         unsigned int reuseLevel = SliceNestReuseLevel(anal, n); 
	 for (unsigned int i = 0; i < reuseLevel; ++i)
             blocksize.push_back(1);
         for ( unsigned int index = reuseLevel; index < num; ++index) 
             blocksize.push_back(GetDefaultBlockSize(fa,n[index]));
   }


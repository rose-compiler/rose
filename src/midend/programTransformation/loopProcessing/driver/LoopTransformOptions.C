


#include <iostream>

#include <LoopTransformOptions.h>
#include <BlockingAnal.h>
#include <InterchangeAnal.h>
#include <FusionAnal.h>
#include <CommandOptions.h>
#include <CopyArrayAnal.h>

using namespace std;

class DynamicTuning {
  static int dt;  
 public:
  static bool Do();
  static int increase_index() { return dt++; }
};

int DynamicTuning::dt = 0;  
bool DynamicTuning::Do()
{
  if (dt == 0) {
    dt = CmdOptions::GetInstance()->HasOption("-dt")? 1 : -1;
  }
  return dt > 0;
}


static void ReadDefaultBlockSize(LoopTransformOptions& opt, vector<string>& argv, unsigned index)
{
 assert (index < argv.size());
 int defaultblocksize = atoi(argv[index].c_str());
 if (defaultblocksize <= 1) {
       std::cerr << "Missing or invalid blocking size: " << argv[index] << "; Use default (16)\n";
       defaultblocksize = 16;
 } else {
       argv.erase(argv.begin() + index);
 }
 opt.SetDefaultBlockSize(defaultblocksize);
}

static int ReadDefaultCopyDim(LoopTransformOptions& opt, vector<string>& argv, unsigned index)
{
 assert (index < argv.size());
 assert (!argv[index].empty());
 int defaultcopysize = 0;
 char hint = argv[index][0]; 
 if (hint >= '0' && hint <= '9' ) {
       defaultcopysize = atoi(argv[index].c_str());
       argv.erase(argv.begin() + index);
 }
 else {
    std::cerr << "invalid array copy dimension: " << argv[index] << "; Use default (0)\n";
 }
 return defaultcopysize;
}
class BlockOuterLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv) 
      { 
        argv.erase(argv.begin() + index);
        ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new OuterLoopReuseBlocking( 1 )); 
      }
  public:
     BlockOuterLoopOpt() : OptRegistryType("-bk1", " <blocksize> :block outer loops") {}
};

class BlockInnerLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
      { 
        argv.erase(argv.begin() + index);
        ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new InnerLoopReuseBlocking());
      }
  public:
     BlockInnerLoopOpt() : OptRegistryType("-bk2", " <blocksize> :block inner loops") {}
};

class BlockAllLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
      {
        argv.erase(argv.begin() + index);
        ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new AllLoopReuseBlocking());
      }
  public:
     BlockAllLoopOpt() : OptRegistryType("-bk3", " <blocksize> :block all loops") {}
};

class CopyArrayOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
      {
        argv.erase(argv.begin() + index);
        int dim = ReadDefaultCopyDim(opt,argv,index); 
        opt.SetCopySel( new CopyArrayUnderSizeLimit(dim));
      }
  public:
     CopyArrayOpt() : OptRegistryType("-cp", " <copydim> :copy array regions with dimensions <= <copydim>") {}
};


class ReuseInterchangeOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { argv.erase(argv.begin() + index); opt.SetInterchangeSel( new ArrangeReuseOrder() ); }
  public:
     ReuseInterchangeOpt() : OptRegistryType("-ic1", " :loop interchange for more reuses") {}
};

class FissionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { argv.erase(argv.begin() + index); opt.SetFusionSel( new LoopNestFusion() ); }
  public:
     FissionOpt() : OptRegistryType("-fs0", " : maximum distribution at all loops") {}
};

class InnerFissionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { argv.erase(argv.begin() + index); opt.SetFusionSel( new SameLevelFusion( new InnermostLoopFission() ) ); }
  public:
     InnerFissionOpt() : OptRegistryType("-fs01", " : maximum distribution at inner-most loops") {}
};

class SingleReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { argv.erase(argv.begin() + index); opt.SetFusionSel( new SameLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
     SingleReuseFusionOpt() 
        : OptRegistryType("-fs1", " :single-level loop fusion for more reuses") {}
};

class MultiReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { argv.erase(argv.begin() + index); opt.SetFusionSel( new MultiLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
    MultiReuseFusionOpt() 
     : OptRegistryType("-fs2",  " :multi-level loop fusion for more reuses") {}
};

class SplitLimitOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { 
            argv.erase(argv.begin() + index);
            assert (index < argv.size());
            int limit = atoi(argv[index].c_str());
            argv.erase(argv.begin() + index);
            opt.SetTransAnalSplitLimit( limit ) ; 
         }
  public:
   SplitLimitOpt() 
      : OptRegistryType("-ta", " <int> :split limit for transitive dep. analysis") {}
};

class CacheLineSizeOpt : public LoopTransformOptions::OptRegistryType
{ 
  virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         { 
           argv.erase(argv.begin() + index);
           int size = atoi(argv[index].c_str()); 
           opt.SetCacheLineSize(size);
           argv.erase(argv.begin() + index);
         }
 public:
   CacheLineSizeOpt() : OptRegistryType("-clsize", " <int> :set cache line size") {}
};

class ReuseDistOpt : public LoopTransformOptions::OptRegistryType
{
  virtual void operator()( LoopTransformOptions &opt, unsigned index, vector<string>& argv)
         {
           argv.erase(argv.begin() + index);
           assert (index < argv.size());
           int size = atoi(argv[index].c_str());
           opt.SetReuseDistance(size);
           argv.erase(argv.begin() + index);
         }
 public:
   ReuseDistOpt() : OptRegistryType("-reuse_dist", " <int> :set reuse distance") {}
};
                                                                                                                                                                                                     
LoopTransformOptions:: LoopTransformOptions()
       : bkOp(0), cpOp(0), cacheline(16), reuseDist(8), splitlimit(20)
{
   icOp =  new ArrangeOrigNestingOrder() ;
   fsOp = new SameLevelFusion( new OrigLoopFusionAnal() );
}

LoopTransformOptions::~LoopTransformOptions() 
{ 
  delete icOp; delete fsOp; 
  if (bkOp != 0) 
     delete bkOp; 
  if (cpOp != 0)
     delete cpOp;
}

void LoopTransformOptions::SetBlockSel( LoopBlockingAnal* sel) 
{ 
  if (bkOp != 0) 
    delete bkOp; 
  bkOp = sel; 
}

void LoopTransformOptions::SetCopySel( CopyArrayOperator* sel) 
{ 
   if (cpOp != 0)
        delete cpOp; 
   cpOp = sel; 
}
void LoopTransformOptions::SetInterchangeSel( ArrangeNestingOrder* sel) 
{ 
   delete icOp; 
  icOp = sel; 
}

void LoopTransformOptions::SetFusionSel( LoopNestFusion* sel)  
{ 
  delete fsOp; 
  fsOp = sel; 
}

LoopTransformOptions::OptType LoopTransformOptions::GetOptimizationType()
   { 
     int t = (fsOp->GetOptimizationType() | ((bkOp == 0)? NO_OPT : bkOp->GetOptimizationType())
             | ((cpOp == 0)? NO_OPT : cpOp->GetOptimizationType()) 
             | icOp->GetOptimizationType());
     return (OptType)t;
   };

LoopTransformOptions* LoopTransformOptions::inst = 0;
LoopTransformOptions* LoopTransformOptions::GetInstance()
{
  if (inst == 0) {
     inst = new LoopTransformOptions();
     inst->RegisterOption( new BlockOuterLoopOpt);
     inst->RegisterOption( new BlockInnerLoopOpt);
     inst->RegisterOption( new BlockAllLoopOpt);
     inst->RegisterOption( new CopyArrayOpt);
     inst->RegisterOption( new ReuseInterchangeOpt);
     inst->RegisterOption( new FissionOpt);
     inst->RegisterOption( new InnerFissionOpt);
     inst->RegisterOption( new SingleReuseFusionOpt);
     inst->RegisterOption( new MultiReuseFusionOpt);
     inst->RegisterOption( new SplitLimitOpt);
     inst->RegisterOption( new CacheLineSizeOpt);
     inst->RegisterOption( new ReuseDistOpt);
  }
  return inst;
}

void LoopTransformOptions :: PrintUsage(std::ostream& stream) const
{  
   for ( SinglyLinkedListWrap <OptRegistryType*>::Iterator p(reg); !p.ReachEnd(); ++p) {
      stream << (*p)->GetName() << (*p)->GetExpl() << "\n";
   }
   stream << "-dt :perform dynamic tuning" << std::endl;
}

bool LoopTransformOptions :: DoDynamicTuning() const
{
  return DynamicTuning::Do();
}

unsigned LoopTransformOptions :: GetDynamicTuningIndex() const
{
  return DynamicTuning::increase_index();
}

void LoopTransformOptions :: RegisterOption( OptRegistryType* t)
{
   reg.AppendLast(t);
}

void LoopTransformOptions :: SetOptions (std::vector<std::string>& argvList)
      {
        for (size_t i = 1;
             i < argvList.size() /* May shrink, so this must be checked in every iteration */ ;
             /* Options should be removed by handlers */ ) {
            SinglyLinkedListWrap <OptRegistryType*>::Iterator p(reg); 
            bool isValidOption = false;
            for ( ; !p.ReachEnd(); ++p) {
                std::string name = (*p)->GetName();
                if ( argvList[i] == name) {
                    (*(*p))(*this, i, argvList);
                    isValidOption = true;
                    break;
                }
            }
            if (!isValidOption) {++i;} // Skip this option
         }
      }




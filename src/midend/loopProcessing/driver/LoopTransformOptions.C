
#include <iostream>

#include <LoopTransformOptions.h>
#include <BlockingAnal.h>
#include <InterchangeAnal.h>
#include <FusionAnal.h>
#include <CommandOptions.h>
#include <CopyArrayAnal.h>

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


static void ReadDefaultBlockSize(LoopTransformOptions& opt, char** argv, unsigned& index)
{
 int defaultblocksize = atoi(argv[index]);
 if (defaultblocksize <= 1) {
       STD cerr << "invalid blocking size: " << argv[index] << "; Use default (16)\n";
       defaultblocksize = 16;
       --index;
 }
 opt.SetDefaultBlockSize(defaultblocksize);
}

static int ReadDefaultCopyDim(LoopTransformOptions& opt, char** argv, unsigned& index)
{
 int defaultcopysize = 0;
 char hint = argv[index][0]; 
 if (hint >= '0' && hint <= '9' ) {
       defaultcopysize = atoi(argv[index]);
 }
 else {
    STD cerr << "invalid array copy dimension: " << argv[index] << "; Use default (0)\n";
    --index;
 }
 return defaultcopysize;
}
class BlockOuterLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv) 
      { 
        ++index;
	ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new OuterLoopReuseBlocking( 1 )); 
      }
  public:
     BlockOuterLoopOpt() : OptRegistryType("-bk1", " <blocksize> :block outer loops") {}
};

class BlockInnerLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
      { 
        ++index;
	ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new InnerLoopReuseBlocking());
      }
  public:
     BlockInnerLoopOpt() : OptRegistryType("-bk2", " <blocksize> :block inner loops") {}
};

class BlockAllLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
      {
        ++index;
	ReadDefaultBlockSize(opt,argv,index); 
        opt.SetBlockSel( new AllLoopReuseBlocking());
      }
  public:
     BlockAllLoopOpt() : OptRegistryType("-bk3", " <blocksize> :block all loops") {}
};

class CopyArrayOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
      {
        ++index;
	int dim = ReadDefaultCopyDim(opt,argv,index); 
        opt.SetCopySel( new CopyArrayUnderSizeLimit(dim));
      }
  public:
     CopyArrayOpt() : OptRegistryType("-cp", " <copydim> :copy array regions with dimensions <= <copydim>") {}
};


class ReuseInterchangeOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { opt.SetInterchangeSel( new ArrangeReuseOrder() ); }
  public:
     ReuseInterchangeOpt() : OptRegistryType("-ic1", " :loop interchange for more reuses") {}
};

class FisionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { opt.SetFusionSel( new LoopNestFusion() ); }
  public:
     FisionOpt() : OptRegistryType("-fs0", " : maximum distribution at all loops") {}
};

class InnerFisionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { opt.SetFusionSel( new SameLevelFusion( new InnermostLoopFision() ) ); }
  public:
     InnerFisionOpt() : OptRegistryType("-fs01", " : maximum distribution at inner-most loops") {}
};

class SingleReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { opt.SetFusionSel( new SameLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
     SingleReuseFusionOpt() 
        : OptRegistryType("-fs1", " :single-level loop fusion for more reuses") {}
};

class MultiReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { opt.SetFusionSel( new MultiLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
    MultiReuseFusionOpt() 
     : OptRegistryType("-fs2",  " :multi-level loop fusion for more reuses") {}
};

class SplitLimitOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { 
            index++;
            int limit = atoi(argv[index]);
            opt.SetTransAnalSplitLimit( limit ) ; 
         }
  public:
   SplitLimitOpt() 
      : OptRegistryType("-ta", " <int> :split limit for transitive dep. analysis") {}
};

class CacheLineSizeOpt : public LoopTransformOptions::OptRegistryType
{ 
  virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         { 
           index ++;
           int size = atoi(argv[index]); 
           opt.SetCacheLineSize(size);
         }
 public:
   CacheLineSizeOpt() : OptRegistryType("-clsize", " <int> :set cache line size") {}
};

class ReuseDistOpt : public LoopTransformOptions::OptRegistryType
{
  virtual void operator()( LoopTransformOptions &opt, unsigned &index, char** argv)
         {
           index ++;
           int size = atoi(argv[index]);
           opt.SetReuseDistance(size);
         }
 public:
   ReuseDistOpt() : OptRegistryType("-reuse_dist", " <int> :set reuse distance") {}
};
                                                                                                                                                                                                     
LoopTransformOptions:: LoopTransformOptions()
       : cacheline(16), splitlimit(20), reuseDist(8), cpOp(0), bkOp(0)
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
     inst->RegisterOption( new FisionOpt);
     inst->RegisterOption( new InnerFisionOpt);
     inst->RegisterOption( new SingleReuseFusionOpt);
     inst->RegisterOption( new MultiReuseFusionOpt);
     inst->RegisterOption( new SplitLimitOpt);
     inst->RegisterOption( new CacheLineSizeOpt);
     inst->RegisterOption( new ReuseDistOpt);
  }
  return inst;
}

void LoopTransformOptions :: PrintUsage(STD ostream& stream) const
{  
   for ( SinglyLinkedListWrap <OptRegistryType*>::Iterator p(reg); !p.ReachEnd(); ++p) {
      stream << (*p)->GetName() << (*p)->GetExpl() << "\n";
   }
   stream << "-dt :perform dynamic tuning" << STD endl;
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

int LoopTransformOptions :: SetOptions (int argc, char* argv[])
      {
        int index = 1;
        for (unsigned i = 1; i < argc; ++i) {
            SinglyLinkedListWrap <OptRegistryType*>::Iterator p(reg); 
            for ( ; !p.ReachEnd(); ++p) {
                if ( argv[i] != 0 && !strcmp( (*p)->GetName().c_str(), argv[i])) {
                    (*(*p))(*this, i, argv);
                    break;
                }
            }
            if (p.ReachEnd())  {
               if (index < i) 
                  argv[index] = argv[i]; 
                ++index;
            }
         }
          return index;
      }




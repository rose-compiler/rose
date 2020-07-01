
#include <iostream>

#include <LoopTransformOptions.h>
#include <BlockingAnal.h>
#include <InterchangeAnal.h>
#include <FusionAnal.h>
#include <CommandOptions.h>
#include <CopyArrayAnal.h>
#include <ParallelizeLoop.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

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

inline unsigned ReadUnsignedInt(LoopTransformOptions& opt, const std::vector<std::string>& argv, unsigned& index, const std::string& name, unsigned def_value)
{
 int val = -1;
 if (index+1 < argv.size())
 {
      const std::string& content = argv[index+1];
      if (content[0] <= '9' && content[0] >= '0')
      {
        val = atoi(content.c_str());
        ++index;
        std::cerr << name << " is " << val << "\n";
        return val;
      }
 }
 std::cerr << "Invalid " << name << "; Use default (" << def_value << ")\n";
 return def_value;
}

class BlockParameterizeOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        opt.SetDefaultBlockSize( ReadUnsignedInt(opt,argv,index,"block size", 16));
        opt.SetBlockSel( new ParameterizeBlocking());
      }
  public:
     BlockParameterizeOpt() : OptRegistryType("-bk_poet", " <blocksize> : parameterize the blocking transformation") {}
};

class POETParallelizeOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        opt.SetParBlockSize( ReadUnsignedInt(opt,argv,index,"parallel block size", 256));
        opt.SetParSel( new ParallelizeBlocking());
      }
  public:
     POETParallelizeOpt() : OptRegistryType("-par_poet", " <blocksize> : paralleization transformation using POET") {}
};

class BlockOuterLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        opt.SetDefaultBlockSize( ReadUnsignedInt(opt,argv,index,"block size", 16));
        opt.SetBlockSel( new OuterLoopReuseBlocking( 1 ));
      }
  public:
     BlockOuterLoopOpt() : OptRegistryType("-bk1", " <blocksize> :block outer loops") {}
};

class BlockInnerLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        opt.SetDefaultBlockSize( ReadUnsignedInt(opt,argv,index,"block size", 16));
        opt.SetBlockSel( new InnerLoopReuseBlocking());
      }
  public:
     BlockInnerLoopOpt() : OptRegistryType("-bk2", " <blocksize> :block inner loops") {}
};

class BlockAllLoopOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        opt.SetDefaultBlockSize( ReadUnsignedInt(opt,argv,index,"block size", 16));
        opt.SetBlockSel( new AllLoopReuseBlocking());
      }
  public:
     BlockAllLoopOpt() : OptRegistryType("-bk3", " <blocksize> :block all loops") {}
};

class CopyArrayDimensionOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        int dim = ReadUnsignedInt(opt,argv,index,"array-copy dimension", 0);
        opt.SetCopySel( new CopyArrayUnderSizeLimit(dim));
      }
  public:
     CopyArrayDimensionOpt() : OptRegistryType("-cp", " <copydim> :copy array regions with dimensions <= <copydim>") {}
};

class ParameterizeCopyArrayOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      {
        int dim = ReadUnsignedInt(opt,argv,index,"array-copy dimension", 0);
        opt.SetCopySel( new ParameterizeCopyArray(dim));
      }
  public:
     ParameterizeCopyArrayOpt() : OptRegistryType("-cp_poet", "<copydim> :parameterize array copy array regions; to be applied together with blocking.") {}
};

class ReuseInterchangeOpt : public LoopTransformOptions::OptRegistryType
{
    virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         { opt.SetInterchangeSel( new ArrangeReuseOrder() ); }
  public:
     ReuseInterchangeOpt() : OptRegistryType("-ic1", " :loop interchange for more reuses") {}
};

class FissionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         { opt.SetFusionSel( new LoopNestFusion() ); }
  public:
     FissionOpt() : OptRegistryType("-fs0", " : maximum distribution at all loops") {}
};

class InnerFissionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
     {
       opt.SetFusionSel( new SameLevelFusion( new InnermostLoopFission() ) ); }
  public:
     InnerFissionOpt() : OptRegistryType("-fs01", " : maximum distribution at inner-most loops") {}
};

class SingleReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
      { opt.SetFusionSel( new SameLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
     SingleReuseFusionOpt()
        : OptRegistryType("-fs1", " :single-level loop fusion for more reuses") {}
};

class MultiReuseFusionOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         { opt.SetFusionSel( new MultiLevelFusion( new AnyReuseFusionAnal() ) ); }
  public:
    MultiReuseFusionOpt()
     : OptRegistryType("-fs2",  " :multi-level loop fusion for more reuses") {}
};

class SplitLimitOpt : public LoopTransformOptions::OptRegistryType
{
   virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         {
            int limit = ReadUnsignedInt(opt,argv,index,"split limit", 20);
            opt.SetTransAnalSplitLimit( limit ) ;
         }
  public:
   SplitLimitOpt()
      : OptRegistryType("-ta", " <int> :split limit for transitive dep. analysis") {}
};

class CacheLineSizeOpt : public LoopTransformOptions::OptRegistryType
{
  virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         {
           int size = 0;
           opt.SetCacheLineSize(size);
         }
 public:
   CacheLineSizeOpt() : OptRegistryType("-clsize", " <int> :set cache line size") {}
};

class ReuseDistOpt : public LoopTransformOptions::OptRegistryType
{
  virtual void operator()( LoopTransformOptions &opt, unsigned& index, const std::vector<std::string>& argv)
         {
           unsigned size = ReadUnsignedInt(opt,argv,index,"reuse distance", 4);
           opt.SetReuseDistance(size);
         }
 public:
   ReuseDistOpt() : OptRegistryType("-reuse_dist", " <int> :set reuse distance") {}
};

LoopTransformOptions:: LoopTransformOptions()
       : parOp(0), cpOp(0), cacheline(16), reuseDist(8), splitlimit(20)
{
   icOp =  new ArrangeOrigNestingOrder() ;
   fsOp = new SameLevelFusion( new OrigLoopFusionAnal() );
   bkOp = new LoopNoBlocking();
}

LoopTransformOptions::~LoopTransformOptions()
{
  delete icOp; delete fsOp;
  if (bkOp != 0)
     delete bkOp;
  if (cpOp != 0)
     delete cpOp;
}

void LoopTransformOptions::SetParSel( LoopPar* sel)
{
  if (parOp != 0)
    delete parOp;
  parOp = sel;
}

void LoopTransformOptions::SetBlockSel( LoopBlocking* sel)
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
     inst->RegisterOption( new BlockParameterizeOpt);
     inst->RegisterOption( new POETParallelizeOpt);
     inst->RegisterOption( new BlockOuterLoopOpt);
     inst->RegisterOption( new BlockInnerLoopOpt);
     inst->RegisterOption( new BlockAllLoopOpt);
     inst->RegisterOption( new CopyArrayDimensionOpt);
     inst->RegisterOption( new ParameterizeCopyArrayOpt);
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

void LoopTransformOptions ::
SetOptions (const std::vector<std::string>& argvList, std::vector<std::string>* unknown_args)
      {
        for (unsigned index = 0; index < argvList.size(); ++index)
        {
           const std::string& opt_name=argvList[index];
           SinglyLinkedListWrap <OptRegistryType*>::Iterator p_registry(reg);
           for ( ; !p_registry.ReachEnd(); ++p_registry)
           {
                OptRegistryType* reg = *p_registry;
                if (opt_name == reg->GetName())
                {
                    (*reg)(*this, index, argvList);
                     break;
                }
           }
           if (unknown_args != 0 && p_registry.ReachEnd())
              unknown_args->push_back(opt_name);
        }
     }

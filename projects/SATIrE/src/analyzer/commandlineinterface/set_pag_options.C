// Author: Markus Schordan, 2006

#include "set_pag_options.h"

extern char* animation;
extern int cfg_ordering;
extern int sel_mapping;
extern int startbanks;
extern int low_perc;
extern int high_perc;
extern int share_min;
extern int share_num;
extern int debug_stat;
extern int global_print_post_info;
extern int global_print_pre_info;
extern int global_proc_as_graphs;
extern int verbose;

void setPagOptions(AnalyzerOptions opt) {
  cfg_ordering=opt.getCfgOrdering();
  low_perc=opt.getGcLow();
  high_perc=opt.getGcHigh();
  debug_stat=opt.statistics(); // bool->int

  //output=(char*)(opt.getGdlFileName().c_str());
  animation=(char*)(opt.getAnimationDirectoryName().c_str());  
  //noresult=!opt.resultGeneration();

  if(!opt.animationGeneration())
    animation=NULL;
  int csl=opt.getCallStringLength();
  if(csl==0 || csl==1) { 
    sel_mapping=csl; 
  }
  if(csl>=2) {
    sel_mapping=2;
    mapping_data.map_n = csl;
  }
  if(csl==-1) {
    sel_mapping=5;
  }
  global_print_post_info=opt.postInfo();
  global_print_pre_info=opt.preInfo();
  global_proc_as_graphs=opt.gdlProcedureSubgraphs();

  //start_bank=opt.getStartBank();
  //share_min=opt.getShareMin();
  //share_num=opt.getShareNum();

  if(opt.vivu()) {
    mapping_data.map_l = opt.getVivuLoopUnrolling(); // [default 2]
    if(opt.getCallStringLength()==1) {
      sel_mapping=3; // VIVU 1 (simple)
    }
    if(opt.getCallStringLength()==2) {
      sel_mapping=4; // VIVU 2 (extended)
    }
    if(opt.getCallStringLength()>=3) {
      sel_mapping=5; // VIVU ht (extended), callstringlength defines chop size
    }
  }
  if(opt.getVivu4MaxUnrolling()!=-1) {
    sel_mapping=6;
    mapping_data.map_global_max_cut=opt.getVivu4MaxUnrolling();
  }
}

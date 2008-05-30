// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: set_pag_options.C,v 1.7 2008-05-30 13:44:07 markus Exp $

// Author: Markus Schordan, 2006+

#include "set_pag_options.h"
#include "o_VariableId.h"
#include "o_ExpressionId.h"
#include <iostream>

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
extern int global_retfunc;

void setPagOptions(AnalyzerOptions opt) {
  cfg_ordering=opt.getCfgOrdering();
  low_perc=opt.getGcLow();
  high_perc=opt.getGcHigh();
  debug_stat=opt.statistics(); // bool->int

  //output=(char*)(opt.getGdlFileName().c_str());
  animation=(char*)(opt.getOutputGdlAnimDirName().c_str());  
  //noresult=!opt.resultGeneration();

  if(!opt.outputGdlAnim())
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

  int formatMode=0;
  {
    VariableId::PrintFormat printMode;

    if(opt.nodeFormatVarId()) {
      formatMode+=1;
    }
    if(opt.nodeFormatVarName()) {
      formatMode+=2;
    }
    switch(formatMode) {
    case 1: printMode=VariableId::F_Id;break;
    case 2: printMode=VariableId::F_Name;break;
    case 3: printMode=VariableId::F_IdAndName;break;
    default: std::cerr << "Wrong format mode or print mode for nodes (Variable) in gdl file"; std::abort();
    }
    VariableId::setPrintFormat(printMode);
  }
  formatMode=0;
  {
    ExpressionId::PrintFormat printMode;

    if(opt.nodeFormatExprId()) {
      formatMode+=1;
    }
    if(opt.nodeFormatExprSource()) {
      formatMode+=2;
    }
    switch(formatMode) {
    case 1: printMode=ExpressionId::F_Id;break;
    case 2: printMode=ExpressionId::F_Expression;break;
    case 3: printMode=ExpressionId::F_IdAndExpression;break;
    default: std::cerr << "Wrong format mode or print mode for nodes (Expression) in gdl file"; std::abort();    
    }
    ExpressionId::setPrintFormat(printMode);
  }

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
  if(opt.retFuncUsed()) {
    /* use retfunc for combining information from local and return edge */
    global_retfunc = 1;
  }
}

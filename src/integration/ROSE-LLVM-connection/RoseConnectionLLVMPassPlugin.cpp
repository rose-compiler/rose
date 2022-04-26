#include "RoseConnectionLLVMPassPlugin.hpp"

extern SgProject* roseProject;

//-----------------------------------------------------------------------------
// ROSEPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.

using namespace RoseConnectionLLVMPassPlugin;

// New PM implementation
PreservedAnalyses ROSEPass::run(Module &M, ModuleAnalysisManager &MAM) {

  // std::cout <<"Module info:" << M.getInstructionCount ()  << std::endl;
  auto &FAM = MAM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();

  // check if debug information is given
  checkCompiledWithDebugInfo(M);  
 
  auto  &funcList = M.getFunctionList();
  Module::global_iterator gv_iter;
  Module::iterator func_iter;

// Get ROSE node mapping in the beginning
  ROSENodeMap = getRoseNodeInfo();

  for(auto a:ROSENodeMap)
  {
    SgNode* node = a.first;
    std::pair<int,int> srcinfo  = a.second;
    outs() << "SgNode(" << node << "):" << " src info: [" << srcinfo.first << ":" << srcinfo.second << "]  \tclass Name:" << node->class_name () << "\n";
  }


  std::string header = "==================================================";

  for (gv_iter = M.global_begin(); gv_iter != M.global_end(); gv_iter++)
  {
     outs() << header << "\n";
     runOnGVariable(*gv_iter);
  }
  
  // Functions
  for (func_iter = M.begin(); func_iter != M.end(); func_iter++)
  {

     // check the alias analysis only when the function definition is defined
     if(!func_iter->isDeclaration())
     {
       outs() << header << "\n";
       runOnFunction(*func_iter, FAM);
     }
  }

  std::cout << "In LLVM Pass: sgproject:" << roseProject << std::endl;
  std::cout << "In LLVM Pass: calling unparser"  << std::endl;
  ::backend(roseProject);

  return PreservedAnalyses::all();
}

PreservedAnalyses ROSEPass::runOnFunction(Function &F, FunctionAnalysisManager &FAM)
{
	unsigned int i = 0;
	Function::arg_iterator arg_iter;
	Function::iterator	bb_iter;
	BasicBlock::iterator inst_iter;
   
        AAResults& AAR = FAM.getResult<AAManager>(F);
        DependenceInfo& Dinfo = FAM.getResult<DependenceAnalysis >(F);

  	outs() << "Name: " << F.getName() << "\n";
	
	// Return type
	outs() << i << ". Return Type: " << *F.getReturnType() << "\n";
	i += 1;

	// Arguments
	outs() << i << ". Arguments: ";
	if (F.arg_size() == 0)
	{
		outs() << "No Arguments" << "\n";
	}
	else
	{
		for (arg_iter = F.arg_begin(); arg_iter != F.arg_end(); arg_iter++)
		{
			outs() << *arg_iter;
			
			if (arg_iter != F.arg_end())
			{
				outs() << ", ";
			}
		}

		outs() << "\n";
	}
	i += 1;

        // map to record all operands, and their line/column info from the instruction
        std::map<Value*, std::pair<int,int>> valueList;
        std::map<Instruction*, std::pair<int,int>> instList;
	// BasicBlocks
	outs() << i << ". IR: " << "\n";
	if (F.isDeclaration() == true)
	{
		outs() << "Declaration. No IR" << "\n";
	}
	else
	{
		for (bb_iter = F.begin(); bb_iter != F.end(); bb_iter++)
		{
			// Each BB is made of one/more instructions.
			// Print them.
			for (inst_iter = (*bb_iter).begin(); inst_iter != (*bb_iter).end(); inst_iter++)
			{
                                std::pair<int, int> srcinfo;
                                StringRef File;
                                StringRef Dir;
                                if(hasDebugInfo())
                                if (DILocation *Loc = inst_iter->getDebugLoc()) { // Here *inst_iter is an LLVM instruction
                                    //Line = Loc->getLine();
                                    //Column = inst_iter->getDebugLoc()->getColumn();
                                    srcinfo.first = Loc->getLine();
                                    srcinfo.second = inst_iter->getDebugLoc()->getColumn();
                                    File = Loc->getFilename();
                                    Dir = Loc->getDirectory();
                                    // outs() << "inst file: "<< File << " line:column = " << Line << ":" << Column  << "\n"; 
                                }

                                Instruction* inst = &*inst_iter;
                                if(instList.find(inst) == instList.end())
                                   instList.insert({inst,srcinfo});

                                for(unsigned opi = 0; opi < inst_iter->getNumOperands() ; opi++)
                                {
                                  Value* val = inst_iter->getOperand (opi);
                                  if(valueList.find(val) == valueList.end())
                                     valueList.insert({val,srcinfo});
                                }
                                if(hasDebugInfo())	
				  outs() << "[" << srcinfo.first << ":" << srcinfo.second << "] ";
                                outs()  <<  *inst_iter << "\n";
			}
		}
	}

        outs()  << "================================================== " << "\n";
        outs()  << "Data dependence analysis: " << "\n";
        for(std::map<Instruction*, std::pair<int,int>>::iterator ii = instList.begin(); ii != instList.end(); ii++)
        {
          for(std::map<Instruction*, std::pair<int,int>>::iterator jj = std::next(ii); jj != instList.end(); jj++)
          {
             
             std::string str1, str2;
             Instruction* i1 = ii->first;
             llvm::raw_string_ostream(str1) << *i1;
             std::pair<int,int> src1 = ii->second;
             std::string i1info = getInstInfo(i1, src1);

             Instruction* i2 = jj->first;
             llvm::raw_string_ostream(str2) << *i2;
             std::pair<int,int> src2 = jj->second;
             std::string i2info = getInstInfo(i2, src2);

             std::unique_ptr< Dependence >  DAresult = Dinfo.depends(i1, i2, false);
             if(DAresult != nullptr)
             {
               outs() << getDAResult(DAresult) << "\n";
               outs() << "\t inst 1: " << str1 << "\n" << i1info << "\n" ;
               outs() << "\t inst 2: " << str2 << "\n" << i2info << "\n" ;
             }
          }
        }
        outs()  << "================================================== " << "\n";
        outs()  << "Alias analysis: " << "\n";
        for(std::map<Value*, std::pair<int,int>>::iterator ii = valueList.begin(); ii != valueList.end(); ii++)
        {
          for(std::map<Value*, std::pair<int,int>>::iterator jj = std::next(ii); jj != valueList.end(); jj++)
          {
             Value* v1 = ii->first;
             std::pair<int,int> src1 = ii->second;
             std::string v1info = getOperandInfo(v1, src1);

             Value* v2 = jj->first;
             std::pair<int,int> src2 = jj->second;
             std::string v2info = getOperandInfo(v2, src2);

             const AliasResult::Kind result = AAR.alias(v1, v2);
             if(static_cast<int>(result) != 0)
             {
                outs() <<  getAliasResult(result) << ":\n";
                outs() << "\t op1: " << v1info << "\n" ;
                outs() << "\t op2: " << v2info << "\n" ;
             }
          }
        }

  	return PreservedAnalyses::all();
}

void ROSEPass::checkCompiledWithDebugInfo(const Module& M) {
  isDebugInfoAvail = (M.getNamedMetadata("llvm.dbg.cu") != NULL);
}

bool ROSEPass::matchROSESrcInfo(std::pair<int,int> llvmsrcinfo)
{
  for(auto a : ROSEPass::ROSENodeMap)
  {
      if (a.second == llvmsrcinfo )
        return true;
  }
  return false;
}

std::pair<SgNode*, std::pair<int ,int >> ROSEPass::getMatchingROSESrcInfo(std::pair<int,int> llvmsrcinfo)
{
  for(auto a : ROSEPass::ROSENodeMap)
  {
      if (a.second == llvmsrcinfo )
        return a;
  }
  return {nullptr,{0,0}};
}


PreservedAnalyses ROSEPass::runOnGVariable(GlobalVariable &G)
{	
	outs() << G << "\n";
	return PreservedAnalyses::all();
}


std::string ROSEPass::getDAResult(std::unique_ptr< Dependence >& result) const {
   std::string ret;
   if(result == nullptr)
     ret = "No dependence"; 
   else if(result->isInput ())
     ret = "Input dependence"; 
   else if(result->isOutput ())
     ret = "Output dependence"; 
   else if(result->isFlow ())
     ret = "Flow dependence"; 
   else if(result->isAnti ())
     ret = "Anti dependence"; 
   else if(result->isOrdered ())
     ret = "Ordered dependence"; 
   else if(result->isUnordered ())
     ret = "Unordered dependence"; 
   return ret;
}

std::string ROSEPass::getAliasResult(AliasResult::Kind kind) const {
   std::string result;
   switch (kind) {
   case AliasResult::Kind::NoAlias:
      result =  "NoAlias";
     break;
   case AliasResult::Kind::MayAlias:
      result =  "MayAlias";
     break;
   case AliasResult::Kind::PartialAlias:
      result =  "PartialAlias";
     break;
   case AliasResult::Kind::MustAlias:
      result =  "MustAlias";
     break;
   }
   return result; 
}

std::string ROSEPass::getInstInfo(Instruction* i, std::pair<int,int> srcinfo)
{
  std::string init = "" ;
  std::string info;
  llvm::raw_string_ostream  os(init);
  if(hasDebugInfo())
    info = "\t src info: [" + std::to_string(srcinfo.first) + ":" + std::to_string(srcinfo.second) + "]\n";
  else
    info = "";
  if(hasDebugInfo() && matchROSESrcInfo(srcinfo))
  {
    std::pair<SgNode*, std::pair<int ,int >> RoseMapInfo = getMatchingROSESrcInfo(srcinfo);
    std::string SgNodeInfo = RoseMapInfo.first->unparseToString();
    if(RoseMapInfo.second.first != 0)
      info += "\t\t ROSE node Info: " + SgNodeInfo  + "\n";
    else
      info += "\t\t ROSE node mapped to SgGlobal at line 0 \n";

  }
  return info;
}

std::string ROSEPass::getOperandInfo(Value* v, std::pair<int,int> srcinfo)
{
  std::string init = "" ;
  std::string info;
  llvm::raw_string_ostream  os(init);
  v->printAsOperand(os);
  if(hasDebugInfo())
    info = "\t src info: [" + std::to_string(srcinfo.first) + ":" + std::to_string(srcinfo.second) + "]\n";
  else
    info = "";
  info += "\t\t LLVM operand info: (" + os.str() + ")\n";
  if(hasDebugInfo() && matchROSESrcInfo(srcinfo))
  {
    std::pair<SgNode*, std::pair<int ,int >> RoseMapInfo = getMatchingROSESrcInfo(srcinfo);
    std::string SgNodeInfo = RoseMapInfo.first->unparseToString();
    if(srcinfo.first != 0)
      info += "\t\t ROSE node Info: " + SgNodeInfo  + "\n";
    else
      info += "\t\t ROSE node Info: SgGlobal \n";

  }
  return info;
}

std::map<SgNode*, std::pair<int,int>> ROSEPass::getRoseNodeInfo()
{
  nodeTraversal travese;
  travese.traverseInputFiles(roseProject,preorder);
  return travese.getNodeMap();
}

void nodeTraversal::push_map_record(SgNode* node, std::pair<int,int> srcInfo)
{
  if(nodeTraversal::m.find(node) == nodeTraversal::m.end())
  {
    nodeTraversal::m.insert({node, srcInfo});
  }
  else
  {
    outs() << "SgNode " << node << " is pushed already\n" ;
  }
}

void nodeTraversal::visit(SgNode* n)
{
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if(locatedNode)
  {
     Sg_File_Info* fileInfo = locatedNode->get_file_info();
     std::pair<int , int> srcinfo = std::make_pair(fileInfo->get_line(), fileInfo->get_col());
     push_map_record(locatedNode, srcinfo); 
  }

}
//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getROSEPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "ROSEPass", LLVM_VERSION_STRING,
          // #1 REGISTRATION FOR "opt -passes=print<ROSEPass>"
          // Register ROSEPass so that it can be used when
          // specifying pass pipelines with `-passes=`.
          [](PassBuilder &PB) {
            FunctionAnalysisManager FAM;
            PB.registerFunctionAnalyses(FAM);
            ModuleAnalysisManager MAM;
            PB.registerModuleAnalyses(MAM);
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ROSEPass") {
                    MPM.addPass(ROSEPass());
                    return true;
                  }
                  return false;
                });
          // #2  Register ROSEPass as a step of an existing pipeline.
          // The insertion point is specified by using the
          // 'PB.registerPipelineStartEPCallback' callback. 
          PB.registerOptimizerLastEPCallback(
              [&](ModulePassManager &MPM, OptimizationLevel Level) {
                MPM.addPass(ROSEPass());
              });


          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize ROSEPass when added to the pass pipeline on the
// command line, i.e. via '-passes=hello-world'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getROSEPassPluginInfo();
}



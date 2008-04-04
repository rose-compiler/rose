// rose.h -- header file for the ROSE Optimizing Preprocessor

#ifndef ROSE_H
#define ROSE_H

#include "sage3.h"
#include "roseInternal.h"

// DQ (10/27/2003): Needed access to global function defined in unparser.h
// I think it makes sense to include all of the unparser into the interface
// of ROSE (don't know why this was left out previously).
#include "unparser.h"

// DQ:7/29/2002, MS:12/11/2002
// Place this at the end (since it is dependent upon ROSE classes.)
// added here to avoid placing it in each header file using the AstProcessingLib
#include <typeinfo>
#include "AstProcessing.h"
#include "AstReverseProcessing.h"
#include "AstPDFGeneration.h"
#include "AstDOTGeneration.h"
#include "AstDiagnostics.h"
// #include "AstStatistics.h"
#include "Ast.h"

// DQ (10/18/2003) Why is this commented out (what is it)
//#include "AgProcessing.h"

// Not sure that we want this here since it uses the rewrite system 
// which has not defined yet (circular reference in the header files)
// #include "AstRestructure.h"

#include "rewrite.h"
#include "roseTranslators.h"

// this is a temporary fix (will become obsolete)
#include "AstClearVisitFlags.h"

// DQ (5/26/2007): This is not depricated
// DQ (8/1/2005): Included Milind's AstMerge mechanism as standard part of ROSE.
// #include "AstMerge.h"

// DQ (5/26/2007): Use the new AST merge mechanism.
#include "merge.h"

// JH (01/18/2006): adding the include file for the AST file I/O (by Jochen)
#include "AST_FILE_IO.h"

// DQ (9/9/2007): Can't use astVisualization/ prefix since it then does not permit use from the install tree
// DQ (5/27/2007): Added astVisualization/ prefix to the header file
// DQ (2/22/2006): Added Andreas' work to graph the AST.
// #include "astVisualization/astGraph.h"
#include "astGraph.h"

// DQ (9/9/2007): Can't use astVisualization/ prefix since it then does not permit use from the install tree
// DQ (5/27/2007): Added astVisualization/ prefix to the header file
// DQ (6/23/2006): Added Andreas's work to support custom DOT graphs using persistant attributes.
// #include "astVisualization/AstAttributeDOT.h"
#include "AstAttributeDOT.h"

// DQ (3/11/2006): Jeremiah Willcock's inliner
#include "inliner.h"

// DQ (3/18/2006): Jeremiah Willcock's partial redundancy elimination (PRE)
#include "pre.h"

// DQ (4/8/2006): Constant folding of the AST (cleans out redundant 
// constant expresion trees save in translation from the frontend).
// Required to be run before PRE!
#include "constantFolding.h"

// DQ (5/8/2007): Added Robert Preissl's support for hidden type and declartion lists.
#include "HiddenList.h"
#include "HiddenList_Output.h"
#include "HiddenList_Intersection.h"

// DQ (1/25/2008): Added cfgToDot.h as suggested by Jeremiah
#include "cfgToDot.h"

// DQ (1/24/2008): Add these here to permit simple and uniform support of binaries.
#ifdef USE_ROSE_BINARY_ANALYSIS_SUPPORT
// File in src/frontend/BinaryDisassembly
  #include "RoseBin_support.h"
  #include "RoseBin_unparse.h"
#ifdef HASH_ROSE_USE_BINARYSQL
  #include "RoseBin.h"
#endif 
  #include "RoseBin_abstract.h"
  #include "RoseFile.h"
  #include "RoseObj.h"

  #include "RoseBin_DotGraph.h"
  #include "RoseBin_GmlGraph.h"
  #include "RoseBin_ControlFlowAnalysis.h"
  #include "RoseBin_DataFlowAnalysis.h"
  #include "RoseBin_CallGraphAnalysis.h"

  #include "AST_BIN_Traversal.h"

// Files in src/frontend/ObjdumpDisassembler
  #include "objdumpToRoseBinaryAst.h"

// Files in src/frontend/X86Disassembler
  #include "x86Disassembler.h"

#endif


// ifndef ROSE_H
#endif

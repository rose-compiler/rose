// rose.h -- header file for the ROSE Optimizing Preprocessor

#ifndef ROSE_H
#define ROSE_H

/* rosePublicConfig.h has some of the same CPP symbol definitions as rose_config.h, except the names have been changed so as
 * to not pollute the global name space.  All the names start with "ROSE_". */
#include "rosePublicConfig.h"

#include "sage3basic.hhh"
// DQ (4/21/2009): Andreas needs to add a comment about what this is for...
#define BOOST_WAVE_PREPROCESS_PRAGMA_BODY 1

#include "rosedefs.h"

#if 0
// Forward references to namespaces used in the binary execuable file format support.
namespace Exec{
namespace ELF{
   int xxx;
}
   int xxx;
}
#endif

// DQ (4/21/2009): This header file contains the definitions of the IR nodes.
// tps : avoid detection" header files are scanned whether they include sage3basic.h for .h and .hh

//#include "sage3.h"

// DQ (3/22/2009): This is already included in sage3.h"
// #include "roseInternal.h"

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

// DQ (6/3/2007): added internal support for AST visualization
// // #include "astVisualization/wholeAST_API.h"
#include "wholeAST_API.h"
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
#ifndef _MSC_VER
// tps (11/23/2009) : Commented out right now to make progress in Windows
#include "AST_FILE_IO.h"
#endif
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
// File in src/frontend/BinaryDisassembly
#include "RoseBin_support.h"
#ifdef HASH_ROSE_USE_BINARYSQL
  #include "RoseBin.h"
#endif


#include "DataConversion.h"
// Files in src/frontend/ExecFormats
#include "MemoryMap.h"

#include "RoseBin_abstract.h"
#include "RoseFile.h"
#include "RoseObj.h"

#include "RoseBin_DotGraph.h"
#include "RoseBin_GmlGraph.h"
#include "RoseBin_ControlFlowAnalysis.h"
#include "RoseBin_DataFlowAnalysis.h"
#include "RoseBin_CallGraphAnalysis.h"

#include "AST_BIN_Traversal.h"

#include "InstructionEnumsX86.h"
#include "x86InstructionProperties.h"
#include "armInstructionEnum.h"
#include "powerpcInstructionEnum.h"
#include "powerpcInstructionProperties.h"

// Files in src/frontend/Disassemblers
#include "Disassembler.h"
#include "DisassemblerArm.h"
#include "DisassemblerPowerpc.h"
#include "DisassemblerX86.h"
#include "Partitioner.h"
#include "Assembler.h"
#include "AssemblerX86.h"

// Files in src/backend/asmUnparser
#include "AsmUnparser.h"
#include "AsmUnparser_compat.h"  /*backward compatibility functions once in unparseAsm.h*/




// ABI struct layout and similar helpers in src/frontend/SageIII/sageInterface
#include "abiStuff.h"

// DQ (3/8/2009): Why is this here?
// added .h file for error check transformations
//#include "RuntimeInstrumentation.h"

// Laksono (12/06/2010): we include PHPFrontend.h if and only if a PHP support is required
#ifdef USE_ROSE_PHP_SUPPORT
#ifndef _MSC_VER
// tps (11/23/2009) : Commented out right now to make progress in Windows
#include "PHPFrontend.h"
#endif
#endif

// Support for Xen/Ether hypervisor
#ifndef _MSC_VER
#include "ether.h"
#endif

// King84 (2010.08.03) : Now that everything is defined we can include all the implementation files for templates.
#define TEMPLATE_IMPLEMENTATIONS
#include "AstDOTGenerationImpl.C"
#include "utility_functionsImpl.C"
#undef TEMPLATE_IMPLEMENTATIONS

// DQ (4/20/2009): Added support to optionally get more information out about new delete operators.
#define COMPILE_DEBUG_STATEMENTS 1

/******************************************************************************************************************************
 *                            THIS CHECK SHOULD BE THE LAST THING IN THIS FILE!
 ******************************************************************************************************************************
 * 
 * Make sure that autoconf macros are not defined in user code.  This test is here because CPP symbols defining the presence or
 * absence of certain features detected by GNU autoconf (or cmake's cmake/ConfigureChecks.cmake) pollute the global name
 * space. This makes it impossible for a user to include both ROSE's configuration results in conjunction with the
 * configuration results of any other package.
 *
 * If a ROSE public header file truly needs to know a configuration result, then modify scripts/publicConfiguration.pl
 * to include the name of the symbol you need (e.g., HAVE_PTHREAD_H).   Then config.status (which is run as part of the
 * configuration process) will create a file named "rosePublicConfig.h" with properly scoped CPP symbols (e.g.,
 * "ROSE_HAVE_PTHREAD_H).
 *
 * For legacy code that includes "rose_config.h" followed by "rose.h", simply undefine CONFIG_ROSE between those two
 * includes. It is safe to do this because CONFIG_ROSE is not used for any other purpose.
 *
 * This test is here rather than in src/testRoseLib.C so that developers will get this error sooner rather than having to
 * wait until all of ROSE is compiled.  However, it does mean that developers will need to be a bit more careful about
 * including both the private (rose_config.h) and public (rosePublicConfig.h) files in tandem.
 ******************************************************************************************************************************/
#ifdef CONFIG_ROSE
#  error "rose_config.h included in public header by mistake. Use rosePublicConfig.h instead."
#endif


#endif /* !ROSE_H !*/

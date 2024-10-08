#ifdef TEMPLATE_IMPLEMENTATIONS

// This is a header file (even though it has a ".C" extension) which is #included in rose.h in order to define
// templates. Therefore it should include neither rose.h nor sage3basic.h. Instead, use the normal ROSE policy for header
// files: include <rose.h> or <sage3basic.h> before including <utility_functionsImpl.C>. If you don't believe this is really
// a header file, just look through the various makefiles and you'll find it lumped into lists of header files, but never
// in lists of source files.
//#include "rose.h"

#ifndef UTILITY_FUNCTION_TEMPLATES_C
#define UTILITY_FUNCTION_TEMPLATES_C

#include "AstDOTGeneration.h"
#include "wholeAST_API.h"

template <typename ExtraNodeInfo_t, typename ExtraNodeOptions_t, typename ExtraEdgeInfo_t, typename ExtraEdgeOptions_t>
struct generateDOTExtended
{
//generateDOTExtended ( const SgProject & project, std::string filenamePostfix = "", ExtraNodeInfo_t eni = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), ExtraNodeOptions_t eno = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), ExtraEdgeInfo_t eei = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), ExtraEdgeOptions_t eeo = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() )
        generateDOTExtended ( const SgProject & project, std::string filenamePostfix = "", ExtraNodeInfo_t eni = ExtraNodeInfo_t(), ExtraNodeOptions_t eno = ExtraNodeOptions_t(), ExtraEdgeInfo_t eei = ExtraEdgeInfo_t(), ExtraEdgeOptions_t eeo = ExtraEdgeOptions_t() )
        {
          // DQ (7/4/2008): Added default parameter to support the filenamePostfix 
          // mechanism in AstDOTGeneration

          // DQ (6/14/2007): Added support for timing of the generateDOT() function.
        //     TimingPerformance timer ("ROSE generateDOT():");

                AstDOTGenerationExtended<ExtraNodeInfo_t, ExtraNodeOptions_t, ExtraEdgeInfo_t, ExtraEdgeOptions_t> astdotgen(eni, eno, eei, eeo);
                SgProject & nonconstProject = (SgProject &) project;

          // Note that the use of generateInputFiles causes the graph to be generated 
          // for only the input source file and not any included header files. The 
          // result is a much smaller file (and generally a more useful one).
#if 0
          // This used to be the default, but it would output too much data (from include files).
                 astdotgen.generate(&nonconstProject);
#else
          // DQ (9/1/2008): This is the default for the last long while, but the SgProject IR nodes 
          // is not being processed (which appears to be a bug). This is because in the implementation
          // of the generateInputFiles the function traverseInputFiles is called.
                astdotgen.generateInputFiles(&nonconstProject,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
#endif
   }
};

template <typename ExtraNodeInfo_t, typename ExtraNodeOptions_t, typename ExtraEdgeInfo_t, typename ExtraEdgeOptions_t>
struct generateDOTExtended_withIncludes
{
  generateDOTExtended_withIncludes (const SgProject & project, std::string /*filenamePostfix = ""*/, ExtraNodeInfo_t eni = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), ExtraNodeOptions_t eno = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), ExtraEdgeInfo_t eei = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), ExtraEdgeOptions_t eeo = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() )
    {
       AstDOTGenerationExtended<ExtraNodeInfo_t, ExtraNodeOptions_t, ExtraEdgeInfo_t, ExtraEdgeOptions_t> astdotgen(eni, eno, eei, eeo);
       SgProject & nonconstProject = (SgProject &) project;

  // Note that the use of generateInputFiles causes the graph to be generated 
  // for only the input source file and not any included header files. The 
  // result is a much smaller file (and generally a more useful one).

  // This used to be the default, but it would output too much data (from include files).
  // It is particularly useful when handling multiple files on the command line and 
  // traversing the files included from each file.
       astdotgen.generate(&nonconstProject);
    }
};

template <typename ExtraNodeInfo_t, typename ExtraNodeOptions_t, typename ExtraEdgeInfo_t, typename ExtraEdgeOptions_t>
struct generateDOTExtendedforMultipleFile
{
        generateDOTExtendedforMultipleFile ( const SgProject & project, std::string filenamePostfix, ExtraNodeInfo_t eni = AstDOTGenerationExtended_Defaults::DefaultExtraNodeInfo(), ExtraNodeOptions_t eno = AstDOTGenerationExtended_Defaults::DefaultExtraNodeOptions(), ExtraEdgeInfo_t eei = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeInfo(), ExtraEdgeOptions_t eeo = AstDOTGenerationExtended_Defaults::DefaultExtraEdgeOptions() )
        {
  //   TimingPerformance timer ("ROSE generateDOT():");

  // This is the best way to handle generation of DOT files where multiple files
  // are specified on the command line.  Later we may be able to filter out the
  // include files (but this is a bit difficult until generateInputFiles() can be
  // implemetned to call the evaluation of inherited and synchizied attributes.
                generateDOTExtended_withIncludes<ExtraNodeInfo_t, ExtraNodeOptions_t, ExtraEdgeInfo_t, ExtraEdgeOptions_t>(project,filenamePostfix, eni, eno, eei, eeo);
        }
};

#endif // UTILITY_FUNCTION_TEMPLATES_C

#else
#error Must include template implementation files after all header files.  If you have, make sure you define TEMPLATE_IMPLEMENTATIONS
#endif // TEMPLATE_IMPLEMENTATIONS


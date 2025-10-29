
#former libtool convenience libraries:

# src/roseSupport/Makefile.am: roseSupport

# src/backend/unparser/Makefile.am: unparse

# src/backend/unparser/FortranCodeGeneration/Makefile.am: unparseFortran

# src/backend/unparser/CxxCodeGeneration/Makefile.am: unparseCxx

# src/backend/unparser/formatSupport/Makefile.am: unparseFormatSupport

# src/backend/unparser/PHPCodeGeneration/Makefile.am: unparsePHP

# src/backend/unparser/languageIndependenceSupport/Makefile.am: unparseLanguageIndependenceSupport

# src/roseExtensions/roseHPCToolkit/src/xml2profir/Makefile.am: xml2profir

# src/roseExtensions/roseHPCToolkit/src/profir2sage/Makefile.am: profir2sage

# src/roseExtensions/roseHPCToolkit/src/xml/Makefile.am: xml

# src/roseExtensions/roseHPCToolkit/src/sage/Makefile.am: sage

# src/roseExtensions/roseHPCToolkit/src/util/Makefile.am: util

# src/roseExtensions/roseHPCToolkit/src/profir/Makefile.am: profir

# src/roseExtensions/roseHPCToolkit/src/gprof/Makefile.am: gprof

# src/roseExtensions/SQLiteConnection/Makefile.am: RoseSQLiteDatabase

# src/roseExtensions/databaseConnection/Makefile.am: rosedatabase

# src/roseExtensions/sqlite3x/Makefile.am: RoseSQLite3xDatabase

# src/midend/abstractHandle/Makefile.am: abstractHandle

# src/midend/astUtil/annotation/Makefile.am: annotation

# src/midend/astUtil/astInterface/Makefile.am: astInterface

# src/midend/astUtil/astSupport/Makefile.am: astSupport

# src/midend/astUtil/symbolicVal/Makefile.am: symbolicVal

# src/midend/binaryAnalyses/Makefile.am: binaryMidend

# src/midend/loopProcessing/slicing/Makefile.am: slicing

# src/midend/loopProcessing/depGraph/Makefile.am: depGraph

# src/midend/loopProcessing/computation/Makefile.am: computation

# src/midend/loopProcessing/outsideInterface/Makefile.am: arrayOptimization

# src/midend/loopProcessing/depInfo/Makefile.am: depInfo

# src/midend/loopProcessing/prepostTransformation/Makefile.am: prepostloop

# src/midend/loopProcessing/driver/Makefile.am: driver

# src/midend/astDiagnostics/Makefile.am: astDiagnostics

# src/midend/programAnalysis/dataflowAnalysis/Makefile.am: dataflowAnalysis

# src/midend/programAnalysis/valuePropagation/Makefile.am: valuePropagation

# src/midend/programAnalysis/distributedMemoryAnalysis/Makefile.am: distributedMemoryAnalysis

# src/midend/programAnalysis/pointerAnal/Makefile.am: pointerAnal

# src/midend/programAnalysis/proceduralSlicing/Makefile.am: alternativeProgramSlicing

set(alternativeProgramSlicing_STAT_SRCS
)

# src/midend/programAnalysis/defUseAnalysis/Makefile.am: DefUseAnalysis

# src/midend/programAnalysis/staticInterproceduralSlicing/Makefile.am: StaticSlicing

# src/midend/programAnalysis/CFG/Makefile.am: CFG

# src/midend/programAnalysis/CallGraphAnalysis/Makefile.am: CallGraph

# src/midend/programAnalysis/OAWrap/Makefile.am: OAWrap

# src/midend/programAnalysis/annotationLanguageParser/Makefile.am: annotationLanguageParser

# src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/Makefile.am: dominance

set(dominance_STAT_SRCS
)

# src/midend/astRewriteMechanism/Makefile.am: rewrite

# src/midend/astQuery/Makefile.am: query

# src/midend/programTransformation/constantFolding/Makefile.am: constantFolding

# src/midend/programTransformation/partialRedundancyElimination/Makefile.am: pre

# src/midend/programTransformation/implicitCodeGeneration/Makefile.am: implicitCodeGeneration

# src/midend/programTransformation/functionCallNormalization/Makefile.am: functionCallNormalization

# src/midend/programTransformation/finiteDifferencing/Makefile.am: finiteDifferencing

# src/midend/astProcessing/Makefile.am: astprocessing

# src/midend/astOutlining/Makefile.am: astOutliner

# src/midend/astInlining/Makefile.am: astinlining

# src/midend/ompLowering/Makefile.am: ompLowering

# src/frontend/SageIII/astMerge/Makefile.am: astMerge

# src/frontend/SageIII/astHiddenTypeAndDeclarationLists/Makefile.am: astHiddenTypeAndDeclarationLists

# src/frontend/SageIII/astFixup/Makefile.am: astFixup

# src/frontend/SageIII/astPostProcessing/Makefile.am: astPostProcessing

# src/frontend/SageIII/sageInterface/Makefile.am: sageInterface

# src/frontend/SageIII/Makefile.am: sage3

# src/frontend/SageIII/astVisualization/Makefile.am: astVisualization

# src/frontend/SageIII/astTokenStream/Makefile.am: astTokenStream

# src/frontend/SageIII/virtualCFG/Makefile.am: virtualCFG

# src/frontend/PHPFrontend/Makefile.am: rosePHPFrontend

# src/frontend/Disassemblers/Makefile.am: roseDisassemblers

# src/frontend/OpenFortranParser_SAGE_Connection/Makefile.am: roseFortran

# src/frontend/CxxFrontend/Makefile.am: roseEDG

# src/frontend/CxxFrontend/EDG_3.3/edgFrontEndWithoutSage/Makefile.am: edg33WithoutSage

set(edg33WithoutSage_STAT_SRCS
)

# src/frontend/CxxFrontend/EDG_3.3/src/Makefile.am: edg33

# src/frontend/CxxFrontend/EDG_3.3/util/Makefile.am: edgutil

set(edgutil_STAT_SRCS
)

# src/frontend/CxxFrontend/EDG_SAGE_Connection/Makefile.am: sage

# src/frontend/CxxFrontend/EDG_SAGE_Connection/Makefile.am: sage

set(sage_STAT_SRCS
)

# src/frontend/CxxFrontend/EDG_SAGE_Connection/Makefile.am: newsage

# src/frontend/ExecFormats/Makefile.am: roseExecFormats

# src/3rdPartyLibraries/MSTL/Makefile.am: mstl

# src/util/stringSupport/Makefile.am: RoseStringSupport

# src/util/graphs/Makefile.am: graphs

# src/util/Makefile.am: roseutil

# src/util/commandlineProcessing/Makefile.am: RoseCommandlineUtil

# src/util/support/Makefile.am: support


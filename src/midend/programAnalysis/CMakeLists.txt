
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(CallGraphAnalysis)
add_subdirectory(OAWrap)

# DQ (11/25/2009) Comment out under Windows.
if (NOT WIN32)
message ("Commented out use of OpenAnalysis in Windows.")
add_subdirectory(CFG)
endif (NOT WIN32)

add_subdirectory(dataflowAnalysis)
add_subdirectory(valuePropagation)
add_subdirectory(defUseAnalysis)
add_subdirectory(dominanceAnalysis)
add_subdirectory(staticInterproceduralSlicing)
add_subdirectory(pointerAnal)
add_subdirectory(annotationLanguageParser)
add_subdirectory(sideEffectAnalysis)
add_subdirectory(distributedMemoryAnalysis)
add_subdirectory(graphAnalysis)


set( midend_pa_src
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-lexer.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-parser.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/annotations.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/annvariable.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/structure.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/enum_property.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/procedure.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/set_property.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/pointerrule.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/analyze.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/rule.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/broadway_expr.cc 
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/annotationLanguageParser/report.cc 
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysisAbstract.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/GlobalVarAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/LivenessAnalysis.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/dfaToDot.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/DefUseAnalysis_perFunction.cpp
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/defUseAnalysis/runTest.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_GmlGraph.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_Graph.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseBin_DotGraph.cpp
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/valuePropagation/ValueAnnot.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/valuePropagation/ValuePropagate.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OAWrap/OAWrap.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OAWrap/SAGE2OA.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/proceduralSlicing/SlicingInfo.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/sideEffectAnalysis/sideEffect.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/CallGraphDB.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/CallGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/callgraph_example.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysis/ClassHierarchyGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/OARIFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/TarjanIntervals.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CFG/CFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/CallGraph/CallGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/BaseGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/Tree.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/DGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/UnionFindUniverse.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/Graph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/trtest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/Utils/grtest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/DomTree.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/SSA.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/SSA/Phi.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createFDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testCDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFCFG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/printDUC.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createDDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/MergedDependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/ControlFlowGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/multiPassTest.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/newControlDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testDDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testSlicing.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/FunctionDependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/ControlDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFBDT.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createSDG.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFDT.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DependenceGraph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/EDefUse.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DataDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/generateSFDF.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/SlicingInfo.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/slice.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/CreateSlice.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/CreateSliceSet.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/newDU.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/testSDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/createCDG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing/DefUseExtension.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis/functionNames.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis/DistributedMemoryAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CFG/CFG_OA.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CFG/CFG_ROSE.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/pointerAnal/PtrAnalCFG.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/pointerAnal/PtrAnal.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/Callgraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/callgraph_example.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/CallGraphAnalysisMySQL/Classhierarchy.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/DataFlowAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/ReachingDefinition.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dataflowAnalysis/DefUseChain.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/SB_Graph.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/CallGraphAnalysis.C
${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/graphAnalysis/RoseSrc_CallGraphAnalysis.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/ControlFlowGraph.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/DominanceFrontier.C
#${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/dominatorTreesAndDominanceFrontiers/DominatorTree.C
)

add_library( midend_pa STATIC ${midend_pa_src} )
add_dependencies( midend_pa ROSETTA )

########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## DQ (5/3/2007): Old version before Christian's work was added.
##SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis
##          valuePropagation  dominatorTreesAndDominanceFrontiers
##          staticProgramSlicing proceduralSlicing
#
#SUBDIRS = CallGraphAnalysis OAWrap CFG dataflowAnalysis \
#          valuePropagation  defUseAnalysis dominanceAnalysis \
#          staticInterproceduralSlicing pointerAnal \
#	  annotationLanguageParser sideEffectAnalysis distributedMemoryAnalysis \
#          graphAnalysis
#
## DQ (9/6/2005): Commented out these directories since they
## represent code that used the older graph interface and they
## no longer compile!  These will be updated later.
## staticProgramSlicing proceduralSlicing
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files cxx_templates
#
#distclean-local:
#	rm -rf Templates.DB
#
##dist-hook:
##	ln -s ../STL-link ${distdir}
#
## AUTOMAKE_OPTIONS = foreign
#
#EXTRA_DIST = OpenAnalysis
#
#
#
#
#

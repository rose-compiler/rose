include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)
AM_LDFLAGS = $(ROSE_RPATHS)

EXTRA_DIST = CMakeLists.txt \
   ROSETTA_macros.h  grammar.h grammarString.h AstNodeClass.h

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .

CXX_GRAMMAR_DIR   = $(top_builddir)/src/frontend/SageIII

CXX_PREPROC_CODES = $(CXX_GRAMMAR_DIR)/Cxx_Grammar.h

if ROSE_USE_SMALLER_GENERATED_FILES
  CXX_PREPROC_CODES += $(CXX_GRAMMAR_DIR)/GENERATED_CODE_DIRECTORY_Cxx_Grammar/SgNode.h
endif

#                   $(CXX_GRAMMAR_DIR)/Cxx_GrammarCheckingIfDataMembersAreInMemoryPool.C

BUILT_SOURCES = $(CXX_PREPROC_CODES)

# Rasmussen (04/17/2019): Support for ATerms has been deprecated as it is no longer needed
# and likely never fully implemented nor tested.  Files remain in the src tree but are no
# longer built. Files buildAtermSupport.C and grammarAtermConstructorDefinitionMacros.macro
# has been removed.

# The generation of the ROSE Grammars depend upon source code from these files
GRAMMAR_SOURCE_FILES = \
     ../Grammar/Statement.code \
     ../Grammar/grammarClassDefinitionMacros.macro \
     ../Grammar/grammarVariantFunctionDefinitionMacros.macro \
     ../Grammar/Common.code \
     ../Grammar/Support.code \
     ../Grammar/grammarConstructorDefinitionMacros.macro \
     ../Grammar/node.macro \
     ../Grammar/Cxx_GlobalDeclarations.macro \
     ../Grammar/Symbol.code \
     ../Grammar/grammarDestructorDefinitionMacros.macro \
     ../Grammar/parserCode.macro \
     ../Grammar/Cxx_Support_Header.macro \
     ../Grammar/Type.code \
     ../Grammar/grammarIsClassNameFunctionDefinitionMacros.macro \
     ../Grammar/parserPrototype.macro \
     ../Grammar/Expression.code \
     ../Grammar/copyMemberFunction.macro \
     ../Grammar/grammarMainClassDeclatationMacros.macro \
     ../Grammar/parserSourceCode.macro \
     ../Grammar/LocatedNode.code \
     ../Grammar/dataMemberAccessFunctions.macro \
     ../Grammar/grammarMainClassParseFunctionSourceCode.macro \
     ../Grammar/Node.code \
     ../Grammar/BinaryInstruction.code \
     ../Grammar/grammarClassDeclarationMacros.macro \
     ../Grammar/grammarMainClassSourceCodeMacros.macro \
     ../Grammar/grammarNewDeleteOperatorMacros.macro \
     ../Grammar/grammarTraverseMemoryPool.macro \
     ../Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro \
     ../Grammar/grammarStorageClassDeclatationMacros.macro \
     ../Grammar/grammarStorageClassDefinitionMacros.macro \
     ../Grammar/grammarAST_FileIoHeader.code \
     ../Grammar/grammarAST_FileIoSource.code  \
     ../Grammar/grammarSourceOfIRNodesAstFileIOSupport.macro \
     ../Grammar/grammarStaticDataManagingClassHeader.macro \
     ../Grammar/grammarStaticDataManagingClassStorageClassHeader.macro \
     ../Grammar/grammarReturnDataMemberPointers.macro \
     ../Grammar/grammarProcessDataMemberReferenceToPointers.macro \
     ../Grammar/grammarGetChildIndex.macro \
     ../astNodeList

# DQ (4/6/2006): Removed from Jochen's new version
# ../Grammar/grammarConstructorOfStorageClass.macro



# VALGRIND_OPTIONS = -v --num-callers=12 --leak-check=yes --error-limit=no --show-reachable=yes
VALGRIND_OPTIONS = --tool=memcheck -v --num-callers=30 --leak-check=no --error-limit=no --show-reachable=yes --trace-children=yes --suppressions=$(top_srcdir)/scripts/rose-suppressions-for-valgrind
# VALGRIND = valgrind $(VALGRIND_OPTIONS)
# VALGRIND =

# DQ (11/5/2016): Adding support for address sanitizer (borrowing the VALGRIND variable to support 
# this, this implementation needs review).  It might be better to prepend the information to the 
# CC and CXX variables. I think that the purify support uses this approach.
if ROSE_USE_SANITIZER
# DQ (11/9/2016): Use option to turn off the memory leak detection because we are not 
# conserned with memory leaks in the ROSE code generator.
# VALGRIND = $(ADDRESS_SANITIZER_OPTIONS)
VALGRIND = ASAN_OPTIONS=detect_leaks=0
else
VALGRIND =
endif

# Build the grammar for generating the C++ preprocessor.
rosetta_generated: $(CXX_PREPROC_CODES)
$(CXX_PREPROC_CODES): CxxGrammarMetaProgram$(EXEEXT) $(GRAMMAR_SOURCE_FILES)
#  Build a link so that the executable can find the *.code file in ../Grammar
#	rm -f Grammar
#	ln -s $(top_srcdir)/src/ROSETTA/Grammar Grammar
	@echo "Running ROSETTA to generated ROSE IR code:"
#       Add "--verbose" if you want the old, noisy output from CxxGrammarMetaProgram
	$(AM_V_GEN) $(VALGRIND) ./CxxGrammarMetaProgram ${top_builddir}/src/frontend/SageIII/

noinst_PROGRAMS = CxxGrammarMetaProgram
CxxGrammarMetaProgram_SOURCES = \
   CxxGrammarMetaProgram.C \
   grammar.C \
   GrammarFile.C \
   grammarGenerator.C \
   grammarString.C \
   AstNodeClass.C \
   type.C \
   symbol.C \
   expression.C \
   statement.C \
   support.C \
   binaryInstruction.C \
   storageSize.C \
   visitorSupport.C \
   poolBasedTraversal.C \
   node.C \
   buildAstFileIO.C \
   buildStorageClasses.C \
   buildConstructorsWithoutSourcePositionInformation.C \
   outputClassesAndFields.C \
   java/terminals.cpp

# buildConstructorsWithoutSourcePositionInformation.o: buildConstructorsWithoutSourcePositionInformation.C

BoostLibraries = \
  $(BOOST_LDFLAGS) $(BOOST_DATE_TIME_LIB)           \
  $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) $(BOOST_PROGRAM_OPTIONS_LIB)  \
  $(BOOST_REGEX_LIB)  $(BOOST_SYSTEM_LIB) $(BOOST_SERIALIZATION_LIB)    \
  $(BOOST_WAVE_LIB) $(BOOST_IOSTREAMS_LIB) $(RT_LIBS) $(top_builddir)/src/util/libroseutil.la

CxxGrammarMetaProgram_LDADD = \
  $(ROSE_BOOST_LIBS) \
  $(top_builddir)/src/util/libroseutil.la \
  $(top_builddir)/src/util/Sawyer/libroseSawyer.la \
  $(ROSE_BOOST_LIBS) \
  $(RT_LIBS)

# DQ (4/12/2016): This is required for use with clang (not clear how to compile ROSE with out it).
if USING_CLANG_COMPILER
CxxGrammarMetaProgram_LDADD += -lstdc++
endif

# DQ (9/11/2008):
# The rose_paths.c is built in ROSE/src/util and this dependence causes
# ROSE to rebuild a lot of code each time configure is rerun. Let's see
# if this can be make a little more tolerable for those without larger
# computers working on ROSE (e.g. students).
# CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_LDADD)
# CxxGrammarMetaProgram_DEPENDENCIES = 
# CxxGrammarMetaProgram_DEPENDENCIES = $(CxxGrammarMetaProgram_SOURCES)


# DQ (7/8/2005): Avoid building Sage III when util changes!
# $(top_builddir)/util/libroseutil.so:
#	cd $(top_builddir)/util; $(MAKE)


# Force recompilation of source files if any of the header files 
# are changed (does not work if *.o files are not present).
$(librosetta_a_OBJECTS): ROSETTA_macros.h grammar.h grammarString.h AstNodeClass.h

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files cxx_templates Cxx_Grammar* grammar.dot translationSupport.code

distclean-local:
	rm -rf Cxx_Grammar.h Cxx_Grammar.C Cxx_GrammarTreeTraversal.C Cxx_GrammarTreeTraversalClass.h \
          Cxx_GrammarTreeTraversalClass.C
	rm -rf grammar.tex grammar.dot

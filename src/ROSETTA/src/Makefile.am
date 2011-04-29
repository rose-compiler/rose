include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

EXTRA_DIST = CMakeLists.txt \
   ROSETTA_macros.h  grammar.h grammarString.h terminal.h

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
VALGRIND =

# Build the grammar for generating the C++ preprocessor.
$(CXX_PREPROC_CODES): CxxGrammarMetaProgram$(EXEEXT) $(GRAMMAR_SOURCE_FILES)
#  Build a link so that the executable can find the *.code file in ../Grammar
#	rm -f Grammar
#	ln -s $(top_srcdir)/src/ROSETTA/Grammar Grammar
	@echo "  GEN     ROSETTA-generated files"
	$(VALGRIND) ./CxxGrammarMetaProgram ${top_builddir}/src/frontend/SageIII/

noinst_PROGRAMS = CxxGrammarMetaProgram
CxxGrammarMetaProgram_SOURCES = \
   CxxGrammarMetaProgram.C \
   grammar.C \
   grammarString.C \
   terminal.C \
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
   outputClassesAndFields.C

# buildConstructorsWithoutSourcePositionInformation.o: buildConstructorsWithoutSourcePositionInformation.C

CxxGrammarMetaProgram_LDADD = $(top_builddir)/src/util/libroseutil.la

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
$(librosetta_a_OBJECTS): ROSETTA_macros.h grammar.h grammarString.h terminal.h

clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files cxx_templates Cxx_Grammar* grammar.dot translationSupport.code

distclean-local:
	rm -rf Cxx_Grammar.h Cxx_Grammar.C Cxx_GrammarTreeTraversal.C Cxx_GrammarTreeTraversalClass.h \
          Cxx_GrammarTreeTraversalClass.C
	rm -rf grammar.tex grammar.dot

















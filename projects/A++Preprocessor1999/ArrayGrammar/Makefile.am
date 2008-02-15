include ../../../config/Makefile.for.ROSE.includes.and.libs

# INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/include -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../ListT @SAGE_INCLUDES@ @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@
# INCLUDES = -I$(top_builddir) -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../TransformBaseClass -I$(srcdir)/../ArrayTransformationGrammar @SAGE_INCLUDES@ -I$(srcdir)/../ListT @STL_INCLUDES@ -I$(top_builddir)/include -I$(top_builddir)/SAGE @INCLUDES@

INCLUDES = \
   @INCLUDES@ \
   $(ROSE_INCLUDES) \
   @SAGE_INCLUDES@ \
   @STL_INCLUDES@ \
   -I$(top_builddir)/../include \
   -I$(top_builddir)/EDG/errinfo \
   -I$(srcdir)/../TransformBaseClass \
   -I$(srcdir)/../ArrayTransformationGrammar  \
   -I$(srcdir)/.. \
   -I$(srcdir)/../ListT

CXX_TEMPLATE_REPOSITORY_PATH = ..

noinst_LIBRARIES = libtransform2.a
libtransform2_a_SOURCES = transformation_2.C transform_2.C grammar.C
# AM_CPPFLAGS = -g

## noinst_HEADERS = grammar.h roseVariants.h transformation_2.h
noinst_HEADERS = roseVariants.h transformation_2.h

# Dependence upon grammar.h required to force it to be built
transformation_2.o : grammar.h
transform_2.o      : grammar.h
grammar.o          : grammar.h


# We have to explicily build this rule since we want to avoid the use of "ar cru"
# (need to use the C++ compiler so that templates will be built)
libtransform2.a: $(libtransform2_a_OBJECTS) $(libtransform2_a_DEPENDENCIES)
	-rm -f $@
	$(CXX_STATIC_LIB_UPDATE) $@ $(libtransform2_a_OBJECTS) $(libtransform2_a_LIBADD)
	test -f $@ || exit 1

# Build the codes generating the C++ preprocessor.
PREPROC_CODES = grammar.h grammar.C

GRAMMAR_SOURCE_FILES = \
   grammarClassDeclatationMacros.macro                            grammarClassDefinitionMacros.macro \
   classDeclarationBody_ROSE_ArrayExpression.macro                memberFunctionDefinition_ROSE_ArrayExpression.macro \
   classDeclarationBody_ROSE_ArrayFunctionExpression.macro        memberFunctionDefinition_ROSE_ArrayFunctionExpression.macro \
   classDeclarationBody_ROSE_ArrayOperandExpression.macro         memberFunctionDefinition_ROSE_ArrayOperandExpression.macro \
   classDeclarationBody_ROSE_ArrayOperator.macro                  memberFunctionDefinition_ROSE_ArrayOperator.macro \
   classDeclarationBody_ROSE_ArrayParenthesisOperator.macro       memberFunctionDefinition_ROSE_ArrayParenthesisOperator.macro \
   classDeclarationBody_ROSE_ArraySQRT.macro                      memberFunctionDefinition_ROSE_ArraySQRT.macro \
   classDeclarationBody_ROSE_ArrayStatement.macro                 memberFunctionDefinition_ROSE_ArrayStatement.macro \
   classDeclarationBody_ROSE_ArrayVariableDeclaration.macro       memberFunctionDefinition_ROSE_ArrayVariableDeclaration.macro \
   classDeclarationBody_ROSE_ArrayVariableExpression.macro        memberFunctionDefinition_ROSE_ArrayVariableExpression.macro \
   classDeclarationBody_ROSE_BinaryArrayOperator.macro            memberFunctionDefinition_ROSE_BinaryArrayOperator.macro \
   classDeclarationBody_ROSE_BinaryArrayOperatorAdd.macro         memberFunctionDefinition_ROSE_floatArrayVariableExpression.macro \
   classDeclarationBody_ROSE_BinaryArrayOperatorEquals.macro      memberFunctionDefinition_ROSE_BinaryArrayOperatorAdd.macro \
   classDeclarationBody_ROSE_BinaryArrayOperatorMultiply.macro    memberFunctionDefinition_ROSE_BinaryArrayOperatorEquals.macro \
   classDeclarationBody_ROSE_C_Expression.macro                   memberFunctionDefinition_ROSE_BinaryArrayOperatorMultiply.macro \
   classDeclarationBody_ROSE_C_Statement.macro                    memberFunctionDefinition_ROSE_C_Expression.macro \
   classDeclarationBody_ROSE_C_VariableDeclaration.macro          memberFunctionDefinition_ROSE_C_Statement.macro \
   classDeclarationBody_ROSE_ElseWhereStatement.macro             memberFunctionDefinition_ROSE_C_VariableDeclaration.macro \
   classDeclarationBody_ROSE_Expression.macro                     memberFunctionDefinition_ROSE_ElseWhereStatement.macro \
   classDeclarationBody_ROSE_ExpressionStatement.macro            memberFunctionDefinition_ROSE_Expression.macro \
   classDeclarationBody_ROSE_ForStatement.macro                   memberFunctionDefinition_ROSE_ExpressionStatement.macro \
   classDeclarationBody_ROSE_IfStatement.macro                    memberFunctionDefinition_ROSE_ForStatement.macro \
   classDeclarationBody_ROSE_RealArrayVariableExpression.macro    memberFunctionDefinition_ROSE_IfStatement.macro \
   classDeclarationBody_ROSE_Statement.macro                      memberFunctionDefinition_ROSE_Node.macro \
   classDeclarationBody_ROSE_StatementBlock.macro                 memberFunctionDefinition_ROSE_RealArrayVariableExpression.macro \
   classDeclarationBody_ROSE_UserFunction.macro                   memberFunctionDefinition_ROSE_Statement.macro \
   classDeclarationBody_ROSE_VariableDeclaration.macro            memberFunctionDefinition_ROSE_StatementBlock.macro \
   classDeclarationBody_ROSE_WhereStatement.macro                 memberFunctionDefinition_ROSE_UserFunction.macro \
   classDeclarationBody_ROSE_doubleArrayVariableExpression.macro  memberFunctionDefinition_ROSE_VariableDeclaration.macro \
   classDeclarationBody_ROSE_floatArrayVariableExpression.macro   memberFunctionDefinition_ROSE_WhereStatement.macro \
   classDeclarationBody_ROSE_intArrayVariableExpression.macro     memberFunctionDefinition_ROSE_doubleArrayVariableExpression.macro \
   node.macro                                                     memberFunctionDefinition_ROSE_intArrayVariableExpression.macro

## Run this perl script to generate the grammar.h and grammar.C files from the $(GRAMMAR_SOURCE_FILES)
## Later this step well be replaced with the ROSETTA grammar generator
## Call grammar perl script with the location of the macro files "$(VPATH)"
## and the location to place the header file "$(top_builddir)/include"

## sourcePath = xxx

$(PREPROC_CODES): $(GRAMMAR_SOURCE_FILES) grammar.perl
	@echo "Calling grammar.perl (perl script) to generate the array class grammar ..."
	@if test "$(VPATH)" = ""; then \
	   echo "I can't seem to set sourcePath: VPATH is NOT set"; \
	   sourcePath="."; \
	   perl $(srcdir)/grammar.perl . .; \
	else \
	   echo "I can't seem to set sourcePath: VPATH IS set"; \
	   sourcePath="$(VPATH)"; \
	   perl $(srcdir)/grammar.perl $(VPATH) .; \
	fi;
	@echo "I can't seem to set sourcePath properly: sourcePath = $(sourcePath) vpath = $(VPATH)";

#	$(srcdir)/grammar.perl $(sourcePath) $(top_builddir)/include

# Copy the source code used to generate the grammar into the distribution
# EXTRA_DIST = $(GRAMMAR_SOURCE_FILES) grammar.perl grammar.C grammar.h
EXTRA_DIST = $(GRAMMAR_SOURCE_FILES) grammar.perl

clean-local:
	rm -rf grammar.h grammar.C Templates.DB ii_files ti_files

















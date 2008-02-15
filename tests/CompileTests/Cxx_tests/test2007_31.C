/*
Hi Dan,

the following is a 2 line test case that fails under Rose build with gcc4.1.1 - not sure if that holds for other builds too.

This test case was stripped down by Jeremiah from the ROSE unparser.c (50.000 LOC including header files).
The error message is attached below.

thanks,
Thomas




01:21 PM :~/development/ROSE-0107/gcc3.4.6/bin > identityTranslator -edg:w testcase.C
Warning in sage_gen_routine_name(): rout->declared_type == NULL
Warning edgRoutine->assoc_scope == 0
Inside of Sg_File_Info::display(Warning edgRoutine->assoc_scope <= 0)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename = /home/panas2/development/ROSE-0107/gcc3.4.6/bin/testcase.C
    line     = 1  column = 28
Warning: In set_scope ( a_routine_ptr edgRoutine, SgFunctionDefinition *functionDefinition ): Sage scope in EDG not set!
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionParameterList): found a null file info pointer
In SgTreeTraversal<I,S>::inFileToTraverse(SgFunctionDeclaration): found a null file info pointer
identityTranslator: Cxx_Grammar.C:5567: bool Sg_File_Info::isCompilerGenerated() const: Assertion this != __null failed.
Aborted
*/


template <typename T> void f();
struct A {static void bar() {f<char>();}};

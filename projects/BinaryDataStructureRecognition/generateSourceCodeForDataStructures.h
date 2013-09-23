
// This header file contains the code to generate the source code to
// represent the header file description of the data strcutures represented
// in the binary.

// Build the code for all language constructs.
void buildSourceCode( SgProject* project );

// Build the code for each kind of language attribute (classes, functions, etc.).
void generateClassStructures ( SgSourceFile* file );
void generateFunctions ( SgSourceFile* file );

// Generate global variables.
void generateGlobalVariables ( SgSourceFile* file );

// Generate stack variables.
void generateStackVariables ( SgFunctionDeclaration* functionDeclaration, FunctionAnalysisAttribute* functionAnalysisAttribute );

// Build the code for each language construct using the analysis attributes.
SgClassDeclaration* buildVirtualClass ( SgGlobal* global, VirtualFunctionTable* vTable );

// Build regular functions
SgFunctionDeclaration* buildFunction ( SgGlobal* globalScope, FunctionAnalysisAttribute* function );

// Build the virtual member functions
SgMemberFunctionDeclaration* buildVirtualFunction ( SgClassDeclaration * decl, VirtualFunction* vFunction );




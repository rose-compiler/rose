#ifndef ROSE_SNIPPET_TESTS_H
#define ROSE_SNIPPET_TESTS_H

#include "rose.h"
#include "midend/astSnippet/Snippet.h"

namespace SnippetTests {

/** Find a file for a snippet. Given part of a file name (e.g., perhaps a base name without an extension), search for the
 *  a snippet source file. */
std::string findSnippetFile(const std::string &fileName);

/** Find a function definition having the specified name. */
SgFunctionDefinition *findFunctionDefinition(SgNode *ast, std::string functionName);

/** Find the last statement in a function definition after which we can reasonably insert another statement. */
SgStatement *findLastAppendableStatement(SgFunctionDefinition*);

/** Find the last statement in a function definition. */
SgStatement *findLastStatement(SgFunctionDefinition *fdef);

/** Find the first statement that references the INSERT_HERE variable. */
SgStatement *findInsertHere(SgFunctionDefinition*);

/** Find a variable declaration. */
SgInitializedName *findVariableDeclaration(SgNode *ast, const std::string &varName);

/** Find all variable declarations in a function definition, including formal arguments. */
std::vector<SgInitializedName*> findFunctionVariables(SgFunctionDefinition*);

} // namespace

#endif

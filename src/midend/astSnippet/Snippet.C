#include "Snippet.h"
#include "AstTraversal.h"
#include "LinearCongruentialGenerator.h"
#include "rose_getline.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace rose {

/*******************************************************************************************************************************
 *                                      SnippetFile
 *******************************************************************************************************************************/

SnippetFile::Registry SnippetFile::registry;
std::vector<std::string> SnippetFile::varNameList;

// Class method
SnippetFilePtr
SnippetFile::instance(const std::string &filename, SgSourceFile *snippetAst/*=NULL*/)
{
    SnippetFilePtr retval = lookup(filename);
    if (retval!=NULL)
        return retval;

    if (!snippetAst) {
        snippetAst = parse(filename);
        assert(snippetAst!=NULL);
    }

    retval = registry[filename] = SnippetFilePtr(new SnippetFile(filename, snippetAst));
    retval->findSnippetFunctions();
    
    return retval;
}

// Class method
SnippetFilePtr
SnippetFile::lookup(const std::string &fileName)
{
    struct stat sb;
    if (-1 == stat(fileName.c_str(), &sb))
        throw std::runtime_error(std::string("rose::Snippet: ") + strerror(errno) + ": " + fileName);
    return registry.get_value_or(fileName, SnippetFilePtr());
}

// Class method
SgSourceFile *
SnippetFile::parse(const std::string &fileName)
{
    assert(!fileName.empty());
    
    // We should never unparse the snippet to a separate file, so provide an invalid name to catch errors
    std::string outputName = "/SNIPPET_SHOULD_NOT_BE_UNPARSED/x";

    // Try to load the snippet by parsing its source file
    SgFile *file = SageBuilder::buildFile(fileName, outputName, SageInterface::getProject());
    SgSourceFile *snippetAst = isSgSourceFile(file);
    assert(snippetAst!=NULL);
    attachPreprocessingInfo(snippetAst);
    resetConstantFoldedValues(file);
    snippetAst->set_skip_unparse(true);
    return snippetAst;
}

void
SnippetFile::findSnippetFunctions()
{
    struct SnippetFinder: AstSimpleProcessing {
        FunctionDefinitionMap &functions;
        SnippetFinder(FunctionDefinitionMap &functions): functions(functions) {}

        void visit(SgNode *node) {
            if (SgFunctionDefinition *fdef = isSgFunctionDefinition(node))
                functions[fdef->get_declaration()->get_qualified_name()].push_back(fdef);
        }
    } snippetFinder(functions);
    snippetFinder.traverse(ast, preorder);
}

SnippetPtr
SnippetFile::findSnippet(const std::string &snippetName)
{
    assert(this!=NULL);
    assert(!snippetName.empty());
    FunctionDefinitionMap::const_iterator found = functions.find(snippetName);
    if (found!=functions.end() && !found->second.empty())
        return SnippetPtr(new Snippet(snippetName, shared_from_this(), found->second.front()));
    return SnippetPtr();
}

std::vector<SnippetPtr>
SnippetFile::findSnippets(const std::string &snippetName)
{
    assert(this!=NULL);
    assert(!snippetName.empty());
    std::vector<SnippetPtr> retval;
    FunctionDefinitionMap::const_iterator found = functions.find(snippetName);
    if (found!=functions.end()) {
        BOOST_FOREACH (SgFunctionDefinition *fdef, found->second)
            retval.push_back(SnippetPtr(new Snippet(snippetName, shared_from_this(), fdef)));
    }
    return retval;
}

std::vector<std::string>
SnippetFile::getSnippetNames() const
{
    assert(this!=NULL);
    std::vector<std::string> retval;
    for (FunctionDefinitionMap::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
        retval.push_back(fi->first);
    return retval;
}

bool
SnippetFile::globallyInjected(SgGlobal *destination)
{
    assert(this!=NULL);
    assert(destination!=NULL);
    return !globals.insert(destination).second; // return true iff not already present
}

bool
SnippetFile::fileIsIncluded(const std::string &filename, SgGlobal *destination_scope)
{
    return !headersIncluded[filename].insert(destination_scope).second;
}

void
SnippetFile::expandSnippets(SgNode *ast)
{
    typedef Map<SgFunctionCallExp*, std::string/*snippetname*/> SnippetCalls;

    // Find statements that are calls to snippets, but don't do anything yet
    struct FindSnippetCalls: AstSimpleProcessing {
        SnippetFile *self;
        SnippetCalls calls;

        FindSnippetCalls(SnippetFile *self): self(self) {}

        void visit(SgNode *node) {
            if (SgExprStatement *stmt = isSgExprStatement(node)) {
                if (SgFunctionCallExp *fcall = isSgFunctionCallExp(stmt->get_expression())) {
                    SgFunctionSymbol *fsym = fcall->getAssociatedFunctionSymbol();
                    SgFunctionDeclaration *fdecl = fsym ? fsym->get_declaration() : NULL;
                    std::string called_name = fdecl ? fdecl->get_qualified_name().getString() : std::string();
                    if (self->functions.exists(called_name))
                        calls.insert(std::make_pair(fcall, called_name));
                }
            }
        }
    } t1(this);
    t1.traverse(ast, preorder);

    // Replace each of the snippet calls by expanding the snippet. The expansion occurs right before the snippet call and then
    // the snippet call is removed.
    for (SnippetCalls::iterator ci=t1.calls.begin(); ci!=t1.calls.end(); ++ci) {
        SgFunctionCallExp *fcall = ci->first;
        assert(fcall!=NULL);
        const SgExpressionPtrList &fcall_args = fcall->get_args()->get_expressions();
        std::vector<SgNode*> actuals(fcall_args.begin(), fcall_args.end());
        SgStatement *toReplace = SageInterface::getEnclosingNode<SgStatement>(fcall);
        SnippetPtr snippet;
        BOOST_FOREACH (SnippetPtr candidate, findSnippets(ci->second)) {
            if (candidate->numberOfArguments() == actuals.size()) {
                if (snippet!=NULL) {
                    throw std::runtime_error("snippet call \"" + ci->second + "\" having " +
                                             StringUtility::plural(actuals.size(), "arguments") +
                                             " matches multiple snippet definitions");
                }
                snippet = candidate;
            }
        }
        if (snippet==NULL) {
            throw std::runtime_error("snippet call \"" + ci->second + "\" having " +
                                     StringUtility::plural(actuals.size(), "arguments") +
                                     " does not match any snippet definition");
        }
        snippet->insert(toReplace, actuals);
        SageInterface::removeStatement(toReplace);
    }
}

// class method
size_t
SnippetFile::loadVariableNames(const std::string &fileName)
{
    size_t nread = 0;
    if (FILE *dict = fopen(fileName.c_str(), "r")) {
        char *line = NULL;
        size_t linesz = 0;
        while (rose_getline(&line, &linesz, dict)>0) {
            std::string word = StringUtility::trim(line);
            if (!word.empty()) {
                if (!isalpha(word[0]) && '_'!=word[0])
                    word[0] = '_';
                for (size_t i=0; i<word.size(); ++i) {
                    if (!isalnum(word[i]) && '_'!=word[i]) {
                        word[i] = '_';
                    } else if (isupper(word[i])) {
                        word[i] = tolower(word[i]);
                    }
                }
                varNameList.push_back(word);
                ++nread;
            }
        }

     // DQ (2/28/2014): This causes my test code to fail (specimen2014_01.c in 
     // ResetParentPointers::evaluateInheritedAttribute() with a memory problem).
        free(line);

        fclose(dict);
    }
    return nread;
}

// class method
std::string
SnippetFile::randomVariableName()
{
    static size_t ncalls = 0;
    static LinearCongruentialGenerator random(2013120913ul);
    if (!varNameList.empty())
        return varNameList[random() % varNameList.size()] + StringUtility::numberToString(++ncalls);
    
    std::string name = "T_";
    static const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    assert(strlen(letters)==2*26);
    for (size_t i=0; i<6; ++i)
        name += letters[random()%(2*26)];
    return name;
}

void
SnippetFile::doNotInsert(const std::string &name, SgType *type/*=NULL*/)
{
    assert(!name.empty());
    blackListedDeclarations[name].push_back(type);
}

bool
SnippetFile::isBlackListed(SgDeclarationStatement *decl)
{
    assert(this!=NULL);
    assert(decl!=NULL);
    bool retval = false;

    // Obtain the name and type for the declaration
    std::string name;
    SgType *type = NULL;
    if (SgClassDeclaration *classDecl = isSgClassDeclaration(decl)) {
        name = classDecl->get_qualified_name().getString();
        type = classDecl->get_type();
    } else if (SgEnumDeclaration *enumDecl = isSgEnumDeclaration(decl)) {
        name = enumDecl->get_qualified_name().getString();
        type = enumDecl->get_type();
    } else if (SgFunctionDeclaration *funcDecl = isSgFunctionDeclaration(decl)) {
        name = funcDecl->get_qualified_name().getString();
        type = funcDecl->get_type();
    } else if (SgTypedefDeclaration *typedefDecl = isSgTypedefDeclaration(decl)) {
        name = typedefDecl->get_qualified_name().getString();
        type = typedefDecl->get_type();
    } else if (SgVariableDeclaration *varDecl = isSgVariableDeclaration(decl)) {
        // We currently only support variable declarations having exactly one SgInitializedName, otherwise we would potentially
        // need to transform the SgVariableDeclaration while it was copied to the target AST in order to remove only those
        // SgInitializedName nodes that are black listed.
        const SgInitializedNamePtrList &inames = varDecl->get_variables();
        if (1!=inames.size()) {
            std::cerr <<"warning: SnippetFile::isBlackListed does not support declaration statements with more than"
                      <<" one variable.\n";
        } else {
            SgInitializedName *iname = inames.front();
            name = iname->get_qualified_name();
            type = iname->get_type();
        }
    }

    // Is the (name,type) pair black listed?  If a black listed type is null then only the name needs to match.
    SgTypePtrList blackListedTypes = blackListedDeclarations.get_value_or(name, SgTypePtrList());
    if (!blackListedTypes.empty()) {
        BOOST_FOREACH (SgType *blackListedType, blackListedTypes) {
            if (!blackListedType || type==blackListedType) {
                retval = true;
                break;
            }
        }
    }

    return retval;
}

/*******************************************************************************************************************************
 *                                      Snippet
 *******************************************************************************************************************************/


// Class method
SnippetPtr
Snippet::instance(const std::string &snippetName, const SnippetFilePtr &snippetFile)
{
    assert(!snippetName.empty());
    assert(snippetFile!=NULL);

    SnippetPtr retval = snippetFile->findSnippet(snippetName);
    if (!retval)
        throw std::runtime_error("rose::Snippet: snippet '"+snippetName+"' not found in file '"+snippetFile->getName()+"'");
    return retval;
}

// Class method
SnippetPtr
Snippet::instanceFromFile(const std::string &snippetName, const std::string &fileName)
{
    assert(!snippetName.empty());
    assert(!fileName.empty());
    SnippetFilePtr snippetFile = SnippetFile::instance(fileName); // cached instance if possible
    return instance(snippetName, snippetFile);
}

size_t
Snippet::numberOfArguments() const
{
    assert(this!=NULL);
    assert(ast!=NULL);
    assert(ast->get_declaration()!=NULL);
    assert(ast->get_declaration()->get_parameterList()!=NULL);
    return ast->get_declaration()->get_parameterList()->get_args().size();
}

void
Snippet::insert(SgStatement *insertionPoint)
{
    std::vector<SgNode*> args;
    insert(insertionPoint, args);
}

void
Snippet::insert(SgStatement *insertionPoint, SgNode *arg1)
{
    std::vector<SgNode*> args;
    args.push_back(arg1);
    insert(insertionPoint, args);
}

void
Snippet::insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2)
{
    std::vector<SgNode*> args;
    args.push_back(arg1);
    args.push_back(arg2);
    insert(insertionPoint, args);
}

void
Snippet::insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2, SgNode *arg3)
{
    std::vector<SgNode*> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    insert(insertionPoint, args);
}

void
Snippet::insert(SgStatement *insertionPoint, SgNode *arg1, SgNode *arg2, SgNode *arg3, SgNode *arg4)
{
    std::vector<SgNode*> args;
    args.push_back(arg1);
    args.push_back(arg2);
    args.push_back(arg3);
    args.push_back(arg4);
    insert(insertionPoint, args);
}

void
Snippet::insert(SgStatement *insertionPoint, const std::vector<SgNode*> &actuals)
{
    using namespace StringUtility;
    assert(this!=NULL);
    assert(insertionPoint!=NULL);
    assert(ast!=NULL);

    SgFunctionDefinition *targetFunction = SageInterface::getEnclosingNode<SgFunctionDefinition>(insertionPoint);
    assert(targetFunction!=NULL);
    SgScopeStatement *targetFunctionScope = targetFunction->get_body();
    assert(targetFunctionScope!=NULL);

    // Find the first declaration statement in the insertion point's function
    SgDeclarationStatement *targetFirstDeclaration = NULL;
    SgStatementPtrList targetStatements = targetFunctionScope->generateStatementList();
    for (size_t i=0; targetFirstDeclaration==NULL && i<targetStatements.size(); ++i)
        targetFirstDeclaration = isSgDeclarationStatement(targetStatements[i]);
    SgStatement *targetFirstStatement = targetStatements.empty() ? NULL : targetStatements.front();

    // Make it look like the entire snippet file actually came from the same file as the insertion point. This is an attempt to
    // avoid unparsing problems for C where the unparser asserts things such as "the file for a function declaration's scope
    // must be the same file as the function declaration". Even if we deep-copy the function declaration from the snippet file
    // and insert it into the specimen, when unparsing the specimen the declaration's scope will still point to the original
    // scope in the snippet file.
    struct T1: AstSimpleProcessing {
        Sg_File_Info *target, *snippet;
        T1(Sg_File_Info *target, Sg_File_Info *snippet): target(target), snippet(snippet) {}
        void fixInfo(Sg_File_Info *info) {
            if (info && info->get_file_id()==snippet->get_file_id()) {
                info->set_file_id(target->get_file_id());
                info->set_line(1);
            }
        }
        void visit(SgNode *node) {
            if (SgLocatedNode *loc = isSgLocatedNode(node)) {
                fixInfo(loc->get_file_info());
                fixInfo(loc->get_startOfConstruct());
                fixInfo(loc->get_endOfConstruct());
            } else if (SgFile *loc = isSgFile(node)) {
                // SgFile is not a subclass of SgLocatedNode, but it still has these Sg_File_Info methods
                fixInfo(loc->get_file_info());
                fixInfo(loc->get_startOfConstruct());
                fixInfo(loc->get_endOfConstruct());
            }
        }
    } t1(insertionPoint->get_file_info(), ast->get_body()->get_file_info());
    if (!SageInterface::is_Java_language())
        t1.traverse(file->getAst(), preorder);

  // insertionPoint->get_file_info()->display("insertionPoint: test 1: debug");

    // Insert the snippet body after the insertion point
    SgTreeCopy deep;
    SgScopeStatement *toInsert = isSgScopeStatement(ast->get_body()->copy(deep));
    assert(toInsert!=NULL);

 // DQ (3/4/2014): This is a test of the structural equality of the original snippet and it's copy.
 // If they are different then we can't support fixing up the AST.  Transformations on the snippet 
 // should have been made after insertion into the AST.  The complexity of this test is a traversal 
 // of the copy of the snippet to be inserted (typically very small compared to the target application).
// Note that we can enforce this test here, but not after calling the replaceArguments() function below.
    bool isStructurallyEquivalent = SageInterface::isStructurallyEquivalentAST(toInsert,ast->get_body());
    ROSE_ASSERT(isStructurallyEquivalent == true);

 // DQ (3/4/2014): I think this is untimately a fundamental problem later (e.g. for mangled name generation).
 // So we have to attached the current scope to the scope of the insertion point.  This will allow the 
 // SgStatement::get_scope() to work (which is a problem for some debugging code (at least).
 // Note that the semantics of the AST copy mechanism is that the parent of the copy is set to NULL 
 // (which is assumed to be fixed up when the copy is inserted into the AST).
    ROSE_ASSERT(toInsert->get_parent() == NULL);
    SgScopeStatement* new_scope = isSgScopeStatement(insertionPoint->get_parent());
    ROSE_ASSERT(new_scope != NULL);
    toInsert->set_parent(new_scope);
    ROSE_ASSERT(toInsert->get_parent() != NULL);

    renameTemporaries(toInsert);
    causeUnparsing(toInsert, insertionPoint->get_file_info());

    switch (insertMechanism) {
        case INSERT_BODY: {
            // Insert the body all at once. This is efficient but doesn't work well because it means that variables declared in
            // one snippet can't be used in a later snippet injected into the same function.
            SageInterface::insertStatementBefore(insertionPoint, toInsert);
            break;
        }
        case INSERT_STMTS: {
            // Insert one statement at a time.  Snippet declarations are placed at the top of the injection point's function,
            // which means that snippet variables cannot always be inialized in their declarations because the initialization
            // expression might be something that's only well defined at the point of insertion.
            const SgStatementPtrList &stmts = toInsert->getStatementList();
            for (size_t i=0; i<stmts.size(); ++i) {
                if (isSgDeclarationStatement(stmts[i])) {
                    switch (locDeclsPosition) {
                        case LOCDECLS_AT_BEGINNING:
                            if (targetFirstDeclaration!=NULL) {
                                SageInterface::insertStatementBefore(targetFirstDeclaration, stmts[i]);
                            } else {
                                SageInterface::insertStatementBefore(targetFirstStatement, stmts[i]);
                            }
                            break;
                        case LOCDECLS_AT_END:
                            SageInterface::insertStatementAfterLastDeclaration(stmts[i], targetFunctionScope);
                            break;
                        case LOCDECLS_AT_CURSOR:
                            SageInterface::insertStatementBefore(insertionPoint, stmts[i]);
                            break;
                    }
                } else {
                    SageInterface::insertStatementBefore(insertionPoint, stmts[i]);
                }
            }
            break;
        }
    }

    // Build a map binding formal argument symbols to their actual values.  The goal is, in the statements we just inserted, to
    // replace all references to snippet formal arguments with the actual arguments supplied to the Snippet::insert() call. The
    // actual arguments are SgInitializedName or SgExpression nodes from the context in which the snippet was inserted; the
    // references to formal arguments are the SgInitializedName nodes in the AST's that we just copied above (the
    // *declarations* for the formal arguments were *not* copied because they are the get_parameterList() of the snippet
    // functionin the snippet file--which we didn't insert into the target).
    ArgumentBindings bindings; // map from formal arg references to actual arguments
    SgFunctionDeclaration *snippet_fdecl = ast->get_declaration();
    const SgInitializedNamePtrList &formals = snippet_fdecl->get_parameterList()->get_args();
    if (actuals.size()!=formals.size()) {
        throw std::runtime_error("rose::Snippet: mismatched snippet arguments: expected " +
                                 plural(formals.size(), "arguments") + " but got " +
                                 numberToString(actuals.size()));
    }
    for (size_t i=0; i<formals.size(); ++i) {
        SgSymbol *formalSymbol = formals[i]->search_for_symbol_from_symbol_table();
        assert(formalSymbol!=NULL);
        if (actuals[i]==NULL)
            throw std::runtime_error("rose::Snippet: snippet '"+name+"' actual argument "+numberToString(i+1)+" is null");
        if (!isSgInitializedName(actuals[i]) && !isSgExpression(actuals[i]))
            throw std::runtime_error("rose::Snippet: snippet '"+name+"' actual argument "+numberToString(i+1)+
                                     " must be a variable declaration or expression"
                                     " but has type "+actuals[i]->class_name());
        bindings[formalSymbol] = actuals[i];
    }
    replaceArguments(toInsert, bindings);

    // Copy into the target file other functions, variables, imports, etc. that are above the snippet SgFunctionDefinition in
    // the snippet's file but which the user wants copied nonetheless.  Some of these things might be referenced by the
    // snippet, and others might completely unrelated but the user wants them copied anyway.
    insertRelatedThings(insertionPoint);

 // insertionPoint->get_file_info()->display("insertionPoint: test 4: debug");

    if (insertRecursively)
        file->expandSnippets(toInsert);

#if 1
    if (fixupAst) {
     // DQ (2/26/2014): Adding support to fixup the AST fragment (toInsert) that is being inserted into the target AST.

     // Build a translation map so that we can save the mapping of scopes between the target AST and the snippet AST.
     // In the case of insertMechanism == INSERT_STMTS we also add the mapping of the scope used for the copy of the body
     // to the parent of the insertionPoint so that symbols can be reset into the scope of the insertionPoint.
     // This map connects snippet scopes (key) to target AST scopes (value).
     // std::map<SgNode*,SgNode*> translationMap;

     // If we are inserting into the end of a scope then we point to the scope since there is no last statement 
     // to insert a statement before.  In this case then insertionPointIsScope == true, else it is false. 
        bool insertionPointIsScope = false;

        if (insertMechanism == INSERT_BODY)
           {
#if 0 // DEBUGGING [DQ 2014-03-07]
             printf ("In Snippet::insert(): insertMechanism == INSERT_BODY: "
                     "(isSgDeclarationStatement(stmts_copy_of_snippet_ast[i]) == false) \n");
#endif
          // Fill in the first entry to inlcude the mapping of the copy of the scope (body) to the associated scope of the
          // insertionPoint.
          // if (translationMap.find(ast->get_body()) == translationMap.end());
          //      translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(), insertionPoint->get_scope() ) );

          // SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,insertionPointIsScope,toInsert,ast->get_body(),translationMap);
             SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,insertionPointIsScope,toInsert,ast->get_body());
           }
          else
           {
             if (insertMechanism == INSERT_STMTS)
                {
                  const SgStatementPtrList &stmts_copy_of_snippet_ast     = toInsert->getStatementList();
                  const SgStatementPtrList &stmts_in_original_snippet_ast = ast->get_body()->getStatementList();
                  ROSE_ASSERT(stmts_copy_of_snippet_ast.size() == stmts_in_original_snippet_ast.size());

                  for (size_t i = 0; i < stmts_copy_of_snippet_ast.size(); ++i) 
                     {
                    // We have to reset this since it can be changed by one of the cases below.
                       insertionPointIsScope = false;

                       if (isSgDeclarationStatement(stmts_copy_of_snippet_ast[i]) != NULL) 
                          {
                            switch (locDeclsPosition) 
                               {
                                 case LOCDECLS_AT_BEGINNING:
                                   // SageInterface::insertStatementBefore(targetFirstDeclaration, stmts[i]);
#if 0 // DEBUGGING [DQ 2014-03-07]
                                      printf ("In Snippet::insert(): insertMechanism == INSERT_STMTS: "
                                              "(isSgDeclarationStatement(stmts_copy_of_snippet_ast[i]) != NULL): "
                                              "case LOCDECLS_AT_BEGINNING \n");
#endif
                                   // Fill in the first entry to inlcude the mapping of the copy of the scope (body) to the
                                   // associated scope of the insertionPoint.
                                   // if (translationMap.find(ast->get_body()) == translationMap.end());
                                   //      translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(),insertionPoint->get_scope() ) );
                                      if (targetFirstDeclaration) {
                                          SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(targetFirstDeclaration,
                                                                                                    insertionPointIsScope,
                                                                                                    stmts_copy_of_snippet_ast[i],
                                                                                                    stmts_in_original_snippet_ast[i]);
                                      } else {
                                          SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(targetFirstStatement,
                                                                                                    insertionPointIsScope,
                                                                                                    stmts_copy_of_snippet_ast[i],
                                                                                                    stmts_in_original_snippet_ast[i]);
                                      }
                                      break;

                                 case LOCDECLS_AT_END:
                                   // SageInterface::insertStatementAfterLastDeclaration(stmts[i], targetFunctionScope);
#if 0 // DEBUGGING [DQ 2014-03-07]
                                      printf ("In Snippet::insert(): insertMechanism == INSERT_STMTS: "
                                              "(isSgDeclarationStatement(stmts_copy_of_snippet_ast[i]) != NULL): "
                                              "case LOCDECLS_AT_END \n");
#endif
                                   // We are providing the scope instead of the declaration reference.
                                      insertionPointIsScope = true;

                                   // Fill in the first entry to inlcude the mapping of the copy of the scope (body) to the
                                   // associated scope of the insertionPoint.
                                   // if (translationMap.find(ast->get_body()) == translationMap.end());
                                   //      translationMap.insert( std::pair<SgNode*,SgNode*>(ast->get_body(), insertionPoint));

                                      SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(targetFunctionScope,
                                                                                                insertionPointIsScope,
                                                                                                stmts_copy_of_snippet_ast[i],
                                                                                                stmts_in_original_snippet_ast[i]);
                                      break;

                                   case LOCDECLS_AT_CURSOR:
                                      SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,
                                                                                                insertionPointIsScope,
                                                                                                stmts_copy_of_snippet_ast[i],
                                                                                                stmts_in_original_snippet_ast[i]);
                                      break;
                               }
                          } 
                         else 
                          {
                         // SageInterface::insertStatementBefore(insertionPoint, stmts[i]);
#if 0 // DEBUGGING [DQ 2014-03-07]
                            printf ("In Snippet::insert(): insertMechanism == INSERT_STMTS: "
                                    "(isSgDeclarationStatement(stmts_copy_of_snippet_ast[i]) == NULL) \n");
#endif
                         // Fill in the first entry to inlcude the mapping of the copy of the scope (body) to the associated
                         // scope of the insertionPoint.
                         // if (translationMap.find(ast->get_body()) == translationMap.end());
                         //      translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(),insertionPoint->get_scope() ) );

                            SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,insertionPointIsScope,
                                                                                      stmts_copy_of_snippet_ast[i],
                                                                                      stmts_in_original_snippet_ast[i]);
                          }
                     }
                }
               else
                {
                  printf ("Error: insertMechanism should be either INSERT_BODY or INSERT_STMTS \n");
                  ROSE_ASSERT(false);
                }
           }
#else // DEBUGGING [DQ 2014-03-07]
         printf ("Skipping code to call the SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst() function \n");
#endif
    }
}

// class method
void
Snippet::causeUnparsing(SgNode *ast, Sg_File_Info *target)
{
    // Make sure that the specified AST is actually unparsed into the place it was inserted. This seems more complicated
    // than it should be.
    struct T1: AstSimpleProcessing {
        Sg_File_Info *target;
        T1(Sg_File_Info *target): target(target) {}
        void visit(SgNode *node) {
            if (SgLocatedNode *loc = isSgLocatedNode(node)) {
                loc->set_file_info(new Sg_File_Info(*target));
                loc->set_startOfConstruct(new Sg_File_Info(*target));
                loc->set_endOfConstruct(new Sg_File_Info(*target));
                loc->get_file_info()->setOutputInCodeGeneration();
            }
        }
    } t1(target);
    t1.traverse(ast, preorder);
}

void
Snippet::renameTemporaries(SgNode *ast)
{
    assert(this!=NULL);

    struct: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgInitializedName *vdecl = isSgInitializedName(node)) {
                if (0==vdecl->get_name().getString().substr(0, 3).compare("tmp")) {
                    std::string newName = SnippetFile::randomVariableName();

                 // DQ (3/2/2014): Need to unload the associated symbol from the symbol tabel and reinsert it using the new
                 // name.  I assume this can not be a function parameter (else there will be no symbol).
                    SgSymbol* variableSymbol = vdecl->search_for_symbol_from_symbol_table();
                    ROSE_ASSERT(variableSymbol != NULL);
                    vdecl->get_scope()->remove_symbol(variableSymbol);

                    vdecl->set_name(newName);

                    vdecl->get_scope()->insert_symbol(newName,variableSymbol);
                }
            }
        }
    } t1;
    t1.traverse(ast, preorder);
}

void
Snippet::replaceVariable(SgVarRefExp *vref, SgExpression *replacement)
{
    struct Replacer: public SimpleReferenceToPointerHandler {
        SgNode *nodeToReplace, *replacement;
        bool replaced;
        Replacer(SgNode *nodeToReplace, SgNode *replacement)
            : nodeToReplace(nodeToReplace), replacement(replacement), replaced(false) {}
        void operator()(SgNode *&node, const SgName &debugStringName, bool/*traverse*/) {
            if (node==nodeToReplace) {
                node = replacement;
                ++replaced;
            }
        }
    } replacer(vref, replacement);
    vref->get_parent()->processDataMemberReferenceToPointers(&replacer);
    assert(replacer.replaced==1);
}

// class method
void
Snippet::replaceArguments(SgNode *toInsert, const ArgumentBindings &bindings)
{
    struct T1: AstSimpleProcessing {
        const ArgumentBindings &bindings;
        T1(const ArgumentBindings &bindings): bindings(bindings) {}

        void visit(SgNode *node) {
            if (SgVarRefExp *vref = isSgVarRefExp(node)) {
                SgSymbol *formal_sym = vref->get_symbol(); // snippet symbol to be replaced
                if (SgNode *bound = bindings.get_value_or(formal_sym, NULL)) {
                    if (SgInitializedName *replacement = isSgInitializedName(bound)) {
                        // Replace one variable reference with another. Rather than creating a new variable reference, we can
                        // just make the existing reference point to the replacement symbol.
                        SgVariableSymbol *new_sym = isSgVariableSymbol(replacement->search_for_symbol_from_symbol_table());
                        assert(new_sym!=NULL);
                        vref->set_symbol(new_sym);
                    } else if (SgExpression *replacement = isSgExpression(bound)) {
                        // The variable reference needs to be replaced by a new expression.
                        replaceVariable(vref, replacement);
                    } else {
                        assert(!"replacement is something weird");
                    }
                }
            } else if (SgTypedefDeclaration *tdef = isSgTypedefDeclaration(node)) {
                std::string tdef_name = tdef->get_name().getString();
                for (ArgumentBindings::const_iterator bi=bindings.begin(); bi!=bindings.end(); ++bi) {
                    if (0==tdef_name.compare("typeof_" + bi->first->get_name().getString())) {
                        if (SgInitializedName *actual = isSgInitializedName(bi->second)) {
                            tdef->set_base_type(actual->get_type());
                        } else if (SgExpression *actual = isSgExpression(bi->second)) {
                            tdef->set_base_type(actual->get_type());
                        } else {
                            assert(!"actual is something weird");
                        }
                    }
                }
            }
        }
    } t1(bindings);
    t1.traverse(toInsert, postorder); // post-order because we're modifying the AST as we retreat
}

void
Snippet::removeIncludeDirectives(SgNode *node)
{
    if (SgLocatedNode *locnode = isSgLocatedNode(node)) {
        // AttachedPreprocessingInfoType is std::vector<PreprocessingInfo>
        if (AttachedPreprocessingInfoType *cpp = locnode->getAttachedPreprocessingInfo()) {
            AttachedPreprocessingInfoType::iterator iter=cpp->begin();
            while (iter!=cpp->end()) {
                if ((*iter)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ||
                    (*iter)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeNextDeclaration) {
                    iter = cpp->erase(iter);
                } else {
                    ++iter;
                }
            }
        }
    }
}

void
Snippet::insertRelatedThings(SgStatement *insertionPoint)
{
    assert(this!=NULL);
    assert(insertionPoint!=NULL);

#if 0 // DEBUGGING [DQ 2014-03-07]
    printf ("Snippet::insertGlobalStuff(): Skip calling this function: insertionPoint = %p = %s \n",
            insertionPoint,insertionPoint->class_name().c_str());
    return;
#endif

    // Have we inserted stuff here already? Also mark that we've now done so.
    SgGlobal *ipointGlobalScope = SageInterface::getGlobalScope(insertionPoint);
    assert(ipointGlobalScope!=NULL);
    if (file->globallyInjected(ipointGlobalScope))
        return;

    // Language specific insertions
    if (SageInterface::is_Java_language()) {
        insertRelatedThingsForJava(insertionPoint);
    } else if (SageInterface::is_C_language()) {
        insertRelatedThingsForC(insertionPoint);
    }
}

void
Snippet::insertRelatedThingsForJava(SgStatement *insertionPoint)
{
    // The insertionPoint is some statement in a SgMemberFunctionDeclaration where this snippet was already inserted.  The
    // snippet's statements have already been inserted into the target function, and now we need to insert declarations that
    // are siblings of this snippet (in the snippet file) as siblings of the target SgMemberFunctionDeclaration into which this
    // snippet was inserted.
    SgStatement *topInsertionPoint = SageInterface::getEnclosingNode<SgMemberFunctionDeclaration>(insertionPoint);
    ROSE_ASSERT(topInsertionPoint || !"Java snippet must have been inserted into a SgMemberFunctionDeclaration");
    
    // Insert whole declarations from the snippet file into the target file.
    SgClassDefinition *snippetClass = SageInterface::getEnclosingNode<SgClassDefinition>(ast); // snippet's class
    ROSE_ASSERT(snippetClass || !"each java snippet must be a member of a class definition");
    BOOST_FOREACH (SgDeclarationStatement *decl, snippetClass->get_members()) {
        // Insert only those things that come from the snippet file, not from header files
        if (!decl->get_file_info()->isSameFile(ast->get_file_info()))
            continue;

        // If a declaration is blacklisted in the snippet file then don't insert it.
        if (file->isBlackListed(decl))
            continue;

        // Insert whole function definitions (snippets) only if the user asked for this feature.
        if (SgFunctionDeclaration *fdecl = isSgMemberFunctionDeclaration(decl)) {
            if (fdecl->get_definition()!=NULL && !file->getCopyAllSnippetDefinitions())
                continue;
        }

        // Insert this declaration
        SgTreeCopy deep;
        SgDeclarationStatement *declCopy = isSgDeclarationStatement(decl->copy(deep));
        causeUnparsing(declCopy, topInsertionPoint->get_file_info());
        SageInterface::insertStatementBefore(topInsertionPoint, declCopy);

     // DQ (3/19/2014): Added fixup of AST for Java declarations copied into the AST.
        if (fixupAst) {
            // DQ (3/13/2014): Added more general support for AST fixup (after insertion into the AST).  If we are inserting into
            // the end of a scope then we point to the scope since there is no last statement to insert a statement before.  In
            // this case then insertionPointIsScope == true, else it is false.  I think that in the cases called by this function
            // insertionPointIsScope is always false.
            bool insertionPointIsScope        = false;
            SgStatement* toInsert             = declCopy;
            SgStatement* original_before_copy = decl;
            // std::map<SgNode*,SgNode*> translationMap;

            SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(topInsertionPoint, insertionPointIsScope, toInsert,
                                                                      original_before_copy);
        }
    }

    // Copy import statements from the snippet's file into the target file.
    SgSourceFile *targetFile = isSgSourceFile(SageInterface::getEnclosingFileNode(insertionPoint));
    ROSE_ASSERT(targetFile || !"snippet insertion point must belong to a file");
    SgSourceFile *snippetFile = isSgSourceFile(SageInterface::getEnclosingFileNode(ast));
    ROSE_ASSERT(snippetFile || !"snippet must belong to a file");
    SgJavaImportStatementList *targetImports = targetFile->get_import_list();
    SgJavaImportStatementList *snippetImports = snippetFile->get_import_list();
    assert(targetImports!=NULL && snippetImports!=NULL);
    BOOST_FOREACH (SgJavaImportStatement *snippetImport, snippetImports->get_java_import_list()) {
        SgTreeCopy deep;
        SgJavaImportStatement *newImport = isSgJavaImportStatement(snippetImport->copy(deep));
        causeUnparsing(newImport, topInsertionPoint->get_file_info());
        targetImports->get_java_import_list().push_back(newImport);
        newImport->set_parent(targetImports);
    }
}

void
Snippet::insertRelatedThingsForC(SgStatement *insertionPoint)
{
    // The insertionPoint was where we inserted the snippet. To insert related global stuff, we need another insertion point
    // that's near the top of the same file where the snippet was inserted.  We want to insert stuff after all the #include
    // directives, so look for the last global declaration that has #include attached in this file. That's where we'll do our
    // inserting.
    SgGlobal *ipointGlobalScope = SageInterface::getGlobalScope(insertionPoint);
    const SgDeclarationStatementPtrList &stmtList = ipointGlobalScope->get_declarations();
    SgStatement *firstDeclSameFile = NULL;              // first declaration in the insertion point's file
    SgStatement *lastDeclWithIncludes = NULL;           // last declaration in this file that has attached #include
    BOOST_FOREACH (SgStatement *decl, stmtList) {
        if (decl->get_file_info()->isSameFile(ipointGlobalScope->get_file_info())) {
            if (NULL==firstDeclSameFile)
                firstDeclSameFile = decl;
            if (AttachedPreprocessingInfoType *cpplist = decl->getAttachedPreprocessingInfo()) {
                BOOST_FOREACH (PreprocessingInfo *cpp, *cpplist) {
                    if (cpp->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ||
                        cpp->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeNextDeclaration) {
                        lastDeclWithIncludes = decl;
                        break;
                    }
                }
            }
        }
    }
    SgStatement *topInsertionPoint = lastDeclWithIncludes ? lastDeclWithIncludes : firstDeclSameFile;
    ROSE_ASSERT(topInsertionPoint || !"cannot find an insertion point for snippet global declarations");
    SgGlobal *snippetGlobalScope = SageInterface::getEnclosingNode<SgGlobal>(ast);
    assert(snippetGlobalScope!=NULL);

    // Insert declarations
    SgStatement *firstInserted = NULL; // earliest node (in traversal) we inserted into the target AST
    BOOST_FOREACH (SgDeclarationStatement *decl, snippetGlobalScope->get_declarations()) {
        // Insert only those things that come from the snippet file, not from header files
        if (!decl->get_file_info()->isSameFile(ast->get_file_info()))
            continue;

        // If a declaration is blacklisted in the snippet file then don't insert it.
        if (file->isBlackListed(decl))
            continue;

        // Insert whole function definitions (snippets) only if the user asked for this feature.
     // if (SgFunctionDeclaration *fdecl = isSgMemberFunctionDeclaration(decl)) {
        if (SgFunctionDeclaration *fdecl = isSgFunctionDeclaration(decl)) {
            if (fdecl->get_definition()!=NULL && !file->getCopyAllSnippetDefinitions())
                continue;
        }

        // Insert this declaration
        SgTreeCopy deep;
        SgDeclarationStatement *declCopy = isSgDeclarationStatement(decl->copy(deep));
        removeIncludeDirectives(declCopy);
        causeUnparsing(declCopy, topInsertionPoint->get_file_info());

     // Error checking on the generated copy.
        SgClassDeclaration* classDeclaration_copy     = isSgClassDeclaration(declCopy);
        SgClassDeclaration* classDeclaration_original = isSgClassDeclaration(decl);
        if (classDeclaration_original != NULL)
           {
             ROSE_ASSERT(classDeclaration_copy != NULL);

             printf ("Error checking for copy of SgClassDeclaration = %p \n",classDeclaration_copy);

          // I think these types should be different,  if so then the associated non-defining 
          // declaration must be copied, and it's types match the defining declaration.
             ROSE_ASSERT(classDeclaration_copy->get_type() != classDeclaration_original->get_type());
             ROSE_ASSERT(classDeclaration_copy->get_firstNondefiningDeclaration() != classDeclaration_original->get_firstNondefiningDeclaration());

             SgClassDeclaration* nondefining_classDeclaration_copy     = isSgClassDeclaration(classDeclaration_copy->get_firstNondefiningDeclaration());
             SgClassDeclaration* nondefining_classDeclaration_original = isSgClassDeclaration(classDeclaration_original->get_firstNondefiningDeclaration());
             ROSE_ASSERT(nondefining_classDeclaration_copy->get_type() != nondefining_classDeclaration_original->get_type());
             if (nondefining_classDeclaration_copy->get_type() != classDeclaration_copy->get_type())
                {
                  printf ("nondefining_classDeclaration_copy->get_type() = %p \n",nondefining_classDeclaration_copy->get_type());
                  printf ("classDeclaration_copy->get_type()             = %p \n",classDeclaration_copy->get_type());
                }
             ROSE_ASSERT(nondefining_classDeclaration_copy->get_type() == classDeclaration_copy->get_type());
             ROSE_ASSERT(nondefining_classDeclaration_original->get_type() == classDeclaration_original->get_type());

             ROSE_ASSERT(classDeclaration_copy->get_definingDeclaration() != classDeclaration_original->get_definingDeclaration());
             SgClassDeclaration* defining_classDeclaration_copy     = isSgClassDeclaration(classDeclaration_copy->get_definingDeclaration());
             SgClassDeclaration* defining_classDeclaration_original = isSgClassDeclaration(classDeclaration_original->get_definingDeclaration());
             ROSE_ASSERT(defining_classDeclaration_original->get_type() == nondefining_classDeclaration_original->get_type());
             ROSE_ASSERT(defining_classDeclaration_copy->get_type() == nondefining_classDeclaration_copy->get_type());
           }

        SageInterface::insertStatementBefore(topInsertionPoint, declCopy);
        if (!firstInserted)
            firstInserted = declCopy;
        
        if (fixupAst) {
            // DQ (3/13/2014): Added more general support for AST fixup (after insertion into the AST).  If we are inserting into
            // the end of a scope then we point to the scope since there is no last statement to insert a statement before.  In
            // this case then insertionPointIsScope == true, else it is false.  I think that in the cases called by this function
            // insertionPointIsScope is always false.
            bool insertionPointIsScope        = false;
            SgStatement* toInsert             = declCopy;
            SgStatement* original_before_copy = decl;
            // std::map<SgNode*,SgNode*> translationMap;

            SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(topInsertionPoint, insertionPointIsScope, toInsert,
                                                                      original_before_copy);
        }
    }

    // If our topInsertionPoint had #include directives and we inserted stuff, then those include directives need to be moved
    // and reattached to the first node we inserted.
    if (firstInserted!=NULL && lastDeclWithIncludes!=NULL)
        SageInterface::movePreprocessingInfo(lastDeclWithIncludes, firstInserted);
}

} // namespace

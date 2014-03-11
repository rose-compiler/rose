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
                functions[fdef->get_declaration()->get_qualified_name()] = fdef;
        }
    } snippetFinder(functions);
    snippetFinder.traverse(ast, preorder);
}

SnippetPtr
SnippetFile::findSnippet(const std::string &snippetName)
{
    assert(this!=NULL);
    assert(!snippetName.empty());
    if (SgFunctionDefinition *fdef = functions.get_value_or(snippetName, NULL))
        return SnippetPtr(new Snippet(snippetName, shared_from_this(), fdef));
    return SnippetPtr();
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
                    if (self->functions.get_value_or(called_name, NULL))
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
        SnippetPtr snippet = findSnippet(ci->second);
        assert(fcall!=NULL && snippet!=NULL);
        const SgExpressionPtrList &fcall_args = fcall->get_args()->get_expressions();
        std::vector<SgNode*> actuals(fcall_args.begin(), fcall_args.end());
        SgStatement *toReplace = SageInterface::getEnclosingNode<SgStatement>(fcall);
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

    // Make it look like the entire snippet file actually came from the same file as the insertion point. This is an attempt to
    // avoid unparsing problems where the unparser asserts things such as "the file for a function declaration's scope must be
    // the same file as the function declaration". Even if we deep-copy the function declaration from the snippet file and
    // insert it into the specimen, when unparsing the specimen the declaration's scope will still point to the original scope
    // in the snippet file.
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
                            SageInterface::insertStatementBefore(targetFirstDeclaration, stmts[i]);
                            break;
                        case LOCDECLS_AT_END:
                            SageInterface::insertStatementAfterLastDeclaration(stmts[i], targetFunctionScope);
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
    if (!file->getCopyRelatedThings()) {
     // DQ (2/26/2014): Adding support to fixup the AST fragment (toInsert) that is being inserted into the target AST.

     // Build a translation map so that we can save the mapping of scopes between the target AST and the snippet AST.
     // In the case of insertMechanism == INSERT_STMTS we also add the mapping of the scope used for the copy of the body
     // to the parent of the insertionPoint so that symbols can be reset into the scope of the insertionPoint.
     // This map connects snippet scopes (key) to target AST scopes (value).
        std::map<SgNode*,SgNode*> translationMap;

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
             if (translationMap.find(ast->get_body()) == translationMap.end());
                  translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(), insertionPoint->get_scope() ) );

             SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,insertionPointIsScope,toInsert,
                                                                       ast->get_body(),translationMap);
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
                                      if (translationMap.find(ast->get_body()) == translationMap.end());
                                           translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(),
                                                                                              insertionPoint->get_scope() ) );

                                      SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(targetFirstDeclaration,
                                                                                                insertionPointIsScope,
                                                                                                stmts_copy_of_snippet_ast[i],
                                                                                                stmts_in_original_snippet_ast[i],
                                                                                                translationMap);
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
                                      if (translationMap.find(ast->get_body()) == translationMap.end());
                                           translationMap.insert( std::pair<SgNode*,SgNode*>(ast->get_body(), insertionPoint));

                                      SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(targetFunctionScope,
                                                                                                insertionPointIsScope,
                                                                                                stmts_copy_of_snippet_ast[i],
                                                                                                stmts_in_original_snippet_ast[i],
                                                                                                translationMap);
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
                            if (translationMap.find(ast->get_body()) == translationMap.end());
                                 translationMap.insert( std::pair<SgNode*,SgNode*>( ast->get_body(),
                                                                                    insertionPoint->get_scope() ) );

                            SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,insertionPointIsScope,
                                                                                      stmts_copy_of_snippet_ast[i],
                                                                                      stmts_in_original_snippet_ast[i],
                                                                                      translationMap);
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
Snippet::insertIncludeDirective(SgStatement *insertionPoint, PreprocessingInfo *includeDirective)
{
    assert(this!=NULL);
    assert(insertionPoint!=NULL);
    assert(includeDirective!=NULL);
    assert(includeDirective->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration);
    std::string inc = includeDirective->getString(); // the entire thing, with comment if any
    SgGlobal *ipoint_globscope = SageInterface::getEnclosingNode<SgGlobal>(insertionPoint);
    assert(ipoint_globscope!=NULL);

    // match "#include <filename> ...."
    std::string filename, m2;
    bool is_system_header = false;
#if 1
    // We do this the hard way because boost::regex doesn't ever seem to work right for me (throws bad_alloc this time).
    do {
        const char *cstr = inc.c_str();
        const char *s = cstr;
        while (isspace(*s)) ++s;
        if ('#'!=*s) break;
        ++s;
        while (isspace(*s)) ++s;
        if (strncmp(s, "include", 7)) break;
        s += 7;
        while (isspace(*s)) ++s;
        if ('"'!=*s && '<'!=*s) break;
        is_system_header = '<'==*s;
        ++s;
        const char *filename_start = s;
        while (*s && '"'!=*s && '>'!=*s) ++s;
        if ('"'!=*s && '>'!=*s) break;
        filename = std::string(filename_start, s);
        ++s;
        while (isspace(*s)) ++s;
        m2 = std::string(s);
    } while (0);
#else
    boost::regex re("(\\s*#\\s*include\\s*[\"<][^\">]*[\">])\\s*(.*)");
    boost::smatch match_data;
    if (boost::regex_match(inc, match_data, re)) {
        filename = std::string(match_data[1].first, match_data[1].second);
        m2 = std::string(match_data[2].first, match_data[2].second);
    }
#endif
    assert(!filename.empty());

    // Skip insertion if we already inserted it.
    if (file->fileIsIncluded(filename, ipoint_globscope))
        return;

    // Strip non-word characters from the beginning and end of m2 (keep '::')
    {
        const char *word_start = m2.c_str();
        while (*word_start && ':'!=*word_start && '_'!=*word_start && !isalnum(*word_start)) ++word_start;
        const char *word_end = word_start;
        while (':'==*word_end || '_'==*word_end || isalnum(*word_end)) ++word_end;
        m2 = std::string(word_start, word_end);
    }

    // Does this m2 word exist as a function declaration or typedef name
    // FIXME[ROBB P. MATZKE 2013-12-27]: We should use AST iterators and avoid visiting subtrees when possible.
    SgNode *exists = NULL;
    struct T1: AstSimpleProcessing {
        std::string name;
        T1(const std::string &name): name(name) {}
        void visit(SgNode *node) {
            if (SgFunctionDeclaration *fdecl = isSgFunctionDeclaration(node)) {
                if (0==fdecl->get_name().getString().compare(name) ||
                    0==fdecl->get_qualified_name().getString().compare(name))
                    throw fdecl; // bypass potentially long traversal
            } else if (SgTypedefType *tdef = isSgTypedefType(node)) {
                if (0==tdef->get_name().getString().compare(name))
                    throw tdef; // bypass potentially long traversal
            }
        }
    } t1(m2);
    try {
        if (!m2.empty())
            t1.traverse(ipoint_globscope, preorder);
    } catch (SgFunctionDeclaration *found) {
        exists = found;
    } catch (SgTypedefType *found) {
        exists = found;
    }

    // Insert the include if necessary
    if (!exists || m2.empty()) {
#if 0
        // [Robb P. Matzke 2014-02-27]: this attaches it too late in the file; we need it before the globals we inserted
        SageInterface::insertHeader(filename, PreprocessingInfo::before, is_system_header, ipoint_globscope);
#else
        insertionPoint->addToAttachedPreprocessingInfo(includeDirective, PreprocessingInfo::before);
#endif
    }
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

bool
Snippet::hasCommentMatching(SgNode *ast, const std::string &toMatch)
{
    struct Visitor: AstSimpleProcessing {
        std::string toMatch;
        bool foundComment;
        Visitor(const std::string &toMatch): toMatch(toMatch), foundComment(false) {}
        void visit(SgNode *node) {
            if (!foundComment) {
                if (SgLocatedNode *lnode = isSgLocatedNode(node)) {
                    if (AttachedPreprocessingInfoType *cpplist = lnode->getAttachedPreprocessingInfo()) {
                        BOOST_FOREACH (PreprocessingInfo *cpp, *cpplist) {
                            switch (cpp->getTypeOfDirective()) {
                                case PreprocessingInfo::C_StyleComment:
                                case PreprocessingInfo::CplusplusStyleComment:
                                case PreprocessingInfo::F90StyleComment:
                                    foundComment = boost::contains(cpp->getString(), toMatch);
                                default:
                                    break;
                            }
                        }
                    }
                }
            }
        }
    } visitor(toMatch);
    visitor.traverse(ast, preorder);
    return visitor.foundComment;
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

    if (file->getCopyAllSnippetDefinitions()|| file->getCopyRelatedThings()) {
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
        if (!decl->get_file_info()->isSameFile(ast->get_file_info()))
            continue;                                   // this came from some other file than the snippet file

        if (SgMemberFunctionDeclaration *fdecl = isSgMemberFunctionDeclaration(decl)) {
            SgFunctionDefinition *fdef = fdecl->get_definition();
            if (file->getCopyAllSnippetDefinitions() && fdef!=NULL) {
                SgTreeCopy deep;
                SgFunctionDeclaration *fdeclCopy = isSgFunctionDeclaration(fdecl->copy(deep));
                causeUnparsing(fdeclCopy, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, fdeclCopy);
                continue;
            }
        }

        if (SgVariableDeclaration *vdecl = isSgVariableDeclaration(decl)) {
            if (file->getCopyRelatedThings()) {
                // Insert non-extern variable declaration
                SgTreeCopy deep;
                SgStatement *newStmt = isSgStatement(vdecl->copy(deep));
                causeUnparsing(newStmt, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, newStmt);
                continue;
            }
        }
    }

    // Copy import statements from the snippet's file into the target file.
    if (file->getCopyRelatedThings()) {
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
        
    // What should be inserted?
    SgGlobal *snippetGlobalScope = SageInterface::getEnclosingNode<SgGlobal>(ast);
    assert(snippetGlobalScope!=NULL);
    const SgDeclarationStatementPtrList &snippetStmts = snippetGlobalScope->get_declarations();

 // DQ (3/6/2014): Check if the declaration referenced in the snippet are expected to be in the target AST.
 // This allows the snippet mechanism to be more general that required for use that would allow the AST fixup 
 // to be done (such as generating unparsable code, but not code that might pass an analysis phase).
    ROSE_ASSERT(getFile() != NULL);
    printf ("getFile()->getCopyRelatedThings() = %s \n",getFile()->getCopyRelatedThings() == true ? "true" : "false");

    // Do the insertion for non-preprocessor stuff
    SgStatement *firstInserted = NULL;
    BOOST_FOREACH (SgStatement *snippetStmt, snippetStmts) {

        // If this node came from a file included by the snippet file, then skip it.  We only ever insert things that came from
        // the snippet file itself.
        if (!snippetStmt->get_file_info()->isSameFile(ast->get_file_info()))
            continue; // this came from some included file rather than the snippet itself.

        // If a declaration is blacklisted in the snippet file then don't insert it.
        if (hasCommentMatching(snippetStmt, "DO_NOT_INSERT"))
            continue;

        if (SgVariableDeclaration *vdecl = isSgVariableDeclaration(snippetStmt)) {
            if (file->getCopyRelatedThings() && !vdecl->get_declarationModifier().get_storageModifier().isExtern()) {
                // Insert non-extern variable declaration
                SgTreeCopy deep;
                SgStatement *newStmt = isSgStatement(snippetStmt->copy(deep));
                removeIncludeDirectives(newStmt);
                causeUnparsing(newStmt, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, newStmt);

                ROSE_ASSERT(file->getCopyRelatedThings());
#if 0 /* [Robb P. Matzke 2014-03-07]: Ast fixups not needed here since SnippetFile::getCopyRelatedThings returns true */
             // DQ (3/2/2014): The copy of a statement will not handle it's associated symbols if they are in a scope outside 
             // of the statement.  So we have to explicitly build symbols for any new statements to be copied and inserted into 
             // the target scope (this is easier than fixing up the AST after the insertion, since it is less clear what has 
             // been inserted).
                SgScopeStatement* scope = isSgScopeStatement(topInsertionPoint->get_parent());
                ROSE_ASSERT(scope != NULL);

                printf ("In Snippet::insertGlobalStuff(): Insert symbols into scope = %p = %s \n",
                        scope,scope->class_name().c_str());
                SgVariableDeclaration* new_vdecl = isSgVariableDeclaration(newStmt);
                ROSE_ASSERT(new_vdecl != NULL);
                SgInitializedNamePtrList & initializedNameList = new_vdecl->get_variables();

             // We have to set the scope explicitly.
                for (size_t i = 0; i < initializedNameList.size(); i++)
                   {
                  // Build symbols for each fo the variables (typically one for C/C++).
                     SgInitializedName* initializedName = initializedNameList[i];
                     ROSE_ASSERT(initializedName != NULL);

                  // Set the scope since it is still set to the scope in the original AST (because the scope is external to the
                  // copy fo a stmt).
                     initializedName->set_scope(scope);

                  // This is set by the AST copy, but we need to unset it when we move the variable to a new file.
                     initializedName->set_prev_decl_item(NULL);

                  // SgVariableSymbol* buildVariableSymbol(initializedName);
                   }

             // fixup the symbol table (add symbols for each SgInitializedName).
                SageInterface::supportForInitializedNameLists(scope,initializedNameList);
#endif

                if (!firstInserted)
                    firstInserted = newStmt;
                continue;
            }
        }
        
        if (SgClassDeclaration *class_decl = isSgClassDeclaration(snippetStmt)) {
            if (file->getCopyRelatedThings() && class_decl->get_definition()!=NULL) {
                SgTreeCopy deep;
                SgStatement *newStmt = isSgClassDeclaration(class_decl);
                removeIncludeDirectives(newStmt);
                causeUnparsing(newStmt, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, newStmt);

                ROSE_ASSERT(file->getCopyRelatedThings());
#if 0 /* [Robb P. Matzke 2014-03-07]: Ast fixups not needed here since SnippetFile::getCopyRelatedThings returns true */
             // DQ (3/2/2014): The copy of a statement will not handle it's associated symbols if they are in a scope outside 
             // of the statement.  So we have to explicitly build symbols for any new statements to be copied and inserted into 
             // the target scope (this is easier than fixing up the AST after the insertion, since it is less clear what has 
             // been inserted).
                SgScopeStatement* scope = isSgScopeStatement(topInsertionPoint->get_parent());
                ROSE_ASSERT(scope != NULL);
                ROSE_ASSERT(scope->get_parent() != NULL);

#if 0 // DEBUGGING [DQ 2014-03-07]
                printf ("Snippet::insertRelatedThingsForC(): scope for insertion point = %p = %s \n",
                        scope,scope->class_name().c_str());
#endif

                SgClassDeclaration* cdecl_copy = isSgClassDeclaration(newStmt);
                cdecl_copy->set_scope(scope);

             // Lookup the symbol in the parent scopes of the insertion point in the traget program (must exist).
                SgClassSymbol* classSymbolInTargetAST = SageInterface::lookupClassSymbolInParentScopes(class_decl->get_name(),
                                                                                                       scope);
                if (classSymbolInTargetAST == NULL) {
                    // If we are requiring that language constructs be copied into the target AST, then we 
                    // assume that they don't already exist and so it is expected that we can't resolve symbols.
                    // Output a message and keep going.
                    throw std::runtime_error("Snippet::insertRelatedThings: cannot find SgClassSymbol for class_decl (" +
                                             class_decl->class_name() + "*)" + boost::lexical_cast<std::string>(class_decl) +
                                             " \"" + class_decl->get_name().str() + "\"");
                }

                ROSE_ASSERT(classSymbolInTargetAST != NULL);

                SgDeclarationStatement* decl = classSymbolInTargetAST->get_declaration();
                ROSE_ASSERT(decl != NULL);

                ROSE_ASSERT(cdecl_copy->get_scope() == scope);
                ROSE_ASSERT(decl->get_scope() == scope);

             // Set the first nondefining declaration to the declaration that exists for the target file (not the snippet file).
                cdecl_copy->set_firstNondefiningDeclaration(decl);

             // I assume this is true for classes defined in header files.
                ROSE_ASSERT(cdecl_copy->get_definingDeclaration() != NULL);
#if 0 // DEBUGGING [DQ 2014-03-07]
                printf ("SgClassDeclaration: Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
#endif

                if (!firstInserted)
                    firstInserted = newStmt;
                continue;
            }
        }

        if (SgFunctionDeclaration *fdecl = isSgFunctionDeclaration(snippetStmt)) {
            SgFunctionDefinition *fdef = fdecl->get_definition();
            
            if (file->getCopyAllSnippetDefinitions() && fdef!=NULL &&
                fdef->get_startOfConstruct()->get_file_id()==ast->get_startOfConstruct()->get_file_id()) {
                SgTreeCopy deep;
                SgFunctionDeclaration *fdecl_copy = isSgFunctionDeclaration(fdecl->copy(deep));
                removeIncludeDirectives(fdecl_copy);
                causeUnparsing(fdecl_copy, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, fdecl_copy);

             // DQ (3/2/2014): The copy of a statement will not handle it's associated symbols if they are in a scope outside 
             // of the statement.  So we have to explicitly build symbols for any new statements to be copied and inserted into 
             // the target scope (this is easier than fixing up the AST after the insertion, since it is less clear what has 
             // been inserted).
                SgScopeStatement* scope = isSgScopeStatement(topInsertionPoint->get_parent());
                ROSE_ASSERT(scope != NULL);
                ROSE_ASSERT(scope->get_parent() != NULL);

                fdecl_copy->set_scope(scope);

             // Lookup the symbol in the parent scopes of the insertion point in the traget program (must exist).
                SgFunctionSymbol* functionSymbolInTargetAST = SageInterface::lookupFunctionSymbolInParentScopes(fdecl->get_name(),
                                                                                                                scope);
                if (functionSymbolInTargetAST == NULL)
                   {
                     printf ("In Snippet::insertGlobalStuff(): Can't location function: name = %s in parent scopes of "
                             "insertion point \n",fdecl->get_name().str());
                     continue;
                   }
                ROSE_ASSERT(functionSymbolInTargetAST != NULL);
                SgDeclarationStatement* decl = functionSymbolInTargetAST->get_declaration();
                ROSE_ASSERT(decl != NULL);

                ROSE_ASSERT(fdecl_copy->get_scope() == scope);
                ROSE_ASSERT(decl->get_scope() == scope);

             // Set the first nondefining declaration to the declaration that exists for the target file (not the snippet file).
                fdecl_copy->set_firstNondefiningDeclaration(decl);

             // I assume this is true for function defined in libraries (fails test5c, OK).
             // ROSE_ASSERT(fdecl_copy->get_definingDeclaration() == NULL);
#if 0 // DEBUGGING [DQ 2014-03-07]
                printf ("Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
                if (!firstInserted)
                    firstInserted = fdecl_copy;
                continue;
            }

            if (fdecl->get_definition()==ast) {
                // Do not insert the snippet itself (this happens separately)
                continue;
            }

            if (file->getCopyRelatedThings()&& fdecl->get_definition()==NULL) {
                // Insert function declaration.
                SgTreeCopy deep;
                SgFunctionDeclaration *fdecl_copy = isSgFunctionDeclaration(fdecl->copy(deep));
                removeIncludeDirectives(fdecl_copy);
                causeUnparsing(fdecl_copy, topInsertionPoint->get_file_info());
                SageInterface::insertStatementBefore(topInsertionPoint, fdecl_copy);

                ROSE_ASSERT(file->getCopyRelatedThings());
#if 0 /* [Robb P. Matzke 2014-03-07]: Ast fixups not needed here since SnippetFile::getCopyRelatedThings returns true */
             // DQ (3/2/2014): The copy of a statement will not handle it's associated symbols if they are in a scope outside 
             // of the statement.  So we have to explicitly build symbols for any new statements to be copied and inserted into 
             // the target scope (this is easier than fixing up the AST after the insertion, since it is less clear what has 
             // been inserted).
                SgScopeStatement* scope = isSgScopeStatement(topInsertionPoint->get_parent());
                ROSE_ASSERT(scope != NULL);
                ROSE_ASSERT(scope->get_parent() != NULL);

                fdecl_copy->set_scope(scope);

             // Lookup the symbol in the parent scopes of the insertion point in the traget program (must exist).
                SgFunctionSymbol* functionSymbolInTargetAST = SageInterface::lookupFunctionSymbolInParentScopes(fdecl->get_name(),
                                                                                                                scope);
                ROSE_ASSERT(functionSymbolInTargetAST != NULL);
                SgDeclarationStatement* decl = functionSymbolInTargetAST->get_declaration();
                ROSE_ASSERT(decl != NULL);

                ROSE_ASSERT(fdecl_copy->get_scope() == scope);
                ROSE_ASSERT(decl->get_scope() == scope);

             // Set the first nondefining declaration to the declaration that exists for the target file (not the snippet file).
                fdecl_copy->set_firstNondefiningDeclaration(decl);

             // I assume this is true for function defined in libraries.
                ROSE_ASSERT(fdecl_copy->get_definingDeclaration() == NULL);
#if 0 // DEBUGGING [DQ 2014-03-07]
                printf ("(fdecl->get_definition()==NULL): Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
#endif
                if (!firstInserted)
                    firstInserted = fdecl_copy;
                continue;
            }
        }
    }

    // If our topInsertionPoint had #include directives and we inserted stuff, then those include directives need to be moved
    // and reattached to the first node we inserted.
    if (firstInserted!=NULL && lastDeclWithIncludes!=NULL)
        SageInterface::movePreprocessingInfo(lastDeclWithIncludes, firstInserted);

    // Insert #include directives above the first thing we already inserted.
    if (file->getCopyRelatedThings()) {
        if (firstInserted)
            topInsertionPoint = firstInserted;
        BOOST_FOREACH (SgStatement *snippetStmt, snippetStmts) {
            if (!snippetStmt->get_file_info()->isSameFile(ast->get_file_info()))
                continue; // this came from some included file rather than the snippet itself.
            if (AttachedPreprocessingInfoType *cpplist = snippetStmt->getAttachedPreprocessingInfo()) {
                // We apparently can't use the cpplist directly because some of the functions in SageInterface modify it while
                // we're trying to iterate.  So make a copy.
                AttachedPreprocessingInfoType cpplist_copy = *cpplist;
                BOOST_FOREACH (PreprocessingInfo *cpp, cpplist_copy) {
                    if (cpp->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ||
                        cpp->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeNextDeclaration)
                        insertIncludeDirective(topInsertionPoint, cpp); // attaches it to an existing node
                }
            }
        }
    }
}

#if 0
// DQ (2/27/2014): I think we should just call the refactored function directly
// (SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst()).
void 
Snippet::fixupSnippetInNewTargetAST(SgStatement *insertionPoint, SgStatement *toInsert)
   {
      ROSE_ASSERT(insertionPoint != NULL);
      ROSE_ASSERT(toInsert != NULL);

      printf ("Inside of fixupBodyInNewTargetAST(): insertionPoint = %p = %s toInsert = %p = %s \n",
              insertionPoint,insertionPoint->class_name().c_str(),toInsert,toInsert->class_name().c_str());

#if 0
      SgFile* targetFile = TransformationSupport::getFile(insertionPoint);

   // For Java support this might be NULL, if the insertion point was in global scope.
      ROSE_ASSERT(targetFile != NULL);

      SgFile* snippetFile = TransformationSupport::getFile(toInsert);

   // At this point the parent pointers are set so that the same SgFile is found via a traversal back to the SgProject.
      ROSE_ASSERT(snippetFile == targetFile);

      printf ("targetFile  = %p = %s \n",targetFile,targetFile->get_sourceFileNameWithPath().c_str());
      printf ("snippetFile = %p = %s \n",snippetFile,snippetFile->get_sourceFileNameWithPath().c_str());
#endif

   // Any node that has entries not referenced in the target file needs to be fixed up.
   // We can assume that any referenced variable or function that is referenced in the 
   // snippet will exist in either the snippet or the target file.
      SageBuilder::fixupCopyOfAstFromSeperateFileInNewTargetAst(insertionPoint,toInsert);
   }
#endif

} // namespace

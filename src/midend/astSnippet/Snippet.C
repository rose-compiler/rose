#include "Snippet.h"
#include "LinearCongruentialGenerator.h"
#include "rose_getline.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace rose {

/*******************************************************************************************************************************
 *                                      SnippetFile
 *******************************************************************************************************************************/

SnippetFile::Registry SnippetFile::registry;

// Class method
SnippetFilePtr
SnippetFile::instance(const std::string &filename)
{
    SnippetFilePtr retval = lookup(filename);
    if (retval!=NULL)
        return retval;

    retval = registry[filename] = SnippetFilePtr(new SnippetFile(filename));
    retval->parse();
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

void
SnippetFile::parse()
{
    assert(this!=NULL);
    assert(!fileName.empty());
    assert(ast==NULL);
    
    // We should never unparse the snippet to a separate file, so provide an invalid name to catch errors
    std::string outputName = "/SNIPPET_SHOULD_NOT_BE_UNPARSED/x";

    // Try to load the snippet by parsing its source file
    ast = SageBuilder::buildFile(fileName, outputName, SageInterface::getProject());
    ast->set_skip_unparse(true);

    // Find all snippet functions (they are the top-level function definitions)
    struct SnippetFinder: AstSimpleProcessing {
        FunctionDefinitionMap &functions;
        SnippetFinder(FunctionDefinitionMap &functions): functions(functions) {}

        void visit(SgNode *node) {
            if (SgFunctionDefinition *fdef = isSgFunctionDefinition(node))
                functions[fdef->get_declaration()->get_qualified_name()] = fdef;
        }
    } snippetFinder(functions);
    snippetFinder.traverse(ast, preorder);
                

#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-04]*/
    // Based on exampleTranslators/defaultTranslator/preprocessingInfoDumpber.C
    // but doesn't appear to work correctly -- "got preprocessing info" is never printed.
    struct: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgLocatedNode *loc = isSgLocatedNode(node)) {
                if (AttachedPreprocessingInfoType *cpp = loc->getAttachedPreprocessingInfo()) {
                    std::cerr <<"ROBB: got preprocessing info\n";
                    for (AttachedPreprocessingInfoType::iterator cppi=cpp->begin(); cppi!=cpp->end(); ++cppi) {
                        switch ((*cppi)->getTypeOfDirective()) {
                            case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                            case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:
                                std::cerr <<"ROBB:   " <<(*cppi)->getString() <<"\n";
                                break;
                            default: break; // prevent gcc warnings
                        }
                    }
                }
            }
        }
    } t1;
    t1.traverse(ast, preorder);
#endif
}

SnippetPtr
SnippetFile::findSnippet(const std::string &snippetName)
{
    assert(this!=NULL);
    assert(!snippetName.empty());
    std::string functionName =  snippetName.substr(0, 2)=="::" ? snippetName : "::" + snippetName;

    if (SgFunctionDefinition *fdef = functions.get_value_or(functionName, NULL))
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

#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-06]*/
        std::cerr <<"ROBB: recursive snippet call to " <<snippet->getName() <<"\n";
#endif
        const SgExpressionPtrList &fcall_args = fcall->get_args()->get_expressions();
        std::vector<SgNode*> actuals(fcall_args.begin(), fcall_args.end());
        SgStatement *toReplace = SageInterface::getEnclosingNode<SgStatement>(fcall);
        snippet->insert(toReplace, actuals);
        SageInterface::removeStatement(toReplace);
    }
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

    // Build a map binding formal argument symbols to their actual values
    ArgumentBindings bindings;
    SgFunctionDeclaration *snippet_fdecl = ast->get_declaration();
    const SgInitializedNamePtrList &formals = snippet_fdecl->get_parameterList()->get_args();
    if (actuals.size()!=formals.size()) {
        throw std::runtime_error("rose::Snippet: mismatched snippet arguments: expected " +
                                 plural(formals.size(), "arguments") + " but got " +
                                 numberToString(actuals.size()));
    }
    for (size_t i=0; i<formals.size(); ++i) {
        SgSymbol *symbol = formals[i]->search_for_symbol_from_symbol_table();
        assert(symbol!=NULL);
        if (actuals[i]==NULL)
            throw std::runtime_error("rose::Snippet: snippet '"+name+"' actual argument "+numberToString(i+1)+" is null");
        if (!isSgInitializedName(actuals[i]) && !isSgExpression(actuals[i]))
            throw std::runtime_error("rose::Snippet: snippet '"+name+"' actual argument "+numberToString(i+1)+
                                     " must be a variable declaration or expression"
                                     " but has type "+actuals[i]->class_name());
        bindings[symbol] = actuals[i];
    }

    // Insert the snippet body after the insertion point
    SgTreeCopy deep;
    SgStatement *toInsert = isSgStatement(ast->get_body()->copy(deep));
    assert(toInsert!=NULL);
    renameTemporaries(toInsert);
    causeUnparsing(toInsert);
    replaceArguments(toInsert, bindings);
    SageInterface::insertStatementBefore(insertionPoint, toInsert);

    insertGlobalStuff(insertionPoint);
    file->expandSnippets(toInsert);
}

// class method
void
Snippet::causeUnparsing(SgNode *ast)
{
    struct: AstSimpleProcessing {
        void visit(SgNode *node) {
            if (SgLocatedNode *loc = isSgLocatedNode(node))
                loc->get_file_info()->setOutputInCodeGeneration();
        }
    } t1;
    t1.traverse(ast, preorder);
}

void
Snippet::renameTemporaries(SgNode *ast)
{
    struct T1: AstSimpleProcessing {
        void visit(SgNode *node) {
            static LinearCongruentialGenerator random(2013120911ul);
            if (SgInitializedName *vdecl = isSgInitializedName(node)) {
                if (0==vdecl->get_name().getString().substr(0, 3).compare("tmp")) {
                    std::string newName = "T_";
#if 0   // This method generates random names that are less likely to conflict with existing names
                    static const char *letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    assert(strlen(letters)==2*26);
                    for (size_t i=0; i<6; ++i)
                        newName += letters[random()%(2*26)];
#else   // This method generates names that are more human friendly
                    static std::vector<std::string> words;
                    if (words.empty()) {
                        // Read variable names from a file
                        if (FILE *dict = fopen("/usr/share/dict/words", "r")) {
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
                                    words.push_back(word);
                                }
                            }
                            free(line);
                            fclose(dict);
                        }
                        assert(!words.empty());
                    }
                    static size_t ncalls = 0;
                    newName = words[random() % words.size()] + StringUtility::numberToString(++ncalls);
#endif
#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-09]*/
                    std::cerr <<"ROBB: renaming temporary variable " <<vdecl->get_name() <<" to \"" <<newName <<"\"\n";
#endif
                    vdecl->set_name(newName);
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
                        std::cerr <<"ROBB: found typedef for " <<bi->first->get_name() <<"\n";
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
Snippet::insertGlobalStuff(SgStatement *insertionPoint)
{
    assert(this!=NULL);
    assert(insertionPoint!=NULL);

    // Where should it be inserted?
    SgGlobal *ipoint_globscope = SageInterface::getEnclosingNode<SgGlobal>(insertionPoint);
    assert(ipoint_globscope!=NULL);
    assert(!ipoint_globscope->get_declarations().empty());
    SgDeclarationStatement *dst_cursor = ipoint_globscope->get_declarations().front();

    // Have we inserted stuff here already (and mark that we've now done so)?
    if (file->globallyInjected(ipoint_globscope))
        return;

    // What should be inserted?
    SgGlobal *snippet_globscope = SageInterface::getEnclosingNode<SgGlobal>(ast);
    assert(snippet_globscope!=NULL);
    const SgDeclarationStatementPtrList &stmts = snippet_globscope->get_declarations();

    // Do the insertion
    for (size_t i=0; i<stmts.size(); ++i) {
        if (SgLocatedNode *loc = isSgLocatedNode(stmts[i])) {
            if (AttachedPreprocessingInfoType *cpp = loc->getAttachedPreprocessingInfo()) {
                for (AttachedPreprocessingInfoType::iterator cppi=cpp->begin(); cppi!=cpp->end(); ++cppi) {
                    if ((*cppi)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ||
                        (*cppi)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeNextDeclaration) {
                        // Inject the #include directive by attaching it to an already present node.
                        std::cerr <<"ROBB: #include injection is not handled yet\n"; // FIXME
                    }
                }
            }

            if (loc->get_startOfConstruct()->get_file_id() != ast->get_startOfConstruct()->get_file_id()) {
                // this came from some included file rather than the snippet itself.
                continue;
            }
        }

        if (SgVariableDeclaration *vdecl = isSgVariableDeclaration(stmts[i])) {
            if (!vdecl->get_declarationModifier().get_storageModifier().isExtern()) {
                // Insert non-extern variable declaration
                SgTreeCopy deep;
                SgStatement *stmt = isSgStatement(stmts[i]->copy(deep));
                causeUnparsing(stmt);
                SageInterface::insertStatementBefore(dst_cursor, stmt);
                continue;
            }
        }
        
        if (SgClassDeclaration *cdecl = isSgClassDeclaration(stmts[i])) {
            if (cdecl->get_definition()!=NULL) {
                SgTreeCopy deep;
                SgStatement *stmt = isSgClassDeclaration(cdecl);
                causeUnparsing(stmt);
                SageInterface::insertStatementBefore(dst_cursor, stmt);
                continue;
            }
        }

        if (isSgFunctionDeclaration(stmts[i]) && isSgFunctionDeclaration(stmts[i])->get_definition()==ast) {
            // Do not insert the snippet itself (this happens separately)
            continue;
        }

#if 1 /*DEBUGGING [Robb P. Matzke 2013-12-05]*/
        std::cerr <<"ROBB: insertGlobalStuff: skipped " <<stmts[i]->class_name() <<"\n";
#endif
    }
}

} // namespace

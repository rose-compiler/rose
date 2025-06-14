#include "sage3basic.h"
#include "rose_config.h"

#include "SageTreeBuilder.h"
#include "Jovial_to_ROSE_translation.h"
#include "ModuleBuilder.h"

#include <boost/optional/optional_io.hpp>
#include <iostream>

namespace Rose {
namespace builder {

using namespace Rose::Diagnostics;

namespace SB = SageBuilder;
namespace SI = SageInterface;
namespace LT = LanguageTranslation;

/// Initialize the global scope and push it onto the scope stack
///
SgGlobal* initialize_global_scope(SgSourceFile* file)
{
 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);

    SgGlobal* globalScope = file->get_globalScope();
    ASSERT_not_null(globalScope);
    ASSERT_not_null(globalScope->get_parent());

 // Fortran and Jovial are case insensitive
    globalScope->setCaseInsensitive(true);

    ASSERT_not_null(globalScope->get_endOfConstruct());
    ASSERT_not_null(globalScope->get_startOfConstruct());

 // Not sure why this isn't set at construction
    globalScope->get_startOfConstruct()->set_line(1);
    globalScope->get_endOfConstruct()->set_line(1);

    SageBuilder::pushScopeStack(globalScope);

    return globalScope;
}

void
SageTreeBuilder::attachComments(SgLocatedNode* node, bool at_end)
{
  PosInfo pos{node};
  attachComments(node, pos, at_end);
}

void
SageTreeBuilder::attachComments(SgExpressionPtrList const &list)
{
  auto jovialStyle{PreprocessingInfo::JovialStyleComment};

  for (auto expr : list) {
    PosInfo exprPos{expr};
    auto commentToken = tokens_->getNextToken();

    // May have problems with multi-line expressions, currently biased to comments following the expression
    if (commentToken && exprPos.getEndLine() == commentToken->getStartLine()) {
      auto commentLocation = PreprocessingInfo::after;
      if (exprPos.getStartCol() >= commentToken->getEndCol()) {
        commentLocation = PreprocessingInfo::before;
      }
      auto info = SI::attachComment(expr, commentToken->getLexeme(), commentLocation, jovialStyle);
      setCommentPositionAndConsumeToken(info);
    }
  }
}

void
SageTreeBuilder::attachComments(SgLocatedNode* node, const PosInfo &pos, bool at_end)
{
  PreprocessingInfo* info{nullptr};
  auto jovialStyle{PreprocessingInfo::JovialStyleComment};

  // Global scope first to catch beginning and terminating comments
  if (isSgGlobal(node)) {
    boost::optional<const Token&> token{};
    // Comments before START line, which is beginning of the global scope
    while ((token = tokens_->getNextToken()) && token->getStartLine() < pos.getStartLine()) {
      info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::before, jovialStyle);
      setCommentPositionAndConsumeToken(info);
    }
    // Comments same START line
    while ((token = tokens_->getNextToken()) && token->getStartLine() == pos.getStartLine()) {
      if (token->getStartCol() < pos.getStartCol()) {
        // Before START
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::before_syntax, jovialStyle);
      }
      else {
        // After START
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::after_syntax, jovialStyle);
      }
      setCommentPositionAndConsumeToken(info);
    }

    // Comments same end line
    while ((token = tokens_->getNextToken()) && token->getEndLine() == pos.getEndLine()) {
      if (token->getEndCol() > pos.getEndCol()) {
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::end_of, jovialStyle);
        setCommentPositionAndConsumeToken(info);
      }
    }
    // Comments after
    while ((token = tokens_->getNextToken()) && token->getEndLine() > pos.getEndLine()) {
      info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::after, jovialStyle);
      setCommentPositionAndConsumeToken(info);
    }
    return;
  }

  // Attach comments at end of a statement or expression
  if (at_end && (isSgStatement(node) || isSgExpression(node))) {
    boost::optional<const Token&> token{};

    // If a scope, some comments should be attached to last statement in scope
    SgStatement* last{nullptr};
    if (auto scope = isSgScopeStatement(node)) {
      last = scope->lastStatement();
    }

    while ((token = tokens_->getNextToken()) && token->getStartLine() <= pos.getEndLine()) {
      if (last && token->getEndLine() < pos.getEndLine()) {
        info = SI::attachComment(last, token->getLexeme(), PreprocessingInfo::after, jovialStyle);
      }
      else {
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::end_of, jovialStyle);
      }
      setCommentPositionAndConsumeToken(info);
    }
    return;
  }

  if (isSgScopeStatement(node)) {
    boost::optional<const Token&> token{};
    // Comments before scoping unit
    while ((token = tokens_->getNextToken()) && token->getStartLine() < pos.getStartLine()) {
      info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::before, jovialStyle);
      setCommentPositionAndConsumeToken(info);
    }
    return;
  }

  if (SgStatement* stmt = isSgStatement(node)) {
    boost::optional<const Token&> token{};
    while ((token = tokens_->getNextToken()) && token->getStartLine() <= pos.getStartLine()) {
      SgLocatedNode* commentNode{stmt};
      if (token->getTokenType() == JovialEnum::comment) {
        auto commentPosition = PreprocessingInfo::before;
        if (token->getStartLine() == pos.getStartLine()) {
          commentPosition = PreprocessingInfo::end_of;
          // check for comment following a variable initializer
          if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt)) {
            for (SgInitializedName* name : varDecl->get_variables()) {
              if (SgInitializer* init = name->get_initializer()) {
                PosInfo initPos{init};
                if (initPos.getEndCol() > token->getStartCol()) {
                  // attach comment after this variable initializer
                  commentNode = init;
                  break;
                }
              }
            }
          }
        }
        info = SI::attachComment(commentNode, token->getLexeme(), commentPosition, jovialStyle);
      }
      setCommentPositionAndConsumeToken(info);
    }
  }
  else if (auto expr = isSgEnumVal(node)) {
    boost::optional<const Token&> token{};
    // try only attaching comments from same line (what about multi-line comments)
    while ((token = tokens_->getNextToken()) && token->getStartLine() <= pos.getStartLine()) {
      if (token->getTokenType() == JovialEnum::comment) {
        if (token->getStartLine() < pos.getStartLine() || token->getEndCol() < pos.getStartCol()) {
          info = SI::attachComment(expr, token->getLexeme(), PreprocessingInfo::before, jovialStyle);
        }
        else {
          info = SI::attachComment(expr, token->getLexeme(), PreprocessingInfo::after, jovialStyle);
        }
      }
      setCommentPositionAndConsumeToken(info);
    }
  }

  else if (isSgJovialTablePresetExp(node)) {
    auto exprList = isSgJovialTablePresetExp(node)->get_preset_list()->get_expressions();
    attachComments(exprList);
  }

// TODO: Not ready yet until testing comments for expressions
#if 0
  else {
    // Additional expressions?
    mlog[WARN] << "SageTreeBuilder::attachComment: not adding node " << node->class_name() << "\n";
  }
#endif
}

/** Attach comments from a vector */
void
SageTreeBuilder::attachComments(SgLocatedNode* node, const std::vector<Token> &tokens, bool at_end) {
  auto commentPosition{PreprocessingInfo::before};
  if (at_end) commentPosition = PreprocessingInfo::after;

  for (auto token : tokens) {
    SI::attachComment(node, token.getLexeme(), commentPosition, PreprocessingInfo::JovialStyleComment);
  }
}

/** Conditionally attach comments from a vector */
void
SageTreeBuilder::attachComments(SgLocatedNode* node, std::vector<Token> &tokens, const PosInfo &pos) {
  int count{0};
  for (auto token : tokens) {
    if (token.getStartLine() < pos.getStartLine()) {
      SI::attachComment(node, token.getLexeme(), PreprocessingInfo::before, PreprocessingInfo::JovialStyleComment);
      count += 1;
    }
    else if (token.getStartLine() == pos.getStartLine()) {
      if (token.getStartCol() < pos.getStartCol()) {
        SI::attachComment(node, token.getLexeme(), PreprocessingInfo::before_syntax, PreprocessingInfo::JovialStyleComment);
      }
      else {
        SI::attachComment(node, token.getLexeme(), PreprocessingInfo::end_of, PreprocessingInfo::JovialStyleComment);
      }
        count += 1;
    }
  }
  if (count>0) tokens.erase(tokens.begin(),tokens.begin()+count);
}

/** Attach any remaining comments to @node */
void
SageTreeBuilder::attachRemainingComments(SgLocatedNode* node, const PosInfo &pos) {
  boost::optional<const Token&> token{};
  PreprocessingInfo* info{nullptr};

  while ((token = tokens_->getNextToken())) {
    if (token->getStartLine() < pos.getEndLine()) {
      info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::before, PreprocessingInfo::JovialStyleComment);
    }
    else if (token->getStartLine() == pos.getEndLine()) {
      if (token->getStartCol() < pos.getEndCol()) {
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::before_syntax, PreprocessingInfo::JovialStyleComment);
      }
      else {
        info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::after_syntax, PreprocessingInfo::JovialStyleComment);
      }
    }
    else {
      info = SI::attachComment(node, token->getLexeme(), PreprocessingInfo::after, PreprocessingInfo::JovialStyleComment);
    }
    setCommentPositionAndConsumeToken(info);
  }
}

/** Move comments preceding @pos to a vector */
void
SageTreeBuilder::consumePrecedingComments(std::vector<Token> &tokens, const PosInfo &pos) {
  boost::optional<const Token&> token{};
  while ((token = tokens_->getNextToken()) && token->getStartLine() <= pos.getStartLine()) {
    tokens.push_back(*token);
    tokens_->consumeNextToken();
  }
}

/** Pop the scope stack and conditionally @attach_comments associated with end of scope */
SgScopeStatement*
SageTreeBuilder::popScopeStack(bool attach_comments) {
  auto scope = SageBuilder::topScopeStack();
  if (attach_comments) {
    attachComments(scope, PosInfo{scope}, /*at_end*/true);
  }
  SageBuilder::popScopeStack();
  return scope;
}

/** Call SageInterface::setBaseTypeDefiningDeclaration but first move comments from @baseDecl to @varDecl */
void
SageTreeBuilder::setBaseTypeDefiningDeclaration(SgVariableDeclaration* varDecl, SgDeclarationStatement* baseDecl)
{
  // There is a bug (see gitlab-issue-349.jov) that arises when baseDecl has a comment and it is the
  // first statement, because baseDecl will be removed (causing comments to be moved, where? good question!).
  // Move comments to varDecl first to circumvent (by artfulness or deception) potential problems.
  //
  if (auto comments = baseDecl->getAttachedPreprocessingInfo()) {
    unsigned index{0};
    std::vector<int> indexList{};
    for (auto comment : *comments) {
      // So far, testing has only found a problem with comments located before, proceed conservatively
      if (comment->getRelativePosition() == PreprocessingInfo::before) {
        indexList.push_back(index);
      }
      index++;
    }
    if (indexList.size() > 0) {
      bool surroundingStatementPreceedsTargetStatement = false; // this keeps before comments remaining before
      SI::moveCommentsToNewStatement(baseDecl, indexList, varDecl, surroundingStatementPreceedsTargetStatement);
    }
  }

  SI::setBaseTypeDefiningDeclaration(varDecl, baseDecl);
}

void
SageTreeBuilder::setSourcePosition(SgLocatedNode* node, const SourcePosition &start, const SourcePosition &end)
{
   ASSERT_not_null(node);

// SageBuilder may have been used and it builds FileInfo
   if (node->get_startOfConstruct() != nullptr) {
      delete node->get_startOfConstruct();
      node->set_startOfConstruct(nullptr);
   }
   if (node->get_endOfConstruct() != nullptr) {
      delete node->get_endOfConstruct();
      node->set_endOfConstruct(nullptr);
   }

   node->set_startOfConstruct(new Sg_File_Info(start.path, start.line, start.column));
   node->get_startOfConstruct()->set_parent(node);

   node->set_endOfConstruct(new Sg_File_Info(end.path, end.line, end.column-1)); // ROSE end is inclusive
   node->get_endOfConstruct()->set_parent(node);

   SageInterface::setSourcePosition(node);

   // and attach comments if they exist
   PosInfo pinfo{start.line,start.column,end.line,end.column};
   attachComments(node, pinfo);
}

/// Constructor
///
SageTreeBuilder::SageTreeBuilder(SgSourceFile* source, LanguageEnum language, std::istringstream &tokens)
  : language_{language}, source_{source}
{
  tokens_ = new TokenStream(tokens);
}


void SageTreeBuilder::Enter(SgScopeStatement* &scope)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgScopeStatement* &) \n";

   scope = isSgGlobal(SageBuilder::topScopeStack());
   ASSERT_not_null(scope);
}

void SageTreeBuilder::Leave(SgScopeStatement* scope)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgScopeStatement*) \n";

   scope = isSgGlobal(SageBuilder::topScopeStack());
   ASSERT_not_null(scope);

// Clear dangling forward references
   if (!forward_var_refs_.empty()) {
     reset_forward_var_refs(scope);
   }

  // Some forward references can't be resolved until the global scope is reached
   if (!forward_var_refs_.empty() && isSgGlobal(scope)) {
     mlog[WARN] << "map for forward variable references is not empty, size is " << forward_var_refs_.size() << "\n";
     forward_var_refs_.clear();
   }
   if (!forward_type_refs_.empty() && isSgGlobal(scope)) {
     mlog[WARN] << "map for forward type references is not empty, size is " << forward_type_refs_.size() << "\n";
     forward_type_refs_.clear();
   }

   // Attaching any remaining comments
   attachRemainingComments(scope, PosInfo{scope});
}

void SageTreeBuilder::Enter(SgBasicBlock* &block, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgBasicBlock* &)\n";

   // Set the parent (at least temporarily) so that symbols can be traced.
   block = SageBuilder::buildBasicBlock_nfi(SageBuilder::topScopeStack());

  // Attach preceeding label(s) to the block (compound) statement
   SgStatement* stmt = wrapStmtWithLabels(block, labels);

// Append now (before Leave is called) so that symbol lookup will work
   SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(block);
}

void SageTreeBuilder::
Leave(SgBasicBlock*, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgBasicBlock*) \n";

  // Attach trailing label(s) to a terminating null statement
  if (labels.size() > 0) {
    SgNullStatement* nullStmt{nullptr};
    Enter(nullStmt);
    Leave(nullStmt, labels);
  }

  SageBuilder::popScopeStack();  // this basic block
}

void SageTreeBuilder::
Enter(SgProgramHeaderStatement* &program_decl,
      const boost::optional<std::string> &name, const std::vector<std::string> &labels,
      const SourcePositions &sources, std::vector<Rose::builder::Token> &comments)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgProgramHeaderStatement* &, ...) "
               << sources.get<0>() << ":" << sources.get<1>() << ":" << sources.get<2>() << "\n";

   SgScopeStatement* scope = SageBuilder::topScopeStack();

   ASSERT_not_null(scope);
   ASSERT_require(scope->variantT() == V_SgGlobal);

   SgName program_name(name.get_value_or(ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME));

   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList_nfi();
   SgFunctionType* function_type = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), param_list);

   program_decl = new SgProgramHeaderStatement(program_name, function_type, /*function_def*/nullptr);
   ASSERT_not_null(program_decl);

// A Fortran program has no non-defining declaration (assume same for other languages)
   program_decl->set_definingDeclaration(program_decl);

   program_decl->set_scope(scope);
   program_decl->set_parent(scope);
   param_list  ->set_parent(program_decl);

   SgBasicBlock* program_body = new SgBasicBlock();
   SgFunctionDefinition* program_def = new SgFunctionDefinition(program_decl, program_body);

   if (SageInterface::is_language_case_insensitive()) {
     program_body->setCaseInsensitive(true);
     program_def ->setCaseInsensitive(true);
   }

   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(program_def);
   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(program_body);

   program_body->set_parent(program_def);
   program_def ->set_parent(program_decl);

// set source position and attach comments (order important as comments may be added as side effect)
   const SourcePosition &ps = sources.get<0>();
   const SourcePosition &bs = sources.get<1>();
   const SourcePosition &pe = sources.get<2>();
   attachComments(program_decl, comments, PosInfo{ps.line,ps.column,pe.line,pe.column});
   attachComments(program_body, comments, PosInfo{bs.line,bs.column,pe.line,pe.column});

   setSourcePosition(program_decl, sources.get<0>(), sources.get<2>());
   setSourcePosition(program_def,  sources.get<1>(), sources.get<2>());
   setSourcePosition(program_body, sources.get<1>(), sources.get<2>());
   SageInterface::setSourcePosition(program_decl->get_parameterList());

// set labels
   if (SageInterface::is_Fortran_language() && labels.size() == 1) {
     SageInterface::setFortranNumericLabel(program_decl, atoi(labels.front().c_str()),
                                           SgLabelSymbol::e_start_label_type, /*label_scope=*/ program_def);
   }

// If there is no program name then there is no ProgramStmt (this probably needs to be marked somehow?)
   if (!name) {
     mlog[WARN] << "no ProgramStmt in the Fortran MainProgram\n";
   }

   ASSERT_require(program_body == SageBuilder::topScopeStack());
   ASSERT_require(program_decl->get_firstNondefiningDeclaration() == nullptr);
}

void SageTreeBuilder::Leave(SgProgramHeaderStatement* program_decl)
{
// On exit, this function will have checked that the program declaration is
// properly connected, cleaned up the scope stack, resolved symbols, and
// inserted the declaration into its scope.

   mlog[TRACE] << "SageTreeBuilder::Leave(SgProgramHeaderStatement*) \n";

   popScopeStack(/*attach_comments*/true);  // program body
   popScopeStack(/*attach_comments*/true);  // program definition

   auto scope = SageBuilder::topScopeStack();

 // The program declaration must go into the global scope
   SgGlobal* global_scope = isSgGlobal(scope);
   ASSERT_not_null(global_scope);

// A symbol using this name should not already exist
   SgName program_name = program_decl->get_name();
   ASSERT_require(!global_scope->symbol_exists(program_name));

// Add a symbol to the symbol table in the global scope
   SgFunctionSymbol* symbol = new SgFunctionSymbol(program_decl);
   global_scope->insert_symbol(program_name, symbol);

// Attach any remaining comments
   scope = program_decl->get_definition()->get_body();
   attachComments(scope, /*at_end*/true);

   SageInterface::appendStatement(program_decl, global_scope);
}

// Fortran has an end statement which may have an optional name and label
void SageTreeBuilder::
setFortranEndProgramStmt(SgProgramHeaderStatement* program_decl,
                         const boost::optional<std::string> &name,
                         const boost::optional<std::string> &label)
{
  ASSERT_not_null(program_decl);

  SgFunctionDefinition* program_def = program_decl->get_definition();
  ASSERT_not_null(program_def);

  if (label) {
    SageInterface::setFortranNumericLabel(program_decl, atoi(label->c_str()),
                                          SgLabelSymbol::e_end_label_type, /*label_scope=*/ program_def);
  }

  if (name) {
    program_decl->set_named_in_end_statement(true);
  }
}

// Used by Fortran BlockData
void SageTreeBuilder::
Enter(SgProcedureHeaderStatement* &declaration, const boost::optional<std::string> &name) {
  mlog[TRACE] << "SageTreeBuilder::Enter(SgProcedureHeaderStatement*&) \n";

  // The block data statement is implemented to build a function (which initializes data)
  // Note that it can be declared with the "EXTERNAL" statement and as such it works much
  // the same as any other procedure.

  SgFunctionDefinition* definition{nullptr};
  SgBasicBlock* body{nullptr};
  auto kind = SgProcedureHeaderStatement::e_block_data_subprogram_kind;

  SgScopeStatement* scope = SageBuilder::topScopeStack();
  ASSERT_not_null(scope);
  ASSERT_require(scope->variantT() == V_SgGlobal);

  SgName sgName{"BlockDataNameNotPresent__"};
  if (name) {
    sgName = *name;
  }

  SgFunctionParameterList* params = SageBuilder::buildFunctionParameterList_nfi();
  SgType* type = SageBuilder::buildVoidType();

  declaration = SageBuilder::buildProcedureHeaderStatement(sgName, type, params, kind, scope);

  definition = declaration->get_definition();
  body = definition->get_body();

  SageBuilder::pushScopeStack(definition);
  SageBuilder::pushScopeStack(body);

  body->set_parent(definition);
  definition->set_parent(declaration);

  // source position (need more accurate information from caller)
  SageInterface::setSourcePosition(declaration);
  SageInterface::setSourcePosition(declaration->get_parameterList());
  SageInterface::setSourcePosition(body);
  SageInterface::setSourcePosition(definition);

  // set labels
#if 0
  if (SageInterface::is_Fortran_language() && labels.size() == 1) {
    SageInterface::setFortranNumericLabel(program_decl, atoi(labels.front().c_str()),
                                          SgLabelSymbol::e_start_label_type, /*label_scope=*/ definition);
  }
#endif
}

void SageTreeBuilder::
Leave(SgProcedureHeaderStatement* declaration, bool hasEndName) {
  mlog[TRACE] << "SageTreeBuilder::Leave(SgProcedureHeaderStatement*) \n";

  ASSERT_not_null(declaration);
  declaration->set_named_in_end_statement(hasEndName);

  popScopeStack(/*attach_comments*/true);  // procedure body
  popScopeStack(/*attach_comments*/true);  // procedure definition

  // Attach any remaining comments
  auto scope = declaration->get_definition()->get_body();
  attachComments(scope, /*at_end*/true);

  SageInterface::appendStatement(declaration, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgFunctionParameterList* &param_list, SgScopeStatement* &param_scope,
      const std::string &function_name, SgType* function_type, bool is_defining_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionParameterList*) \n";

   param_list = SageBuilder::buildFunctionParameterList_nfi();
   param_scope = nullptr;

// If this is a defining declaration (has a function body) then an SgBasicBlock must be created
// to temporarily store declarations needed to build the types of the initialized names in the
// parameter list. These declarations are transferred to the function definition scope during
// later processing: Leave(SgFunctionDeclaration*).
//
   if (is_defining_decl) {
     param_scope = new SgBasicBlock();
   }
   else {
     param_scope = new SgFunctionParameterScope();
   }

   ASSERT_not_null(param_scope);
   SageInterface::setSourcePosition(param_scope);

// The parameter scope must be attached so that symbol lookups can happen
   ASSERT_require(param_scope->get_parent() == nullptr);
   param_scope->set_parent(SageBuilder::topScopeStack());

   if (SageInterface::is_language_case_insensitive()) {
     param_scope->setCaseInsensitive(true);
   }

// Build the initialized name and symbol for the function result. It is needed because in
// Fortran and Jovial the function name is used as a variable to set the return result value.
// The initialized name will need to be transferred to the function definition scope later.
//
   if (function_type) {
     SgInitializedName* result_name = SageBuilder::buildInitializedName_nfi(function_name, function_type, /*initializer*/nullptr);
     SageInterface::setSourcePosition(result_name);
     result_name->set_scope(param_scope);
     SgVariableSymbol* result_symbol = new SgVariableSymbol(result_name);
     param_scope->insert_symbol(result_name->get_name(), result_symbol);
   }

   SageBuilder::pushScopeStack(param_scope);
}

void SageTreeBuilder::
Leave(SgFunctionParameterList* param_list, SgScopeStatement* param_scope, const std::list<LT::FormalParameter> &param_name_list)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionParameterList*) \n";

   ASSERT_not_null(param_list);
   ASSERT_not_null(param_scope);

// Sanity check
   ASSERT_require(param_scope == SageBuilder::topScopeStack());

// Populate the function parameter list from declarations in the parameter block
   for (const LT::FormalParameter &param : param_name_list)
     {
       SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(param.name, param_scope);

       if (symbol == nullptr) {
         mlog[ERROR] << "SageTreeBuilder::Leave(SgFunctionParameterList*) - symbol lookup failed for name " << param.name;
         ASSERT_not_null(symbol);
       }

    // Create a new initialized name for the parameter list
       SgInitializedName* init_name = symbol->get_declaration();
       SgType* type = init_name->get_type();
       SgInitializedName* new_init_name = SageBuilder::buildInitializedName_nfi(param.name, type, /*initializer*/nullptr);
       SageInterface::setSourcePosition(new_init_name);

       param_list->append_arg(new_init_name);

       if (param.output) {
         init_name    ->get_storageModifier().setMutable();
         new_init_name->get_storageModifier().setMutable();
       }
     }

   SageBuilder::popScopeStack(); // remove parameter scope from the stack
}

void SageTreeBuilder::
Leave(SgFunctionParameterList* param_list, SgScopeStatement* param_scope, const std::list<std::string> &dummy_arg_name_list)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionParameterList* for Fortran) \n";

   ASSERT_not_null(param_scope);

   for (std::string name : dummy_arg_name_list) {
      // TODO: deal with fortran functions when the dummy argument is not declared and implicitly typed.
      SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(name, param_scope);
      ASSERT_not_null(symbol);
      SgInitializedName* init_name = symbol->get_declaration();
      ASSERT_not_null(init_name);
      param_list->append_arg(init_name);
   }

   SageBuilder::popScopeStack(); // remove parameter scope from the stack
}

void SageTreeBuilder::
Enter(SgFunctionDefinition* &function_def)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionDefinition*) \n";

   SgBasicBlock* block = SageBuilder::buildBasicBlock_nfi();

   function_def = new SgFunctionDefinition(block);
   ASSERT_not_null(function_def);
   SageInterface::setSourcePosition(function_def);

   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());
   SageBuilder::pushScopeStack(function_def);
}

void SageTreeBuilder::
Leave(SgFunctionDefinition*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionDefinition*) \n";
   // don't pop the scope stack here as the function declaration will need it on enter
}

void SageTreeBuilder::
Enter(SgFunctionDeclaration* &declaration, const std::string &name, SgType* returnType, SgFunctionParameterList* params,
                                           const LanguageTranslation::FunctionModifierList &modifiers, bool isDefDecl,
                                           const SourcePositions &sources, std::vector<Rose::builder::Token> &comments)
{
  mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionDeclaration* &, ...) "
              << sources.get<0>() << ":" << sources.get<1>() << ":" << sources.get<2>() << "\n";

  SgFunctionDefinition* definition{nullptr};
  SgBasicBlock* body{nullptr};
  SgProcedureHeaderStatement::subprogram_kind_enum kind;

  declaration = nullptr;

  SgScopeStatement* scope = SageBuilder::topScopeStack();
  ASSERT_not_null(scope);

  if (returnType == nullptr) {
    returnType = SageBuilder::buildVoidType();
    kind = SgProcedureHeaderStatement::e_subroutine_subprogram_kind;
  }
  else {
    kind = SgProcedureHeaderStatement::e_function_subprogram_kind;
  }

  if (isDefDecl) {
    // Warning: this calls the unparser to get mangled function name (potentially slow)!
    declaration = SB::buildProcedureHeaderStatement(SgName(name), returnType, params, kind, scope);

    definition = declaration->get_definition();
    body = definition->get_body();
    ASSERT_not_null(definition);
    ASSERT_not_null(body);

    SageBuilder::pushScopeStack(definition);
    SageBuilder::pushScopeStack(body);
  }
  else {
    declaration = SB::buildNondefiningProcedureHeaderStatement(SgName(name), returnType, params, kind, scope);
  }
  ASSERT_not_null(declaration);

  // set source position and attach comments (order important, from list first, decl before body)
  const SourcePosition &fs = sources.get<0>();
  const SourcePosition &bs = sources.get<1>();
  const SourcePosition &fe = sources.get<2>();
  attachComments(declaration, comments, PosInfo{fs.line,fs.column,fe.line,fe.column});
  attachComments(body, comments, PosInfo{bs.line,bs.column,fe.line,fe.column});

  if (declaration) setSourcePosition(declaration, sources.get<0>(), sources.get<2>());
  if (definition) setSourcePosition(definition, sources.get<1>(), sources.get<2>());
  if (body) setSourcePosition(body, sources.get<1>(), sources.get<2>());

  SageInterface::setSourcePosition(declaration->get_parameterList());

  if (list_contains(modifiers, LT::e_function_modifier_definition))  declaration->get_declarationModifier().setJovialDef();
  if (list_contains(modifiers, LT::e_function_modifier_reference ))  declaration->get_declarationModifier().setJovialRef();

  if (list_contains(modifiers, LT::e_function_modifier_recursive))   declaration->get_functionModifier().setRecursive();
  if (list_contains(modifiers, LT::e_function_modifier_reentrant))   declaration->get_functionModifier().setReentrant();

  if (list_contains(modifiers, LT::e_function_modifier_pure     ))   declaration->get_functionModifier().setPure();
  if (list_contains(modifiers, LT::e_function_modifier_elemental))   declaration->get_functionModifier().setElemental();

  if (list_contains(modifiers, LT::e_function_modifier_cuda_device)) declaration->get_functionModifier().setCudaDevice();
  if (list_contains(modifiers, LT::e_function_modifier_cuda_host))   declaration->get_functionModifier().setCudaHost();
  if (list_contains(modifiers, LT::e_function_modifier_cuda_global)) declaration->get_functionModifier().setCudaGlobalFunction();
  if (list_contains(modifiers, LT::e_function_modifier_cuda_grid_global)) declaration->get_functionModifier().setCudaGridGlobal();
}

void SageTreeBuilder::
Leave(SgFunctionDeclaration* declaration, SgScopeStatement* param_scope)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionDeclaration*) \n";

   ASSERT_not_null(declaration);

   SgName function_name = declaration->get_name();
   SgVariableSymbol* result_symbol = param_scope->lookup_variable_symbol(declaration->get_name());
   bool is_defining_decl = (isSgFunctionParameterScope(param_scope) == nullptr);

// If this is a defining declaration then the function body has to be moved from the
// temporary parameter scope (param_scope is a SgBasicBlock*)
   if (is_defining_decl)
     {
       SgBasicBlock* function_body = isSgBasicBlock(SageBuilder::topScopeStack());
       ASSERT_not_null(function_body);

    // Move all of the statements temporarily stored in param_scope into the scope of the function body
       if (isSgBasicBlock(param_scope)) {
         SageInterface::moveStatementsBetweenBlocks (isSgBasicBlock(param_scope), function_body);
       }

    // Connect the result SgInitializedName initially created in param_scope into the scope of the function body
       if (result_symbol) {
         auto proc = isSgProcedureHeaderStatement(declaration);
         auto result = isSgInitializedName(result_symbol->get_declaration());
         ASSERT_not_null(proc);
         ASSERT_not_null(result);

         proc->set_result_name(result);
         result->set_parent(declaration);
         result->set_scope(function_body);
         ASSERT_not_null(function_body->lookup_symbol(function_name));
       }

    // The param_scope (SgBasicBlock) is still connected, so try to set its parent
    // to nullptr and delete it.
       param_scope->set_parent(nullptr);
       delete param_scope;

       SageBuilder::popScopeStack();  // function body
       SageBuilder::popScopeStack();  // function definition
     } // is_def_decl
   else
     {
       ASSERT_not_null(isSgFunctionParameterScope(param_scope));
       ASSERT_require(declaration->get_functionParameterScope() == nullptr);

       declaration->set_functionParameterScope(isSgFunctionParameterScope(param_scope));
       param_scope->set_parent(declaration);

       // The scope of the parameter names may end up connected incorrectly (gitlab-issue-287.jov)
       if (auto proc = isSgProcedureHeaderStatement(declaration)) {
         for (auto arg : declaration->get_parameterList()->get_args()) {
           SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(arg->get_name(), param_scope);
           if (symbol) {
             mlog[WARN] << "potential duplicate initialized name, arg: " << arg << " scope: " << arg->get_scope() << "\n";
           }
         }
       }

       if (result_symbol) {
         SgProcedureHeaderStatement* proc_decl = isSgProcedureHeaderStatement(declaration);
         SgInitializedName* result_name = isSgInitializedName(result_symbol->get_declaration());
         ASSERT_not_null(proc_decl);
         ASSERT_not_null(result_name);

         proc_decl->set_result_name(result_name);
         result_name->set_parent(declaration);
       }
     }

   if (is_defining_decl) {
      // Attach any remaining comments
      auto scope = declaration->get_definition()->get_body();
      attachComments(scope, /*at_end*/true);
   }

   // Finished using the map for labels
   labels_.clear();

   // Reset any dangling forward references to this function
   if (!forward_var_refs_.empty()) {
      reset_forward_var_refs(SageBuilder::topScopeStack());
   }

   SageInterface::appendStatement(declaration, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgFunctionDeclaration* declaration, SgScopeStatement* param_scope, bool have_end_stmt, const std::string &result_name /* = "" */)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFunctionDeclaration*) \n";

   // Call more generic leave for SgFunctionDeclaration, will move declarations out of param_scope into
   // the body of the function declaration and will set the result name as name of the function
   Leave(declaration, param_scope);

   // If result is named, get symbol and init name of the result to set it for the function declaration
   if (!result_name.empty()) {
      // Get symbol and associated initialized name
      SgFunctionDefinition* func_def = declaration->get_definition();
      ASSERT_not_null(func_def);
      SgBasicBlock* body = func_def->get_body();
      ASSERT_not_null(body);
      SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(result_name, body);
      ASSERT_not_null(symbol);
      SgInitializedName* init_name = symbol->get_declaration();
      ASSERT_not_null(init_name);

      SgProcedureHeaderStatement* proc_header_stmt = isSgProcedureHeaderStatement(declaration);
      ASSERT_not_null(proc_header_stmt);

      // If result is named but not declared, need to fix up initialized name created earlier for it
      if (!init_name->get_parent()) {
         init_name->set_parent(proc_header_stmt);
         init_name->set_scope(body);
         proc_header_stmt->get_scope()->insert_symbol(result_name, symbol);
      }

      // Reset the result name to the correct initialized name
      proc_header_stmt->set_result_name(init_name);
   }

   // Set named end statement if needed
   if (have_end_stmt) {
      declaration->set_named_in_end_statement(have_end_stmt);
   }
}

void SageTreeBuilder::
Enter(SgDerivedTypeStatement* & derived_type_stmt, const std::string & name)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDerivedTypeStatement* &, ...) \n";

   derived_type_stmt = SageBuilder::buildDerivedTypeStatement(name, SageBuilder::topScopeStack());

   SgClassDefinition* class_defn = derived_type_stmt->get_definition();
   ASSERT_not_null(class_defn);
   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());

// Append now (before Leave is called) so that symbol lookup will work
   SageInterface::appendStatement(derived_type_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(class_defn);
}

void SageTreeBuilder::
Leave(SgDerivedTypeStatement*)
{
  mlog[TRACE] << "SageTreeBuilder::Leave(SgDerivedTypeStatement*) \n";
  SageBuilder::popScopeStack();  // class definition
}

void SageTreeBuilder::
Leave(SgDerivedTypeStatement* stmt, std::list<LanguageTranslation::ExpressionKind> &modifiers)
{
  mlog[TRACE] << "SageTreeBuilder::Leave(SgDerivedTypeStatement*) \n";
  using namespace LanguageTranslation;

  for (ExpressionKind modifier : modifiers) {
    switch(modifier) {
      case ExpressionKind::e_access_modifier_private:
        stmt->get_declarationModifier().get_accessModifier().setPrivate();
        break;
      case ExpressionKind::e_access_modifier_public:
        stmt->get_declarationModifier().get_accessModifier().setPublic();
        break;
      case ExpressionKind::e_type_modifier_bind_c:
        stmt->get_declarationModifier().get_typeModifier().setBind();
        break;
      default: break;
    }
  }
  Leave(stmt);
}

// Statements
//

void SageTreeBuilder::
Enter(SgNamespaceDeclarationStatement* &namespace_decl, const std::string &name, const SourcePositionPair &)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgNamespaceDeclarationStatement* &, ...) \n";

// Build a namespace to contain the module
   namespace_decl = SageBuilder::buildNamespaceDeclaration_nfi(name, true, SageBuilder::topScopeStack());
   SageInterface::setSourcePosition(namespace_decl);

   SgNamespaceDefinitionStatement* namespace_defn = namespace_decl->get_definition();
   ASSERT_not_null(namespace_defn);
   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());

   // TEMPORARY: fix in SageBuilder
   namespace_defn->setCaseInsensitive(true);
   ASSERT_require(namespace_defn->isCaseInsensitive());

   // Append before push (so that symbol lookup will work)
   SageInterface::appendStatement(namespace_decl, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(namespace_defn);
}

void SageTreeBuilder::
Leave(SgNamespaceDeclarationStatement*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgNamespaceDeclarationStatement*, ...) \n";

   SageBuilder::popScopeStack();  // namespace definition
}

void SageTreeBuilder::
Enter(SgAttributeSpecificationStatement* &stmt, SgAttributeSpecificationStatement::attribute_spec_enum kind)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgAttributeSpecificationStatement* &,...) \n";
   stmt = SageBuilder::buildAttributeSpecificationStatement(kind);
}

void SageTreeBuilder::
Leave(SgAttributeSpecificationStatement* stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgAttributeSpecificationStatement*) \n";
   SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgDataStatementGroup* &stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDataStatementGroup* &) \n";
   stmt = new SgDataStatementGroup;
}

void SageTreeBuilder::
Enter(SgDataStatementObject* &dataObject)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDataStatementObject* &) \n";
   dataObject = new SgDataStatementObject;

   auto refList{SageBuilder::buildExprListExp_nfi()};
   dataObject->set_variableReference_list(refList);
   refList->set_parent(dataObject);
   SageInterface::setSourcePosition(refList);
}

void SageTreeBuilder::
Enter(SgDataStatementValue* &dataValue, SgDataStatementValue::data_statement_value_enum kind)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDataStatementValue* &) \n";
   dataValue = new SgDataStatementValue(kind);

   auto initList{SageBuilder::buildExprListExp_nfi()};
   dataValue->set_initializer_list(initList);
   initList->set_parent(dataValue);
   SageInterface::setSourcePosition(initList);

   // TODO: other variants
   ASSERT_require(kind == SgDataStatementValue::e_explicit_list);
}

void SageTreeBuilder::
Enter(SgExprStatement* &stmt, const std::string &name,
      SgExprListExp* params, const std::string & /*abortPhrase*/)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgExprStatement* &, ...) \n";

   // I think entering an expression is a little awkward (what about leave an expression, maybe ok)
   SgFunctionCallExp* expr;
   Enter(expr, name, params);

   // TODO: AbortPhrase for Jovial
   stmt = SageBuilder::buildExprStatement_nfi(expr);
}

void SageTreeBuilder::
Enter(SgExprStatement* &assignStmt, SgExpression* &rhs, const std::vector<SgExpression*> &vars)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgExprStatement* &, ...) \n";

   SgExpression* lhs{nullptr};
   SgAssignOp* assignOp{nullptr};
   SgEnumVal* oldVal{isSgEnumVal(rhs)};

   // For Jovial, the symbol table may have multiple enumerators with the same name. Check and
   // replace a Jovial status constant with the correct value based on the type of the variable.
   if (oldVal) {
      SgEnumType* enumType{isSgEnumType(vars[0]->get_type())};
      if (auto tableType = isSgJovialTableType(vars[0]->get_type())) {
         enumType = isSgEnumType(tableType->get_base_type());
      }
      ASSERT_not_null(enumType);
      rhs = getEnumVal(enumType, oldVal);
   }

// Jovial may have more than one variable in an assignment statement
   if (vars.size() == 1) {
      lhs = vars[0];
   }
   else if (vars.size() > 1) {
      lhs = SB::buildExprListExp_nfi(vars);
   }
   ASSERT_not_null(lhs);

   assignOp = SB::buildBinaryExpression_nfi<SgAssignOp>(lhs, rhs);
   assignStmt = SB::buildExprStatement_nfi(assignOp);
}

void SageTreeBuilder::
Leave(SgExprStatement* exprStmt, std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgExprStatement*) \n";

   SgStatement* stmt = wrapStmtWithLabels(exprStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgFunctionCallExp* &func_call, const std::string &name, SgExprListExp* params)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFunctionCallExp* &, ...) \n";

   func_call = nullptr;

   // Function calls are ambiguous with arrays in Fortran (and type casts and the replication operator
   // in Jovial).  Start out by assuming it's a function call if another symbol doesn't exist.

   SgFunctionSymbol* func_symbol = SageInterface::lookupFunctionSymbolInParentScopes(name, SageBuilder::topScopeStack());

   if (func_symbol == nullptr) {
      SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name, SageBuilder::topScopeStack());
      if (symbol || isInitializationContext()) {
         // There is a symbol (but not a function symbol) or name that could be part of an
         // initialization expression, punt and let variable handling take care of it.
         return;
      }
      else {
         // Assume a void return type.
         SgType* return_type = SageBuilder::buildVoidType();
         func_call = SB::buildFunctionCallExp(SgName(name), return_type, params, SageBuilder::topScopeStack());
      }
   }
   else {
      func_call = SageBuilder::buildFunctionCallExp(func_symbol, params);
   }

   ASSERT_not_null(func_call);
   SageInterface::setSourcePosition(func_call);
}

void SageTreeBuilder::
Enter(SgReplicationOp* &rep_op, const std::string &name, SgExpression* value)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgReplicationOp* &, ...) \n";

   SgVariableSymbol* symbol = SageInterface::lookupVariableSymbolInParentScopes(name, SageBuilder::topScopeStack());
   ASSERT_not_null(symbol);

   SgVarRefExp* count = SageBuilder::buildVarRefExp(name, SageBuilder::topScopeStack());
   rep_op = SageBuilder::buildReplicationOp_nfi(count, value);
}

void SageTreeBuilder::
Enter(SgCastExp* &cast_expr, const std::string &name, SgExpression* cast_operand)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgCastExp* &, ...) \n";

   SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(name, SageBuilder::topScopeStack());

   // Jovial specific comment:
   //   If the symbol is an Enum it is a StatusConversion
   if (isSgTypedefSymbol(symbol) == nullptr  && isSgEnumSymbol(symbol) == nullptr) {
      mlog[ERROR] << "UNIMPLEMENTED: SageTreeBuilder::Enter(SgCastExp* ...) for name " << name;
      ROSE_ABORT();
   }

   SgType* conv_type = symbol->get_type();
   cast_expr = SageBuilder::buildCastExp_nfi(cast_operand, conv_type, SgCastExp::e_default);
}

void SageTreeBuilder::
Enter(SgPntrArrRefExp* &array_ref, const std::string &name, SgExprListExp* subscripts, SgExprListExp* /*cosubscripts*/)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgPntrArrRefExp* &, ...) \n";

   SgVarRefExp* var_ref = nullptr;
   Enter(var_ref, name, false);
   Leave(var_ref);

   // No cosubscripts for now
   array_ref = SageBuilder::buildPntrArrRefExp_nfi(var_ref, subscripts);
}

void SageTreeBuilder::
Enter(SgVarRefExp* &varRef, const std::string &name, bool compilerGenerate)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgVarRefExp* &, ...) \n";

   SgVariableSymbol* varSym = SageInterface::lookupVariableSymbolInParentScopes(name, SageBuilder::topScopeStack());
   if (!varSym && compilerGenerate) {
      SgVariableDeclaration* varDecl;

      //TODO: check for single letter for name (Jovial control letter) and type is not really known
      SgType* type = SageBuilder::buildIntType();

      // Build variable declaration for the control letter
      Enter(varDecl, name, type, nullptr, std::vector<std::string>{}/*labels*/);
      Leave(varDecl);

      varSym = SageInterface::lookupVariableSymbolInParentScopes(name, SageBuilder::topScopeStack());
   }
   ASSERT_not_null(varSym);

   varRef = SageBuilder::buildVarRefExp_nfi(varSym);
}

void SageTreeBuilder::
Enter(SgIfStmt* &if_stmt, SgExpression* conditional, SgBasicBlock* true_body, SgBasicBlock* false_body,
      std::vector<Rose::builder::Token> &comments, bool is_ifthen, bool has_end_stmt, bool is_else_if)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgIfStmt* &, ...) \n";

   ASSERT_not_null(conditional);
   ASSERT_not_null(true_body);

   SgStatement* conditional_stmt = SageBuilder::buildExprStatement_nfi(conditional);
   if_stmt = SageBuilder::buildIfStmt_nfi(conditional_stmt, true_body, false_body);

   if (is_ifthen) {
      if_stmt->set_use_then_keyword(true);
   }
   if (has_end_stmt) {
      if_stmt->set_has_end_statement(true);
   }
   if (is_else_if) {
      if_stmt->set_is_else_if_statement(true);
   }

   attachComments(if_stmt, comments);
}

void SageTreeBuilder::
Leave(SgIfStmt* ifStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgIfStmt*) \n";

   // Append final label statement, if there are labels, otherwise stmt==ifStmt
   SgStatement* stmt = wrapStmtWithLabels(ifStmt, labels);
   SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgContinueStmt* &continueStmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgContinueStmt*, ...)\n";

   continueStmt = SB::buildContinueStmt_nfi();
}

void SageTreeBuilder::
Leave(SgContinueStmt* continueStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgContinueStmt*, ...)\n";

   // Append final label statement, if there are labels, otherwise stmt==continueStmt
   SgStatement* stmt = wrapStmtWithLabels(continueStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgFortranContinueStmt* &continueStmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgFortranContinueStmt*, ...)\n";

   continueStmt = SB::buildFortranContinueStmt_nfi();
}

void SageTreeBuilder::
Leave(SgFortranContinueStmt* continueStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgFortranContinueStmt*, ...)\n";

   // Append final label statement, if there are labels, otherwise stmt==continueStmt
   SgStatement* stmt = wrapStmtWithLabels(continueStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgGotoStatement* &gotoStmt, const std::string &label)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgGotoStatement*, ...)\n";

   SgLabelStatement* labelStmt{nullptr};
   gotoStmt = nullptr;

   // Ensure a label statement exists for the statement to goto
   if (labels_.find(label) != labels_.end()) {
     labelStmt = labels_[label];
   }
   else {
     // Build a temporary placeholder
     labelStmt = SB::buildLabelStatement_nfi(label, nullptr, SB::topScopeStack());
     labels_[label] = labelStmt;
   }

   ASSERT_not_null(labelStmt);
   gotoStmt = SB::buildGotoStatement_nfi(labelStmt);
}

void SageTreeBuilder::
Leave(SgGotoStatement* gotoStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgGotoStatement*, ...)\n";

   // Append final label statement (if there are labels, otherwise stmt==gotoStmt)
   SgStatement* stmt = wrapStmtWithLabels(gotoStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgLabelStatement* &labelStmt, const std::string &label)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgLabelStatement*, ...)\n";

   labelStmt = nullptr;

   // Perhaps a label statement already exists
   if (labels_.find(label) != labels_.end()) {
     labelStmt = labels_[label];
   }
   else {
     // Build a temporary placeholder
     labelStmt = SB::buildLabelStatement_nfi(label, nullptr, SB::topScopeStack());
     labels_[label] = labelStmt;
   }
   ASSERT_not_null(labelStmt);
}

void SageTreeBuilder::
Leave(SgLabelStatement* labelStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgLabelStatement*, ...)\n";

   // Append final label statement (if there are labels, otherwise stmt==labelStmt)
   SgStatement* stmt = wrapStmtWithLabels(labelStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgNullStatement* &stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgNullStatement*)\n";

   stmt = SageBuilder::buildNullStatement();
   SageInterface::setSourcePosition(stmt);
}

void SageTreeBuilder::
Leave(SgNullStatement* nullStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgNullStatement*, ...)\n";

   // Append final label statement (if there are labels, otherwise stmt==nullStmt)
   SgStatement* stmt = wrapStmtWithLabels(nullStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgProcessControlStatement* &control_stmt, const std::string &stmt_kind,
      const boost::optional<SgExpression*> &opt_code, const boost::optional<SgExpression*> &opt_quiet)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgProcessControlStatement* &, ...) \n";

   SgExpression* code =  (opt_code)  ? *opt_code  : SageBuilder::buildNullExpression_nfi();
   SgExpression* quiet = (opt_quiet) ? *opt_quiet : SageBuilder::buildNullExpression_nfi();

   ASSERT_not_null(code);
   control_stmt = new SgProcessControlStatement(code);
   ASSERT_not_null(control_stmt);
   SageInterface::setSourcePosition(control_stmt);

   ASSERT_not_null(quiet);
   control_stmt->set_quiet(quiet);

   if (stmt_kind == "abort") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_abort);
   }
   else if (stmt_kind == "error_stop") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_error_stop);
   }
   else if (stmt_kind == "exit") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_exit);
   }
   else if (stmt_kind == "fail_image") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_fail_image);
   }
   else if (stmt_kind == "pause") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_pause);
   }
   else if (stmt_kind == "stop") {
      control_stmt->set_control_kind(SgProcessControlStatement::e_stop);
   }
   else {
      mlog[FATAL] << "SageTreeBuilder::Enter(SgProcessControlStatement* &, ...): incorrect statement kind\n";
      ROSE_ABORT();
   }

   code->set_parent(control_stmt);
   quiet->set_parent(control_stmt);
}

void SageTreeBuilder::
Leave(SgProcessControlStatement* controlStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgProcessControlStatement*, ...) \n";
   ASSERT_not_null(controlStmt);

   // Append final label statement (if there are labels, otherwise stmt==controlStmt)
   SgStatement* stmt = wrapStmtWithLabels(controlStmt, labels);
   SageInterface::appendStatement(stmt, SB::topScopeStack());
}

void SageTreeBuilder::
Enter(SgSwitchStatement* &switch_stmt, SgExpression* selector, const SourcePositionPair &)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgSwitchStatement* &, ...) \n";

   ASSERT_not_null(selector);
   SgExprStatement* selector_stmt = SageBuilder::buildExprStatement_nfi(selector);
   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();

   switch_stmt = SageBuilder::buildSwitchStatement_nfi(selector_stmt, body);

// Append before push (so that symbol lookup will work)
   SageInterface::appendStatement(switch_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgSwitchStatement* switch_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgSwitchStatement*, ...) \n";
   ASSERT_not_null(switch_stmt);

   SageBuilder::popScopeStack();  // switch statement body
}

void SageTreeBuilder::
Enter(SgReturnStmt* &return_stmt, const boost::optional<SgExpression*> &opt_expr)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgReturnStmt* &, ...) \n";

   SgExpression* return_expr = (opt_expr) ? *opt_expr : SageBuilder::buildNullExpression_nfi();
   ASSERT_not_null(return_expr);

   return_stmt = SageBuilder::buildReturnStmt_nfi(return_expr);
}

void SageTreeBuilder::
Leave(SgReturnStmt* returnStmt, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgReturnStmt*, ...) \n";
   ASSERT_not_null(returnStmt);

   // Append final label statement (if there are labels, otherwise stmt==returnStmt)
   SgStatement* stmt = wrapStmtWithLabels(returnStmt, labels);
   SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgCaseOptionStmt* &case_option_stmt, SgExprListExp* key)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgCaseOptionStmt* &, ...) \n";
   ASSERT_not_null(key);

   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();
   case_option_stmt = SageBuilder::buildCaseOptionStmt_nfi(key, body);

// Append before push (so that symbol lookup will work)
   SageInterface::appendStatement(case_option_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgCaseOptionStmt* case_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgCaseOptionStmt*, ...) \n";
   ASSERT_not_null(case_option_stmt);

   SageBuilder::popScopeStack();  // case_option_stmt body
}

void SageTreeBuilder::
Enter(SgDefaultOptionStmt* &default_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgDefautlOptionStmt* &, ...) \n";

   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();
   default_option_stmt = SageBuilder::buildDefaultOptionStmt(body);

// Append before push (so that symbol lookup will work)
   SageInterface::appendStatement(default_option_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgDefaultOptionStmt* default_option_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgDefautlOptionStmt*, ...) \n";
   ASSERT_not_null(default_option_stmt);

   SageBuilder::popScopeStack();  // default_option_stmt body
}

void SageTreeBuilder::
Enter(SgFortranDo* &doStmt, SgExpression* initialization, SgExpression* bound, SgExpression* increment)
{
  mlog[TRACE] << "SageTreeBuilder::Enter(SgDoWhileStmt* &, ...) \n";

  auto body = SageBuilder::buildBasicBlock_nfi();
  doStmt = SB::buildFortranDo_nfi(initialization, bound, increment, body);

  // output "END DO"
  doStmt->set_has_end_statement(true);

// Append before push (so that symbol lookup will work)
  SageInterface::appendStatement(doStmt, SageBuilder::topScopeStack());
  SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgFortranDo* doStmt)
{
  mlog[TRACE] << "SageTreeBuilder::Leave(SgFortranDo*, ...) \n";
  ASSERT_not_null(doStmt);

  SageBuilder::popScopeStack();  // do statement body
}

void SageTreeBuilder::
Enter(SgPrintStatement* &print_stmt, SgExpression* format, std::list<SgExpression*> &expr_list)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgPrintStmt* &, ...) \n";

   ASSERT_not_null(format);

   print_stmt = new SgPrintStatement();
   ASSERT_not_null(print_stmt);
   SageInterface::setSourcePosition(print_stmt);

   print_stmt->set_format(format);

   SgExprListExp* io_stmt_list = SageBuilderCpp17::buildExprListExp_nfi(expr_list);
   print_stmt->set_io_stmt_list(io_stmt_list);
}

void SageTreeBuilder::
Leave(SgPrintStatement* print_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgPrintStmt*, ...) \n";
   ASSERT_not_null(print_stmt);

   SageInterface::appendStatement(print_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgWhileStmt* &stmt, SgExpression* condition)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgWhileStmt* &, ...) \n";
   ASSERT_not_null(condition);

   SgExprStatement* conditionStmt = SageBuilder::buildExprStatement_nfi(condition);
   SgBasicBlock* body = SageBuilder::buildBasicBlock_nfi();

   stmt = SageBuilder::buildWhileStmt_nfi(conditionStmt, body, /*else_body*/nullptr);

// Append before push (so that symbol lookup will work)
   SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(body);
}

void SageTreeBuilder::
Leave(SgWhileStmt* stmt, bool hasEndDo)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgWhileStmt*, ...) \n";
   ASSERT_not_null(stmt);

   if (hasEndDo) {
      stmt->set_has_end_statement(true);
   }

   SageBuilder::popScopeStack();  // while statement body
}

void SageTreeBuilder::
Enter(SgImplicitStatement* &implicit_stmt, bool none_external, bool none_type)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgImplicitStatement* &, bool none_external, bool none_type)\n";
   // Implicit None

   implicit_stmt = new SgImplicitStatement(true /* implicit none*/);
   ASSERT_not_null(implicit_stmt);
   SageInterface::setSourcePosition(implicit_stmt);

   if (none_external && none_type) {
      implicit_stmt->set_implicit_spec(SgImplicitStatement::e_none_external_and_type);
   }
   else if (none_external) {
      implicit_stmt->set_implicit_spec(SgImplicitStatement::e_none_external);
   }
   else if (none_type) {
      implicit_stmt->set_implicit_spec(SgImplicitStatement::e_none_type);
   }
}

void SageTreeBuilder::
Enter(SgImplicitStatement* &, std::list<std::tuple<SgType*,
      std::list<std::tuple<char, boost::optional<char>>>>> &implicit_spec_list)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgImplicitStatement* &, implicit_spec_list)\n";
   // Implicit with Implicit-Spec

   //TODO: Create SgImplicitStatement with the list of Implicit Specs,
   // perhaps wait until SageBuilder function is created

   // Step through the list of Implicit Specs
   for (std::tuple<SgType*, std::list<std::tuple<char, boost::optional<char>>>> implicit_spec : implicit_spec_list) {
      SgType* type;
      std::list<std::tuple<char, boost::optional<char>>> letter_spec_list;
      std::tie(type, letter_spec_list) = implicit_spec;

      mlog[DEBUG] << "The type is " << type->class_name() << " and the letters are ";

      // Traverse the list of letter specs
      for (std::tuple<char, boost::optional<char>> letter_spec : letter_spec_list) {
         char first;
         boost::optional<char> second;
         std::tie(first, second) = letter_spec;

         mlog[DEBUG] << first;

         if (second) {
            mlog[DEBUG] << " - " << second;
         }
         mlog[DEBUG] << "\n";
      }
   }
}

void SageTreeBuilder::
Leave(SgImplicitStatement* implicit_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgImplicitStatement*, ...) \n";
   ASSERT_not_null(implicit_stmt);

   SageInterface::appendStatement(implicit_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgModuleStatement* &module_stmt, const std::string &name)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgModuleStatement* &, ...)\n";

   module_stmt = SageBuilder::buildModuleStatement(name, SageBuilder::topScopeStack());

   SgClassDefinition* class_def = module_stmt->get_definition();
   ASSERT_not_null(class_def);

   // Append now (before Leave is called) so that symbol lookup will work
   SageInterface::appendStatement(module_stmt, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(class_def);
}

void SageTreeBuilder::
Leave(SgModuleStatement* module_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgModuleStatement*, ...) \n";
   ASSERT_not_null(module_stmt);

   SageBuilder::popScopeStack();  // class definition
}

void SageTreeBuilder::
Enter(SgUseStatement* &use_stmt, const std::string &module_name, const std::string &module_nature)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgUseStatement* &, ...)\n";

   use_stmt = new SgUseStatement(module_name, false, module_nature);
   ASSERT_not_null(use_stmt);
   SageInterface::setSourcePosition(use_stmt);

   SgClassSymbol* module_symbol = SageInterface::lookupClassSymbolInParentScopes(module_name);
   ASSERT_not_null(module_symbol);

   SgClassDeclaration* decl = module_symbol->get_declaration();
   ASSERT_not_null(decl);

   SgModuleStatement* module_stmt = isSgModuleStatement(decl);
   ASSERT_not_null(module_stmt);

   use_stmt->set_module(module_stmt);
}

void SageTreeBuilder::
Leave(SgUseStatement* use_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgUseStatement*, ...) \n";
   ASSERT_not_null(use_stmt);

   SageInterface::appendStatement(use_stmt, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgContainsStatement* &contains_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgContainsStatement* &, ...)\n";

   contains_stmt = new SgContainsStatement();
   ASSERT_not_null(contains_stmt);
   SageInterface::setSourcePosition(contains_stmt);
}

void SageTreeBuilder::
Leave(SgContainsStatement* contains_stmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgContainsStatement*, ...) \n";
   ASSERT_not_null(contains_stmt);

   SageInterface::appendStatement(contains_stmt, SageBuilder::topScopeStack());
}

// For Jovial, the symbol table may have multiple enumerators with the same name. This
// function returns the correct value based on the type of the variable.
SgEnumVal* SageTreeBuilder::
getEnumVal(SgEnumType* enum_type, SgEnumVal* old_val)
{
   SgEnumDeclaration* enum_decl = isSgEnumDeclaration(enum_type->get_declaration());
   ASSERT_not_null(enum_decl);
   ASSERT_not_null(old_val);

   SgInitializedNamePtrList &enum_list = enum_decl->get_enumerators();
   SgInitializedName* init_name = nullptr;

   SgName name = old_val->get_name();
   for (SgInitializedName* status_constant : enum_list) {
      if (status_constant->get_name() == name) {
         init_name = status_constant;
         break;
      }
   }

   if (!init_name) {
     return old_val;
   }

   SgAssignInitializer* assign_init = isSgAssignInitializer(init_name->get_initptr());
   ASSERT_not_null(assign_init);

   SgEnumVal* new_val = isSgEnumVal(assign_init->get_operand());
   ASSERT_not_null(new_val);

   return new_val;
}

// Jovial specific nodes
//

#ifdef ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION

void SageTreeBuilder::
Enter(SgJovialDefineDeclaration* &define_decl, const std::string &define_string)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDefineDeclaration* &, ...) \n";

   define_decl = new SgJovialDefineDeclaration(define_string);
   ASSERT_not_null(define_decl);
   SageInterface::setSourcePosition(define_decl);

// The first nondefining declaration must be set
   define_decl->set_firstNondefiningDeclaration(define_decl);
}

void SageTreeBuilder::
Leave(SgJovialDefineDeclaration* define_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDirectiveStatement*) \n";

   ASSERT_not_null(define_decl);

   SageInterface::appendStatement(define_decl, SageBuilder::topScopeStack());
   ASSERT_require(define_decl->get_parent() == SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialDirectiveStatement* &directive, const std::string &directive_string)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDirectiveStatement* &, ...) \n";

   directive = new SgJovialDirectiveStatement(directive_string, SgJovialDirectiveStatement::e_unknown);
   ASSERT_not_null(directive);
   SageInterface::setSourcePosition(directive);

// The first nondefining declaration must be set
   directive->set_firstNondefiningDeclaration(directive);
}

void SageTreeBuilder::
Enter(SgJovialDirectiveStatement* &directive, const std::string &compool_name, std::vector<std::string> &import_names)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialDirectiveStatement* &, ...) \n";

   std::string directive_string = "'" + compool_name + "'";
   if (import_names.size() == 0) {
     directive_string = "(" + directive_string + ")";
   } else {
     bool first = true;
     for (std::string &name : import_names) {
       if (first) {
         directive_string += " " + name;
         first = false;
       }
       else {
         directive_string += ", " + name;
       }
     }
   }

   directive = new SgJovialDirectiveStatement(directive_string, SgJovialDirectiveStatement::e_unknown);
   ASSERT_not_null(directive);
   SageInterface::setSourcePosition(directive);

// The first nondefining declaration must be set
   directive->set_firstNondefiningDeclaration(directive);

// Can't use SgJovialDirectiveStatement::e_compool enum as function parameter to SageTreeBuilder
// because API can't see Sage nodes until C++17, so set it correctly as it is known here.
   directive->set_directive_type(SgJovialDirectiveStatement::e_compool);

   ModuleBuilder & compool_builder = ModuleBuilderFactory::get_compool_builder();
   compool_builder.loadModule(compool_name, import_names, isSgGlobal(SageBuilder::topScopeStack()));
}

void SageTreeBuilder::
Leave(SgJovialDirectiveStatement* directive)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialDirectiveStatement*) \n";

   ASSERT_not_null(directive);

   switch (directive->get_directive_type())
     {
       case SgJovialDirectiveStatement::e_compool: {
          SageBuilder::popScopeStack();
          break;
       }
       case SgJovialDirectiveStatement::e_unknown:
          mlog[ERROR] << "SageTreeBuilder::Leave(SgJovialDirectiveStatement*) directive_type is unknown \n";
          break;
       default:
          break;
     }

   SageInterface::appendStatement(directive, SageBuilder::topScopeStack());
   ASSERT_require(directive->get_parent() == SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialForThenStatement* &forStmt)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialForThenStatement* &, ...) \n";

   forStmt = SB::buildJovialForThenStatement_nfi();
   SI::appendStatement(forStmt, SB::topScopeStack());

   SB::pushScopeStack(forStmt);
   SB::pushScopeStack(forStmt->get_loop_body());
}

void SageTreeBuilder::
Leave(SgJovialForThenStatement* forStmt)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialForThenStatement*, ...) \n";

   ASSERT_not_null(forStmt);

   if (forStmt->get_while_expression() == nullptr) {
      forStmt->set_while_expression(SB::buildNullExpression_nfi());
      forStmt->get_while_expression()->set_parent(forStmt);
   }
   if (forStmt->get_by_or_then_expression() == nullptr) {
      forStmt->set_by_or_then_expression(SB::buildNullExpression_nfi());
      forStmt->get_by_or_then_expression()->set_parent(forStmt);
   }

   SB::popScopeStack();  // for loop body
   SB::popScopeStack();  // for statement
}

void SageTreeBuilder::
Enter(SgJovialCompoolStatement* &compool_decl, const std::string &name, const SourcePositionPair &)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialCompoolStatement* &, ...) \n";

   compool_decl = new SgJovialCompoolStatement(name);
   SageInterface::setSourcePosition(compool_decl);

   compool_decl->set_definingDeclaration(compool_decl);
   compool_decl->set_firstNondefiningDeclaration(compool_decl);

   SageInterface::appendStatement(compool_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgJovialCompoolStatement*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialCompoolStatement*, ...) \n";
}

void SageTreeBuilder::
Enter(SgJovialOverlayDeclaration* &overlay_decl, SgExpression* address, SgExprListExp* overlay)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialOverlayDeclaration* &, ...) \n";

   ASSERT_not_null(address);
   ASSERT_not_null(overlay);

   //TOOD: SageBuilder function
   overlay_decl = new SgJovialOverlayDeclaration(address, overlay);
   ASSERT_not_null(overlay_decl);
   SageInterface::setSourcePosition(overlay_decl);

   address->set_parent(overlay_decl);
   overlay->set_parent(overlay_decl);
}

void SageTreeBuilder::
Leave(SgJovialOverlayDeclaration* overlay_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialOverlayDeclaration*) \n";

   SageInterface::appendStatement(overlay_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgJovialTableStatement* &table_decl,
      const std::string &name, const SourcePositionPair &, bool is_block)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgJovialTableStatement* &, ...) \n";

   SgName type_name = name;
   SgClassDeclaration::class_types struct_kind = SgClassDeclaration::e_jovial_table;
   if (is_block) struct_kind = SgClassDeclaration::e_jovial_block;

   // This function builds a class declaration and definition with both the defining and nondefining declarations
   table_decl = SageBuilder::buildJovialTableStatement(type_name, struct_kind, SageBuilder::topScopeStack());
   ASSERT_not_null(table_decl);
   SageInterface::setSourcePosition(table_decl);

   SgClassDefinition* table_def = table_decl->get_definition();
   ASSERT_not_null(table_def);

   if (SageInterface::is_language_case_insensitive()) {
      table_def->setCaseInsensitive(true);
   }

   SgType* type = table_decl->get_type();
   SgJovialTableType* table_type = isSgJovialTableType(type);
   ASSERT_not_null(table_type);
   ASSERT_require(SageBuilder::topScopeStack()->isCaseInsensitive());

// Fix forward type references
   reset_forward_type_refs(name, table_type);

// Append now (before Leave is called) so that symbol lookup will work
   SageInterface::appendStatement(table_decl, SageBuilder::topScopeStack());
   SageBuilder::pushScopeStack(table_def);
}

void SageTreeBuilder::
Leave(SgJovialTableStatement*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgJovialTableStatement*) \n";

   SageBuilder::popScopeStack();  // class definition
}
#endif // ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION

void SageTreeBuilder::
Enter(SgVariableDeclaration* &varDecl, const std::string &name, SgType* type,
      SgExpression* initExpr, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgVariableDeclaration* &, ...) \n";
   ASSERT_not_null(type);

   SgName varName = name;
   SgInitializer* varInit = nullptr;

   if (initExpr) {
     varInit = SageBuilder::buildAssignInitializer_nfi(initExpr, type);
   }

// Reset pointer base-type name so the base type can be replaced when it has been declared
   if (SgPointerType* pointer = isSgPointerType(type)) {
      if (SgTypeUnknown* unknown = isSgTypeUnknown(pointer->get_base_type())) {
         // Reset the type name to the variable name. This allows the variable symbol
         // for name to be found from the forward_type_refs_ map of pointers.
         unknown->set_type_name(name);
      }
   }

   varDecl = SB::buildVariableDeclaration_nfi(varName, type, varInit, SB::topScopeStack());

   if (varDecl->get_definingDeclaration() == nullptr) {
     varDecl->set_definingDeclaration(varDecl);
   }

   SgVariableDefinition* varDef = varDecl->get_definition();
   ASSERT_not_null(varDef);

   SgInitializedName* initName = varDecl->get_decl_item(varName);
   ASSERT_not_null(initName);

   SgDeclarationStatement* declPtr = initName->get_declptr();
   ASSERT_not_null(declPtr);
   ASSERT_require(declPtr == varDef);

   SgInitializedName* varDefn = varDef->get_vardefn();
   ASSERT_not_null(varDefn);
   ASSERT_require(varDefn == initName);

   // Append final label statement (if there are labels, otherwise stmt==varDecl)
   SgStatement* stmt = wrapStmtWithLabels(varDecl, labels);
   SI::appendStatement(stmt, SB::topScopeStack());

// Look for a symbol previously implicitly declared and fix the variable reference
   if (forward_var_refs_.find(name) != forward_var_refs_.end()) {
     if (SgVariableSymbol* varSym = SI::lookupVariableSymbolInParentScopes(name)) {
        // Iterate over all variable references associated with this name
        auto range = forward_var_refs_.equal_range(name);
        for (auto it = range.first; it != range.second; ++it) {
          SgVarRefExp* prevVarRef = it->second;
          SgVariableSymbol* prevVarSym = prevVarRef->get_symbol();
          ASSERT_not_null(prevVarSym);

          SgInitializedName* prevInitName = prevVarSym->get_declaration();
          ASSERT_require(prevInitName->get_name() == initName->get_name());

          // Reset the symbol for the variable reference to the symbol for the explicit variable declaration
          prevVarRef->set_symbol(varSym);

          // Delete the previous symbol and initialized name
          delete prevVarSym;
          delete prevInitName;
        }
        // Remove all variable refs associated with name
        forward_var_refs_.erase(name);
     }
   }
}

void SageTreeBuilder::
Enter(SgVariableDeclaration* &varDecl, SgType* baseType,
      std::list<std::tuple<std::string, SgType*, SgExpression*>> &initInfo, const std::vector<std::string> &labels)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgVariableDeclaration* &, std::tuple<...>, ...) \n";

   // Step through list of tuples to create the multi variable declaration
   for (std::list<std::tuple<std::string, SgType*, SgExpression*>>::iterator it = initInfo.begin(); it != initInfo.end(); ++it) {
      std::string name;
      SgType* type;
      SgExpression* initExpr;
      std::tie(name, type, initExpr) = *it;

      if (!type) {
         type = baseType;
      }

      if (it == initInfo.begin()) {   // On first pass, call Enter() to create variable declaration
         Enter(varDecl, name, type, initExpr, labels);
      } else {                         // On later passes, create new initialized name and append to the var decl
         SgAssignInitializer* init = nullptr;
         if (initExpr) {
            init = SageBuilder::buildAssignInitializer_nfi(initExpr, type);
         }

         SgInitializedName* initName = SageBuilder::buildInitializedName_nfi(name, type, init);
         varDecl->append_variable(initName, init);
         initName->set_declptr(varDecl);

         // A symbol for the variable also has to be created
         SgVariableSymbol* varSym = new SgVariableSymbol(initName);
         ASSERT_not_null(varSym);
         SageBuilder::topScopeStack()->insert_symbol(SgName(name), varSym);
      }
   }
}

void SageTreeBuilder::
Leave(SgVariableDeclaration*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgVariableDeclaration*) \n";
}

void SageTreeBuilder::
Leave(SgVariableDeclaration* varDecl, std::list<LanguageTranslation::ExpressionKind> &modifier_enum_list)
{
  mlog[TRACE] << "SageTreeBuilder::Leave(SgVariableDeclaration*) with modifiers \n";
  using namespace LanguageTranslation;

  for (ExpressionKind modifier_enum : modifier_enum_list) {
    switch(modifier_enum) {
      case ExpressionKind::e_storage_modifier_contiguous:
        varDecl->get_declarationModifier().get_storageModifier().setContiguous();
        break;
      case ExpressionKind::e_type_modifier_intent_in:
        varDecl->get_declarationModifier().get_typeModifier().setIntent_in();
        break;
      case ExpressionKind::e_type_modifier_intent_out:
        varDecl->get_declarationModifier().get_typeModifier().setIntent_out();
        break;
      case ExpressionKind::e_type_modifier_intent_inout:
        varDecl->get_declarationModifier().get_typeModifier().setIntent_inout();
        break;
      case ExpressionKind::e_type_modifier_parameter:
        varDecl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
        break;
      default: break;
    }
  }

  Leave(varDecl);
}

void SageTreeBuilder::
Enter(SgEnumDeclaration* &enum_decl, const std::string &name)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgEnumDeclaration* &, ...) \n";

   enum_decl = SageBuilder::buildEnumDeclaration_nfi(name, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgEnumDeclaration* enum_decl)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgEnumDeclaration*) \n";

   SageInterface::appendStatement(enum_decl, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Enter(SgEnumVal* &enum_val, const std::string &name, SgEnumDeclaration* enum_decl, int value, SgCastExp* cast)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgEnumVal*) \n";

   ASSERT_not_null(enum_decl);
   SgEnumType* enum_type = enum_decl->get_type();
   SgScopeStatement* scope = enum_decl->get_scope();

   SgEnumDeclaration* def_decl = isSgEnumDeclaration(enum_decl->get_definingDeclaration());
   ASSERT_not_null(def_decl);
   SgEnumDeclaration* nondef_decl = isSgEnumDeclaration(enum_decl->get_firstNondefiningDeclaration());
   ASSERT_not_null(nondef_decl);

   // There doesn't have to be an SgEnumVal, there shall be an SgInitializedName
   SgExpression* init_expr = nullptr;
   if (cast) {
      init_expr = cast;
   }
   else {
      enum_val = SageBuilder::buildEnumVal_nfi(value, nondef_decl, name);
      init_expr = enum_val;
   }

   SgAssignInitializer* initializer = SageBuilder::buildAssignInitializer_nfi(init_expr, enum_type);
   SgInitializedName* init_name = SageBuilder::buildInitializedName_nfi(name, enum_type, initializer);

   def_decl->get_enumerators().push_back(init_name);
   init_name->set_scope(scope);
   init_name->set_declptr(def_decl);
   init_name->set_parent(def_decl);

   // Add an associated field symbol to the symbol table
   SgEnumFieldSymbol* enum_field_symbol = new SgEnumFieldSymbol(init_name);
   ASSERT_not_null(enum_field_symbol);
   scope->insert_symbol(name,enum_field_symbol);

   if (enum_type->get_parent() == nullptr) {
      enum_type->set_parent(enum_field_symbol);
   }

   // Add enum to containing scope for Jovial
   if (isSgClassDefinition(scope)) {
      if (SgJovialTableStatement* table = isSgJovialTableStatement(scope->get_parent())) {
         if (SgScopeStatement* containing_scope = table->get_scope()) {
            containing_scope->insert_symbol(name,enum_field_symbol);
         }
      }
   }

   // Also add enum alias to global scope as StatusConstants are globally visible
   auto global_scope = SI::getGlobalScope(scope);
   auto alias_sym = new SgAliasSymbol(enum_field_symbol);
   ASSERT_not_null(global_scope);
   ASSERT_not_null(alias_sym);
   global_scope->insert_symbol(name, alias_sym);
}

void SageTreeBuilder::
Enter(SgTypedefDeclaration* &type_def, const std::string &name, SgType* type)
{
   mlog[TRACE] << "SageTreeBuilder::Enter(SgTypedefDeclaration*) \n";
   SgScopeStatement* scope = SageBuilder::topScopeStack();

   type_def = SageBuilder::buildTypedefDeclaration_nfi(name, type, scope, nullptr);

   // These things should be setup properly in SageBuilder?
   SgTypedefSymbol* symbol = SageInterface::lookupTypedefSymbolInParentScopes(name, scope);
   ASSERT_not_null(symbol);
   SgTypedefType* typedef_type = type_def->get_type();
   ASSERT_not_null(typedef_type);

   type_def->set_base_type(type);
   type_def->set_parent_scope(symbol);
   typedef_type->set_parent_scope(symbol);
   ASSERT_not_null(type_def->get_parent_scope());
   ASSERT_not_null(typedef_type->get_parent_scope());

// Fix forward type references
   reset_forward_type_refs(name, type_def->get_type());

   SageInterface::appendStatement(type_def, SageBuilder::topScopeStack());
}

void SageTreeBuilder::
Leave(SgTypedefDeclaration*)
{
   mlog[TRACE] << "SageTreeBuilder::Leave(SgTypedefDeclaration*) \n";
}

// Fortran specific nodes

void SageTreeBuilder::
Enter(SgCommonBlock* &stmt)
{
  mlog[TRACE] << "SageTreeBuilder::Enter(SgCommonBlock* &)\n";

  stmt = SageBuilder::buildCommonBlock();
  SageInterface::setSourcePosition(stmt);
}

void SageTreeBuilder::
Leave(SgCommonBlock* stmt)
{
  mlog[TRACE] << "SageTreeBuilder::Leave(SgCommonBlock*)\n";
  ASSERT_not_null(stmt);

  SageInterface::appendStatement(stmt, SageBuilder::topScopeStack());
}

// Jovial allows implicitly declared variables (like Fortran?) but does require there to
// be an explicit declaration at some point (unlike Fortran). This builder function manages
// name and symbol information so that the variable reference can be cleaned/fixed up when
// the explicit declaration is seen.
SgVarRefExp* SageTreeBuilder::
buildVarRefExp_nfi(const std::string &name)
{
   SgVarRefExp* var_ref = SageBuilder::buildVarRefExp(name, SageBuilder::topScopeStack());
   ASSERT_not_null(var_ref);
   SageInterface::setSourcePosition(var_ref);

   if (SageInterface::lookupSymbolInParentScopes(name) == nullptr) {
      forward_var_refs_.insert({name, var_ref});
   }
   return var_ref;
}

// Jovial allows pointers to types which haven't been declared yet. This builder function manages
// type name and symbol information so that the pointer variable reference can be cleaned/fixed up when
// the explicit type declaration is seen.
SgPointerType* SageTreeBuilder::
buildPointerType(const std::string& base_type_name, SgType* base_type)
{
  SgPointerType* type = nullptr;

  if (base_type == nullptr) {
    // Constructors are used here rather than SageBuilder functions because these
    // types will be replaced (and deleted) once the actual base type is declared.
    SgTypeUnknown* unknown = new SgTypeUnknown();
    ASSERT_not_null(unknown);
    unknown->set_type_name(base_type_name);

    type = new SgPointerType(unknown);
    ASSERT_not_null(type);

    forward_type_refs_.insert(std::make_pair(base_type_name,type));
  }
  else {
    type = SageBuilder::buildPointerType(base_type);
  }
  ASSERT_not_null(type);

  return type;
}

void SageTreeBuilder::
setCommentPositionAndConsumeToken(PreprocessingInfo* info) {
  // Turn off transformations for get_line() to work (otherwise returns 0)
  if (info) {
    info->get_file_info()->unsetTransformation();
    info->get_file_info()->set_line(tokens_->getNextToken()->getStartLine());
    info->get_file_info()->set_col(tokens_->getNextToken()->getStartCol());
  }
    tokens_->consumeNextToken();
}

void SageTreeBuilder::
reset_forward_type_refs(const std::string &typeName, SgNamedType* type)
{
  auto range = forward_type_refs_.equal_range(typeName);

  bool present = false;
  for (auto pair = range.first; pair != range.second; pair++) {
    SgPointerType* ptr{pair->second};
    ASSERT_not_null(ptr);
    present = true;

    // The placeholder
    SgTypeUnknown* unknown = isSgTypeUnknown(ptr->get_base_type());
    ASSERT_not_null(unknown);

    // The type name may have been replaced by the variable or class type name
    const std::string &name = unknown->get_type_name();

    if (auto varSym = SageInterface::lookupVariableSymbolInParentScopes(name)) {
      SgInitializedName* initName = varSym->get_declaration();
      ASSERT_not_null(initName);
      initName->set_type(SageBuilder::buildPointerType(type));

      // Delete the placeholder and its base type
      delete unknown;
      delete ptr;
    }
    else {
      auto classSym = SageInterface::lookupClassSymbolInParentScopes(name);
      ASSERT_not_null(classSym);

      auto decl = classSym->get_declaration();
      ASSERT_not_null(decl);

      auto declPtr = decl->get_type();
      ASSERT_not_null(declPtr);

      // Replace placeholder
      ptr->set_base_type(declPtr);
      declPtr->set_ptr_to(ptr);
      delete unknown;
    }
  }

  // Remove the type name from the multimap
  if (present) {
    forward_type_refs_.erase(typeName);
  }
}

void SageTreeBuilder::
reset_forward_var_refs(SgScopeStatement* scope)
{
   if (scope == nullptr) {
      scope = SageBuilder::topScopeStack();
   }

// Clear any dangling forward references
   if (!forward_var_refs_.empty()) {
     std::map<const std::string, SgVarRefExp*>::iterator it = forward_var_refs_.begin();
     while (it != forward_var_refs_.end()) {
       if (SgFunctionSymbol* func_sym = SageInterface::lookupFunctionSymbolInParentScopes(it->first, scope)) {
         SgVarRefExp* prev_var_ref = it->second;
         SgVariableSymbol* prev_var_sym = prev_var_ref->get_symbol();
         ASSERT_not_null(prev_var_sym);

         SgInitializedName* prev_init_name = prev_var_sym->get_declaration();
         SgNode* prev_parent = prev_var_ref->get_parent();

      // There may be more options but only three are known so far
         if (isSgUnaryOp(prev_parent) || isSgBinaryOp(prev_parent) || isSgExprStatement(prev_parent)) {
           SgExprListExp* params = SageBuilder::buildExprListExp_nfi();
           SgFunctionCallExp* func_call = SageBuilder::buildFunctionCallExp(func_sym, params);
           func_call->set_parent(prev_parent);

           if (SgExprStatement* expr_stmt = isSgExprStatement(prev_parent)) {
             expr_stmt->set_expression(func_call);
           }
           else if (SgUnaryOp* unary_op = isSgUnaryOp(prev_parent)) {
             SgVarRefExp* var_ref = isSgVarRefExp(unary_op->get_operand());
             if (var_ref == prev_var_ref) {
               unary_op->set_operand(func_call);
             }
             ASSERT_require(var_ref == prev_var_ref);
           }
           else if (SgBinaryOp* bin_op = isSgBinaryOp(prev_parent)) {
             // Is this left or right operand
             SgVarRefExp* var_ref = isSgVarRefExp(bin_op->get_rhs_operand());
             if (var_ref == prev_var_ref) {
               bin_op->set_rhs_operand(func_call);
             }
             else if ((var_ref = isSgVarRefExp(bin_op->get_lhs_operand()))) {
               bin_op->set_lhs_operand(func_call);
             }
             ASSERT_require(var_ref == prev_var_ref);
           }

        // The dangling variable reference has been fixed
           it = forward_var_refs_.erase(it);

        // Delete the previous variable reference, symbol and initialized name
           delete prev_init_name;
           delete prev_var_sym;
           delete prev_var_ref;
         }
         else {
           // Unexpected previous parent node
           mlog[WARN] << "{" << it->first << ": " << it->second << " parent is " << prev_parent << "}\n";
           it++;
         }
       }
       else it++;
     }
   }
}

// Jovial TableItem and Block data members have visibility outside of their declarative class.
// Both tables and blocks are SgJovialTableStatements deriving from SgClassDeclaration.  So if the
// current scope is SgClassDefinition, this function creates an alias to the data item variable
// and inserts it in global scope.
void SageTreeBuilder::
injectAliasSymbol(const std::string &name)
{
   auto top_scope{SB::topScopeStack()};
   if (auto class_def = isSgClassDefinition(top_scope)) {
      auto var_sym = SI::lookupVariableSymbolInParentScopes(SgName{name}, top_scope);
      ASSERT_not_null(var_sym);

      SgJovialTableStatement* table_decl = isSgJovialTableStatement(class_def->get_declaration());
      ASSERT_not_null(table_decl);
      SgScopeStatement* decl_scope = table_decl->get_scope();
      ASSERT_not_null(decl_scope);

      // Tables may be embedded in other tables or blocks, find parent of outermost table/block declaration
      while (isSgClassDefinition(decl_scope)) {
        table_decl = isSgJovialTableStatement(decl_scope->get_parent());
        ASSERT_not_null(table_decl);
        decl_scope = table_decl->get_scope();
        ASSERT_not_null(decl_scope);
      }

      if (!isSgFunctionParameterScope(decl_scope)) {
        SgAliasSymbol* alias_sym = new SgAliasSymbol(var_sym);
        ASSERT_not_null(alias_sym);

        // Inject the alias symbol in the namespace or basic block (if there is one).
        // Otherwise put the alias in global scope. I believe if it's a basic block that
        // it will placed in a function scope, which is probably the correct thing to do so
        // that table items declared inside a function don't end up in global scope.
        SgScopeStatement* scope = nullptr;
        if (isSgNamespaceDefinitionStatement(decl_scope) || isSgBasicBlock(decl_scope)) {
          scope = decl_scope;
        }
        else {
          scope = SI::getGlobalScope(table_decl);
        }
        ASSERT_not_null(scope);
        scope->insert_symbol(SgName(name), alias_sym);
      }
   }
}

// Jovial TableItem and Block data members have visibility outside of their declarative class.
// If an anonymous type is created because of a variable declaration, an alias to that type
// may need to be placed in the symbol table for the namespace (or perhaps file scope).
void SageTreeBuilder::
injectAliasTypeSymbol(const std::string &name)
{
   auto scope = SB::topScopeStack();
   auto symbol = SI::lookupSymbolInParentScopes(SgName{name}, scope);

   // Return if not needed
   if (!symbol) return;

   // Tables may be embedded in other tables or blocks, find parent of outermost table/block declaration
   while (isSgClassDefinition(scope)) {
     auto table_decl = isSgJovialTableStatement(scope->get_parent());
     ASSERT_not_null(table_decl);
     scope = table_decl->get_scope();
   }
   ASSERT_not_null(scope);

   // Function parameter scopes are funky, don't go near them
   if (!isSgFunctionParameterScope(scope)) {
     // Don't need to inject symbols that already exist
     if (!scope->symbol_exists(name)) {
       SgAliasSymbol* alias_sym = new SgAliasSymbol(symbol);
       ASSERT_not_null(alias_sym);
       scope->insert_symbol(SgName(name), alias_sym);
     }
   }
}

SgStatement* SageTreeBuilder::
wrapStmtWithLabels(SgStatement* stmt, const std::vector<std::string> &labels)
{
   ASSERT_not_null(stmt);

   // Order of the labels need to be reversed to come out right
   std::vector<std::string> reversed = labels;
   auto lbegin = reversed.begin();
   auto lend = reversed.end();
   std::reverse(lbegin,lend);

   // Statements may have a label(s), wrap the statement with its label(s)
   for (auto label : reversed) {
      // A label statement may already exist for this label, e.g., from a
      // placeholder created previously for an SgGotoStatement, for example, check.
      SgLabelStatement* labelStmt{nullptr};
      if (labels_.find(label) != labels_.end()) {
         labelStmt = labels_[label];
      }
      else {
         labelStmt = SB::buildLabelStatement_nfi(label, stmt, SB::topScopeStack());
         labels_[label] = labelStmt;
      }

      if (labelStmt && labelStmt->get_statement() == nullptr) {
        // Found a placeholder label statement
        labelStmt->set_statement(stmt);
        stmt->set_parent(labelStmt);
      }

      stmt = labelStmt;
   }
   ASSERT_not_null(stmt);

   return stmt;
}

#define NO_CPP17 0

// Temporary wrappers for SageInterface functions (needed until ROSE builds with C++17)
//
namespace SageBuilderCpp17 {

// Types
//
SgType* buildArrayType(SgType* baseType, std::list<SgExpression*> &explicitShapeList)
{
   SgExprListExp* dimInfo = SageBuilder::buildExprListExp_nfi();

   for (SgExpression* expr: explicitShapeList) {
      dimInfo->get_expressions().push_back(expr);
   }

   return SageBuilder::buildArrayType(baseType, dimInfo);
}

// SgBasicBlock
//

#if NO_CPP17
#endif

// Operators
//
SgExpression* buildAddOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildAddOp_nfi(lhs, rhs);
}

SgExpression* buildAndOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildAndOp_nfi(lhs, rhs);
}

SgExpression* buildDivideOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildDivideOp_nfi(lhs, rhs);
}

SgExpression* buildEqualityOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildEqualityOp_nfi(lhs, rhs);
}

SgExpression* buildGreaterThanOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildGreaterThanOp_nfi(lhs, rhs);
}

SgExpression* buildGreaterOrEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildGreaterOrEqualOp_nfi(lhs, rhs);
}

SgExpression* buildMultiplyOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildMultiplyOp_nfi(lhs, rhs);
}

SgExpression* buildLessThanOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildLessThanOp_nfi(lhs, rhs);
}

SgExpression* buildLessOrEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildLessOrEqualOp_nfi(lhs, rhs);
}

SgExpression* buildNotEqualOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildNotEqualOp_nfi(lhs, rhs);
}

SgExpression* buildOrOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildOrOp_nfi(lhs, rhs);
}

SgExpression* buildMinusOp_nfi(SgExpression* i, bool is_prefix /* = true */)
{
   SgUnaryOp::Sgop_mode mode_enum;

   if (is_prefix) {
     mode_enum = SgUnaryOp::Sgop_mode::prefix;
   } else {
     mode_enum = SgUnaryOp::Sgop_mode::postfix;
   }

   return SageBuilder::buildMinusOp_nfi(i, mode_enum);
}

SgExpression* buildSubtractOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildSubtractOp_nfi(lhs, rhs);
}

// Expressions
//
SgExpression* buildConcatenationOp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildConcatenationOp_nfi(lhs, rhs);
}

SgExpression* buildExprListExp_nfi()
{
   return SageBuilder::buildExprListExp_nfi();
}

SgExpression* buildBoolValExp_nfi(bool value)
{
   return SageBuilder::buildBoolValExp_nfi(value);
}

SgExpression* buildIntVal_nfi(int value = 0)
{
   return SageBuilder::buildIntVal_nfi(value);
}

SgExpression* buildStringVal_nfi(std::string value)
{
   return SageBuilder::buildStringVal_nfi(value);
}

SgExpression* buildFloatVal_nfi(const std::string &str)
{
   return SageBuilder::buildFloatVal_nfi(str);
}

SgExpression* buildComplexVal_nfi(SgExpression* real_value, SgExpression* imaginary_value, const std::string &str)
{
   SgValueExp* real = isSgValueExp(real_value);
   SgValueExp* imaginary = isSgValueExp(imaginary_value);

   ASSERT_not_null(real);
   ASSERT_not_null(imaginary);

   return SageBuilder::buildComplexVal_nfi(real, imaginary, str);
}

SgExpression* buildVarRefExp_nfi(std::string &name, SgScopeStatement* scope)
{
   SgVarRefExp* var_ref = SageBuilder::buildVarRefExp(name, scope);
   SageInterface::setSourcePosition(var_ref);

   return var_ref;
}

SgExpression* buildSubscriptExpression_nfi(SgExpression* lower_bound, SgExpression* upper_bound, SgExpression* stride)
{
   return SageBuilder::buildSubscriptExpression_nfi(lower_bound, upper_bound, stride);
}

SgExpression* buildPntrArrRefExp_nfi(SgExpression* lhs, SgExpression* rhs)
{
   return SageBuilder::buildPntrArrRefExp_nfi(lhs, rhs);
}

SgExpression* buildAggregateInitializer_nfi(SgExprListExp* initializers, SgType* type)
{
   return SageBuilder::buildAggregateInitializer_nfi(initializers, type);
}

SgExpression* buildAsteriskShapeExp_nfi()
{
   SgAsteriskShapeExp* shape = new SgAsteriskShapeExp();
   ASSERT_not_null(shape);
   SageInterface::setSourcePosition(shape);

   return shape;
}

SgExpression* buildNullExpression_nfi()
{
   return SageBuilder::buildNullExpression_nfi();
}

SgExpression* buildFunctionCallExp(SgFunctionCallExp* func_call)
{
   return func_call;
}

SgExprListExp* buildExprListExp_nfi(const std::list<SgExpression*> &list)
{
   SgExprListExp* expr_list = SageBuilder::buildExprListExp_nfi();

   for (SgExpression* expr : list) {
      expr_list->get_expressions().push_back(expr);
   }
   return expr_list;
}

SgCommonBlockObject* buildCommonBlockObject(std::string name, SgExprListExp* expr_list)
{
   SgCommonBlockObject* common_block_object = SageBuilder::buildCommonBlockObject(name, expr_list);
   SageInterface::setSourcePosition(common_block_object);
   return common_block_object;
}

void set_false_body(SgIfStmt* &if_stmt, SgBasicBlock* false_body)
{
   ASSERT_not_null(if_stmt);
   if_stmt->set_false_body(false_body);
}

void set_need_paren(SgExpression* &expr)
{
   ASSERT_not_null(expr);
   expr->set_need_paren(true);
}

void fixUndeclaredResultName(const std::string &result_name, SgScopeStatement* scope, SgType* result_type)
{
   // This function should only be called if there is no symbol and there is a result type
   SgSymbol* symbol = SageInterface::lookupSymbolInParentScopes(result_name, scope);
   ASSERT_require(symbol == nullptr);
   ASSERT_not_null(result_type);

   SgInitializedName* init_name = SageBuilder::buildInitializedName_nfi(result_name, result_type, /*initializer*/nullptr);
   SageInterface::setSourcePosition(init_name);
   init_name->set_scope(scope);
   SgVariableSymbol* result_symbol = new SgVariableSymbol(init_name);
   ASSERT_not_null(result_symbol);
   scope->insert_symbol(result_name, result_symbol);
}

SgFunctionRefExp* buildIntrinsicFunctionRefExp_nfi(const std::string &name, SgScopeStatement* scope)
{
   SgFunctionRefExp* func_ref = nullptr;

  // assumes Fortran for now
   SgFunctionSymbol* symbol = SageInterface::lookupFunctionSymbolInParentScopes(name, scope);

   if (symbol) {
   }
   else {
     // Look for intrinsic name
     if (name == "num_images") {
       // TODO
       mlog[WARN] << "need to build a function reference to num_images\n";
#if 0
       // Doesn't work
       // func_ref = SageBuilder::buildFunctionRefExp(SgName(name), scope);
       SgType* return_type = SB::buildIntType();
       SgFunctionParameterList *parList = SB::buildFunctionParameterList();
       SgGlobal* globalscope = SI::getGlobalScope(scope);
       SgFunctionDeclaration * funcDecl = SB::buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);
#endif
     }
   }

   return func_ref;
}

SgFunctionCallExp*
buildIntrinsicFunctionCallExp_nfi(const std::string &name, SgExprListExp* params, SgScopeStatement* scope)
{
  SgType* return_type = nullptr;
  SgFunctionCallExp* func_call = nullptr;

  if (!params) {
    params = SageBuilder::buildExprListExp_nfi();
  }
  if (!scope) {
    scope = SageBuilder::topScopeStack();
  }
  ASSERT_not_null(params);
  ASSERT_not_null(scope);

  // Create a return type based on the intrinsic name
  if (name == "num_images") {
    return_type = SageBuilder::buildIntType();
  }
  else {
    return_type = SageBuilder::buildVoidType();
  }

  if (return_type) {
    func_call = SageBuilder::buildFunctionCallExp(SgName(name), return_type, params, scope);
    ASSERT_not_null(func_call);
    SageInterface::setSourcePosition(func_call);
  }

  return func_call;
}

} // namespace SageBuilderCpp17

} // namespace builder
} // namespace Rose

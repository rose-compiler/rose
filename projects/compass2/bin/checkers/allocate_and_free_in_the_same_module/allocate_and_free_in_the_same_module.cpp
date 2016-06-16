/**
 * \file allocate_and_free_in_the_same_module.cpp
 * \author Sam Kelly
 * \date Tuesday, July 18, 2013
 */

#include "rose.h"
#include <boost/foreach.hpp>

#include "rose.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "compass2/compass.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const allocateAndFreeInTheSameModuleChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_ALLOCATE_AND_FREE_IN_THE_SAME_MODULE_H
#define COMPASS_ALLOCATE_AND_FREE_IN_THE_SAME_MODULE_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace AllocateAndFreeInTheSameModule
{
extern const string checker_name;
extern const string short_description;
extern const string long_description;
extern       string source_directory;

/**
 * \brief Specificaiton of checker results.
 */
class CheckerOutput: public Compass::OutputViolationBase {
 public:
  explicit CheckerOutput(SgNode *const node);

 private:
  DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
};

bool IsNodeNotInUserLocation(const SgNode* node)
{
  const SgLocatedNode* located_node = isSgLocatedNode(node);
  if (located_node != NULL)
  {
    return ! Compass::IsNodeInUserLocation(
        located_node,
        AllocateAndFreeInTheSameModule::source_directory);
  }
  else
  {
    return true;
  }
};

} // ::CompassAnalyses
} // ::AllocateAndFreeInTheSameModule
#endif // COMPASS_ALLOCATE_AND_FREE_IN_THE_SAME_MODULE_H

/*-----------------------------------------------------------------------------
 * Description
 *
 * This checker checks that for each malloc() or calloc() call, there exists
 * at least one place in the source code where (it has been determined) a
 * variable that COULD be holding the memory address originally returned by
 * the malloc() or calloc() call has been freed. Thus cascading assignments
 * of variables, even across function and scope boundaries, are supported.
 * In general, when this checker says that a free is missing, this is a
 * very reliable result, however there are certain scenarios where this
 * checker will NOT detect that a free is missing. This is especially true
 * in cases where a variable is used to house multiple dynamically allocated
 * objects over the course of its lifetime. This is because this checker
 * does not take execution order into account, so once a variable has been
 * marked as free, this checker will not be able to figure out that later
 * in the code it houses an additional malloc that must be freed, because
 * this checker has no notion of "later in the code".
 *
 *
 * The verbose output provides some very useful debugging information,
 * including the dependency graph used to resolve relationships between
 * variables and across function and scope boundaries. The verbose output
 * also indicates the checker's degree of confidence that variables it has
 * marked as free are in fact free (values of "PROBABLY" and
 * "ALMOST DEFINITELY" are possible.)
 *
 *
 * This checker uses a custom algorithm written by Sam Kelly
 * (kelly64@llnl.gov or kellys@dickinson.edu), and is able to provide
 * the maximal amount of information possible about the lifetime of
 * each dynamic memory allocation WITHOUT relying on flow analysis or
 * other slower, more advanced techniques that are able to take into
 * account the program execution order. The basic concept of this
 * algorithm is to generate an adjacency list that tracks all
 * assignment statements and chains of assignments that originally
 * resulted from a variable or function return statement containing
 * a malloc or calloc call. Thus for each malloc or calloc call in
 * the program, we generate a list of variables where a free() call
 * on any of these variables probably indicates that the original
 * malloc or calloc has been successfully freed.
 *
 *
 * NOTE ON TERMINOLOGY: the following diagram may be helpful in
 * understanding what is meant by the inner/outer function var
 * mappings, because this is somewhat non obvious:
 *
 * int *function1(int *var1)
 * {
 *    return var1;
 * }
 *
 * var2 = function(malloc(..));
 *
 * In the above code, the SgSymbol of var1 is the INNER VAR,
 * and the SgSymbol of var2 is the OUTER VAR, thus inner/outer
 * mappings are used when we are trying to trace a variable's
 * progress as it moves from the inside of a function, outside
 * through the return statement
 *---------------------------------------------------------------------------*/

//TODO: use flow analysis
//TODO: differentiate between different instances of structs / container objects
//      that contain a pointer.. right now the following will not be caught, but
//      can be caught without flow analysis:
/*
 * struct pointer_container
 * {
 *   int *contained_ptr;
 * };
 *
 * int main()
 * {
 *   pointer_container ctr1;
 *   ctr1.contained_ptr = (int*)malloc(sizeof(int));
 *   free(ctr1.contained_ptr);
 *   pointer_container ctr2;
 *   ctr2.contained_ptr = (int*)malloc(sizeof(int)); // not detected because
 *   // the variable id for pointer_container.contained_ptr has already been
 *   // marked as free
 * }
 *
 * TODO: handle cases where malloc or calloc is called, but the return value
 *       is not stored in a variable. Make sure that if the malloc or calloc
 *       call is contained within a free(), that this isn't marked as missing
 *       a free, even though such a scenario is superflous
 */

namespace CompassAnalyses
{
namespace AllocateAndFreeInTheSameModule
{
const string checker_name      = "AllocateAndFreeInTheSameModule";
const string short_description = "malloc() or calloc() might be missing a matching free!";
const string long_description  = "malloc() and calloc() should always have a matching free()";

string source_directory = "/";
}
}

CompassAnalyses::AllocateAndFreeInTheSameModule::
CheckerOutput::CheckerOutput(SgNode *const node)
: OutputViolationBase(node,
                      ::allocateAndFreeInTheSameModuleChecker->checkerName,
                       ::allocateAndFreeInTheSameModuleChecker->shortDescription) {}


static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
{


  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  string target_directory =
      parameters["general::target_directory"].front();
  CompassAnalyses::AllocateAndFreeInTheSameModule::source_directory.assign(target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();
  ROSE_ASSERT(sageProject != NULL);

  SgNode* root_node = (SgNode*) sageProject;
  ROSE_ASSERT(root_node != NULL);

  bool verbose = sageProject->get_verbose();

  // maps variables in the symbol table to a boolean
  // determining whether that variable has been freed
  // or not
  boost::unordered_map<SgSymbol*, bool> var_free_status;

  // maps variables in the symbol table to the original malloc
  // call that was performed on them so we can return this
  // as checker output if no matching frees are found
  boost::unordered_map<SgSymbol*, SgNode*> malloc_nodes;

  // keeps track of the dynamically allocated variables in the
  // symbol table that were freed
  boost::unordered_set<SgSymbol*> freed;

  // keeps track of the dynamically allocated variables in the
  // symbol table
  std::vector<SgSymbol*> unfreed;

  // tracks function calls that explicitly return a malloc (e.g. return malloc()...)
  boost::unordered_set<SgFunctionCallExp*> explicit_malloc_func_calls;

  // keeps track of the inner variable being returned by a pointer function
  // e.g. return var1; appearing in the function func1 would map func1 to var1
  boost::unordered_map<SgFunctionCallExp*, SgSymbol*> func_inner_var_mappings;

  // keeps track of the outer variable that receives the value of a pointer function
  // e.g. var2 = func1(); would map func1 to var2
  boost::unordered_map<SgFunctionCallExp*, SgSymbol*> func_outer_var_mappings;

  // keeps a list of all functions added to either of the above two hash maps
  boost::unordered_set<SgFunctionCallExp*> tracked_functions;


  // for each pointer variable, tracks the list of all other pointer
  // variables in the program that are ever set to the value (and are thus
  // associated with) this pointer variable
  boost::unordered_map<SgSymbol*, boost::unordered_set<SgSymbol*> > var_mappings;
  std::vector<SgSymbol*> var_mappings_list;

  VariableIdMapping var_ids;
  var_ids.computeVariableSymbolMapping(sageProject);

  // we have to do SgFunctionCallExp's first because the inner/outer
  // variable mappings require this to be pre-processed
  AstMatching func_call_matcher;
  MatchResult func_call_matches = func_call_matcher
      .performMatching("$f=SgFunctionCallExp", root_node);
  BOOST_FOREACH(SingleMatchVarBindings match, func_call_matches)
  {
    SgFunctionCallExp *func_call = (SgFunctionCallExp *)match["$f"];
    SgFunctionRefExp *func_ref = isSgFunctionRefExp(func_call->get_traversalSuccessorByIndex(0));
    if(func_ref == NULL) continue;
    std::string func_str = func_ref->get_symbol()->get_name().getString();
    if(func_str.compare("free") == 0)
    {
      AstMatching var_matcher;
      SgVarRefExp *var_ref = isSgVarRefExp(var_matcher
                                           .performMatching("$r=SgVarRefExp", func_call)
                                           .back()["$r"]);
      if(var_ref)
      {
        SgSymbol *var_ref_symbol = var_ref->get_symbol();
        if(verbose)
          std::cout << "FREE FOUND ON: " << var_ref_symbol->get_name() << std::endl;
        freed.insert(var_ref_symbol);
      }
    } else {

      SgExprListExp *expr_list = func_call->get_args();
      SgFunctionDefinition *func_def = SgNodeHelper::determineFunctionDefinition(func_call);
      if(func_def == NULL) continue;
      SgFunctionDeclaration *func_dec = func_def->get_declaration();
      if(func_dec == NULL) continue;
      SgFunctionParameterList *func_params = func_dec->get_parameterList();
      if(func_params == NULL) continue;
      for(int i = 0; i < expr_list->get_numberOfTraversalSuccessors(); i++)
      {
        if(i >= func_params->get_numberOfTraversalSuccessors()) break;
        SgNode* param = expr_list->get_traversalSuccessorByIndex(i);
        SgInitializedName* func_param =
            isSgInitializedName(func_params->get_traversalSuccessorByIndex(i));

        VariableId func_param_id = var_ids.variableId(func_param);
        SgSymbol *func_param_sym = var_ids.getSymbol(func_param_id);

        ROSE_ASSERT(func_param != NULL);
        ROSE_ASSERT(func_param_sym != NULL);

        // we use an AST match in case this is a container type
        // the last match will be a reference to the actual variable
        SgVarRefExp *param_var = NULL;
        AstMatching param_var_matcher;
        MatchResult param_var_matches = param_var_matcher
            .performMatching("$v=SgVarRefExp", param);
        if(param_var_matches.size() > 0)
        {
          param_var = (SgVarRefExp *)param_var_matches.back()["$v"];
        }
        if(param_var != NULL)
        {
          SgSymbol *param_sym = param_var->get_symbol();
          // now we must add var mappings from the inner parameter of the function

          if(var_mappings.find(param_sym) == var_mappings.end())
          {
            var_mappings_list.push_back(param_sym);
            boost::unordered_set<SgSymbol*> set;
            set.insert(func_param_sym);
            var_mappings[param_sym] = set;
          } else {
            var_mappings[param_sym].insert(func_param_sym);
          }

        } else {
          // handle the case of malloc() or calloc() being passed as a function parameter
          AstMatching func_ref_matcher;
          MatchResult found_func_matches =
              func_ref_matcher.performMatching("$r=SgFunctionRefExp", param);
          if(found_func_matches.size() > 0)
          {
            SgFunctionRefExp *found_func = isSgFunctionRefExp(
                found_func_matches.front()["$r"]);
            if(!found_func->isDefinable()) continue;
            std::string found_func_str = found_func->get_symbol()
                                             ->get_name().getString();
            if(found_func_str.compare("malloc") == 0
                || found_func_str.compare("calloc") == 0)
            {
              // found a malloc call within a function parameter
              // we must map this to whatever variable it would be assigned in the
              // parent function
              var_free_status[func_param_sym] = false;
              malloc_nodes[func_param_sym] = found_func->get_symbol();
              unfreed.push_back(func_param_sym);
            }
          }
        }
      }

      // figure out if a malloc is returned (directly) by this function
      AstMatching return_matcher;
      MatchResult return_matches = return_matcher
          .performMatching("$r=SgReturnStmt", func_def);
      BOOST_FOREACH(SingleMatchVarBindings return_match, return_matches)
      {
        SgReturnStmt *func_ret = (SgReturnStmt *)return_match["$r"];

        // map MALLOCS & CALLOC out
        AstMatching return_func_matcher;
        MatchResult return_func_matches = return_func_matcher
            .performMatching("$f=SgFunctionRefExp", func_ret);
        BOOST_FOREACH(SingleMatchVarBindings return_func_match, return_func_matches)
        {
          SgFunctionRefExp *return_func_ref = (SgFunctionRefExp *)return_func_match["$f"];
          SgFunctionCallExp *return_func_call = isSgFunctionCallExp(return_func_ref->get_parent());
          if(return_func_call == NULL) continue;
          std::string return_func_str = return_func_ref->get_symbol()->get_name().getString();

          if(return_func_str.compare("malloc") == 0 || return_func_str.compare("calloc") == 0)
          {
            if(verbose && explicit_malloc_func_calls.find(func_call)
                == explicit_malloc_func_calls.end())
            {
              std::cout << "MALLOC FOUND ON: \"" << func_str << "\" return statement" << std::endl;
            }
            explicit_malloc_func_calls.insert(func_call);
            // once we have one malloc we can ignore the rest of the return statement
            break;
          }
        }

        // do inner var mappings
        AstMatching return_var_matcher;
        MatchResult return_var_matches = return_var_matcher
            .performMatching("$v=SgVarRefExp", func_ret);
        BOOST_FOREACH(SingleMatchVarBindings return_var_match, return_var_matches)
        {
          SgVarRefExp *return_var = (SgVarRefExp *)return_var_match["$v"];
          // skip non pointer variables
          if(!SageInterface::isPointerType(return_var->get_type())) continue;
          SgSymbol *return_var_sym = return_var->get_symbol();
          if(return_var_sym == NULL) continue;
          func_inner_var_mappings[func_call] = return_var_sym;
          tracked_functions.insert(func_call);
          if(verbose)
            std::cout << "INNER VARIABLE MAPPING: " << func_str << " returns " << return_var_sym->get_name() << std::endl;
        }
      }
    }
  }

  // do a global search for assignment operations
  AstMatching main_matcher;
  MatchResult main_matches = main_matcher
      .performMatching("$a=SgAssignOp|$i=SgAssignInitializer", root_node);
  BOOST_FOREACH(SingleMatchVarBindings match, main_matches)
  {
    SgAssignOp *assign_op = isSgAssignOp(match["$a"]);
    SgAssignInitializer *assign_init = isSgAssignInitializer(match["$i"]);

    SgNode *assign_RHS = NULL;
    SgNode *assign_LHS = NULL;
    SgSymbol *LHS_symbol = NULL;

    if(assign_op != NULL || assign_init != NULL)
    {
      if(assign_init != NULL)
      {
        assign_RHS = assign_init->get_operand();
        ROSE_ASSERT(assign_RHS != NULL);

        // we will try to get the symbol from the parent
        SgInitializedName *var_name =
            isSgInitializedName(assign_init->get_parent());
        if(var_name != NULL)
        {
          LHS_symbol = var_name->get_symbol_from_symbol_table();
          if(LHS_symbol == NULL) continue;
        }
      } else {
        assign_RHS = assign_op->get_rhs_operand();
        ROSE_ASSERT(assign_RHS != NULL);
        assign_LHS = assign_op->get_lhs_operand();
        ROSE_ASSERT(assign_LHS != NULL);
      }
      ROSE_ASSERT(assign_RHS != NULL);

      if(LHS_symbol == NULL)
      {
        ROSE_ASSERT(assign_op != NULL);
        SgNode* lhs_operand = assign_op->get_lhs_operand();
        ROSE_ASSERT(lhs_operand != NULL);

        AstMatching LHS_matcher;
        MatchResult var_ref_matches =
            LHS_matcher.performMatching("$r=SgVarRefExp", lhs_operand);

        // give up to avoid assertion fails
        if(var_ref_matches.size() == 0) continue;

        SgVarRefExp *LHS = (SgVarRefExp*)(var_ref_matches.back()["$r"]);
        ROSE_ASSERT(LHS != NULL);

        LHS_symbol = LHS->get_symbol();
        ROSE_ASSERT(LHS_symbol != NULL);
      }

      // ignore non pointer variables
      if(!SageInterface::isPointerType(LHS_symbol->get_type())) continue;

      // build initial variable mappings
      if(var_mappings.find(LHS_symbol) == var_mappings.end())
      {
        boost::unordered_set<SgSymbol*> new_set;
        var_mappings[LHS_symbol] = new_set;
        var_mappings_list.push_back(LHS_symbol);
      }

      // handle pointer->pointer assignments
      SgVarRefExp *RHS_var_ref = isSgVarRefExp(assign_RHS);
      SgSymbol *RHS_symbol = NULL;
      if(RHS_var_ref == NULL)
      {
        // if the RHS isn't an SgVarRefExp, let's see if one of the
        // children is
        AstMatching RHS_var_matcher;
        MatchResult RHS_var_matches =
            RHS_var_matcher.performMatching("$r=SgVarRefExp", assign_op);
        if(RHS_var_matches.size() > 0)
          RHS_symbol = ((SgVarRefExp*)RHS_var_matches.back()["$r"])->get_symbol();
      } else {
        RHS_symbol = RHS_var_ref->get_symbol();
      }
      if(RHS_symbol != NULL && LHS_symbol != NULL && LHS_symbol != RHS_symbol
          && SageInterface::isPointerType(RHS_symbol->get_type()))
      {

        // defensive -- should never actually happen
        if(var_mappings.find(RHS_symbol) == var_mappings.end())
        {
          boost::unordered_set<SgSymbol*> new_set;
          var_mappings[RHS_symbol] = new_set;
          var_mappings_list.push_back(RHS_symbol);
        }
        // add the LHS to the associated list for the RHS
        var_mappings[RHS_symbol].insert(LHS_symbol);
      }

      // we do a search for SgFunctionCallExp's on the RHS
      // because it could be nested inside of a cast expression
      AstMatching RHS_func_matcher;
      MatchResult RHS_func_matches = RHS_func_matcher
          .performMatching("$f=SgFunctionCallExp", assign_RHS);
      BOOST_FOREACH(SingleMatchVarBindings RHS_func_match, RHS_func_matches)
      {
        // do outer var mappings
        SgFunctionCallExp *func_call = (SgFunctionCallExp *)RHS_func_match["$f"];
        if(func_call->getAssociatedFunctionSymbol() == NULL) continue;
        std::string func_str = func_call->getAssociatedFunctionSymbol()->get_name();
        // if the function is a malloc or a calloc or a function that is known
        // to explicitly return a malloc or calloc
        if(func_str.compare("malloc") == 0 || func_str.compare("calloc") == 0)
        {
          var_free_status[LHS_symbol] = false;
          malloc_nodes[LHS_symbol] = func_call;
          unfreed.push_back(LHS_symbol);
        } else {
          if(verbose)
            std::cout << "OUTER VARIABLE MAPPING: " << func_call
            ->getAssociatedFunctionSymbol()->get_name() << " => "
            << LHS_symbol->get_name() << std::endl;
          func_outer_var_mappings[func_call] = LHS_symbol;
          tracked_functions.insert(func_call);
          if(explicit_malloc_func_calls.find(func_call) != explicit_malloc_func_calls.end())
          {
            var_free_status[LHS_symbol] = false;
            malloc_nodes[LHS_symbol] = func_call;
            unfreed.push_back(LHS_symbol);
          }
        }
      }
    }
  }

  // process function var mappings (inner var -> return statement -> outer var)
  for(boost::unordered_set<SgFunctionCallExp*>::iterator itr
      = tracked_functions.begin();
      itr != tracked_functions.end(); ++itr)
  {
    SgFunctionCallExp *func_call = (SgFunctionCallExp *)(*itr);
    SgSymbol *inner_var = func_inner_var_mappings[func_call];
    SgSymbol *outer_var = func_outer_var_mappings[func_call];
    if(inner_var != NULL && outer_var != NULL)
    {
      if(var_mappings.find(inner_var) == var_mappings.end())
      {
        var_mappings_list.push_back(inner_var);
        boost::unordered_set<SgSymbol*> set;
        set.insert(outer_var);
        var_mappings[inner_var] = set;
      } else {
        var_mappings[inner_var].insert(outer_var);
      }
    }
  }

  // expand variable mappings
  // for most general purpose cases won't require more than 2 passes
  bool changed;
  do
  {
    changed = false;
    BOOST_FOREACH(SgSymbol *key_sym, var_mappings_list)
    {
      for(boost::unordered_set<SgSymbol*>::iterator itr
          = var_mappings[key_sym].begin();
          itr != var_mappings[key_sym].end(); ++itr)
      {
        SgSymbol* current = *itr;
        boost::unordered_set<SgSymbol*> inner_set = var_mappings[current];
        for(boost::unordered_set<SgSymbol*>::iterator itr2 = inner_set.begin();
            itr2 != inner_set.end(); ++itr2)
        {
          if(var_mappings[key_sym].find(*itr2) == var_mappings[key_sym].end())
          {
            var_mappings[key_sym].insert(*itr2);
            changed = true;
          }
        }
      }
    }
  } while(changed);

  // useful for debugging -- maps out variable dependency list
  if(verbose)
  {
    BOOST_FOREACH(SgSymbol *key_sym, var_mappings_list)
    {
      std::string RHS_name = key_sym->get_name();
      std::cout << RHS_name << std::endl;
      for(boost::unordered_set<SgSymbol*>::iterator itr
          = var_mappings[key_sym].begin();
          itr != var_mappings[key_sym].end(); ++itr)
      {
        SgSymbol* val_sym = *itr;
        std::string LHS_name = val_sym->get_name();
        std::cout << " |-" << LHS_name << std::endl;
      }
    }
  }


  BOOST_FOREACH(SgSymbol *key_sym, unfreed)
  {
    std::string key_sym_name = key_sym->get_name();
    if(freed.find(key_sym) != freed.end())
    {
      if(verbose)
        std::cout << "MARKING "<< key_sym_name <<
        " [ALMOST DEFINITELY FREE]" << std::endl;
      var_free_status[key_sym] = true;
    }
    for(boost::unordered_set<SgSymbol*>::iterator itr
        = var_mappings[key_sym].begin();
        itr != var_mappings[key_sym].end(); ++itr)
    {
      SgSymbol* val_sym = *itr;
      if(freed.find(val_sym) != freed.end())
      {
        var_free_status[key_sym] = true;
        if(verbose)
          std::cout << "MARKING "<< key_sym_name <<
          " [PROBABLY FREE] (associated variable "
            << val_sym->get_name() << " marked as FREE)" << std::endl;
      }
    }
  }

  BOOST_FOREACH(SgSymbol *var_symbol, unfreed)
  {
    if(var_free_status[var_symbol] == false)
    {
      if(verbose)
        std::cout << "MARKING " << var_symbol->get_name()
        << " [NOT FREE]!" << std::endl;
      output->addOutput(
          new CompassAnalyses::AllocateAndFreeInTheSameModule::
          CheckerOutput(malloc_nodes[var_symbol]));
    }
  }
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
{
  return NULL;
}

extern const Compass::Checker* const allocateAndFreeInTheSameModuleChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AllocateAndFreeInTheSameModule::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::AllocateAndFreeInTheSameModule::short_description,
        CompassAnalyses::AllocateAndFreeInTheSameModule::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);


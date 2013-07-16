/**
 * \file allocate_and_free_in_the_same_module.cpp
 * \author Sam Kelly
 * \date Tuesday, July 9, 2013
 */

#include "rose.h"
#include <boost/foreach.hpp>

#include "rose.h"
#include "CodeThorn/src/AstMatching.h"
#include "CodeThorn/src/AstTerm.h"
#include "compass2/compass.h"

using std::string;
using namespace StringUtility;
using namespace CodeThorn;

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
 * Implementation
 *
 * 1. Performs an AST matching search for all SgAssignOp's, SgFunctionRefExp's
 *    and SgAssignInitializer's.
 *
 * 2. For the SgAssignOps and SgAssignInitializers, examines the original
 *    variable, and the value being assigned to it, and checks to see if
 *    a malloc() or calloc() function call is being made in the value
 *    being assigned.
 *
 * 3. Examines all pointer assignment statements, to detect scenarios where
 *    a pointer value is passed on to another pointer, so the original pointer
 *    could then be freed from the second pointer. An adjacency list is
 *    constructed (using the var_mappings hash table) to keep track of
 *    this.
 *
 * 4. If a malloc() or calloc() is found, marks the associated symbol table
 *    reference in the var_free_status hash table as false, meaning that
 *    symbol table reference has not been freed yet.
 *
 * 5. For the SgFunctionRefExp's, checks to see if a free() call is being
 *    made, and if so, adds the associated symbol table entry to the
 *    freed hash set.
 *
 * 6. Iterates through the unfreed vector. For each variable, we check to see
 *    if it is in the free hash set (if so we mark its var_free_status as true).
 *    Then we iterate over all variables that might share pointer values with
 *    our current variable by looking at our current variable's entry in the
 *    var_mappings hash table (the inner data type is a hash set). If any of
 *    these is marked as freed in the var_free_status hash table, then we mark
 *    the original variable as also freed.
 *
 *    Structure
 *    -----------------------------------------------------------------------
 *    This checker was organized in such a way that only one global match
 *    on the entire AST is required. Thus the outer-most for-each loop
 *    is actually tri-purpose: it is responsible for handling SgAssignOp's,
 *    SgAssignInitializer's, and SgFunctionRefExp's, which can make the code
 *    a little bit confusing to follow.
 *
 *    Technical Considerations:
 *    -----------------------------------------------------------------------
 *    This approach will catch the vast majority of cases where a programmer
 *    fails to free a malloc()'ed variable. Because of the nature of this
 *    approach, there shouldn't be any false positives (the checker should
 *    never report a failure that is not in fact a legitimate failure),
 *    however, there is one known (and very rare) scenario where this approach
 *    will miss. For now, this checker also does not properly handle cases
 *    where a malloc or free occurs during a separate function call, or in
 *    a different scope.
 *---------------------------------------------------------------------------*/

// TODO: add additional processing to detect if malloc'ed pointers are ever
//       assigned to different variables and then freed from those variables

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
  //std::vector<SgSymbol*> freed;
  boost::unordered_set<SgSymbol*> freed;

  // keeps track of the dynamically allocated variables in the
  // symbol table
  std::vector<SgSymbol*> unfreed;

  // for each pointer variable, tracks the list of all other pointer
  // variables in the program that are ever set to the value (and are thus
  // associated with) this pointer variable
  boost::unordered_map<SgSymbol*, boost::unordered_set<SgSymbol*> > var_mappings;
  std::vector<SgSymbol*> var_mappings_list;

  AstMatching main_matcher;
  // do a global search for SgAssignOp's and SgFunctionRefExp's
  MatchResult main_matches = main_matcher
      .performMatching("$a=SgAssignOp|$f=SgFunctionRefExp|$i=SgAssignInitializer", root_node);

  BOOST_FOREACH(SingleMatchVarBindings match, main_matches)
  {
    SgAssignOp *assign_op = isSgAssignOp(match["$a"]);
    SgAssignInitializer *assign_init = isSgAssignInitializer(match["$i"]);
    SgFunctionRefExp *func_ref = isSgFunctionRefExp(match["$f"]);

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
        if(var_name == NULL)
        {
          // skip if there is no SgInitializedName parent
          continue;
        } else {
          LHS_symbol = var_name->get_symbol_from_symbol_table();
          ROSE_ASSERT(LHS_symbol != NULL);
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
        ROSE_ASSERT (var_ref_matches.size() > 0);

        SgVarRefExp *LHS = (SgVarRefExp*)(var_ref_matches.back()["$r"]);
        ROSE_ASSERT(LHS != NULL);

        LHS_symbol = LHS->get_symbol();
        ROSE_ASSERT(LHS_symbol != NULL);
      }

      // build initial variable mappings
      if(SageInterface::isPointerType(LHS_symbol->get_type())
      && var_mappings.find(LHS_symbol) == var_mappings.end())
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
          SgSymbol *RHS_symbol =
              ((SgVarRefExp*)RHS_var_matches.back()["$r"])->get_symbol();
      } else {
        RHS_symbol = RHS_var_ref->get_symbol();
      }
      if(RHS_symbol != NULL && LHS_symbol != NULL && LHS_symbol != RHS_symbol
          && SageInterface::isPointerType(RHS_symbol->get_type())
          && SageInterface::isPointerType(LHS_symbol->get_type()))
      {

        /*// defensive -- should never actually happen
        if(var_mappings.find(RHS_symbol) == var_mappings.end())
        {
          boost::unordered_set<SgSymbol*> new_set;
          var_mappings[RHS_symbol] = new_set;
          var_mappings_list.push_back(RHS_symbol);
        }*/
        // add the LHS to the associated list for the RHS
        var_mappings[RHS_symbol].insert(LHS_symbol);
      }


      AstMatching malloc_matcher;
      MatchResult malloc_matches = malloc_matcher
          .performMatching("$m=SgFunctionRefExp", assign_RHS);

      if(malloc_matches.size() > 0)
      {
        SgFunctionRefExp *matched_func_ref =
            isSgFunctionRefExp(malloc_matches.front()["$m"]);
        ROSE_ASSERT(matched_func_ref != NULL);

        SgSymbol* symbol = matched_func_ref->get_symbol();
        ROSE_ASSERT(symbol != NULL);

        std::string func_str = symbol->get_name().getString();
        if(func_str.compare("malloc") == 0 || func_str.compare("calloc") == 0)
        {
          // mark LHS as not freed
          var_free_status[LHS_symbol] = false;
          malloc_nodes[LHS_symbol] = matched_func_ref;
          unfreed.push_back(LHS_symbol);
        }
      } else {
        continue;
      }
    }
    else if(func_ref)
    {
      // handle SgFunctionRefExp
      std::string func_str = func_ref->get_symbol()->get_name().getString();
      SgFunctionCallExp* func_call = (SgFunctionCallExp*)func_ref->get_parent();
      if(func_str.compare("free") == 0)
      {
        AstMatching var_matcher;
        SgVarRefExp *var_ref = isSgVarRefExp(var_matcher
                                             .performMatching("$r=SgVarRefExp", func_call)
                                             .back()["$r"]);
        if(var_ref)
        {
          SgSymbol *var_ref_symbol = var_ref->get_symbol();
          //freed.push_back(var_ref_symbol);
          freed.insert(var_ref_symbol);
        }
      } else {

        SgExprListExp *expr_list = func_call->get_args();

        for(int i = 0; i < expr_list->get_numberOfTraversalSuccessors(); i++)
        {
          SgNode* param = expr_list->get_traversalSuccessorByIndex(i);

          SgFunctionParameterList *func_params = func_call
              ->getAssociatedFunctionDeclaration()->get_parameterList();
          SgInitializedName* func_param =
              isSgInitializedName(func_params->get_traversalSuccessorByIndex(i));
          //if(func_param == NULL) break;
          SgSymbol *func_param_sym = NULL;
          std::cout << " HAHAHAHA" << std::endl;
         // std::cout << func_param_sym << std::endl;
          SgVarRefExp* param_var = isSgVarRefExp(param);
          if(param_var != NULL)
          {

            // handles inter function mappings of variables here
            SgSymbol *param_sym = param_var->get_symbol();
            // now we must add var mappings from the inner parameter of the function
            // to the original passed variable
            //if(func_param_sym == param_sym) break;
            if(func_param_sym == NULL) std::cout << " OH NO " << std::endl;
            var_mappings[func_param_sym].insert(param_sym);
            //std::cout << func_param_sym << " = " << param_sym << std::endl;
            //std::cout << func_param_sym->get_name() << " = " << param_sym->get_name() << std::endl;
          } else {
            //TODO: handle case of malloc being passed as function parameter
            AstMatching func_ref_matcher;
            MatchResult found_func_matches =
                func_ref_matcher.performMatching("$r=SgFunctionRefExp", param);
            if(found_func_matches.size() > 0)
            {
              SgFunctionRefExp *found_func = isSgFunctionRefExp(
                  found_func_matches.front()["$r"]);
              std::string found_func_str = found_func->get_symbol()
                  ->get_name().getString();
              std::cout << "found function call in function param!" << std::endl;
              if(found_func_str.compare("malloc") == 0)
              {
                // found a malloc call within a function parameter
                // we must map this to whatever variable it would be assigned in the
                // parent function
                //var_free_status[func_param_sym] = false;
                //malloc_nodes[func_param_sym] = found_func->get_symbol();
                //unfreed.push_back(func_param_sym);

              }
            }
          }
        }
      }
    } else {
      //TODO: handle case of malloc in an SgReturnStmt
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
      for(boost::unordered_set<SgSymbol*>::iterator itr = var_mappings[key_sym].begin();
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
  /*BOOST_FOREACH(SgSymbol *key_sym, var_mappings_list)
  {
    std::string RHS_name = key_sym->get_name();
    std::cout << "RHS_NAME:" << RHS_name << std::endl;
    for(boost::unordered_set<SgSymbol*>::iterator itr = var_mappings[key_sym].begin();
        itr != var_mappings[key_sym].end(); ++itr)
    {
      SgSymbol* val_sym = *itr;
      std::string LHS_name = val_sym->get_name();
      std::cout << " |--" << LHS_name << std::endl;
    }
  }*/

  BOOST_FOREACH(SgSymbol *key_sym, unfreed)
  {
    std::string key_sym_name = key_sym->get_name();
    if(freed.find(key_sym) != freed.end())
    {
      std::cout << "MARKING "<< key_sym_name << " [ALMOST DEFINITELY FREE]" << std::endl;
      var_free_status[key_sym] = true;
    }
    for(boost::unordered_set<SgSymbol*>::iterator itr = var_mappings[key_sym].begin();
        itr != var_mappings[key_sym].end(); ++itr)
    {
      SgSymbol* val_sym = *itr;
      if(freed.find(val_sym) != freed.end())
      {
        var_free_status[key_sym] = true;
        std::cout << "MARKING "<< key_sym_name << " [PROBABLY FREE] (associated variable "
            << val_sym->get_name() << " marked as FREE)" << std::endl;
      }
    }
  }

  BOOST_FOREACH(SgSymbol *var_symbol, unfreed)
  {
    if(var_free_status[var_symbol] == false)
    {
      std::cout << "MARKING " << var_symbol->get_name() << " [NOT FREE]!" << std::endl;
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


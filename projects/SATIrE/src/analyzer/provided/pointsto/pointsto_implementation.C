#include <memory>
#include <boost/pending/disjoint_sets.hpp>
// Relevant functions provided by Boost's disjoint sets data structure:
// make_set, find_set, union_set, link; link is on class representatives,
// union_set on any members of the respective classes

#include "pointsto.h"
#include "pointsto_implementation.h"

// Use these macros to control how much debug output is printed.
#define DEBUG 1
// #define VERBOSE_DEBUG 1

namespace SATIrE {

namespace Analyses {

// ========== PointsToAnalysis::Location members ==========

PointsToAnalysis::Location::Location(
        PointsToAnalysis::Location *b, PointsToAnalysis::Location *r)
  : base_location(b), ellipsis_location(NULL), return_location(r),
    dummy(false), literal(false), special_function(false),
    first_access_type(NULL), collapsed(false), parent_struct(NULL),
    collapseBase(false), mayBeAliased(false), array(false),
    functionSummary(false), ownerInfo(NULL)
{
}

void
PointsToAnalysis::Location::pointTo(Location *base)
{
#if VERBOSE_DEBUG
    std::cout
        << (base_location == NULL ? "new" : "MOD") << " "
        << (void *) this << "/" << this->id
        << " -> "
        << (void *) base << "/" << base->id
        << std::endl;
#endif
    base_location = base;
    base_location->mayBeAliased = true;
 // if (base_location->id == 21) std::abort();
}

PointsToAnalysis::Location *
PointsToAnalysis::Location::baseLocation() const
{
    return base_location;
}


// ========== useful function templates ==========

namespace Utils
{
 // Get a std::string from some ROSE class that has a get_name() method.
    template <class NamedThing>
    std::string name(NamedThing *thing)
    {
        if (thing != NULL)
            return thing->get_name().str();
        else
            return "<null>";
    }

 // Get a std::string from some ROSE class that has a get_mangled_name()
 // method.
    template <class NamedThing>
    std::string mangled_name(NamedThing *thing)
    {
        if (thing != NULL)
            return thing->get_mangled_name().str();
        else
            return "<null>";
    }

 // Get a comma-separated list of names out of a container of named things.
    template <class NamedThingContainer>
    std::string names(NamedThingContainer &things)
    {
        std::stringstream result;
        typename NamedThingContainer::iterator i;
        i = things.begin();
        if (i != things.end())
        {
            result << name(*i++);
            while (i != things.end())
                result << ", " << name(*i++);
        }
        return result.str();
    }

 // Pop a container, by which we mean removing the first element and
 // returning its value. This is to wrap the annoying (though sort of
 // sensible, separating queries from operations) STL interface
 // that requires two function calls for this operation.
    template <class FrontPopContainer>
    typename FrontPopContainer::value_type
    pop_front(FrontPopContainer &container)
    {
        typename FrontPopContainer::value_type v;
        v = container.front();
        container.pop_front();
        return v;
    }
} // namespace Utils

// These utilities are moved off into their separate namespace, but the
// using declaration ensures that we can use them without qualification as
// long as they can be uniquely resolved.
using namespace Utils;


// ========== PointsToAnalysis::Implementation members ==========

void
PointsToAnalysis::Implementation::atIcfgTraversalStart()
{
 // Create function summary node for main info. This function has a single
 // ellipsis location, which forces all of its arguments to be aliased.
 // Also, we make sure its return location is aliased to its argument.
    Location *fsn = createFunctionLocation();
    fsn->functionSummary = true;
    fsn->arg_locations.push_back(createLocation());
    fsn->ellipsis_location = fsn->arg_locations.front();
    assign(fsn->return_location, fsn->ellipsis_location);
    mainInfo->functionSummaryNode = createLocation(fsn);

    CFG *icfg = icfgTraversal->get_icfg();
    std::deque<Procedure *> &procedures = *icfg->procedures;
    size_t procs = procedures.size();
    mainInfo->procedureLocations = std::vector<Location *>(procs, NULL);
    size_t l;
    for (l = 0; l < procs; ++l)
    {
     // Create a location for the function, before any function bodies are
     // traversed. We do want to have correctly named argument locations, so
     // we traverse the function declaration's parameter list here to
     // collect a dummy location containing the args.
        SgFunctionParameterList *params
            = procedures[l]->decl->get_parameterList();

     // First, save each function globally.
        info = mainInfo;
        Location *argDummy
            = AstBottomUpProcessing<Location *>::traverse(params);
        mainInfo->procedureLocations[l]
            = function_location(procedures[l]->decl, argDummy);
     // freeDummyLocation(argDummy);

#if HAVE_PAG
     // Then, decorate each info with its very own copy of its function.
        if (contextSensitive)
        {
            int arity = kfg_arity_id(kfg_get_id(icfg, kfg_numproc(icfg, l)));
            for (int p = 0; p < arity; p++)
            {
                ContextInformation::Context context(l, p, icfg);
                info = allInfos[context];
                info->context = new ContextInformation::Context(context);
                Location *argDummy
                    = AstBottomUpProcessing<Location *>::traverse(params);
                info->procedureLocation
                    = function_location(procedures[l]->decl, argDummy);
             // freeDummyLocation(argDummy);
            }
        }
#endif
    }
}

void
PointsToAnalysis::Implementation::icfgVisit(SgNode *node)
{
 // If this is not the context-sensitive analysis, we simply traverse the
 // statement.
    if (!contextSensitive)
    {
        AstBottomUpProcessing<Location *>::traverse(node);
    }
 // Otherwise, if this is a statement node, we loop through all contexts for
 // this ICFG node and traverse the statement once in each context.
    else if (is_icfg_statement())
    {
        int id = get_node_id();
        int procnum = get_node_procnum();
#if HAVE_PAG
        int arity = kfg_arity_id(id);
        for (int position = 0; position < arity; position++)
        {
            info = allInfos[
                ContextInformation::Context(procnum, position, get_icfg())];
            AstBottomUpProcessing<Location *>::traverse(node);
        }
#endif
    }
 // Yet otherwise, this is some global stuff; traverse in the main context.
    else
    {
        info = mainInfo;
        AstBottomUpProcessing<Location *>::traverse(node);
    }
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::evaluateSynthesizedAttribute(
        SgNode *node,
        PointsToAnalysis::Implementation::SynthesizedAttributesList synlist)
{
    Location *result = NULL;
    Location *a, *b;
    SynthesizedAttributesList::iterator syn;
    std::vector<Location *>::iterator s, t;
 // Access to the current ICFG node. Note that we also traverse some stuff
 // that is not within ICFG nodes, such as global initializers. So this
 // requires some care.
    CFG *icfg = icfgTraversal->get_icfg();
    BasicBlock *icfgNode = NULL;
    if (icfgTraversal->is_icfg_statement())
        icfgNode = icfg->nodes[icfgTraversal->get_node_id()];

#if VERBOSE_DEBUG
 // Display the node type and its synthesized attributes.
    std::cout
        << node->class_name()
        << " [";
    SynthesizedAttributesList::iterator sali;
    for (sali = synlist.begin(); sali != synlist.end(); ++sali)
    {
        if (sali != synlist.begin())
            std::cout << ", ";
        std::cout << (void *) *sali;
    }
    std::cout
        << "]" << std::endl;
#endif

    switch (node->variantT())
    {
    case V_SgVarRefExp:
     // If this variable is an object reference by itself, i.e., not a
     // struct member reference, look up the variable's location, creating one
     // if necessary. Otherwise, we do not return anything yet; the parent
     // expression, which is a dot or arrow expression, will take care of
     // computing the location of the complete expression.
        if (!isStructMemberReference(isSgVarRefExp(node)))
        {
         // Looking up a variable's location is somewhat complicated because
         // the ICFG contains some variables that need special handling. For
         // return variables, we do not want the retvar's location; rather,
         // we need to look up the called function's return location.
            SgVarRefExp *varRef = isSgVarRefExp(node);
            SgSymbol *varSym = varRef->get_symbol();
            if (varSym == icfgTraversal->get_icfg()
                                       ->global_return_variable_symbol)
            {
             // The global return variable: Look up the call target's return
             // location.
             // a = expressionLocation(icfgNode->call_target);
             // result = a->baseLocation()->return_location;
             // The above lookup only works when invoked during an ICFG
             // traversal, but not when this method is invoked via the PAG
             // support function exprid_location. So we had to move to an
             // attribute-based solution as for the local return variables.
             // Which means that this case and the next should be merged as
             // soon as this is debugged.
#if VERBOSE_DEBUG
                std::cout
                    << "--- looking at global return variable in context "
                    << info->prefix << std::endl;
#endif
                if (!contextSensitive)
                {
                    CallAttribute *ca;
                    assert(varRef->attributeExists("SATIrE: call target"));
                    ca = (CallAttribute *) varRef->getAttribute(
                                                   "SATIrE: call target");
                    a = expressionLocation(ca->call_target);
                    result = a->baseLocation()->return_location;
                }
                else
                {
#if HAVE_PAG
                 // node->isReturnStmt
                 // In the very special case that this is a
                 // context-sensitive analysis and we are in the middle of
                 // an icfg traversal and the current ICFG node represents a
                 // return statement in the current function, the global
                 // return variable symbol's location is simply the return
                 // location of the current context's procedure location.
                    CFG *icfg = icfgTraversal->get_icfg();
                    if (icfgTraversal->is_icfg_statement()
                        && icfg->nodes[icfgTraversal->get_node_id()]
                               ->isReturnStmt)
                    {
                        result = allInfos[*info->context]->procedureLocation
                                                         ->baseLocation()
                                                         ->return_location;
                    }
                 // Otherwise, this must be a ReturnAssignment statement if
                 // it refers to the global retvar. We don't care about this
                 // case, as we don't care about ReturnAssignment at all.
#endif
                }
            }
            else if (varRef->attributeExists("SATIrE: call target"))
            {
             // A call-site-specific local return variable: Look up the call
             // target's return location.
#if VERBOSE_DEBUG
                std::cout
                    << "--- looking at local return variable in context "
                    << info->prefix << std::endl;
#endif
                if (!contextSensitive)
                {
                    CallAttribute *ca;
                    ca = (CallAttribute *) varRef->getAttribute(
                                                   "SATIrE: call target");
                    a = expressionLocation(ca->call_target);
                    result = a->baseLocation()->return_location;
                }
                else
                {
#if HAVE_PAG
                 // In the context-sensitive case, we need to find all
                 // possible called contexts and look at their return
                 // locations. If there is more than one candidate, we need
                 // to merge return locations! However, this should be rare
                 // (hopefully).
                    std::vector<ContextInformation::Context>::const_iterator
                        beg, end, ctx;
                    CallAttribute *ca;
                    ca = (CallAttribute *) varRef->getAttribute(
                                                    "SATIrE: call target");
                    BasicBlock *call_block
                        = icfg->call_target_call_block[ca->call_target];
                    int call_id = call_block->id;
                 // See if the call block has outgoing call edges; if yes,
                 // we can fetch information from the call's child context.
                 // Otherwise, it's an external function to a (hopefully)
                 // special function.
                    if (kfg_which_out_edges(call_block) & (1U << CALL_EDGE))
                    {
                        beg = icfg->contextInformation->childContexts(
                                call_id, *info->context).begin();
                        end = icfg->contextInformation->childContexts(
                                call_id, *info->context).end();
                        result = allInfos[*beg]->procedureLocation
                                           ->baseLocation()->return_location;
                        for (ctx = beg + 1; ctx != end; ++ctx)
                        {
                            Location *a = allInfos[*ctx]->procedureLocation;
                            a = a->baseLocation()->return_location;
                            result = assign(result, a);
                        }
                    }
                    else
                    {
                        Location *fp = expressionLocation(ca->call_target);
                        result = fp->baseLocation()->return_location;
#if VERBOSE_DEBUG
                        std::cout
                            << "fp: " << fp->id
                            << " funcloc: " << fp->baseLocation()->id
                            << " result: " << result->id
                            << std::endl;
#endif
                    }
#endif
                }
            }
            else
            {
             // Unattributed variable reference. Ideally, these should just
             // be the original program variables.
                std::string symnam = Utils::name(varSym);
                if (symnam.find("$tmpvar$") != std::string::npos)
                {
                 // So we found a tmpvar somewhere. Quick tests suggest that
                 // this only happens in ParamAssignments (which we ignore
                 // altogether) and in ArgumentAssignments (where we ignore
                 // the lhs operand). So we should be fine simply doing
                 // nothing and returning NULL; this value will never be
                 // looked at. Hopefully.
#if 0
                    std::cout
                        << "strange: identified variable '"
                        << symnam
                        << "' (location " << result->id << ")!"
                        << std::endl;
                    if (icfgNode != NULL)
                    {
                        SgStatement *stmt = icfgNode->statements.front();
                        std::cout
                            << "in statement: "
                            << Ir::fragmentToString(stmt)
                            << " (" << stmt->class_name() << ")"
                            << std::endl;
                    }
                    else
                    {
                        std::cout
                            << "not in the ICFG, which is weird"
                            << std::endl;
                    }
#endif
                }
                else
                {
                 // This is the normal case of a variable that occurred in
                 // the original program.
                    result = symbol_location(varSym);
                 // Need to check whether this is an array symbol, and
                 // whether it is initialized to point to an array location
                 // yet.
                    if (result->baseLocation() == NULL)
                    {
                        SgType *t = varSym->get_type();
                        t = t->stripType(SgType::STRIP_MODIFIER_TYPE
                                       | SgType::STRIP_REFERENCE_TYPE
                                       | SgType::STRIP_TYPEDEF_TYPE);
                        if (isSgArrayType(t))
                        {
                            Location *arrayNode = createLocation();
                            arrayNode->array = true;
                            arrayNode->symbols = result->symbols;
                            result->pointTo(arrayNode);
                        }
                    }
                }
            }
        }
        else
            result = NULL;

#if VERBOSE_DEBUG
        std::cout
            << "V_SgVarRefExp: for symbol "
            << name(isSgVarRefExp(node)->get_symbol())
            << ": returning result ptr "
            << (void *) result << std::flush;
        std::cout
            << ", id = "
            << (result != NULL ? result->id : 0)
            << std::endl;
#endif

        break;

    case V_SgFunctionRefExp:
     // Look up the function's location, creating one if necessary.
     // This lookup always happens in the main info as function symbols are
     // sort of global.
#if VERBOSE_DEBUG
        std::cout
            << "function ref to func named: "
            << name(isSgFunctionRefExp(node)->get_symbol())
            << std::endl;
#endif
        if (mainInfo->specialFunctionNames.find(
                    name(isSgFunctionRefExp(node)->get_symbol()))
            != mainInfo->specialFunctionNames.end())
        {
            std::string specialFuncName
                = name(isSgFunctionRefExp(node)->get_symbol());
            size_t arity = isSgFunctionDeclaration(
                                isSgFunctionRefExp(node)
                                    ->get_symbol()->get_declaration())
                            ->get_args().size();
#if VERBOSE_DEBUG
            std::cout
                << "special function name: " << specialFuncName
                << " arity: " << arity
                << " in context: "
                << (info->context != NULL
                        ? info->context->toString() : "<none>")
                << " in node: " << icfgNode->id
                << " @" << (void *) node
                << std::endl;
#endif
            result = specialFunctionLocation(node, specialFuncName, arity);
        }
        else
        {
            PointsToInformation *oldInfo = info;
            info = mainInfo;
            result = functionSymbol_location(
                    isSgFunctionRefExp(node)->get_symbol());
#if VERBOSE_DEBUG
            std::cout
                << "for function "
                << name(isSgFunctionRefExp(node)->get_symbol())
                << ": symbol location = " << (void *) result
                << std::endl;
#endif
            info = oldInfo;
        }
     // TODO: can this symbol ever be NULL? can this lookup ever be tried
     // for some function declaration whose symbol was NULL, and fail?
        if (result->baseLocation()->return_location == NULL)
        {
#if VERBOSE_DEBUG
         // The return location is NULL for some special function summaries.
         // Not sure how much sense it makes to create an artificial return
         // location here, but it won't hurt.
            std::cout
                << "NULL return location for func ref! id: " << result->id
                << " name: " << name(isSgFunctionRefExp(node)->get_symbol())
                << std::endl;
#endif
            result->baseLocation()->return_location = createLocation();
        }
#if VERBOSE_DEBUG
        if (SgFunctionRefExp *ref = isSgFunctionRefExp(node))
        {
            std::string name = Utils::name(ref->get_symbol());
            std::string mname
                = mangled_name(ref->get_symbol()->get_declaration());
            if (name.find("yy_") != std::string::npos)
            {
                Sg_File_Info *finf = ref->get_file_info();
                std::cout
                    << "found function ref to: "
                    << name << "/" << mname
                    << " at: "
                    << finf->get_filenameString()
                    << ":" << finf->get_line()
                    << std::endl;
            }
        }
#endif
        break;

    case V_SgAddressOfOp:
     // Create a new pointer location with the synthesized location as base
     // location.
     // result = createType(synlist[SgAddressOfOp_operand_i]);
     // Note that the location created here will always be lost, or used in
     // an assignment; however, when used in an assignment, at most its base
     // location will be copied, nobody will ever keep a pointer to this
     // location itself. (I think!) Thus this can be a dummy node that is
     // freed after the one assignment it is involved in.
     // GB (2009-04-12): Nope, a dummy won't always do.
     // result = createDummyLocation(synlist[SgAddressOfOp_operand_i]);
        result = createLocation(synlist[SgAddressOfOp_operand_i]);
        break;

    case V_SgPointerDerefExp:
     // For a pointer dereference, we basically follow the synthesized
     // locations's base locations pointer. If it does not have a base
     // location, we create one. However, this is only done for data
     // pointers: Calls through function pointers also have a dereference
     // node around the pointer, but this kind of dereference is handled by
     // the code for SgFunctionCallExp.
        a = synlist[SgPointerDerefExp_operand_i];
        if (a == NULL)
        {
            std::cerr
                << "error: pointer dereference with NULL base location!"
                << std::endl;
            debugDump(isSgPointerDerefExp(node));
        }
#if VERBOSE_DEBUG
        std::cout
            << "* dereferencing " << a->id << " ("
            << (a->baseLocation() != NULL ? a->baseLocation()->id : 0)
            << ")"
            << " expr: " << Ir::fragmentToString(node)
            << " in context " << (info->context != NULL ?
                                    info->context->toString() : "<none>")
            << std::endl;
#endif
     // Only dereference the pointer (ensuring it has a base location) if it
     // is not a function pointer used in a call.
        if (!isSgFunctionCallExp(node->get_parent())
         || isSgFunctionCallExp(node->get_parent())->get_function() != node)
        {
         // when dereferencing a data pointer, make sure that it is indeed a
         // pointer location, i.e., that it has a base location
            if (a->baseLocation() == NULL)
                a->pointTo(createLocation(NULL));
         // force pending joins: the dereference operation tells us that
         // this is indeed a pointer, so we must unify conditionally
         // assigned locations
            join_pending_locations(a);
         // Check or set the first_access_type for the base node, collapsing
         // it if necessary.
#if 0
            SgType *access_type
                = isSgPointerDerefExp(node)->get_operand()
                    ->get_type()->findBaseType();
#endif
            SgType *access_type = stripOnePointerLayer(
                    isSgPointerDerefExp(node)->get_operand()->get_type());
            checkOrSetFirstAccessType(a->baseLocation(), access_type);
            if (SgClassType *class_type = isSgClassType(access_type))
                materializeAllStructMembers(a->baseLocation(), class_type);

            if (a->collapseBase)
                collapseStructure(a->baseLocation());
            result = a->baseLocation();
        }
        else
        {
         // do not dereference function pointers in calls; the handling of
         // the function call node takes care of dereferencing the pointer
         // it gets
            result = a;
        }
#if VERBOSE_DEBUG
        std::cout
            << "result of dereference: " << result->id
            << " [" << names(result->symbols) << "]"
            << std::endl;
#endif
        break;

    case V_SgPntrArrRefExp:
     // This case is a little simpler than the pointer deref expression, as
     // we don't need to worry about function locations, I think; however,
     // there is the small complication that a[42] is the same as 42[a],
     // which is weird and very unusual, but legal; thus we need to make
     // sure that we actually know which of the operands is of a C pointer
     // type---it should be exactly one of the two operands once we know
     // which of the two is the array/pointer, simply dereference that; we
     // treat the array as a single block of memory.
        a = synlist[SgPntrArrRefExp_lhs_operand_i];
        b = synlist[SgPntrArrRefExp_rhs_operand_i];
        result = pickThePointer(isSgPntrArrRefExp(node), a, b);
        if (result == NULL)
        {
#if DEBUG
            std::cout
                << "panic! found array expression without any ptr or array"
                << std::endl;
            debugDump(node);
#endif
            result = createLocation();
        }
        if (result->baseLocation() == NULL)
        {
#if VERBOSE_DEBUG
            std::cout
                << "base location NULL in array ref exp on location "
                << result->id << std::endl;
#endif
            result->pointTo(createLocation());
        }
     // This is a dereference, so pending locations must be joined.
        join_pending_locations(result);
     // We need to check the access type, which is complicated by the fact
     // that we don't know which of the operands is actually being
     // dereferenced; but we can recover that information by comparing
     // result to a and b.
        {
            SgExpression *operand = NULL;
            if (result == a)
                operand = isSgPntrArrRefExp(node)->get_lhs_operand();
            else if (result == b)
                operand = isSgPntrArrRefExp(node)->get_rhs_operand();
         // SgType *access_type = operand->get_type()->findBaseType();
            SgType *access_type = stripOnePointerLayer(operand->get_type());
            checkOrSetFirstAccessType(result->baseLocation(), access_type);
            if (SgClassType *class_type = isSgClassType(access_type))
            {
                materializeAllStructMembers(result->baseLocation(),
                                            class_type);
            }
        }

        if (result->collapseBase)
            collapseStructure(result->baseLocation());
        result = result->baseLocation();
        break;

    case V_SgAddOp:
    case V_SgSubtractOp:
    case V_SgMultiplyOp:
    case V_SgDivideOp:
    case V_SgModOp:
    case V_SgRshiftOp:
    case V_SgLshiftOp:
    case V_SgAndOp:
    case V_SgOrOp:
    case V_SgBitAndOp:
    case V_SgBitOrOp:
    case V_SgBitXorOp:
     // We handle binary arithmetic operations uniformly. Some of these --
     // addition and subtraction -- are routinely applied to pointers,
     // others require casts at some point. This means that we might be
     // confronted with data that contains pointer *values* in both
     // operands even where language rules do not allow two pointer *types*
     // for the operands.
     // Additionally, both original pointers can usually be recovered given
     // one of the operands and the result; so if at least one of the
     // operands is a pointer, the result must be one as well. This means
     // that, if both operands are pointer locations, we unify them and use
     // the unified location as the result.
     // Note that normal pointer arithmetic, which is probably the main use
     // of these operators on pointers, is only defined in Standard C in
     // cases where both operands are pointers into the same object, or
     // where a pointer inside the same object as one of the operands is
     // computed. In those cases, no precision will be lost by unifying a
     // location with itself. On programs that do weird casting *and*
     // arithmetic on pointers, we will lose some precision, but that's
     // really not our fault.
     // If at most one of the operands is a pointer, we use pickThePointer
     // to return the corresponding location -- this assumes that the pointer
     // arithmetic does not compute an address outside the pointed-to
     // object.
        a = synlist[SgBinaryOp_lhs_operand_i];
        b = synlist[SgBinaryOp_rhs_operand_i];
        if (a != NULL && a->baseLocation() != NULL
         && b != NULL && b->baseLocation() != NULL)
            result = assign(a, b);
        else
            result = pickThePointer(isSgBinaryOp(node), a, b);
        break;

    case V_SgPlusAssignOp:
    case V_SgMinusAssignOp:
    case V_SgMultAssignOp:
    case V_SgDivAssignOp:
    case V_SgModAssignOp:
    case V_SgAndAssignOp:
    case V_SgIorAssignOp:
    case V_SgXorAssignOp:
    case V_SgLshiftAssignOp:
    case V_SgRshiftAssignOp:
     // Binary op= operators are similar to binary arithmetic ops, but not
     // quite identical. In addition to evaluating the binary operation, we
     // must perform an assignment to the lhs operand. In usual cases, the
     // pointer, if any, will be the lhs operand, so no precision is lost.
        a = synlist[SgBinaryOp_lhs_operand_i];
        b = synlist[SgBinaryOp_rhs_operand_i];
        if (a != NULL && a->baseLocation() != NULL
         && b != NULL && b->baseLocation() != NULL)
            result = assign(a, b);
        else
            result = assign(a, pickThePointer(isSgBinaryOp(node), a, b));
        break;

    case V_SgPlusPlusOp:
    case V_SgMinusMinusOp:
     // the pointer still points into the same area it pointed to before, so
     // return that area's location
        result = synlist[SgUnaryOp_operand_i];
        break;

    case V_SgUnaryAddOp:
    case V_SgMinusOp:
    case V_SgBitComplementOp:
     // Whether the first operand is a pointer or not, its location is
     // preserved.
        result = synlist[SgUnaryOp_operand_i];
        break;

    case V_SgDotExp:
        a = synlist[SgDotExp_lhs_operand_i];
     // The rhs location is either present as one of a's members, or it must
     // be created from scratch. The structMemberLocation function takes
     // care of checking the access type.
        b = structMemberLocation(a,
                isSgDotExp(node)->get_lhs_operand(),
                isSgVarRefExp(isSgDotExp(node)->get_rhs_operand()));
        result = b;
        break;

    case V_SgArrowExp:
        a = synlist[SgArrowExp_lhs_operand_i];
     // a is supposed to be a pointer location.
        if (a->baseLocation() == NULL)
            a->pointTo(createLocation());
        if (a->collapseBase)
            collapseStructure(a->baseLocation());
     // Dereference.
        a = a->baseLocation();
     // The rhs location is either present as one of a's members, or it must
     // be created from scratch. The structMemberLocation function
     // materializes the structure members if necessary and returns the
     // location of the member we are looking for. It also takes care of
     // checking the access type.
        b = structMemberLocation(a,
                isSgArrowExp(node)->get_lhs_operand(),
                isSgVarRefExp(isSgArrowExp(node)->get_rhs_operand()));
        result = b;
#if DEBUG
        if (result == NULL)
        {
            std::cout
                << "*** panic: arrow exp returning NULL!"
                << std::endl;
        }
#endif
        break;

    case V_SgConditionalExp:
     // unify the two arms of the conditional
        a = synlist[SgConditionalExp_true_exp];
        b = synlist[SgConditionalExp_false_exp];
        result = (a != NULL || b != NULL ? assign(a, b) : b);
        break;

    case V_SgCastExp:
     // Casts are mostly irrelevant to the points-to analysis as any
     // programming language type might contain a pointer value. We do treat
     // a few cases specially, however: We create "integer location" nodes for
     // casts from integer constant expressions to pointer types; further,
     // we collapse any structure if its address is cast to an incompatible
     // pointer type, as this suggests that something fishy is planned with
     // the pointer.
     // TODO: Review the rest of this comment in the light of the new
     // structure type handling.
     // We must also be careful with casts from some incompatible type *to*
     // a structure type. However, we cannot collapse too early, as dynamic
     // memory allocation using malloc would then always create collapsed
     // structures. We only want these to be collapsed if the original
     // pointer is used to access the structure's representation.
        result = synlist[SgCastExp_operand_i];
        if (SgCastExp *cast = isSgCastExp(node))
        {
            if (isSgPointerType(cast->get_type())
             && cast->get_operand()->get_type()->isIntegerType()
             && isSgValueExp(cast->get_operand()))
            {
                if (result == NULL)
                    result = info->integerConstantLocation;
            }

         // Removed this section for now. The new style of handling struct
         // types, where each node is tagged with its access type, should be
         // superior.
#if 0
            else if (isCastFromStructPtrToIncompatibleType(cast))
            {
             // Do not collapse right away because that messes with our
             // handling of special functions: Even though memcpy can have a
             // nice polymorphic summary in our system, conversion of struct
             // pointers to (void *) would cause needless collapsing of the
             // structures.
             // Thus, we do a "pending collapse": We tell the pointer to
             // cause a collapse of the struct it points to only if it is
             // dereferenced. Our function summaries allow us to bypass some
             // function bodies, and thus some critical dereferences.
             // collapseStructure(result->base_type);
                result->collapseBase = true;
            }
            else if (isCastFromIncompatibleTypeToStructPtr(cast))
            {
             // Someone is casting from an incompatible pointer type to a
             // struct pointer. We create a *new* pointer to the struct and
             // set the "collapse base on dereference" flag in the old
             // pointer. Thus, we can use the struct pointer type-safely and
             // without losing precision by collapsing; if the original
             // pointer is used to mess with the struct's representation, we
             // collapse the struct.
             // This does not cover all possible cases; in particular, if
             // several incompatible-type pointers to the struct exist, we
             // can only mark one of them, and all the others can still mess
             // with our struct unnoticed. This could be solved using
             // reference counting, I think: Collapse the struct if there
             // are more pointers to it than you can mark with
             // "collapseBase". For now, I will just live with this problem
             // and assume that it's not relevant in practice.
                a = synlist[SgCastExp_operand_i];
                if (a != NULL)
                {
                 // result = createType(a->base_type);
                    b = createType();
                    result = b;
                    assign(b, a);
                    a->collapseBase = true;
#if VERBOSE_DEBUG
                    std::cout
                        << "created type " << b->id << " for to-struct cast"
                        << std::endl
                        << "result is: " << result->id << std::endl;
#endif
                }
            }
#endif
        }
        break;

    case V_SgAssignOp:
     // the complexity of assignment is handled using a helper function
#if 0
        std::cout
            << "assign op! synlist: length "
            << synlist.size()
            << ": "
            << "["
            << (void *) synlist[0]
            << ", "
            << (void *) synlist[1]
            << "]"
            << std::endl;
#endif
        a = synlist[SgAssignOp_lhs_operand_i];
        b = synlist[SgAssignOp_rhs_operand_i];
#if 0
        std::cout
            << "a = " << (void *) a << ", "
            << "b = " << (void *) b
            << std::endl;
#endif
#if 0
        if (icfgNode != NULL && icfgNode->call_target != NULL)
        {
         // Check if this is an assignment to the global return variable.
         // If it is, we change it into an assignment to the enclosing
         // function's return location.
            SgVarRefExp *lhsVarRef
                = isSgVarRefExp(isSgAssignOp(node)->get_lhs_operand());
            if (lhsVarRef && lhsVarRef->get_symbol()
                    == icfgTraversal->get_icfg()
                                    ->global_return_variable_symbol)
            {
#if DEBUG
                std::cout
                    << "found assignment to global return variable: "
                    << Ir::fragmentToString(node)
                    << std::endl
                    << "for call: "
                    << Ir::fragmentToString(icfgNode->call_target)
                    << std::endl;
                std::cout
                    << "a location is: " << a->id
                    << ": " << names(a->symbols)
                    << std::endl;
#endif
             // Re-route a to refer to the enclosing function's return
             // location.
                a = expressionLocation(icfgNode->call_target);
                a = a->baseLocation()->return_location;
#if DEBUG
                std::cout
                    << "new a location is: " << a->id
                    << ": " << names(a->symbols)
                    << std::endl;
#endif
            }
        }
#endif
#if VERBOSE_DEBUG
        std::cout
            << "assignment: " << Ir::fragmentToString(node)
            << std::endl;
        std::cout << "! assignment to location "
            << (void *) a << std::endl;
        std::cout
            << "lhs type: "
            << isSgAssignOp(node)->get_lhs_operand()->class_name()
            << std::endl;
#endif
#if VERBOSE_DEBUG
        std::cout << "*-> assignment: " << (a != NULL ? a->id : 0);
        if (a != NULL && a->baseLocation() != NULL)
            std::cout << " (" << a->baseLocation()->id << ")";
        std::cout << " := ";
        std::cout << (b != NULL ? b->id : 0);
        if (b != NULL && b->baseLocation() != NULL)
            std::cout << " (" << b->baseLocation()->id << ")";
        std::cout << std::endl;
#endif
     // This used to yield a if b was NULL. That's nonsense, however: The
     // value of an assignment is certainly not the lhs lvalue!
        result = (b != NULL ? assign(a, b) : NULL);
        break;

    case V_SgInitializedName:
     // Initialized names must be associated with their initializer values,
     // which is sort of an assignment. However, struct members cannot and
     // should not be initialized at the point of the structure definition,
     // so only deal with variable declarations outside of structs. (We only
     // want to create nodes for struct members at the first point a struct
     // is used, not where the struct is defined.)
        result = NULL;
        if (!isSgClassDefinition(node->get_parent()->get_parent()))
        {
            SgSymbol *sym
                = isSgInitializedName(node)->get_symbol_from_symbol_table();
            b = synlist[SgInitializedName_initptr];
            if (sym == NULL && b != NULL)
                sym = findSymbolForInitializedName(
                        isSgInitializedName(node));
         // If there is an initializer, we perform an assignment to the
         // initialized name's location; otherwise, that location is the
         // result.
            if (b != NULL)
            {
                a = symbol_location(sym);
#if VERBOSE_DEBUG
                std::cout << "initialization: " << a->id;
                if (a->baseLocation() != NULL)
                    std::cout << " (" << a->baseLocation()->id << ")";
                std::cout << " := ";
                std::cout << (b != NULL ? b->id : 0);
                if (b != NULL && b->baseLocation() != NULL)
                    std::cout << " (" << b->baseLocation()->id << ")";
                std::cout << std::endl;
                std::cout << "symbol is: "
                    << (void *) sym << " "
                    << (sym != NULL ? name(sym) : "<no name>") << " "
                    << (sym != NULL ? sym->class_name() : "<no class>")
                    << std::endl;
                std::cout << std::flush;
#endif
             // This is not a simple assignment, as it must possibly
             // recursively walk data structures and initialize
             // substructures.
             // result = (b != NULL ? assign(a, b) : a);
                SgType *symtype = NULL;
                if (SgVariableSymbol *varsym = isSgVariableSymbol(sym))
                    symtype = varsym->get_type();
                else if (SgEnumFieldSymbol *enumsym = isSgEnumFieldSymbol(sym))
                    symtype = enumsym->get_type();
                if (symtype == NULL && b != NULL)
                {
                 // This shouldn't happen, but it looks like we sometimes
                 // have NULL symbols in function parameter lists --
                 // however, these cannot have initializers in C, which is
                 // what we are targeting here. So basically, we should
                 // never get here.
                 // On the other hand, we also get this for variables at
                 // file scope that are redeclared. So we do get here.
                    std::cerr
                        << "*** warning: trying to initialize a symbol "
                        << "without a type: "
                        << (void *) sym << " "
                        << (sym != NULL ? name(sym) : "<no name>") << " "
                        << (sym != NULL ? sym->class_name() : "<no class>")
                        << std::endl;
                    std::cerr
                        << "*** node's parent: "
                     // << node->get_parent()->unparseToString()
                        << " " << node->get_parent()->class_name()
                        << std::endl;
                    std::cerr
                        << "*** node's grandparent: "
                     // << node->get_parent()->get_parent()->unparseToString()
                        << " "
                        << node->get_parent()->get_parent()->class_name()
                        << std::endl;
                    std::cerr << std::flush;
                    std::abort();
                }
                if (symtype == NULL && b == NULL)
                 // See above. The symbol should never be NULL, but if it is,
                 // and we do not have an initializer, then we're fine.
                    result = a;
                else
                    result = initializerAssignment(symtype, a, b);
            }
            else
            {
             // There is no initializer. If sym is a valid symbol pointer,
             // we return its location; otherwise, this is an unnamed variable
             // (such as an unnamed function argument) without an
             // initializer, which can have a NULL location if it likes. I
             // think.
                if (sym != NULL)
                    result = symbol_location(sym);
                else
                    result = NULL;
            }
        }
        else
        {
#if VERBOSE_DEBUG
            std::cout
                << "initialized name in struct "
                << name(isSgInitializedName(node))
                << " has initializer: "
                << (void *) isSgInitializedName(node)->get_initializer()
                << (isSgInitializedName(node)->get_initializer() != NULL ?
                        ", OOPS, should be NULL!" : ", ok")
                << std::endl;
#endif
        }
        break;

    case V_SgCharVal:
    case V_SgUnsignedCharVal:
    case V_SgShortVal:
    case V_SgUnsignedShortVal:
    case V_SgIntVal:
    case V_SgUnsignedIntVal:
    case V_SgLongIntVal:
    case V_SgLongLongIntVal:
    case V_SgUnsignedLongVal:
    case V_SgUnsignedLongLongIntVal:
    case V_SgEnumVal:
     // integer value expressions suck: if they denote constant memory
     // addresses, we don't know whether those regions are disjoint or
     // connected by pointer arithmetic; conservatively, we must, I think,
     // assign the same pointer location to any integer constant, but this
     // means that any pair of such pointers may be aliased, which may of
     // course be very coarse -- in the future, programmer-provided
     // annotations could be used to disentangle these locations, or we
     // could treat them as distinct as long as their integer constant
     // values are different, and no arithmetic is performed on them
        result = mainInfo->integerConstantLocation;
     // disregarding for the moment everything that was written above, we
     // will not treat integers as pointers unless casted to a pointer type;
     // in this case, the cast rule will take care of matters
        result = NULL;
        break;

    case V_SgFloatVal:
    case V_SgDoubleVal:
    case V_SgLongDoubleVal:
     // I refuse to believe that anyone would cast a floating-point constant
     // to a pointer; same goes for bools.
    case V_SgBoolValExp:
        result = NULL;
        break;

    case V_SgStringVal:
     // For now, assume all string constants are shared. This introduces
     // some inprecision, since in reality, string constants can only be
     // shared if one is a suffix of the other. However, as modifying string
     // constants is illegal, we might be fine identifying only strings that
     // are equal. Overall, I'm not sure what string constant points-to
     // information might be good for.
        result = mainInfo->stringConstantLocation;
        break;

    case V_SgNullExpression:
     // Null expressions can appear in empty for loop conditions.
        result = NULL;
        break;

    case V_SgAssignInitializer:
     // simply pass through the initializer expression's location
        result = synlist[SgAssignInitializer_operand_i];
        break;

    case V_SgAggregateInitializer:
     // pass through the dummy node with the initializer list
        result = synlist[SgAggregateInitializer_initializers];
        break;

#if 0
    case V_SgFunctionDefinition:
     // This shouldn't be necessary.
#if VERBOSE_DEBUG
        {
            SgFunctionDefinition *fd = isSgFunctionDefinition(node);
            std::string funcname = Utils::mangled_name(fd);
            std::cout
                << "found funcdef with mangled name " << funcname
                << ", its declaration is: " << (void *) fd->get_declaration()
                << ", name: " << name(fd->get_declaration())
                << std::endl;
        }
        result = NULL;
#endif
        break;
#endif

    case V_SgFunctionParameterList:
     // Build a dummy location recording the locations of the function
     // parameters. These might be nontrivial locations due to default
     // function arguments (though not really, as we are only targeting C).
     // a = createDummyLocation();
        a = createLocation();
        fillDummyLocation(a, synlist);
        if (SgFunctionParameterList *l = isSgFunctionParameterList(node))
        {
            SgInitializedNamePtrList &args = l->get_args();
            if (!args.empty() && isSgTypeEllipse(args.back()->get_type()))
            {
             // Make sure the last argument exists, and assign it to
             // ellipsis_location. If all is well, ellipsis_location
             // is NULL before this point, so the assignment should be fine.
                if (a->arg_locations.back() == NULL)
                    a->arg_locations.back() = createLocation();
                a->ellipsis_location = a->arg_locations.back();
            }
        }
        result = a;
        break;

    case V_SgFunctionDeclaration:
     // First, unify the function's argument locations with any locations
     // derived from the parameter list. We obtain the function location by
     // looking up the location for the function declaration (i.e., its
     // symbol), which is a location that points to the function node itself.
        a = function_location(isSgFunctionDeclaration(node))->baseLocation();
        b = synlist[SgFunctionDeclaration_parameterList];

#if VERBOSE_DEBUG
        std::cout
            << "at function declaration; a has " << a->arg_locations.size()
            << " parameters, b has " << b->arg_locations.size()
            << std::endl;
#endif

        unifyArgListLocations(a, b);

     // b was a "dummy location" meant only to hold a temporary list of
     // locations; free it now
        freeDummyLocation(b);
#if 0 && DEPRECATED
     // This function takes each location from the return_locations list and
     // unifies it with a->return location.
        unifyReturnLocation(a->return_location);
#endif
     // This function takes care of copying all the accumulated data from
     // this function that needs to be copied, and clears everything that
     // may not escape the function.
        performFunctionEndBookkeeping(isSgFunctionDeclaration(node));
        result = NULL;
        break;

    case V_SgExprListExp:
     // This node is used to represent function argument expression lists,
     // as well as lists of aggregate initializers; we need to keep a list
     // of the corresponding locations, so we construct a dummy location
     // containing just a list of the expression locations, and the parent
     // node will handle these appropriately.
        a = createDummyLocation();
        fillDummyLocation(a, synlist);
        result = a;
        break;

    case V_SgFunctionCallExp:
     // TODO: Port this code to the ICFG-specific call node!

     // Unify argument expression locations with function parameter locations,
     // return the function's return location as result location.
        a = synlist[SgFunctionCallExp_function];
        b = synlist[SgFunctionCallExp_args];

     // First, make sure the function expression is associated with a
     // pointer-to-function location, creating it if necessary.
        if (a->baseLocation() == NULL)
        {
            a->pointTo(createFunctionLocation());
#if VERBOSE_DEBUG
            std::cout
                << "a->baseLocation() == NULL for a->id = "
                << a->id << std::endl;
            std::cout
                << "new location: " << a->baseLocation()->id << std::endl;
#endif
         // this can't hurt (except that it might hurt precision, but I
         // don't think so)
            join_pending_locations(a);
        }

     // Record the function expression and its location node.
     // callSiteExpressions.push_back(
#if FIXME
        info->callGraph.callSiteExpressionMap[this_function_symbol].push_back(
                isSgFunctionCallExp(node)->get_function());
#endif
        info->callLocationMap[isSgFunctionCallExp(node)->get_function()]
                = a->baseLocation();
#if VERBOSE_DEBUG
        std::cout
            << "recorded function location " << a->baseLocation()->id
            << " for function exp "
            << isSgFunctionCallExp(node)->get_function()->unparseToString()
            << std::endl;
#endif

     // Special function handling.
        result = NULL;
        if (a->baseLocation() != NULL && a->baseLocation()->special_function)
        {
            SgFunctionRefExp *fref = isSgFunctionRefExp(
                    isSgFunctionCallExp(node)->get_function());
            if (fref != NULL)
            {
                std::string name = Utils::name(fref->get_symbol_i());
#if VERBOSE_DEBUG
                std::cout << "attempting call to special function '"
                    << name << "'" << std::endl;
#endif
             // the newSpecialFunctionContext function takes care of
             // unifying argument and return locations, and returns != NULL if
             // it succeeded
                result = newSpecialFunctionContext(name, synlist);
#if VERBOSE_DEBUG
                if (result != NULL)
                {
                    std::cout
                        << "special function returned location "
                        << result->id
                        << std::endl;
                }
                else
                {
                    std::cout
                        << "special function returned NULL"
                        << std::endl;
                }
#endif
            }
        }
        else
        {
         // Normal function handling.
         // point a at the actual function location node
            a = a->baseLocation();
#if VERBOSE_DEBUG
            std::cout
                << "call to function at location " << a->id << ", which is "
                << (a->special_function ? "" : "presumably NOT ")
                << "a special function"
                << std::endl;
#endif
         // Check the access type, then "call" the function by unifying
         // its parameter list with the argument list, and by returning its
         // return location.
            SgType *access_type = isSgFunctionCallExp(node)->get_function()
                                    ->get_type()->findBaseType();
            checkOrSetFirstAccessType(a, access_type);
            unifyArgListLocations(a, b, /* function call = */ true);
            result = a->return_location;
        }

     // b was a "dummy location" meant only to hold a temporary list of
     // locations; free it now
        freeDummyLocation(b);

        break;

    case V_SgVarArgStartOp:
     // We need to unify the lhs operand, which is the va_list object, with
     // the enclosing function's ellipsis parameter.
        result = synlist[SgVarArgStartOp_lhs_operand];
#if DEBUG
        std::cout
            << "var arg start lhs op: "
            << (result != NULL ? result->id : 0) << " "
            << (result != NULL ? names(result->symbols) : "")
            << std::endl;
#endif
     // If this is the first va_list for this function, set it as the
     // current one; if there is already a current one which is different,
     // unify the two.
        a = functionLocationForProcnum(get_node_procnum());
        if (a->ellipsis_location == NULL)
        {
#if DEBUG
            std::cout
                << "shouldn't happen: encountered VarArgStartOp, but "
                << "ellipsis_location is not set for function location "
                << a->id
                << std::endl;
#endif
            a->ellipsis_location = result;
        }
        else if (a->ellipsis_location != result)
        {
#if DEBUG
            std::cout
                << "unifying va_list " << result->id
                << "with ellipsis_location "
                << a->ellipsis_location->id
                << std::endl;
#endif
            result = assign(result, a->ellipsis_location);
        }
        break;

    case V_SgVarArgEndOp:
     // There is nothing to do: va_end does not return or modify anything.
        break;

    case V_SgVarArgOp:
     // The result is equivalent to the enclosing function's ellipsis
     // parameter. If va_start takes care of unifying the ellipsis with the
     // appropriate va_list object, then all we need to do is to use the lhs
     // argument's location here, which is the va_list. Let's do that.
        result = synlist[SgVarArgOp_operand_expr];
        break;

    case V_SgVarArgCopyOp:
     // This is an assignment, essentially.
        a = synlist[SgVarArgCopyOp_lhs_operand];
        b = synlist[SgVarArgCopyOp_rhs_operand];
        result = assign(a, b);
        break;

    case V_SgCommaOpExp:
     // The result of the comma expression is the last operand, which in our
     // case is the rhs (comma is left-associative).
        result = synlist[SgCommaOpExp_rhs_operand_i];
        break;

    case V_SgReturnStmt:
     // Find the return expression's location and assign it to the enclosing
     // function's return location.
        a = synlist[SgReturnStmt_expression];
        b = functionLocationForProcnum(get_node_procnum());
        if (b->return_location == NULL)
        {
#if DEBUG
            std::cout
                << "function location " << b->id << " has no return location!"
                << std::endl;
#endif
            b->return_location = createLocation();
        }
        if (a != NULL)
            assign(b->return_location, a);
#if 0 && DEPRECATED
        if (a != NULL)
            return_locations.push_back(a);
#endif
#if VERBOSE_DEBUG
        std::cout
            << "return statement: returning location "
            << (a != NULL ? a->id : 0)
            << std::endl;
#endif
     // No need to return any location for statements.
        result = NULL;
        break;

    case V_SgExprStatement:
     // The expression statement itself returns a NULL location; it has
     // nothing to propagate upwards. However, we will give it a side
     // effect: If the location of the expression is a dummy location, we
     // free that location because it can certainly not be referenced
     // anymore.
        a = synlist[SgExprStatement_expression];
        if (a != NULL && a->dummy == true)
            freeDummyLocation(a);
        result = NULL;
        break;

    case V_SgStatement:
     // SATIrE's ICFG-specific statements derive from the SgStatement class
     // and do not overwrite its variantT method. Thus, these nodes (are the
     // only ones that) have the V_SgStatement variant value.
     // We handle ICFG statements in a special function.
        if (IcfgStmt *icfgStmt = isIcfgStmt(node))
        {
            result = handleIcfgStatement(icfgStmt, synlist);
        }
        else
        {
            std::cerr
                << "*** error: found node with variant V_SgStatement, "
                << "class name: " << node->class_name()
                << std::endl;
            std::abort();
        }
        break;


     // ========== TODO section follows---implement these ==========

     // This section lists node types that (might) have some pointer-related
     // semantics and should be handled (or evaluated); leave these
     // commented out for now so we keep getting the warning messages
     // nagging us to finally implement this stuff.


     // ========== ignoring all node types below this line ==========

     // Nodes listed here cannot yield or update pointer locations; these
     // rules use fall-throughs, so don't mix with nodes that do have some
     // pointer-related semantics.

     // Expression-kind of nodes that we can ignore; these must be chosen
     // carefully as pointed out by Steensgaard: pointer subtraction does
     // not yield a pointer, but can be used with one of the operands to
     // yield a pointer again.
     // Relational operators should be all right to ignore, or they
     // shouldn't, depending on one's point of view: according to the
     // standards, these may only be applied to pointers within the same
     // object, so in conforming programs, the operands will have the same
     // location anyway (we could sanity-check this, I guess); for
     // nonconforming programs that iterate pointers from one object to the
     // next, we might get results stating that the operands cannot overlap,
     // which may be wrong. But we cannot analyze every weird form of
     // nonconforming program; let's only do that if really needed for some
     // concrete application.
    case V_SgNotOp:
    case V_SgEqualityOp:
    case V_SgLessThanOp:
    case V_SgGreaterThanOp:
    case V_SgNotEqualOp:
    case V_SgLessOrEqualOp:
    case V_SgGreaterOrEqualOp:

     // We're paranoid about integer constants, but I'm pretty sure nobody
     // in their right mind would dereference the value yielded by a sizeof,
     // so let's ignore it.
    case V_SgSizeOfOp:

     // statement-kind of nodes that we can ignore
    case V_SgProject:
    case V_SgFile:
    case V_SgGlobal:
    case V_SgVariableDeclaration:
     // Note that initializations inside variable declarations are handled
     // by the nested SgInitializedName nodes. Thus there is nothing to do
     // for the variable declaration node.
    case V_SgTypedefDeclaration:
    case V_SgClassDeclaration:
    case V_SgClassDefinition:
    case V_SgEnumDeclaration:
    case V_SgForInitStatement:
     // Initializations inside the for init statement are handled by the
     // constructs nested inside the for init statement.
    case V_SgBreakStmt:
    case V_SgContinueStmt:
    case V_SgGotoStatement:
    case V_SgBasicBlock:
    case V_SgIfStmt:
    case V_SgWhileStmt:
    case V_SgDoWhileStmt:
    case V_SgForStatement:
    case V_SgSwitchStatement:
    case V_SgCaseOptionStmt:
    case V_SgDefaultOptionStmt:
    case V_SgLabelStatement:
    case V_SgFunctionDefinition:
     // The function definition node contains the body of a function. It is
     // always contained inside a function declaration node, which takes
     // care of unifying return locations and such stuff.

     // Ignore all these node types; to handle all of ROSE, this list will
     // become really really long.
        result = NULL;
        break;

    default:
        std::cout
            << "*** warning: ignoring node of type "
            << node->class_name() << std::endl;
        result = NULL;
        break;
    }

#if VERBOSE_DEBUG
 // Display the synthesized attribute for this node.
    std::cout
        << " -> " << (void *) result << std::endl;
#endif

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::handleIcfgStatement(
        IcfgStmt *icfgStmt, SynthesizedAttributesList synlist)
{
    Location *result = NULL;
    Location *a, *b;
    SynthesizedAttributesList::iterator syn;
    std::vector<Location *>::iterator s, t;
    SgFunctionSymbol *funcSym;
    SgExpression *funcExpr;
 // Access to the current ICFG node. Note that we also traverse some stuff
 // that is not within ICFG nodes, such as global initializers. So this
 // requires some care.
    BasicBlock *icfgNode = NULL;
    CFG *icfg = icfgTraversal->get_icfg();
    if (icfgTraversal->is_icfg_statement())
        icfgNode = icfg->nodes[icfgTraversal->get_node_id()];

    switch (icfgStmt->satireVariant())
    {
     // There is nothing to do for function entry, exit, and return nodes.
     // These are essentially just markers for certain program points
     // without any interesting semantics.
    case V_FunctionEntry:
    case V_FunctionExit:
    case V_FunctionReturn:
    case V_ExternalReturn:
        result = NULL;
        break;

     // Function calls are an important case. We can treat "normal"
     // statically resolved and "external" not statically resolved calls the
     // same way, I think; we simply use the ICFG's function call attribute
     // in both cases.
     // There is not much to do, in fact, except to fetch the call target's
     // location and record a few facts for the call graph. Unification of
     // function arguments and return values is handled below, in the rules
     // for ArgumentAssignment etc.
    case V_FunctionCall:
    case V_ExternalCall:
     // Determine the enclosing function's symbol, the call target
     // expression, and the corresponding location.
        funcSym = (*icfgTraversal->get_icfg()->procedures)[icfgNode->procnum]
                                                          ->funcsym;
        funcExpr = icfgNode->call_target;
        a = expressionLocation(funcExpr);
     // Store the information in the maps, and we're done!
        info->callGraph.callSiteExpressionMap[funcSym].push_back(funcExpr);
        info->callLocationMap[funcExpr] = a->baseLocation();
#if VERBOSE_DEBUG
        std::cout
            << "in function " << Utils::name(funcSym) << ": "
            << "recorded call to " << Ir::fragmentToString(funcExpr)
            << " with location " << a->baseLocation()->id
            << std::endl;
#endif
        result = NULL;
        break;

     // Declare/undeclare statements are irrelevant to the points-to
     // analysis because it is flow-insensitive and thus computes global
     // information for all objects in the program, without any notion of
     // going in and out of scope.
    case V_DeclareStmt:
    case V_UndeclareStmt:
        result = NULL;
        break;

     // Constructor and destructor calls can't happen for C programs, which
     // are the targets of the points-to analysis. We silently ignore them
     // for now, we could in theory raise a warning at some point.
    case V_ConstructorCall:
    case V_DestructorCall:
        result = NULL;
        break;

     // For ArgumentAssignments, get the call target to find the called
     // function's location, and get the call index to find out which
     // argument to assign to. Unify that with the location for the
     // ArgumentAssignment's rhs expression (the argument expression).
    case V_ArgumentAssignment:
     // If the analysis is context-insensitive, locate the call target from
     // the target expression; otherwise, use the call string info to locate
     // all possible callee contexts, and use their procedureLocation.
        {
#if HAVE_PAG
            std::vector<ContextInformation::Context>::const_iterator beg;
            std::vector<ContextInformation::Context>::const_iterator end;
#endif
            BasicBlock *call_block
                = icfg->call_target_call_block[icfgNode->call_target];
            bool resolved
                = kfg_which_out_edges(call_block) & (1U << CALL_EDGE);
            if (!contextSensitive || !resolved)
                a = expressionLocation(icfgNode->call_target);
            else
            {
#if HAVE_PAG
             // Context-sensitive call which is resolved in the ICFG.
                int call_id = call_block->id;
#if VERBOSE_DEBUG
                std::cout
                    << "argass " << get_node_id()
                    << " for call " << call_id
                    << std::endl;
                std::cout
                    << "this context: " << info->context->toString()
                    << std::endl;
#endif
                beg = icfg->contextInformation->childContexts(
                        call_id, *info->context).begin();
                end = icfg->contextInformation->childContexts(
                        call_id, *info->context).end();
#endif
            }

#if HAVE_PAG
            do // loop over all called contexts
            {
#endif
             // First, compute the location for the call target...
                if (contextSensitive && resolved)
                {
#if HAVE_PAG
                    ContextInformation::Context c = *beg;
                    a = allInfos[c]->procedureLocation;
#if VERBOSE_DEBUG
                    std::cout
                        << "a = " << (void *) a << std::endl;
                    std::cout
                        << "found procloc "
                        << allInfos[c]->prefix << a->id
                        << std::endl;
                    std::cout
                        << "  -> proc: "
                        << allInfos[c]->prefix << a->baseLocation()->id
                        << std::endl;
                    std::cout
                        << "  -> arg_0: "
                        << allInfos[c]->prefix
                            << a->baseLocation()->arg_locations[0]->id
                            << " ("
                            << (void *) a->baseLocation()->arg_locations[0]
                            << ")"
                        << std::endl;
#endif
#endif
                }
             // ... dereference it, since it's a pointer to function ...
                a = a->baseLocation();
#if VERBOSE_DEBUG
                if (info->prefix != "")
                    std::cout << "in ctx " << info->prefix << ": ";
                std::cout
                    << "call to function at location " << a->id
                    << " (" << names(a->func_symbols) << ")"
                    << std::endl;
#endif
             // ... and then find the correct argument location. This is
             // complicated by the fact that the call index may be larger
             // than the number of arguments -- if the function has varargs.
             // In this case, we use the last arg location, which is the
             // ellipse.
                a = a->arg_locations[
                    icfgNode->call_index < a->arg_locations.size()
                                        ? icfgNode->call_index
                                        : a->arg_locations.size() - 1];

             // synlist[1] is the synthesized attribute for the second
             // traversal successor, which is the rhs expression.
                b = synlist[1];

             // Perform the assignment and clean up if necessary.
#if VERBOSE_DEBUG
                std::cout
                    << "argument assignment";
                if (info->prefix != "")
                    std::cout << " in ctx " << info->prefix;
                std::cout << ": "
                    << a->id
                    << " (" << (void *) a << ")"
                    << " := "
                    << (b != NULL ? b->id : 0)
                    << " (" << (void *) b << ")"
                 // << " / "
                 // << Ir::fragmentToString(icfgStmt)
                    << std::endl;
#endif
                result = (b != NULL ? assign(a, b) : a);
#if VERBOSE_DEBUG
                std::cout
                    << "assignment result: "
                    << (result != NULL ? result->id : 0)
                    << " (" << (void *) result << ")"
                    << std::endl
                    << std::endl;
#endif
#if VERBOSE_DEBUG
                if (b != NULL && b->dummy)
                {
                    std::cout
                        << "dummy location for ArgumentAssignment rhs: "
                        << "expr = "
                        << Ir::fragmentToString(
                                isArgumentAssignment(icfgStmt)->get_rhs())
                        << ", b->id = " << b->id
                        << ", has ret: "
                            << (b->return_location != NULL ? "yes" : "no")
                        << std::endl;
                }
#endif
                if (b != NULL && b->dummy)
                    freeDummyLocation(b);
#if HAVE_PAG
            } while (contextSensitive && resolved && ++beg != end);
#endif
        }
        break;

     // Find the called function's return location and assign it to the
     // return variable for this call site.
    case V_ReturnAssignment:
     // GB (2009-03-27): It seems like all of this stuff is really not
     // necessary. A ReturnAssignment is only relevant to assign the global
     // retvar's value to a local retvar; however, our handling of retvars
     // bypasses this assignment completely and goes directly to the called
     // functions (contexts) to fetch the return location. So
     // ReturnAssignment is a no-op.
#if 0
     // The assignment's lhs is synlist[0], the location for the temporary
     // return variable holding the return value for this call site.
        a = synlist[0];
#if VERBOSE_DEBUG
        std::cout
            << "doing ReturnAssignment "
            << Ir::fragmentToString(icfgStmt)
            << std::endl
            << "assignment is in procedure " << icfgNode->procnum
                << ", " << (*icfgTraversal->get_icfg()->procedures)
                                [icfgNode->procnum]->name
                << " in context "
                    << (info->prefix != "" ? info->prefix : "main")
                << std::endl;
#endif
     // The assignment's rhs in the ICFG is the global return variable,
     // which we are not interested in. Rather, we fetch the location for
     // the call expression associated with this ICFG node.
     // For context-sensitive analysis, this must loop over the return
     // locations of all possibly called contexts.
        {
            std::vector<ContextInformation::Context>::const_iterator beg;
            std::vector<ContextInformation::Context>::const_iterator end;

            if (!contextSensitive)
                b = expressionLocation(icfgNode->call_target);
            else
            {
                int call_id
                    = icfg->call_target_call_block[icfgNode->call_target]->id;
                beg = icfg->contextInformation->childContexts(
                        call_id, *info->context).begin();
                end = icfg->contextInformation->childContexts(
                        call_id, *info->context).end();
            }

            do {
             // b = expressionLocation(icfgNode->call_target);
                if (contextSensitive)
                {
                    ContextInformation::Context c = *beg;
                    b = allInfos[c]->procedureLocation;
                }
                b = b->baseLocation();
#if VERBOSE_DEBUG
                std::cout
                    << "for call to function location " << b->id
                    << ": " << (b->func_symbols.empty()
                                ? "<unknown>"
                                : names(b->func_symbols))
                    << std::endl;
#endif
             // Finally, assign the called function's return location to the
             // local temporary variable.
                std::cout << "actual ReturnAssignment: ";
                result = assign(a, b->return_location);
            } while (contextSensitive && ++beg != end);
        }
#endif
        break;

     // I think the ParamAssignment can be ignored since we assign argument
     // expressions directly to the appropriate argument locations in the
     // function location node.
    case V_ParamAssignment:
        result = NULL;
        break;

     // The logical if statement has no interesting semantics, as its
     // condition is evaluated in the recursive traversal.
    case V_LogicalIf:
        result = NULL;
        break;

     // Join points possess no interesting semantics.
    case V_IfJoin:
    case V_WhileJoin:
        result = NULL;
        break;

    default:
        std::cout
            << "*** warning: ignoring ICFG statement of type "
            << icfgStmt->class_name() << std::endl;
        result = NULL;
        break;
    }

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::assign(
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
 // treat every assignment as a potential pointer assignment because we
 // need to treat integers, and pretty much anything else, as potential
 // pointers
    if (a == NULL)
        a = createLocation(NULL);
    if (b == NULL)
        b = createLocation(NULL);
    Location *result = NULL;

#if VERBOSE_DEBUG
    std::cout
        << "handle assignment to location "
        << a->id << ": " << names(a->symbols);
    if (a->baseLocation() != NULL)
    {
        std::cout
            << " (" << a->baseLocation()->id << ": "
            << names(a->baseLocation()->symbols) << ")";
    }
    std::cout
        << " from location "
        << b->id << ": " << names(b->symbols);
    if (b->baseLocation() != NULL)
    {
        std::cout
            << " (" << b->baseLocation()->id << ": "
            << names(b->baseLocation()->symbols) << ")";
    }
    std::cout
        << std::endl;
#endif

    if (b->collapseBase == true)
        a->collapseBase = true;

    result = (a != b ? cjoin(a, b) : b);

#if VERBOSE_DEBUG
    std::cout << "--------------------------------------------" << std::endl;
#endif

 // If b is a dummy location, and it is not identical to the result, it will
 // not be used anymore; free it.
    if (b->dummy == true && result != b)
        freeDummyLocation(b);
 // Conversely, if a is a dummy, it shouldn't be. Dummies can be assigned to
 // in a few intricate cases, such as returning a local variable's address.
 // Make this a non-dummy location in such cases.
    if (a->dummy == true)
    {
#if VERBOSE_DEBUG
        std::cout
            << "a is a dummy: a->id = " << a->id
            << "; b = " << b->id << ", assign result = " << result->id
            << std::endl;
#endif
        a->dummy = false;
    }

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::join(
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
 // Precondition:
 //     a != NULL, b != NULL
 // Side effects:
 //     may modify anything reachable from a or b
 // Returns:
 //     the location of b, essentially
    Location *result = b;
    Location *representative = NULL;

#if VERBOSE_DEBUG
    std::cout << "joining " << a->id << " and " << b->id << std::endl;
#endif

    if (a->baseLocation() != NULL && b->baseLocation() != NULL
                                  && a->baseLocation() != b->baseLocation())
    {
        Location *a_set = location_representative(a->baseLocation());
        Location *b_set = location_representative(b->baseLocation());

     // disjointSets.union_set(a->base_type, b->base_type);
        if (a_set != b_set)
        {
#if VERBOSE_DEBUG
            std::cout
                << "linking " << a_set->id << " and " << b_set->id
                << std::endl;
#endif
            mainInfo->disjointSets.link(a_set, b_set);
#if VERBOSE_DEBUG
            std::cout
                << "a = " << (void *) a
                << " base = " << (void *) a->baseLocation()
                << " repr = "
                    << (void *) location_representative(a->baseLocation())
                << " id = "
                    << location_representative(a->baseLocation())->id
                << std::endl;
            std::cout
                << "b = " << (void *) b
                << " base = " << (void *) b->baseLocation()
                << " repr = "
                    << (void *) location_representative(b->baseLocation())
                << " id = "
                    << location_representative(b->baseLocation())->id
                << std::endl;
#endif
            unify(a, b);
        }
#if VERBOSE_DEBUG
        else
        {
            std::cout
                << "a_set == b_set == "
                << (void *) a_set << " == " << (void *) b_set
                << "  with id  " << a_set->id << " == " << b_set->id
                << std::endl;
        }
#endif

        representative = location_representative(b->baseLocation());
        if (representative != b->baseLocation())
        {
            representative->symbols.splice(representative->symbols.begin(),
                                           b->baseLocation()->symbols);
            representative->func_symbols.splice(
                    representative->func_symbols.begin(),
                    b->baseLocation()->func_symbols);
        }
        b->pointTo(representative);
        result = b;

     // this is a little questionable; how do we know whether this makes the
     // result really contain just the right symbol? however, these
     // labels are (for now) mainly for documentation purposes anyway, so
     // they can be a little off if they really want to
        if (b->baseLocation() != a->baseLocation())
        {
            b->baseLocation()->symbols.splice(
                    b->baseLocation()->symbols.begin(),
                    a->baseLocation()->symbols);
            b->baseLocation()->func_symbols.splice(
                    b->baseLocation()->func_symbols.begin(),
                    a->baseLocation()->func_symbols);
        }
#if VERBOSE_DEBUG
        std::cout << "result id: " << result->id << std::endl;
        std::cout
            << "! a->baseLocation() != NULL && b->baseLocation() != NULL"
                << std::endl
            << "result vars: " << names(result->symbols) << std::endl
            << "a vars:      " << names(a->symbols) << std::endl
            << "b vars:      " << names(b->symbols) << std::endl
            << "a->basevars: " << names(a->baseLocation()->symbols)
                << std::endl
            << "b->basevars: " << names(b->baseLocation()->symbols)
                << std::endl;
#endif
    }
    if (a->return_location != NULL && b->return_location != NULL)
    {
     // Unifying the argument lists; note that unifyArgListLocations is
     // directional, and b is the destination location.
        unifyArgListLocations(b, a);
    }
    if (a->return_location != NULL && b->return_location != NULL
                                   && a->return_location != b->return_location)
    {
        mainInfo->disjointSets.union_set(a->return_location,
                                         b->return_location);
        result = b;
        result->return_location
            = location_representative(b->return_location);
     // Not sure why this is/was unify(a, b), and why it should be, if it
     // should. This call can lead to infinite cycles. Let's try to only
     // unify return locations.
     // unify(a, b);
        unify(a->return_location, b->return_location);
        if (b->return_location != a->return_location)
        {
            b->return_location->symbols.splice(
                    b->return_location->symbols.begin(),
                    a->return_location->symbols);
            b->return_location->func_symbols.splice(
                    b->return_location->func_symbols.begin(),
                    a->return_location->func_symbols);
        }
    }

    if (a->baseLocation() != NULL && b->baseLocation() == NULL)
    {
        b->pointTo(a->baseLocation());
        join_pending_locations(b);
#if VERBOSE_DEBUG
        std::cout
            << "! a->baseLocation() != NULL && b->baseLocation() == NULL"
                << std::endl
            << "a vars:      " << names(a->symbols) << std::endl
            << "b vars:      " << names(b->symbols) << std::endl;
#endif
        result = b;
    }
    if (a->return_location != NULL && b->return_location == NULL)
    {
        b->return_location = a->return_location;
     // Unifying the argument lists; note that unifyArgListLocations is
     // directional, and b is the destination location.
        unifyArgListLocations(b, a);
        join_pending_locations(b);
        result = b;
    }

    if (a->baseLocation() == NULL && b->baseLocation() != NULL)
    {
        a->pointTo(b->baseLocation());
        join_pending_locations(a);
#if VERBOSE_DEBUG
        std::cout
            << "! a->baseLocation() == NULL && b->baseLocation() != NULL"
                << std::endl
            << "a vars:      " << names(a->symbols) << std::endl
            << "b vars:      " << names(b->symbols) << std::endl;
#endif
#if VERBOSE_DEBUG
        std::cout
            << "a now points to: " << a->baseLocation()->id
            << " (" << (void *) a->baseLocation() << ")" << std::endl
            << "b now points to: " << b->baseLocation()->id
            << " (" << (void *) b->baseLocation() << ")" << std::endl;
#endif
        result = b;
    }
    if (a->return_location == NULL && b->return_location != NULL)
    {
        a->return_location = b->return_location;
     // Unifying the argument lists; note that unifyArgListLocations is
     // directional, and b is the destination location.
        unifyArgListLocations(b, a);
        join_pending_locations(a);
        result = b;
    }

    if (a->baseLocation() == NULL && b->baseLocation() == NULL
     && a->return_location == NULL && b->return_location == NULL
     && !hasPointerMembers(a) && !hasPointerMembers(b))
    {
#if VERBOSE_DEBUG
        std::cout << "*** both base locations NULL in join!" << std::endl;
        std::cout << "a: " << a->id << ", b: " << b->id << std::endl;
#endif
        if (a != b)
        {
         // The splice operation is only well-defined for non-overlapping
         // lists; it can create nasty cycles otherwise.
            b->pending.splice(b->pending.begin(), a->pending);
         // b->pending.push_back(a);
        }
        cjoin(a, b);
        result = b;
    }

 // putting this here is sort of a guess, but it kind of seems to work
    if (isNonCollapsedStructure(a) || isNonCollapsedStructure(b))
        unify(a, b);

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::cjoin(
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
    Location *result;
    
#if VERBOSE_DEBUG
    std::cout
        << "conditionally joining " << a->id << " and " << b->id
        << std::endl;
#endif
    a = location_representative(a);
    b = location_representative(b);
    if (a == b
        || (a->baseLocation() != NULL
            && a->baseLocation() == b->baseLocation()))
    {
     // There is nothing to join, a and b refer to the same location
     // already.
        result = b;
    }
    else if (b->baseLocation() == NULL && b->return_location == NULL
          && !hasPointerMembers(b))
    {
     // Actual conditional join: save a as pending for the case that b's
     // base location or function location becomes non-NULL, or any member
     // becomes a pointer.
     // Quick and dirty hack: If the first element is already a, do not add
     // it again; this is simply a workaround for one of my test cases that
     // pushes something twice, leading to ugly multiple edges that are
     // otherwise not semantically significant. So we avoid this case where
     // it's easy to avoid.
        if (b->pending.empty() || b->pending.front() != a)
            b->pending.push_back(a);
        result = b;
    }
    else
    {
     // Perform real join because a has no base location, or one that
     // differs from b's.
        result = join(a, b);
    }

    return result;
}

void
PointsToAnalysis::Implementation::join_pending_locations(
        PointsToAnalysis::Location *a)
{
#if VERBOSE_DEBUG
    std::cout << a->id << " pending: "
        << a->pending.size() << " locations" << std::endl;
#endif
    while (!a->pending.empty())
    {
        Location *t = location_representative(pop_front(a->pending));
#if VERBOSE_DEBUG
        std::cout << "joining pending location " << t->id << std::endl;
#endif
     // Steensgaard's paper has the pending join this way round:
     // join(a, t);
     // However, our struct assignments are directional, and it seems that
     // it makes more sense to join the other way round. The pending join
     // arose from some assignment of the form  t = a  at some point in the
     // program, so that's also the direction we should perform the join.
        join(t, a);
    }
 // If we are dereferencing a struct member, the containing structure's
 // assignments must also be forced.
    if (a->parent_struct != NULL && a->parent_struct != a)
        join_pending_locations(a->parent_struct);
}

void
PointsToAnalysis::Implementation::unify(
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
#if VERBOSE_DEBUG
    std::cout
        << "unifying "
        << (a != NULL ? a->id : 0)
        << ", "
        << (b != NULL ? b->id : 0)
        << std::endl;
#endif
 // There is nothing to do if one of the locations is NULL.
    if (a == NULL || b == NULL)
        return;

 // Collapse base locations if the collapseBase flag is set.
    if (a->baseLocation() != b->baseLocation())
    {
        if (a->collapseBase)
            collapseStructure(a->baseLocation());
        if (b->collapseBase)
            collapseStructure(b->baseLocation());
    }

 // If the first_access_type fields are incompatible, and at least one is a
 // struct, we need to collapse.
    if (a->first_access_type != NULL && b->first_access_type != NULL)
    {
        SgClassType *a_class_type = isSgClassType(a->first_access_type);
        SgClassType *b_class_type = isSgClassType(b->first_access_type);
        if (a->first_access_type != b->first_access_type
                && (a_class_type || b_class_type))
        {
            if (!a->collapsed)
                collapseStructure(a);
            if (!b->collapsed)
                collapseStructure(b);
        }
    }
    else if (a->first_access_type == NULL && b->first_access_type != NULL)
    {
     // Unification means that we should copy the first access type, I
     // guess. I think we must also materialize members, if appropriate.
     // Note that materialization will set the type field.
        if (SgClassType *b_class_type = isSgClassType(b->first_access_type))
            materializeAllStructMembers(a, b_class_type);
        else
            a->first_access_type = b->first_access_type;
    }
    else if (a->first_access_type != NULL && b->first_access_type == NULL)
    {
        if (SgClassType *a_class_type = isSgClassType(a->first_access_type))
            materializeAllStructMembers(b, a_class_type);
        else
            b->first_access_type = a->first_access_type;
    }
    else // a->first_access_type == NULL && b->first_access_type == NULL
    {
     // Not entirely sure what to do here. I think making this a pending
     // join is a good idea. Dereferences and struct field accesses must
     // make sure to force pending joins, then we should be fine. Then this
     // unify call will be repeated, but with the first access types set,
     // and then we will know what to do.
        if (b->pending.empty() || b->pending.front() != a)
            b->pending.push_back(a);
    }

 // If one of the locations is a collapsed structure, both must be. Thus,
 // collapse the one that is not collapsed. This must be done before
 // anything else, since unifying the base locations etc. below should refer
 // to the already collapsed locations.
    if (a->collapsed || b->collapsed)
    {
        if (a->collapsed && !b->collapsed)
            collapseStructure(b);
        else if (b->collapsed && !a->collapsed)
            collapseStructure(a);
     // otherwise, both are collapsed, and there is nothing to do
    }

    if (a->baseLocation() != NULL && b->baseLocation() != NULL
     && a->baseLocation() != b->baseLocation()
#if 0
     && disjointSets.find_set(a->baseLocation())
        != disjointSets.find_set(b->baseLocation())
#endif
        )
    {
#if VERBOSE_DEBUG
        std::cout
            << "unify joining base locations: "
            << a->id << "->"
                << (a->baseLocation() != NULL ? a->baseLocation()->id : 0)
            << " with "
            << b->id << "->"
                << (b->baseLocation() != NULL ? b->baseLocation()->id : 0)
            << std::endl;
#endif
        join(a->baseLocation(), b->baseLocation());
    }
 // join function argument and return locations
    std::vector<Location *>::iterator s, t;
    s = a->arg_locations.begin();
    t = b->arg_locations.begin();
    while (s != a->arg_locations.end() && t != b->arg_locations.end())
    {
        if (*s != NULL || *t != NULL)
        {
         // both locations must be present for a join
            if (*s == NULL)
                *s = createLocation();
            if (*t == NULL)
                *t = createLocation();
        }
        join(*s, *t);
        s++, t++;
    }
    if (a->return_location != NULL && b->return_location != NULL)
    {
        join(a->return_location, b->return_location);
    }
    if (a->array || b->array)
    {
        a->array = b->array = true;
    }

 // join members of non-collapsed structures; if the structures were
 // collapsed, the code above should have taken care of everything
    if (isNonCollapsedStructure(a) || isNonCollapsedStructure(b))
    {
#if VERBOSE_DEBUG
        std::cout
            << "unifying struct members of " << a->id
            << " and " << b->id
            << std::endl;
#endif
#if VERBOSE_DEBUG
        if (a->struct_members.size() != b->struct_members.size())
        {
            std::cout
                << "unifying structures that should be equal, but aren't!"
                << std::endl
                << "a->size: " << a->struct_members.size() << ", "
                << "b->size: " << b->struct_members.size()
                << std::endl;
        }
#endif
#if VERBOSE_DEBUG
        if (a->struct_members.empty() || b->struct_members.empty())
        {
            std::cout
                << "entered unification of non-collapsed structures"
                << std::endl;
            std::cout
                << "a has " << a->struct_members.size() << " members"
                << std::endl;
            if (a->struct_members.empty())
            {
                std::cout
                    << "materializing with type "
                    << name(isSgClassType(a->first_access_type))
                    << std::endl;
                materializeAllStructMembers(a,
                        isSgClassType(a->first_access_type));
            }
            std::cout
                << "b has " << b->struct_members.size() << " members"
                << std::endl;
            if (b->struct_members.empty())
            {
                std::cout
                    << "materializing with type "
                    << name(isSgClassType(b->first_access_type))
                    << std::endl;
                materializeAllStructMembers(b,
                        isSgClassType(b->first_access_type));
            }
        }
#endif
     // We proceed by unifying b's members with a's corresponding members.
     // All the checks above should have made sure that a and b have the
     // same struct type in their first_access_type fields, which must also
     // mean that they have just the same set of members.
        std::map<SgSymbol *, Location *>::iterator bm;
        std::map<SgSymbol *, Location *> &members = b->struct_members;
        for (bm = members.begin(); bm != members.end(); ++bm)
        {
            std::map<SgSymbol *, Location *>::iterator am;
            am = a->struct_members.find(bm->first);
            Location *a_member_location = NULL;
            if (am != a->struct_members.end())
            {
                a_member_location = am->second;
            }
            else
            {
             // Can't happen. I hope.
#if DEBUG
                std::cout
                    << "** problem: compatible struct unification, "
                    << "and some member is missing!"
                    << std::endl;
                std::cout
                    << "a: " << name(isSgClassType(a->first_access_type))
                    << ", " << a->struct_members.size() << " members; "
                    << "b: " << name(isSgClassType(b->first_access_type))
                    << ", " << b->struct_members.size() << " members"
                    << std::endl;
#endif
                a_member_location = createStructMemberLocation(a, bm->first);
            }
            if (bm->second == NULL)
                bm->second = createLocation();
         // Try to avoid infinite recursion on cyclic data structures.
            if (bm->second != a && a_member_location != b)
                join(a_member_location, bm->second);
        }
    }
}

void
PointsToAnalysis::Implementation::fillDummyLocation(
        PointsToAnalysis::Location *a,
        PointsToAnalysis::Implementation::SynthesizedAttributesList synlist)
{
#if VERBOSE_DEBUG
    if (!a->dummy)
    {
        std::cout
            << "fillDummyLocation called for non-dummy location " << a->id
            << std::endl;
    }
#endif

    SynthesizedAttributesList::iterator syn;
    for (syn = synlist.begin(); syn != synlist.end(); ++syn)
    {
#if VERBOSE_DEBUG
        std::cout
            << "adding location " << (*syn != NULL ? (*syn)->id : 0)
            << " to dummy list"
            << std::endl;
#endif
        a->arg_locations.push_back(*syn);
    }
}

void
PointsToAnalysis::Implementation::unifyArgListLocations(
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b,
        bool functionCall)
{
    std::vector<Location *>::iterator s, t;
    s = a->arg_locations.begin();
    t = b->arg_locations.begin();

    while (s != a->arg_locations.end() && t != b->arg_locations.end())
    {
        Location *u = *s;
        Location *v = *t;

#if VERBOSE_DEBUG
        std::cout
            << (u != NULL ? u->id : 0)
            << " <- "
            << (v != NULL ? v->id : 0)
            << std::endl;
#endif
        if (u != NULL && v != NULL)
        {
            Location *r = assign(u, v);
         // The function argument v might be a dummy; if so, it is either
         // freed by assign, or returned from it. If it's returned, free it
         // here.
            if (r->dummy == true && r != v /* && r != u */)
                freeDummyLocation(r);
        }

        s++, t++;
    }
    if (s != a->arg_locations.end() || t != b->arg_locations.end())
    {
     // If we got here, exactly one of the lists still contains some
     // elements. We need to get those elements into the other list. In
     // general, if both lists are nonempty, we unify all remaining members
     // of the longer list with the last element of the shorter list. This
     // is mainly designed to model varargs functions, where all extra
     // arguments are to be unified with the final ellipse.
     // In weird cases where functions are called with a wrong number of
     // arguments (no prototypes, cast function pointers), this will hurt
     // precision, but those programs get what they deserve.

     // All righty then. We make sure we know what container's remaining
     // elements to traverse, what the other container's last element is, if
     // any, and whether to unify with the last element or copy elements to
     // the second list.
        std::vector<Location *>::iterator it;
        std::vector<Location *>::iterator end;
        std::vector<Location *> *other;
        Location *last = NULL;
        bool unifyWithLast;
        if (s != a->arg_locations.end())
        {
            it = s;
            end = a->arg_locations.end();
            other = &b->arg_locations;
        }
        else // t != b->arg_locations.end()
        {
            it = t;
            end = b->arg_locations.end();
            other = &a->arg_locations;
        }
        if (!other->empty())
        {
            if (other->back() == NULL)
                other->back() = createLocation();
            last = other->back();
            unifyWithLast = true;
        }
        else
            unifyWithLast = false;

     // There is one (I hope it's only one!) subtle case: If this call to
     // unifyArgListLocations arose from analysis of the call of a function
     // (denoted by the functionCall flag), and a's argument locations list
     // contains exactly one more element than b's list, then this is a
     // varargs call in which no argument is passed for the ellipse. In that
     // case, we make sure that we do nothing.
        if (functionCall
            && a->arg_locations.size() == b->arg_locations.size() + 1)
         // That will take care of it.
            it = end;

     // And do the actual iteration.
        while (it != end)
        {
            Location *v = *it;
            if (unifyWithLast && v != NULL)
            {
                Location *r = assign(last, v);
             // The function argument v might be a dummy; if so, it is
             // either freed by assign, or returned from it. If it's
             // returned, free it here.
                if (r->dummy == true && r != v /* && r != last */)
                    freeDummyLocation(r);
            }
            else if (!unifyWithLast && v != NULL)
                other->push_back(v);
            else if (!unifyWithLast && v == NULL)
                other->push_back(createLocation());
            ++it;
        }
    }
#if 0
    while (t != b->arg_types.end())
    {
     // if this loop is entered, the a->arg_types list has been
     // completely iterated over, that is, b contains more types than a;
     // simply push the remaining types onto the end of a's list
     // TODO: handle vararg lists at some point
        Type *v = *t;
        if (v == NULL)
            v = createType();
#if VERBOSE_DEBUG
        std::cout
            << "pushing type "
            << (v != NULL ? v->id : 0);
        if (v != NULL)
            std::cout
                << " ("
                << (v->base_type != NULL ? v->base_type->id : 0)
                << ")";
        std::cout
            << std::endl;
#endif
     // In some rare cases, we might end up using dummies as function
     // arguments (some variable's address, for instance); when this
     // happens, the type shouldn't be a dummy after all.
        if (v->dummy == true)
            v->dummy = false;
        a->arg_types.push_back(v);

        t++;
    }
#endif
}

void
PointsToAnalysis::Implementation::debugDump(SgNode *node)
{
    class DebugDumper: private AstPrePostProcessing
    {
    public:
        void dump(SgNode *node)
        {
            stream << "|   " << node->unparseToString() << std::endl;
            stream << "|   ";
            traverse(node);
            stream << std::endl;
        }

        DebugDumper(std::ostream &s = std::cout)
          : stream(s)
        {
        }

    private:
        void preOrderVisit(SgNode *node)
        {
            stream << node->class_name();
            if (SgExpression *expr = isSgExpression(node))
                stream << "::" << expr->get_type()->class_name();
            stream << "( ";
        }

        void postOrderVisit(SgNode *)
        {
            stream << ") ";
        }

        std::ostream &stream;
    };

    std::cout << "+-- debug dump requested" << std::endl;
    DebugDumper dumper;
    dumper.dump(node);
    std::cout << "+-- debug dump done" << std::endl;
}

// See which one of the binary op's operands has a pointer or array type in
// the AST; we assume it's at most one, as in an add operation or an array
// access (where it's exactly one of the operands). Return a or b, depending
// on whether the pointer is the first or second operand; NULL if it's
// neither.
PointsToAnalysis::Location *
PointsToAnalysis::Implementation::pickThePointer(
        SgBinaryOp *binOp,
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
    Location *result = NULL;
    SgExpression *operand = NULL;
    bool isArrayType = false;

 // see if the first operand is the pointer; it might have typedefs,
 // references, modifiers wrapped around it
    operand = binOp->get_lhs_operand();
    SgType *t = operand->get_type();
    t = t->stripType(SgType::STRIP_MODIFIER_TYPE
                   | SgType::STRIP_REFERENCE_TYPE
                   | SgType::STRIP_TYPEDEF_TYPE);
    if (isSgPointerType(t) || isSgArrayType(t))
    {
        result = a;
     // We might have gotten a NULL argument, but we need the result to be
     // an actual location.
        if (result == NULL)
            result = createLocation();
        if (isSgArrayType(t))
            isArrayType = true;
    }
    else
    {
     // see if the second operand is the pointer, same game as above
        operand = binOp->get_rhs_operand();
        t = operand->get_type();
        t = t->stripType(SgType::STRIP_MODIFIER_TYPE
                       | SgType::STRIP_REFERENCE_TYPE
                       | SgType::STRIP_TYPEDEF_TYPE);
        if (isSgPointerType(t) || isSgArrayType(t))
        {
            result = b;
         // We might have gotten a NULL argument, but we need the result to
         // be an actual location.
            if (result == NULL)
                result = createLocation();
            if (isSgArrayType(t))
                isArrayType = true;
        }
    }

    if (result != NULL && !operand->attributeExists("SATIrE: call target"))
    {
     // If we identified one of the operands as a pointer, make sure that
     // the associated points-to location is a pointer location, i.e., that
     // it has a base location.
     // But only do this if the operand is not a tmpvar, which has a fake
     // pointer type.
#if VERBOSE_DEBUG
        std::cout << "pickThePointer: " << Ir::fragmentToString(binOp)
            << " / " << binOp->class_name() << std::endl;
#endif
        if (result->baseLocation() == NULL)
        {
            Location *arrayNode = createLocation();
            if (isArrayType)
            {
                arrayNode->array = true;
                arrayNode->symbols = result->symbols;
            }
            result->pointTo(arrayNode);
        }
    }

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::newAllocationSite()
{
    static int allocation_site = 0;
    std::stringstream name;
    name << "<allocation site " << allocation_site++ << ">";

    Sg_File_Info *symfinfo
        = Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
    SgName symname = name.str();

    SgInitializedName *initname
        = new SgInitializedName(symfinfo, symname, mainInfo->ptr_to_void_type,
                                NULL, NULL, NULL, NULL);
    SgVariableSymbol *sym = new SgVariableSymbol(initname);
#if HAVE_PAG
    addVariableIdForSymbol(sym);
#endif

#if VERBOSE_DEBUG
    std::cout
        << "created allocation variable " << name.str() << " with type "
        << (mainInfo->ptr_to_void_type != NULL
                ? Ir::fragmentToString(mainInfo->ptr_to_void_type)
                : "<null>")
        << std::endl;
#endif

 // The result of an allocator function is a pointer to the pseudo variable
 // denoting the allocation site.
    Location *result = createDummyLocation(symbol_location(sym));
    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::newSpecialFunctionContext(
        std::string name,
        PointsToAnalysis::Implementation::SynthesizedAttributesList synlist)
{
 // This function is the key to polymorphic treatment of some functions in
 // the analyzed program, and to providing summaries for external functions
 // called from the analyzed program.
 // For each function to be treated specially, we build a graph of locations
 // summarizing the function's effect on aliasing. Those parts of these
 // graphs that are not shared between two calls to
 // newSpecialFunctionContext are effectively context-sensitive (dependent
 // on the call site, to be more exact). For example, we create a new
 // pointer location for each call to malloc, that is, the memory regions
 // created by distinct allocation sites are disjoints.
 // Conversely, some locations may be shared between calls, in particular,
 // some global data structure the function accesses. Such locations should
 // not be recreated but rather shared.
 // The function contexts encoded here are not checked against any
 // implementation of the function that may also be available; this code
 // takes precedence wherever possible. Only direct calls to special
 // functions are treated specially, calls through function pointers must
 // fall back on the normal context-insensitive mechanism.

    Location *arg_dummy = synlist[SgFunctionCallExp_args];
    Location *return_location = NULL;

    if (name == "__assert_fail") {
     // __assert_fail never returns. So we return NULL.
        return_location = NULL;
    } else if (name == "__ctype_b_loc") {
     // This is apparently a function in glibc that returns a pointer to a
     // static lookup table to support the ctype.h functions.
        ensurePointerLocation(specialFunctionAuxLocation("ctype_b_loc_table"));
        return_location = specialFunctionAuxLocation("ctype_b_loc_table");
    } else if (name == "__errno_location") {
     // The same as for the ctype stuff.
        ensurePointerLocation(specialFunctionAuxLocation("errno_location"));
        return_location = specialFunctionAuxLocation("errno_location");
    } else if (name == "__fxstat") {
     // This is some internal stuff used by fstat. We are not interested in
     // the arguments and return nothing.
        return_location = NULL;
    } else if (name == "__lxstat") {
     // This is some internal stuff used by lstat. We are not interested in
     // the arguments and return nothing.
        return_location = NULL;
    } else if (name == "__xmknod") {
     // This is some internal stuff used by mknod. We are not interested in
     // the arguments and return nothing.
        return_location = NULL;
    } else if (name == "__xstat") {
     // This is some internal stuff used by stat. We are not interested in
     // the arguments and return nothing.
        return_location = NULL;
    } else if (name == "access") {
     // int access(const char *pathname, int mode);
     // Ignore everything, the return location is NULL (an integer that is
     // not a pointer).
        return_location = NULL;
    } else if (name == "atoi") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "close") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "exit") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "fclose") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "fcntl") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "fflush") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "fgets") {
     // Return the first argument (the same pointer). Make sure the first
     // argument exists and is a pointer location.
        ensureArgumentCount(arg_dummy, 3);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "fileno") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "fopen") {
     // Ignore the arguments, return a newly allocated FILE structure.
        return_location = newAllocationSite();
    } else if (name == "fprintf") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "fputc") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "free") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "fsync") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "ftruncate") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "getcwd") {
     // Return the first argument (the same pointer).
        ensureArgumentCount(arg_dummy, 2);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "getenv") {
     // This function may return the same string (the same pointer) at
     // several call sites, depending on the contents of the string it is
     // passed. Conservatively, we must assume that it returns the *same*
     // string at all sites, regardless of the argument.
        ensurePointerLocation(
                specialFunctionAuxLocation("getenv_return_location"));
        return_location = specialFunctionAuxLocation("getenv_return_location");
    } else if (name == "getpid") {
     // Return nothing.
        return_location = NULL;
    } else if (name == "getpwuid") {
     // The getpwuid function may return a pointer to a statically-allocated
     // structure, so we must assume that it will return the same pointer at
     // several call sites.
        ensurePointerLocation(
                specialFunctionAuxLocation("getpwuid_return_location"));
        return_location
            = specialFunctionAuxLocation("getpwuid_return_location");
    } else if (name == "gettimeofday") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "getuid") {
     // Return nothing.
        return_location = NULL;
    } else if (name == "isatty") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "localtime") {
     // This function returns a pointer to a statically-allocated structure.
        ensurePointerLocation(
                specialFunctionAuxLocation("localtime_return_location"));
        return_location
            = specialFunctionAuxLocation("localtime_return_location");
    } else if (name == "lseek") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "malloc") {
     // we ignore the size argument completely, it is not a pointer
     // the returned pointer is new for each context
        return_location = newAllocationSite();
    } else if (name == "memcmp") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "memcpy") {
     // We ignore the size argument (the third argument) completely; for the
     // purposes of memcpy, it can never be a pointer, and it never escapes
     // the memcpy function.
     // Make sure the first two arguments exist and are pointers.
        ensureArgumentCount(arg_dummy, 3);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        ensurePointerLocation(arg_dummy->arg_locations[1]);
     // The first and second arguments point to the same *values*, typically
     // in distinct memory regions.
        assign(arg_dummy->arg_locations[0]->baseLocation(),
               arg_dummy->arg_locations[1]->baseLocation());
     // The return location is aliased to the first argument.
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "memset") {
     // Return the first argument, ignore the others.
        ensureArgumentCount(arg_dummy, 3);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "open") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "printf") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "read") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "realloc") {
     // This function may return its first argument, or may return a newly
     // allocated memory region. In modeling this, we create a new region,
     // perform an assignment to the first argument, and return the result.
     // But note that the overall effect of this is the same as if we just
     // returned the first argument.
        ensureArgumentCount(arg_dummy, 2);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        Location *newArea = newAllocationSite();
        return_location = assign(arg_dummy->arg_locations[0], newArea);
    } else if (name == "signal") {
     // The signal function has interesting semantics, in that it can
     // indirectly cause calls to the function passed as its second
     // argument. But we cannot model that in any useful way, as far as I
     // can tell, so we just ignore this nuance altogether. However, signal
     // returns the previous handler, which means that that handler must be
     // stored inside, and must be aliased to both the second argument and
     // the return value.
        ensurePointerLocation(
                specialFunctionAuxLocation("sighandler_location"));
        ensureArgumentCount(arg_dummy, 2);
        ensurePointerLocation(arg_dummy->arg_locations[1]);
        assign(specialFunctionAuxLocation("sighandler_location"),
               arg_dummy->arg_locations[1]);
        return_location = specialFunctionAuxLocation("sighandler_location");
    } else if (name == "sleep") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "sprintf") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "strcat") {
     // This is sort of a copy of the second argument's pointed-to region
     // into the first; return the first argument.
        ensureArgumentCount(arg_dummy, 2);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        ensurePointerLocation(arg_dummy->arg_locations[1]);
        assign(arg_dummy->arg_locations[0]->baseLocation(),
               arg_dummy->arg_locations[1]->baseLocation());
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "strcmp") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "strcpy") {
     // This is sort of a copy of the second argument's pointed-to region
     // into the first; return the first argument.
        ensureArgumentCount(arg_dummy, 2);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        ensurePointerLocation(arg_dummy->arg_locations[1]);
        assign(arg_dummy->arg_locations[0]->baseLocation(),
               arg_dummy->arg_locations[1]->baseLocation());
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "strlen") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "strncmp") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    } else if (name == "strncpy") {
     // This is sort of a copy of the second argument's pointed-to region
     // into the first; return the first argument.
        ensureArgumentCount(arg_dummy, 3);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        ensurePointerLocation(arg_dummy->arg_locations[1]);
        assign(arg_dummy->arg_locations[0]->baseLocation(),
               arg_dummy->arg_locations[1]->baseLocation());
        return_location = arg_dummy->arg_locations[0];
    } else if (name == "time") {
     // The value of the region pointed to by the first argument is the same
     // as the return value. Nobody will ever dereference this, but while
     // we're at it...
        ensureArgumentCount(arg_dummy, 1);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = createLocation();
        return_location = assign(return_location,
                                 arg_dummy->arg_locations[0]->baseLocation());
    } else if (name == "tolower") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "toupper") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "unlink") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "write") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    }

 // Functions specific to sqlite3
    else if (name == "sqlite3FreeX") {
     // Ignore the argument, return nothing.
        return_location = NULL;
    } else if (name == "sqlite3Malloc") {
     // Return newly allocated memory region.
        return_location = newAllocationSite();
    } else if (name == "sqlite3MallocX") {
     // Return newly allocated memory region.
        return_location = newAllocationSite();
    } else if (name == "sqlite3MallocFailed") {
     // Do nothing.
        return_location = NULL;
    } else if (name == "sqlite3MallocRaw") {
     // Return newly allocated memory region. 
        return_location = newAllocationSite();
    } else if (name == "sqlite3StrDup") {
     // Return newly allocated memory region with the same contents as the
     // first argument.
        ensureArgumentCount(arg_dummy, 1);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = newAllocationSite();
        ensurePointerLocation(return_location);
        assign(return_location->baseLocation(),
               arg_dummy->arg_locations[0]->baseLocation());
    } else if (name == "sqlite3StrNDup") {
     // Return newly allocated memory region with the same contents as the
     // first argument.
        ensureArgumentCount(arg_dummy, 1);
        ensurePointerLocation(arg_dummy->arg_locations[0]);
        return_location = newAllocationSite();
        ensurePointerLocation(return_location);
        assign(return_location->baseLocation(),
               arg_dummy->arg_locations[0]->baseLocation());
    } else if (name == "sqlite3ErrorMsg") {
     // Ignore the arguments, return nothing.
        return_location = NULL;
    }

    else
    {
        std::cerr
            << "*** error: no function summary for special function "
            << name << std::endl;
        std::abort();
    }

    return return_location;
}

void
PointsToAnalysis::Implementation::ensurePointerLocation(Location * &location)
{
 // Note that this function takes a reference to a pointer and may modify
 // it! Make sure the given location exists and is a pointer location (i.e.,
 // it has a base location).
    if (location == NULL)
        location = createLocation();
    if (location->baseLocation() == NULL)
        location->pointTo(createLocation());
}

void
PointsToAnalysis::Implementation::ensureArgumentCount(
        Location *location, size_t n)
{
    while (location->arg_locations.size() < n)
        location->arg_locations.push_back(createLocation());
}

bool
PointsToAnalysis::Implementation::isStructMemberReference(SgVarRefExp *varRef)
{
 // Returns true iff this SgVarRefExp is a reference to a struct (or union,
 // I guess) member. This should be the case iff the expression is the rhs
 // operand of a dot or arrow expression.
    SgBinaryOp *parent = isSgBinaryOp(varRef->get_parent());
    bool result = (isSgDotExp(parent) || isSgArrowExp(parent))
                && parent->get_rhs_operand() == varRef;
    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::structMemberLocation(
        PointsToAnalysis::Location *structure,
        SgExpression *lhsExpr, SgVarRefExp *varRef)
{
    SgVariableSymbol *sym = varRef->get_symbol();
    Location *result = NULL;

#if DEBUG
    if (structure == NULL)
    {
        std::cout
            << "structMemberLocation called with NULL structure!"
            << std::endl;
    }
#endif

    structure = location_representative(structure);

 // Look up or create a member location for the given symbol in the given
 // structure. However, if the structure is collapsed, we do not create a
 // member but rather just yield the struct's location.
    if (!structure->collapsed)
    {
        if (isUnionReference(lhsExpr))
        {
         // If this is a union reference, collapse the location right away!
         // That is all the special handling that should be necessary for
         // unions, I think.
            collapseStructure(structure);
            result = structure;
        }
        else
        {
         // Otherwise, we are dealing with a normal struct.
         // We materialize the struct with the struct type on the lhs of the
         // dot or arrow expression. If it's already materialized, this is a
         // no-op. The materialization also takes care of checking the first
         // access type and collapsing the structure if necessary.
            SgClassType *struct_type
                = isSgClassType(lhsExpr->get_type()->findBaseType());
            materializeAllStructMembers(structure, struct_type);
         // Now that the structure is materialized, we can perform a simple
         // lookup.
            result = structMemberLocation(structure, sym);
#if DEBUG
            if (result == NULL)
            {
                std::cout
                    << "lookup of symbol "
                    << (void *) sym << " "
                    << (sym != NULL ? name(sym) : "<no name>")
                    << " failed in location " << structure->id << "!"
                    << std::endl;
                std::cout
                    << "structure: "
                    << name(isSgClassType(
                                lhsExpr->get_type()->findBaseType()))
                    << std::endl;
                std::cout
                    << "our type: "
                    << name(isSgClassType(structure->first_access_type))
                    << ", " << (structure->collapsed ? "" : "un")
                    << "collapsed"
                    << std::endl;
                std::cout
                    << "our members: "
                    << std::endl;
                std::map<SgSymbol *, Location *>::iterator m;
                std::map<SgSymbol *, Location *> &mems
                    = structure->struct_members;
                for (m = mems.begin(); m != mems.end(); ++m)
                {
                    SgSymbol *sym = m->first;
                    Location *t = m->second;
                    std::cout
                        << (void *) sym << " "
                        << (sym != NULL ? name(sym) : "<no name>")
                        << " -> "
                        << (void *) t << " "
                        << (t != NULL ? t->id : 0)
                        << std::endl;
                }
            }
#endif
        }
    }
    else
    {
     // Collapsed structure.
        result = structure;
    }

#if DEBUG
    if (result == NULL)
    {
        std::cout
            << "structMemberLocation returning NULL!"
            << std::endl;
    }
#endif

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::structMemberLocation(
        PointsToAnalysis::Location *structure, SgSymbol *member)
{
 // This function assumes that the structure has been completely
 // materialized already!
 // return structure->struct_members[member];
 // Need to handle collapsed structures as well.
    Location *result = NULL;
    if (structure->collapsed == true)
        result = structure;
    else
        result = structure->struct_members[member];
    return result;
}

void
PointsToAnalysis::Implementation::collapseStructure(
        PointsToAnalysis::Location *structure)
{
 // just to be sure
    if (structure == NULL)
        return;

    if (structure->collapsed == false)
    {
        structure->collapsed = true;
        std::map<SgSymbol *, Location *>::iterator m;
        std::map<SgSymbol *, Location *> &members = structure->struct_members;
     // recursively collapse members and unify with the structure itself
        for (m = members.begin(); m != members.end(); ++m)
        {
            collapseStructure(m->second);
            assign(structure, m->second);
        }
    }
}

bool
PointsToAnalysis::Implementation::isNonCollapsedStructure(
        PointsToAnalysis::Location *structure)
{
    bool result = structure != NULL
               && !structure->collapsed
               && !structure->struct_members.empty();
    return result;
}

#if 0
bool
PointsToAnalysis::Implementation::isCastFromStructPtrToIncompatibleType(
        SgCastExp *cast)
{
    bool result = false;
    SgType *cast_type = cast->get_type()->findBaseType();
    SgType *orig_type = cast->get_operand()->get_type()->findBaseType();
    if (isSgClassType(orig_type))
    {
#if VERBOSE_DEBUG
        std::cout
            << "(cast) " << cast->get_parent()->unparseToString()
            << std::endl;
        std::cout
            << "cast type: " << cast_type->unparseToString()
                << " @ " << (void *) cast_type
            << std::endl
            << "orig type: " << orig_type->unparseToString()
                << " @ " << (void *) orig_type
            << std::endl;
        std::cout << "----------" << std::endl;
#endif
     // Treat any other type as incompatible. Note, however, that the
     // findBaseType operation above will have stripped any const modifiers,
     // so casts to and from consts will not be treated as incompatible
     // struct casts. Which is good.
        if (cast_type != orig_type)
            result = true;
    }
    return result;
}

bool
PointsToAnalysis::Implementation::isCastFromIncompatibleTypeToStructPtr(
        SgCastExp *cast)
{
    bool result = false;
    SgType *cast_type = cast->get_type()->findBaseType();
    SgType *orig_type = cast->get_operand()->get_type()->findBaseType();
    if (isSgClassType(cast_type))
    {
#if VERBOSE_DEBUG
        std::cout
            << "(cast) " << cast->get_parent()->unparseToString()
            << std::endl;
        std::cout
            << "cast type: " << cast_type->unparseToString()
                << " @ " << (void *) cast_type
            << std::endl
            << "orig type: " << orig_type->unparseToString()
                << " @ " << (void *) orig_type
            << std::endl;
        std::cout << "----------" << std::endl;
#endif
     // Treat any other type as incompatible. Note, however, that the
     // findBaseType operation above will have stripped any const modifiers,
     // so casts to and from consts will not be treated as incompatible
     // struct casts. Which is good.
        if (cast_type != orig_type)
            result = true;
    }
    return result;
}
#endif

bool
PointsToAnalysis::Implementation::isUnionReference(SgExpression *expr)
{
    bool result = false;
    SgClassType *class_type
        = isSgClassType(expr->get_type()->findBaseType());
    if (class_type)
    {
        SgClassDeclaration *decl
            = isSgClassDeclaration(class_type->get_declaration());
        if (decl && decl->get_class_type() == SgClassDeclaration::e_union)
            result = true;
    }
    return result;
}

bool
PointsToAnalysis::Implementation::hasPointerMembers(Location *structure)
{
    bool result = false;

    std::map<SgSymbol *, Location *>::iterator m;
    std::map<SgSymbol *, Location *> &members = structure->struct_members;
    for (m = members.begin(); result == false && m != members.end(); ++m)
    {
        if (m->second != NULL && (m->second->baseLocation() != NULL
                               || m->second->return_location != NULL
                               || hasPointerMembers(m->second)))
        {
            result = true;
        }
    }

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createStructMemberLocation(
        PointsToAnalysis::Location *structure, SgSymbol *sym)
{
 // Note that distinct structure objects may have members with the same
 // symbol, which is why we do not use the symbol_location function here.
    Location *t = createLocation();
    t->symbols.push_back(sym);
    structure->struct_members[sym] = t;
    t->parent_struct = structure;
#if VERBOSE_DEBUG
    std::cout
        << "created struct member location "
        << structure->id << "->" << t->id
        << " " << name(sym)
        << std::endl;
#endif
    return t;
}

SgDeclarationStatementPtrList &
PointsToAnalysis::Implementation::membersOfExpressionClass(SgExpression *expr)
{
    SgClassType *class_type
        = isSgClassType(expr->get_type()->findBaseType());
    if (class_type)
    {
        SgDeclarationStatement *type_decl = class_type->get_declaration();
        SgClassDeclaration *class_decl
            = isSgClassDeclaration(type_decl->get_definingDeclaration());
        if (class_decl)
        {
            SgClassDefinition *class_def = class_decl->get_definition();
            return class_def->get_members();
        }
    }

 // Can't get here. That is, we *shouldn't* get here, but who knows what
 // interesting feature of C, or ROSE's implementation of it, we are
 // overlooking.
    std::cerr
        << "*** error in points-to analysis: can't compute list of members "
        << "of class type for expression " << expr->unparseToString()
        << std::endl;
    std::abort();
}

std::vector<SgSymbol *>
PointsToAnalysis::Implementation::getVariableSymbolsFromDeclaration(
        SgDeclarationStatement *d)
{
    SgVariableDeclaration *vardecl = isSgVariableDeclaration(d);
    std::vector<SgSymbol *> syms;
    SgInitializedNamePtrList &initnames = vardecl->get_variables();
    SgInitializedNamePtrList::iterator i;
    for (i = initnames.begin(); i != initnames.end(); ++i)
        syms.push_back((*i)->get_symbol_from_symbol_table());
    return syms;
}

std::vector<SgSymbol *>
PointsToAnalysis::Implementation::classTypeMemberVariables(
        SgClassType *class_type)
{
#if VERBOSE_DEBUG
    std::cout
        << "members of class type: " << name(class_type)
        << std::endl;
#endif
    std::vector<SgSymbol *> result;
    SgDeclarationStatement *type_decl = class_type->get_declaration();
#if VERBOSE_DEBUG
    std::cerr
        << "type " << (void *) class_type << " '" << name(class_type) << "'"
        << " has decl: " << (void *) type_decl
        << ", defining decl: "
        << (void *) type_decl->get_definingDeclaration()
        << std::endl;
#endif
    SgClassDeclaration *class_decl
        = isSgClassDeclaration(type_decl->get_definingDeclaration());
    if (class_decl != NULL)
    {
        SgClassDefinition *class_def = class_decl->get_definition();
        SgDeclarationStatementPtrList &members = class_def->get_members();
        SgDeclarationStatementPtrList::iterator d;
        for (d = members.begin(); d != members.end(); ++d)
        {
            if (isSgVariableDeclaration(*d))
            {
                std::vector<SgSymbol *> syms
                    = getVariableSymbolsFromDeclaration(*d);
                std::copy(syms.begin(), syms.end(),
                          std::back_inserter(result));
#if VERBOSE_DEBUG
                std::cout
                    << "| added " << names(syms)
                    << std::endl;
#endif
            }
        }
    }
    else
    {
#if VERBOSE_DEBUG
     // As far as I can tell, this can only happen for external variable
     // declarations, which can be done without a type definition.
     // Otherwise, the frontend should catch any aggregate type usage
     // without a type definition.
        std::cout
            << "*** warning: class type '" << name(class_type) << "'"
            << " has no definition"
            << std::endl;
#endif
    }
    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::initializerAssignment(
        SgType *type,
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
    Location *result = NULL;
    type = type->stripTypedefsAndModifiers();

#if VERBOSE_DEBUG
    std::cout
        << "initializer assignment for type "
        << type->unparseToString()
        << ", variable location: " << a->id
        << ", initializer location: " << (b != NULL ? b->id : 0)
        << std::endl;
#endif

    if (b != NULL)
    {
        if (SgClassType *class_type = isSgClassType(type))
            result = initializeClass(class_type, a, b);
        else if (SgArrayType *array_type = isSgArrayType(type))
            result = initializeArray(array_type, a, b);
        else
         // non-aggregate initializer: base case
            result = assign(a, b);

     // Free b, if it's a dummy. It might have been a recursive dummy, but
     // then the recursive calls to initializerAssignment (inside
     // initializeClass or initializeArray) have already taken care of
     // freeing the deeper levels.
        if (b->dummy)
            freeDummyLocation(b);
    }
    else
     // non-aggregate case, no initializer
        result = a;

    return result;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::initializeClass(
        SgClassType *class_type,
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
    if (a->struct_members.empty())
        materializeAllStructMembers(a, class_type);
    std::vector<SgSymbol *> members = classTypeMemberVariables(class_type);
 // b is a dummy location containing the initializers for the class members;
 // use these in turn to initialize the corresponding members. Stop when at
 // the end of b's list; there might be fewer initializers than members, but
 // not the other way round.
    std::vector<SgSymbol *>::iterator member = members.begin();
    std::vector<Location *>::iterator i;
    for (i = b->arg_locations.begin(); i != b->arg_locations.end(); ++i)
    {
        SgVariableSymbol *sym = isSgVariableSymbol(*member++);
        Location *m = structMemberLocation(a, sym);
        initializerAssignment(sym->get_type(), m, *i);
    }
    return a;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::initializeArray(
        SgArrayType *array_type,
        PointsToAnalysis::Location *a, PointsToAnalysis::Location *b)
{
    if (a->baseLocation() == NULL)
        a->pointTo(createLocation());
    Location *elements = a->baseLocation();
 // b is a dummy location containing the array elements; use these in turn
 // to initialize elements. This means that each array element will be
 // unified with the single array base location, i.e. we collapse the whole
 // array.
    std::vector<Location *>::iterator i;
    for (i = b->arg_locations.begin(); i != b->arg_locations.end(); ++i)
        initializerAssignment(array_type->get_base_type(), elements, *i);
    return a;
}

void
PointsToAnalysis::Implementation::materializeAllStructMembers(
        PointsToAnalysis::Location *structure, SgClassType *class_type)
{
 // Check or set the structure's type.
    checkOrSetFirstAccessType(structure, class_type);
    if (structure->struct_members.empty())
    {
     // Materialize the members, if the structure is not collapsed now.
        if (structure->collapsed == false)
        {
            std::vector<SgSymbol *> members
                = classTypeMemberVariables(class_type);
            std::vector<SgSymbol *>::iterator s;
            for (s = members.begin(); s != members.end(); ++s)
                createStructMemberLocation(structure, *s);
        }
    }
}

void
PointsToAnalysis::Implementation::checkOrSetFirstAccessType(
        PointsToAnalysis::Location *a, SgType *access_type)
{
 // This function checks whether the current access to node a, through an
 // expression of type access_type, conforms to the previous accesses to a.
 // If there was no previous access, we simply set the first_access_type; if
 // the first or the current access is through a structure type, and the
 // types are incompatible, the struct must be collapsed.
    if (a->first_access_type == NULL)
        a->first_access_type = access_type;
    else if (a->first_access_type != access_type
      && (isSgClassType(a->first_access_type)
          || isSgClassType(access_type)))
    {
     // Found a node that is accessed both as a struct and some other type
     // (possibly an incompatible struct). Collapse it.
#if VERBOSE_DEBUG
        if (a->collapsed == false)
        {
            std::cout
                << (a->collapsed ? "collapsed" : "uncollapsed")
                << " object has first access type "
                << a->first_access_type->unparseToString()
                << ", accessed as "
                << access_type->unparseToString()
                << " -> collapsing"
                << std::endl;
        }
#endif
        collapseStructure(a);
    }
}

SgSymbol *
PointsToAnalysis::Implementation::findSymbolForInitializedName(
        SgInitializedName *initname)
{
    SgSymbol *sym = NULL;
    sym = initname->get_symbol_from_symbol_table();
    if (sym == NULL)
    {
        std::string name = Utils::name(initname);
        std::string mangled_name = Utils::mangled_name(initname);
        SgScopeStatement *scope = initname->get_scope();
#if VERBOSE_DEBUG
        std::cout
            << "looking for variable with name '"
            << name << "' in scope "
            << (void *) scope
            << " " << scope->class_name()
            << std::endl;
        if (name == "" || mangled_name == "")
        {
            std::cout
                << "something's empty! name: '" << name
                << "', mangled name: '" << mangled_name
                << "', unparsed: '" << initname->unparseToString()
                << "'"
                << std::endl;
        }
#endif
        while (sym == NULL && scope != NULL)
        {
            sym = scope->lookup_symbol(name);
            if (sym == NULL)
            {
            // Try searching in the next enclosing scope.
                SgNode *p = scope->get_parent();
                while (p != NULL && !isSgScopeStatement(p))
                    p = p->get_parent();
                scope = isSgScopeStatement(p);
            }
        }
    }
#if VERBOSE_DEBUG
    std::cout
        << "findSymbolForInitializedName: returning "
        << (void *) sym << std::endl;
#endif
    return sym;
}

void
PointsToAnalysis::Implementation::run(Program *program)
{
 // SgProject *ast = program->astRoot; // actually: run on program->icfg

 // traverse(ast);
 // IcfgTraversal::traverse(program->icfg);
    run(program->icfg);
}

void
PointsToAnalysis::Implementation::run(CFG *icfg)
{
    if (contextSensitive)
    {
#if HAVE_PAG
        TimingPerformance t("Context-sensitive points-to analysis setup:");
     // Make sure the ICFG has context information
        if (icfg->contextInformation == NULL)
            icfg->contextInformation = new ContextInformation(icfg);
     // Allocate a PointsToInformation object for each context we will be
     // looking at.
        const std::vector<ContextInformation::Context> &contexts
            = icfg->contextInformation->allContexts();
        std::vector<ContextInformation::Context>::const_iterator c;
        for (c = contexts.begin(); c != contexts.end(); ++c)
        {
            PointsToInformation *info = new PointsToInformation();
            std::stringstream s;
            s << c->procnum << "_" << c->position << "_";
            info->prefix = s.str();
            info->context = new ContextInformation::Context(*c);
            allInfos[*c] = info;
        }
#endif
    }

    TimingPerformance *timer
        = new TimingPerformance("Points-to analysis traversal:");
    IcfgTraversal::traverse(icfg);
    delete timer;

 // After the traversal, we need to finalize the call graph by resolving
 // function symbols from function expression location nodes. To do this,
 // iterate over each entry in callLocationMap, which maps expressions to
 // points-to locations; simply extract the set of all function symbols in
 // each location, and insert it into the callGraph's callCandidateMap.
    timer = new TimingPerformance("Call graph finalization:");
    PointsToInformation::CallLocationMap::iterator t;
    for (t = info->callLocationMap.begin();
         t != info->callLocationMap.end();
         ++t)
    {
        SgExpression *expr = t->first;
     // The default constructor will create an empty set in the map for us.
        std::set<SgFunctionSymbol *> &functionSyms
            = info->callGraph.callCandidateMap[expr];
     // There is a special case here: If the expression is a function ref
     // expression, then it is statically bound to its function, even if the
     // points-to location node for that function may be shared for several
     // functions.
        if (SgFunctionRefExp *fref = isSgFunctionRefExp(expr))
        {
            functionSyms.insert(canonicalSymbol(fref->get_symbol()));
#if VERBOSE_DEBUG
            std::cout
                << "| recorded: " << Ir::fragmentToString(expr)
                << " (static call)"
                << std::endl;
#endif
        }
        else
        {
         // Normal case: Take the symbols from the function node.
            Location *func_location = location_representative(t->second);
            std::list<SgFunctionSymbol *>::iterator sym;
            std::list<SgFunctionSymbol *> &syms = func_location->func_symbols;
            for (sym = syms.begin(); sym != syms.end(); ++sym)
            {
                functionSyms.insert(canonicalSymbol(*sym));
#if VERBOSE_DEBUG
                std::cout
                    << "| recorded: " << Ir::fragmentToString(expr)
                    << " (location " << func_location->id << ")"
                    << "  -->  " << Utils::name(*sym)
                    << std::endl;
#endif
            }
        }
    }
    delete timer;

#if VERBOSE_DEBUG
 // This should be somewhere else, I think. Maybe. Actually, debug code
 // should be nowhere, because there should be no bugs. Anyway, dump some
 // information on the canonical and redundant function symbols.
 // First, make sure we have the same number of entries in the
 // function_locations and canonicalFSymbols maps; there should be one
 // each for each function defined/declared/referenced/whatever in the
 // program.
    size_t function_locations_size = info->function_locations.size();
    size_t canonical_size = info->canonicalFSymbols.size();
    std::cout
        << "function_locations: " << function_locations_size << " entries, "
        << "canonicalFSymbols: " << canonical_size << " entries, "
        << "which is "
        << (function_locations_size == canonical_size ? "good" : "VERY BAD!")
        << std::endl;
    if (function_locations_size != canonical_size)
    {
        std::cout
            << "functions in function_locations that are missing in "
            << "canonicalFSymbols:"
            << std::endl;
        std::map<std::string, Location *>::iterator d;
        for (d = info->function_locations.begin();
             d != info->function_locations.end(); ++d)
        {
            std::map<std::string, SgFunctionSymbol *>::iterator p;
            p = info->canonicalFSymbols.find(d->first);
            if (p == info->canonicalFSymbols.end())
            {
                std::cout
                    << "    " << d->first
                    << std::endl;
            }
        }
        std::cout
            << std::endl;
    }
    std::map<std::string, SgFunctionSymbol *>::iterator f;
    for (f = info->canonicalFSymbols.begin();
         f != info->canonicalFSymbols.end(); ++f)
    {
        std::string fname = f->first;
        SgFunctionSymbol *sym = f->second;
        std::cout
            << "function " << fname << " has canonical symbol: "
            << (void *) sym
            << std::endl;
        std::cout
            << "    mangled name: "
            << Utils::mangled_name(sym->get_declaration())
            << std::endl;
        std::map<std::string, std::set<SgFunctionSymbol *> >::iterator r;
        r = info->redundantFunctionSymbols.find(fname);
        if (r != info->redundantFunctionSymbols.end())
        {
            std::set<SgFunctionSymbol *> &fss = r->second;
            std::set<SgFunctionSymbol *>::iterator fs;
            std::cout
                << "    redundant symbols for " << fname << ":";
            for (fs = fss.begin(); fs != fss.end(); ++fs)
            {
                std::cout
                    << " " << (void *) *fs;
            }
            std::cout
                << std::endl;
        }
        else
        {
            std::cout
                << "    no redundant function symbols for " << fname
                << std::endl;
        }
    }
#endif
}

void
PointsToAnalysis::Implementation::print()
{
    TimingPerformance timer("Points-to analysis, printing:");
    print(std::cout, "", "", "");
}

void
PointsToAnalysis::Implementation::doDot(std::string filename)
{
    TimingPerformance timer("Points-to analysis, printing (DOT file):");
    std::string dotfilename = filename + ".dot";
    std::ofstream dotfile(dotfilename.c_str());
    dotfile << "digraph " << filename << " {" << std::endl;

    if (!contextSensitive)
    {
        print(dotfile, "\"", "\"", ";");
    }
    else
    {
#if HAVE_PAG
        info = mainInfo;
        print(dotfile, "\"", "\"", ";");
        const std::vector<ContextInformation::Context> &contexts
            = get_icfg()->contextInformation->allContexts();
        std::vector<ContextInformation::Context>::const_iterator c;
        for (c = contexts.begin(); c != contexts.end(); ++c)
        {
            dotfile
                << "subgraph cluster_context_"
                << c->procnum << "_" << c->position << " {" << std::endl
                << "label = \"" << c->toString() << "\""
                << ";" << std::endl;
            info = allInfos[*c];
            print(dotfile, "\"", "\"", ";");
            dotfile << "}" << std::endl;
        }
#endif
    }

    dotfile << "}" << std::endl;
    std::stringstream dotcommand;
    dotcommand
        << "dot -Tps " << dotfilename << " > " << filename << ".eps";
     // << "fdp -Tps " << dotfilename << " > " << filename << ".eps";
    std::system(dotcommand.str().c_str());
}

PointsToAnalysis::Implementation::Implementation(bool contextSensitive)
  : info(new PointsToAnalysis::PointsToInformation()),
    auxiliaryTraversal(new Implementation(info, this, info)),
    icfgTraversal(this), contextSensitive(contextSensitive), mainInfo(info)
{
 // The constructor without arguments constructs a "fresh" instance with a
 // new info object. It also creates an auxiliary implementation instance
 // which shares the info object.

    info->ptr_to_void_type = SgTypeVoid::createType()->get_ptr_to();
    info->integerConstantLocation = createLiteralLocation();
    info->stringConstantLocation = createLiteralLocation();
}

PointsToAnalysis::Implementation::Implementation(
        PointsToAnalysis::PointsToInformation *info,
        PointsToAnalysis::Implementation *icfgTraversal,
        PointsToAnalysis::PointsToInformation *mainInfo)
  : info(info),
    auxiliaryTraversal(NULL),
    icfgTraversal(icfgTraversal),
    contextSensitive(false), // this instance works with a given info object
    mainInfo(mainInfo)
{
 // The private constructor with an info argument constructs an "auxiliary"
 // instance of the implementation class, sharing an existing info object.
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::symbol_location(SgSymbol *sym)
{
    if (SgFunctionSymbol *func_sym = isSgFunctionSymbol(sym))
        return functionSymbol_location(func_sym);

    if (sym == NULL)
    {
     // Symbols may be NULL, in particular unnamed function parameters;
     // what do we do? Creating a dummy location should be harmless, it will
     // be unified with the real symbol's location eventually (if there is a
     // real symbol, at least...)
        Location *t = createLocation();
#if DEBUG
        std::cout << "created location " << t->id << " for NULL sym"
            << std::endl;
#endif
     // Can't happen; and if it can, then we need to deal with this in some
     // other way.
        std::abort();
        return t;
    }

    std::map<SgSymbol *, Location *>::iterator i;
    i = info->symbol_locations.find(sym);
    if (i != info->symbol_locations.end())
    {
        Location *location = i->second;
#if VERBOSE_DEBUG
        std::cout
            << "found location " << location->id
            << " for symbol " << name(sym)
            << std::endl;
#endif
        return location;
    }
    else
    {
        Location *location = createLocation();
        location->symbols.push_back(sym);
        info->symbol_locations[sym] = location;
#if VERBOSE_DEBUG
        std::cout
            << "created location " << location->id
            << " for symbol " << name(sym)
            << std::endl;
#endif
     // Materialize variable symbols referring to structs right away.
     // This is because in struct assignments, all members must be
     // present.
        if (SgVariableSymbol *varsym = isSgVariableSymbol(sym))
        {
            SgClassType *class_type = isSgClassType(varsym->get_type());
            if (varsym->get_type() == NULL)
            {
#if DEBUG
                std::cout
                    << "** panic: variable symbol without a type"
                    << std::endl;
#endif
            }
            if (class_type != NULL)
                materializeAllStructMembers(location, class_type);
        }
        return location;
    }
}

#if HAVE_PAG
PointsToAnalysis::Location *
PointsToAnalysis::Implementation::symbol_location(
        SgSymbol *sym, const ContextInformation::Context &ctx)
{
    PointsToInformation *old_info = info;
    info = allInfos[ctx];
    Location *result = symbol_location(sym);
    info = old_info;
    return result;
}
#endif

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::function_location(
        SgFunctionDeclaration *fd, Location *argDummy)
{
 // std::string funcname = Utils::mangled_name(fd);
 // std::string nonmangledname = name(fd);
    std::string funcname = name(fd);
    std::string nonmangledname = funcname;
    std::string mangledname = Utils::mangled_name(fd);

#if VERBOSE_DEBUG
    static std::map<std::string, std::string> func_mnames;
    std::map<std::string, std::string>::iterator fmn;
    fmn = func_mnames.find(funcname);
    if (fmn != func_mnames.end())
    {
        std::string stored_mname = fmn->second;
        if (stored_mname != mangledname)
        {
            std::cout
                << "strange: function '" << funcname
                << "' previously stored with mangled name '"
                << stored_mname << "', now has mangled name '"
                << mangledname << "'!"
                << std::endl;
        }
    }
    else
    {
        info->func_mnames[funcname] = mangledname;
    }
#endif

    std::map<std::string, Location *>::iterator i;
    i = info->function_locations.find(funcname);
    if (i != info->function_locations.end())
    {
        Location *location = i->second;
#if VERBOSE_DEBUG
        std::cout
            << "info " << (void *) info
            << " map " << (void *) &info->function_locations << ": "
                << std::endl << "    "
            << "found location " << location->id
                << " (" << (void *) location << ")"
            << " for function " << funcname
            << " in context " << (info->prefix == "" ? "main" : info->prefix)
            << std::endl;
        std::cout
            << "    "
            << "location: " << location-> id
            << "/" << names(location->symbols)
            << "/" << names(location->func_symbols)
            << std::endl;
#endif
        return location;
    }
    else if (argDummy != NULL)
    {
     // Create a new function location for a declaration, with the given
     // argument locations.
     // any mention of a function must be treated like a pointer to the
     // function!
        Location *func_location = createFunctionLocation();
        Location *location = createLocation(func_location);
#if VERBOSE_DEBUG
        std::cout
            << "info " << (void *) info
            << " map " << (void *) &info->function_locations << ": "
                << std::endl << "    "
            << "created location " << location->id
                << " (" << (void *) location << ")"
            << " for function " << funcname
            << " in context " << (info->prefix == "" ? "main" : info->prefix)
            << std::endl;
        std::cout
            << "    "
            << "location: " << location-> id
            << "/" << names(location->symbols)
            << "/" << names(location->func_symbols)
            << std::endl;
#endif

     // setup argument list from the argDummy location
        std::vector<Location *> &args = argDummy->arg_locations;
        std::vector<Location *>::iterator a;
        for (a = args.begin(); a != args.end(); ++a)
            func_location->arg_locations.push_back(*a);
     // record the ellipsis location for this function, if any
        SgInitializedNamePtrList &params = fd->get_args();
        if (!params.empty() && isSgTypeEllipse(params.back()->get_type()))
        {
            assert(func_location->arg_locations.back() != NULL);
            func_location->ellipsis_location
                = func_location->arg_locations.back();
#if DEBUG
            std::cout
                << "setting ellipsis location for function location "
                << func_location->id << ": ellipsis is location "
                << func_location->ellipsis_location->id
                << std::endl;
#endif
        }

     // set special_function flag if necessary
        determineSpecialFunction(nonmangledname, func_location);

     // store the symbol association
        SgSymbol *funcsym = fd->get_symbol_from_symbol_table();
        if (funcsym == NULL)
            funcsym = findFunctionSymbolFromDeclaration(fd);
        if (funcsym != NULL)
        {
         // For functions, we store symbols in two places: There is a symbol
         // in the "normal" symbols list of the location that points to the
         // function location; this is also the location the symbol maps to
         // in the symbol_locations map. But we also record the function's
         // name in the function node itself, in the func_symbols list.
            SgFunctionSymbol *ffuncsym = isSgFunctionSymbol(funcsym);
            if (ffuncsym == NULL)
            {
                std::cerr
                    << "*** error: symbol " << name(funcsym)
                    << " should be a function symbol, is: "
                    << funcsym->class_name()
                    << std::endl;
                std::abort();
            }
            else
            {
                ffuncsym = canonicalSymbol(ffuncsym);
                location->symbols.push_back(ffuncsym);
                info->symbol_locations[ffuncsym] = location;
                func_location->func_symbols.push_back(ffuncsym);
            }
        }
        else
        {
#if DEBUG
            std::cout
                << "found NULL function symbol for function "
                << funcname << std::endl;
            std::abort();
#endif
        }
        info->function_locations[funcname] = location;
     // mainInfo->function_locations[funcname] = location;
#if VERBOSE_DEBUG
        std::cout
            << "created location " << location->id
            << " for function " << funcname
            << std::endl;
#endif
        return location;
    }
    else // argDummy == NULL, mention of external function
    {
#if VERBOSE_DEBUG
        std::cout
            << "points-to info: using function summary "
            << "for external function " << funcname << std::endl;
#endif
        return mainInfo->functionSummaryNode;
    }
}

void
PointsToAnalysis::Implementation::determineSpecialFunction(
        std::string name, Location *func_location)
{
    std::set<std::string>::iterator pos;
    pos = info->specialFunctionNames.find(name);
    if (pos != info->specialFunctionNames.end())
        func_location->special_function = true;

#if 0
    if (name == "malloc")
        func_type->special_function = true;
    else if (name == "memcpy")
        func_type->special_function = true;
    else if (name == "access")
        func_type->special_function = true;
#endif

#if VERBOSE_DEBUG
    if (func_location->special_function == true)
    {
        std::cout
            << "determined special function status for function "
            << name << std::endl;
    }
    else
    {
        std::cout
            << name << " is not a special function" << std::endl;
    }
#endif
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createLocation(
        Location *t, Location *return_location)
{
    Location *location = new Location(t, return_location);
    location->ownerInfo = info;
    if (info != mainInfo)
        info->locations.push_back(location);
    mainInfo->locations.push_back(location);
    location->id = mainInfo->locations.size();
    mainInfo->disjointSets.make_set(location);
#if VERBOSE_DEBUG
    std::cout
        << "created new location " << location->id
        << " with base location " << (t != NULL ? t->id : 0)
        << ", return location "
            << (return_location != NULL ? return_location->id : 0)
        << ", representative "
            << (location_representative(location) != NULL
                    ? location_representative(location)->id : 0)
        << std::endl;
#endif
 // if (location->id == 17) std::abort();
    return location;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createLocation()
{
    return createLocation(NULL);
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createFunctionLocation()
{
    return createLocation(NULL, createLocation());
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createLiteralLocation()
{
    Location *t = createLocation();
    t->literal = true;
    return t;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::createDummyLocation(Location *base)
{
    Location *t = createLocation(base);
    t->dummy = true;
    return t;
}

void
PointsToAnalysis::Implementation::freeDummyLocation(Location *t)
{
 // GB (2009-04-17): All this manual garbage collection business does not
 // work very well (double frees). So let's not do it anymore, and maybe
 // debug it at some point using valgrind.
#if 0
    int id = t->id;
    if (!t->dummy)
    {
        std::cerr
            << "*** error: freeDummyLocation applied to non-dummy "
            << id << std::endl;
    }
    if (mainInfo->locations[id-1] != t)
    {
        std::cerr
            << "*** error: location " << id << " not present at index "
            << id-1 << ", which holds "
            << (mainInfo->locations[id-1] != NULL
                    ? mainInfo->locations[id-1]->id : 0)
            << " instead"
            << std::endl;
        return; // skip the freeing!
    }
    delete t;
    mainInfo->locations[id-1] = NULL;
#endif
}

const std::vector<PointsToAnalysis::Location *> &
PointsToAnalysis::Implementation::get_locations() const
{
    return mainInfo->locations;
}

bool
PointsToAnalysis::Implementation::mayBeAliased(
        PointsToAnalysis::Location *loc) const
{
    return loc->mayBeAliased;
}

bool
PointsToAnalysis::Implementation::isArrayLocation(
        PointsToAnalysis::Location *loc) const
{
    return loc->array;
}

const std::list<SgSymbol *> &
PointsToAnalysis::Implementation::location_symbols(
        PointsToAnalysis::Location *loc) const
{
    return loc->symbols;
}

const std::list<SgFunctionSymbol *> &
PointsToAnalysis::Implementation::location_funcsymbols(
        PointsToAnalysis::Location *loc) const
{
    return loc->func_symbols;
}

unsigned long
PointsToAnalysis::Implementation::location_id(
        PointsToAnalysis::Location *loc) const
{
    return loc->id;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::location_representative(
        PointsToAnalysis::Location *loc) const
{
#if VERBOSE_DEBUG
    std::cout
        << "finding location " << (loc != NULL ? loc->id : 0)
        << std::endl;
    if (loc != NULL)
        std::cout << "syms: " << names(loc->symbols) << std::endl;
#endif
    Location *r = mainInfo->disjointSets.find_set(loc);
#if VERBOSE_DEBUG
    std::cout
        << " => " << (void *) r << " "
        << (r != NULL ? r->id : 0)
        << std::endl;
#endif
 // if (r == NULL)
 //     r = mainInfo->disjointSets.find_set(loc);
    if (loc != NULL && r == NULL)
    {
     // r = loc;
        std::cerr
            << "*** no representative for loc "
            << (loc != NULL ? loc->id : 0)
            << " in mainInfo = " << (void *) mainInfo
            << std::endl;
        std::abort();
    }

#if VERBOSE_DEBUG
    if (loc != NULL)
    {
        std::cout
            << "representative: " << loc->id << " => " << r->id
            << std::endl;
    }
#endif

    return r;
}

void
PointsToAnalysis::Implementation::print(
        std::ostream &stream, std::string pre, std::string post,
        std::string lineend)
{
    std::vector<Location *>::iterator t;
    for (t = info->locations.begin(); t != info->locations.end(); ++t)
    {
        Location *location = *t;

        if (location == NULL)
            continue;
     // Avoid doing locations twice -- they are each registered in the
     // mainInfo's locations, and (typically) also within some function.
        if (location->ownerInfo != info)
            continue;

     // show only nonempty locations?
        bool show_this_location = false;
     // show always?
        show_this_location = true;
     // if (!location->symbols.empty()) show_this_location = true;
        if (show_this_location)
        {
         // location node
#if VERBOSE_DEBUG
            std::cout
                << "showing location "
                << location->ownerInfo->prefix << location->id
                << ": "
                    << (location->return_location != NULL ? "hexagon" :
                       !location->struct_members.empty() ? "house" : "box")
                << std::endl;
#endif
            stream
                << pre << location->ownerInfo->prefix << location->id << post
                << " [shape="
                 // show functions as hexagons, structs as houses, other
                 // nodes as boxes
                    << (location->return_location != NULL ? "hexagon" :
                       !location->struct_members.empty() ? "house" : "box")
                << ", label=\""
                    << location->id
                    << (!location->symbols.empty() ? ": " : "")
                    << names(location->symbols)
                    << "; "
                    << names(location->func_symbols)
#if VERBOSE_DEBUG
                    << "\\n" << (void *) location
#endif
                    << (location->mayBeAliased ? "\\naliased" : "")
                    << (location->array ? "\\narray" : "")
                    << (location->functionSummary ? "\\nfunctionSummary" : "")
                    << (location->literal ? "\\nliteral" : "")
                    << (location->special_function ? "\\n[special]" : "")
                    << (location->collapsed ? "\\n[collapsed]" : "")
                    << (location->collapseBase? "\\n[collapseBase]" : "")
                    << "\""
             // there should be no dummy locations anymore, show as red
                << (location->dummy ? ", color=red" : "")
                << "]"
                << lineend << std::endl;
         // edge from location to base location, if any
            if (location->baseLocation() != NULL)
            {
                Location *base
                    = location_representative(location->baseLocation());
#if VERBOSE_DEBUG
                std::cout
                    << (void *) location << " -b-> "
                    << (void *) location->baseLocation() << " -r-> "
                    << (void *) base
                    << std::endl;
#endif
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << base->ownerInfo->prefix << base->id << post
                    << lineend << std::endl;
            }
         // edges from location to argument locations, if any
            std::vector<Location *>::iterator a;
            int n = 0;
            for (a = location->arg_locations.begin();
                 a != location->arg_locations.end(); ++a)
            {
                if (*a == NULL)
                    continue;
                Location *arg = location_representative(*a);
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << arg->ownerInfo->prefix << arg->id << post
                    << " [color=green, label=\"arg_" << n++ << "\"]"
                    << lineend << std::endl;
            }
         // edge from location to return location, if any
            if (location->return_location != NULL)
            {
                Location *base
                    = location_representative(location->return_location);
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << base->ownerInfo->prefix << base->id << post
                    << " [color=gold, label=\"ret\"]"
                    << lineend << std::endl;
            }
         // edge from location to equivalence class representative, if
         // different
            Location *ecr = location_representative(location);
            if (location != ecr)
            {
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << ecr->ownerInfo->prefix << ecr->id << post
                    << " [style=dashed]"
                    << lineend << std::endl;
            }
         // edges to struct member locations
            std::map<SgSymbol *, Location *>::iterator m;
            std::map<SgSymbol *, Location *> &members
                = location->struct_members;
            for (m = members.begin(); m != members.end(); ++m)
            {
                std::string name = Utils::name(m->first);
                Location *member = m->second;
                if (member == NULL)
                {
                    std::cout
                        << "*** found NULL struct member in location "
                        << location->id << "!"
                        << std::endl;
                    std::abort();
                }
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << member->ownerInfo->prefix << member->id << post
                    << " [color=firebrick4, label=\"" << name << "\"]"
                    << lineend << std::endl;
            }
         // edges to pending join partners; skip join partners for the
         // integer and string location!
            if (location != info->integerConstantLocation
                && location != info->stringConstantLocation)
            {
                std::list<Location *>::iterator p;
                std::list<Location *> &pending = location->pending;
                for (p = pending.begin(); p != pending.end(); ++p)
                {
                    stream
                        << pre << location->ownerInfo->prefix << location->id
                        << post
                        << " -> "
                        << pre << (*p)->ownerInfo->prefix << (*p)->id << post
                        << " [color=blue, constraint=false]"
                        << lineend << std::endl;
                }
            }
         // edge to enclosing struct, if any
            if (location->parent_struct != NULL)
            {
                stream
                    << pre << location->ownerInfo->prefix << location->id
                    << post
                    << " -> "
                    << pre << location->parent_struct->ownerInfo->prefix
                        << location->parent_struct->id << post
                    << " [color=gainsboro, constraint=false]"
                    << lineend << std::endl;
            }
        }
    }

    std::map<SgSymbol *, Location *>::iterator i;
    for (i = mainInfo->symbol_locations.begin();
         i != mainInfo->symbol_locations.end(); ++i)
    {
        SgSymbol *var = i->first;
        Location *location = location_representative(i->second);

     // Only show symbols if there is a problem, namely if the
     // symbol is not contained in its location's symbols list.
        std::list<SgSymbol *>::iterator pos;
        pos = find(location->symbols.begin(), location->symbols.end(), var);
        if (pos == location->symbols.end())
        {
         // symbol node
            stream
                << pre << name(var) << post
                << lineend << std::endl;
         // edge from symbol to location
            stream
                << pre << name(var) << post
                << " -> "
                << pre << location->ownerInfo->prefix << location->id << post
                << " [style=dotted, color=red]"
                << lineend << std::endl;
        }
     // maybe TODO: it is also a problem if a pointer with a function symbol
     // points to some node that does not contain that function symbol in
     // the func_symbols list
    }
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::expressionLocation(SgExpression *expr)
{
 // This method computes the location for the given expression which refers
 // to a function (such as the "function" child of a function call
 // expression).
    if (expr == NULL)
    {
        std::cerr
            << "*** error: trying to find location for NULL expression!"
            << std::endl;
        std::abort();
    }

 // If we don't have an auxiliary traversal, create one. In general, it
 // seems that sometimes (for $tmpvars, although we try to avoid these) the
 // "main" traversal's auxiliary traversal might need one. This is not a
 // problem, but we also ensure that nobody tries to create an unreasonable
 // number of these.
    if (auxiliaryTraversal == NULL)
    {
        auxiliaryTraversal = new Implementation(info, this, mainInfo);
        info->auxctr++;
        if (info->auxctr >= 10)
        {
            std::cerr
                << "*** error: SATIrE points-to analysis internal error: "
                << "auxctr >= 10, this probably means infinite recursion"
                << std::endl;
            std::abort();
        }
    }

 // Make sure the auxiliaryTraversal runs in the correct context.
    PointsToInformation *auxInfo = auxiliaryTraversal->info;
    auxiliaryTraversal->info = info;
#if VERBOSE_DEBUG
    std::cout
        << "for expr " << Ir::fragmentToString(expr) << ": "
        << "starting aux traversal with info replacement "
        << (auxInfo->prefix != "" ? auxInfo->prefix : "main")
        << " -> "
        << (info->prefix != "" ? info->prefix : "main")
        << std::endl;
#endif
    Location *result = auxiliaryTraversal->
                            AstBottomUpProcessing<Location *>::traverse(expr);
    auxiliaryTraversal->info = auxInfo;

#if VERBOSE_DEBUG
    if (result == NULL)
    {
     // Not sure whether this should be enabled in general. The output might
     // annoy users, but it may be interesting to developers.
        std::cerr
            << "*** warning: tried to determine location for expr '"
            << Ir::fragmentToString(expr)
            << "' (" << expr->class_name() << ")"
            << ", got NULL!"
            << std::endl;
    }
#endif

    if (result != NULL)
    {
     // Make sure to return a canonical representative.
        result = location_representative(result);

#if DEBUG
        if (result == NULL)
        {
            std::cerr
                << "*** warning: NULL representative location for expr '"
                << Ir::fragmentToString(expr)
                << "' (" << expr->class_name() << ")"
                << std::endl;
        }
#endif
    }

 // If this expression refers to a function, make sure the location we are
 // returning is a function location and has at least as many argument
 // locations as dictated by the static type of the expression.
    if (SgFunctionType *ft = isSgFunctionType(expr->get_type()))
    {
        assert(result != NULL);
        if (result->baseLocation() == NULL)
            result->pointTo(createFunctionLocation());
        if (result->baseLocation()->return_location == NULL)
            result->baseLocation()->return_location = createLocation();
        ensureArgumentCount(result->baseLocation(),
                            ft->get_arguments().size());
    }

#if VERBOSE_DEBUG
    std::cout
        << "+-----" << std::endl
        << "result = " << (void *) result
        << ", baseLocation = "
        << (void *) (result != NULL ? result->baseLocation() : NULL)
        << std::endl;
    std::cout
        << "expr: "
        << Ir::fragmentToString(expr)
        << " (" << expr->class_name() << ")"
        << " type: " << expr->get_type()->class_name()
        << std::endl
        << "+-----"
        << std::endl;
#endif

    return result;
}

#if HAVE_PAG
PointsToAnalysis::Location *
PointsToAnalysis::Implementation::expressionLocation(
        SgExpression *expr, const ContextInformation::Context &ctx)
{
    PointsToInformation *old_info = info;
    info = allInfos[ctx];
    Location *result = expressionLocation(expr);
    info = old_info;
    return result;
}
#endif

std::string
PointsToAnalysis::Implementation::locationAttributeString(
        PointsToAnalysis::Location *location)
{
    location = location_representative(location);
    std::stringstream ss;
    ss << "location " << location->id;
    return ss.str();
}

const PointsToAnalysis::CallGraph &
PointsToAnalysis::Implementation::getCallGraph()
{
    return mainInfo->callGraph;
}

// unused
// PointsToAnalysis::PointsToInformation *
// PointsToAnalysis::Implementation::getPointsToInformation() const
// {
//     return info;
// }

#if 0 && DEPRECATED
void
PointsToAnalysis::Implementation::unifyReturnLocation(
        PointsToAnalysis::Location *return_location)
{
    Location *b;
    std::list<Location *>::iterator t = return_locations.begin();
    while (t != return_locations.end())
    {
        b = *t;
#if VERBOSE_DEBUG
        std::cout
            << "function def return join: "
            << "return location: " << a->return_location->id << ", "
            << "body location: " << b->id
            << std::endl;
#endif
        join(return_location, b);
        if (b != NULL && b->dummy == true && b != return_location)
        {
         // Normally, the assign function takes care of freeing an
         // unused dummy return location, but here we are calling join
         // directly.
#if VERBOSE_DEBUG
            std::cout
                << "b with id " << b->id
                << " is a useless dummy after return join"
                << std::endl;
#endif
            freeDummyLocation(b);
        }
        ++t;
    }
}
#endif

void
PointsToAnalysis::Implementation::performFunctionEndBookkeeping(
        SgFunctionDeclaration *fdecl)
{
 // This function deletes/clears/empties a few things that are only of local
 // interest within each function in the source code, and may not escape and
 // be mixed with the information for other functions.

#if 0 && DEPRECATED
 // Clear the ellipsis location for this function; argument lists must have
 // been unified before!
    current_ellipsis_location = NULL;
#endif
#if 0 && DEPRECATED
 // Clear the list of return locations for this function; the return
 // locations must have been unified before!
    return_locations.clear();
#endif
#if 0 && DEPRECATED
 // Associate all call site expressions within the function with the
 // function's symbol, if possible; then clear the list of call sites. This
 // only needs to be done if this function declaration is a defining
 // declaration.
    if (fdecl->get_definition() != NULL)
    {
        SgFunctionSymbol *sym
            = isSgFunctionSymbol(fdecl->get_symbol_from_symbol_table());
        if (sym == NULL)
            sym = findFunctionSymbolFromDeclaration(fdecl);
        if (sym != NULL)
        {
            sym = canonicalSymbol(sym);
         // Store the callSiteExpressions. But do not overwrite an existing
         // nonempty set of callSiteExpressions!
            CallGraph::CallSiteExpressionMap::iterator pos;
            pos = callGraph.callSiteExpressionMap.find(sym);
            if (pos != callGraph.callSiteExpressionMap.end())
            {
                if (pos->second.empty())
                {
                    callGraph.callSiteExpressionMap[sym] = callSiteExpressions;
                }
                else
                {
#if DEBUG
                    std::cout
                        << "for function: " << name(fdecl) << std::endl
                        << " NOT overwriting callSiteExpression set of size "
                        << pos->second.size()
                        << " with set of size "
                        << callSiteExpressions.size()
                        << std::endl;
#endif
                }
            }
            else
            {
             // No call site set for this function yet.
                callGraph.callSiteExpressionMap[sym] = callSiteExpressions;
            }
        }
        else
        {
#if DEBUG
            std::cerr
                << "*** NULL function symbol for function '"
                << name(fdecl) << "', symbol from table: "
                << (void *) fdecl->get_symbol_from_symbol_table()
                << std::endl;
            std::abort();
#endif
        }
    }
    callSiteExpressions.clear();
#endif
}

SgType *
PointsToAnalysis::Implementation::stripOnePointerLayer(
        SgType *arg_type)
{
    SgType *type = arg_type;
    type = type->stripType(SgType::STRIP_MODIFIER_TYPE
                         | SgType::STRIP_REFERENCE_TYPE
                         | SgType::STRIP_TYPEDEF_TYPE);
    if (!isSgPointerType(type) && !isSgArrayType(type))
    {
        std::cerr
            << "*** error: type " << (void *) type
            << " '" << type->unparseToString()
            << "' should be a pointer or array type! is: "
            << type->class_name()
            << std::endl;
        std::abort();
    }

    type = type->dereference();
    type = type->stripType(SgType::STRIP_MODIFIER_TYPE
                         | SgType::STRIP_REFERENCE_TYPE
                         | SgType::STRIP_TYPEDEF_TYPE);

#if VERBOSE_DEBUG
    std::cout
        << "stripped type '" << arg_type->unparseToString() << "' ("
        << arg_type->class_name() << ") to type '"
        << type->unparseToString() << "' (" << type->class_name() << ")"
        << std::endl;
#endif

    return type;
}

SgFunctionSymbol *
PointsToAnalysis::Implementation::findFunctionSymbolFromDeclaration(
        SgFunctionDeclaration *fd)
{
    SgFunctionSymbol *sym
        = isSgFunctionSymbol(fd->get_symbol_from_symbol_table());
    if (sym == NULL)
    {
        std::string funcname = Utils::name(fd);
#if VERBOSE_DEBUG
        std::cout
            << "looking for function with name '"
            << funcname << "'" << std::endl;
#endif
        SgScopeStatement *scope = fd->get_scope();
#if VERBOSE_DEBUG
        std::cout
            << "decl scope: " << (void *) scope
            << " parent: " << (void *) fd->get_parent()
            << " " << fd->get_parent()->class_name()
            << std::endl;
#endif
        while (sym == NULL && scope != NULL)
        {
            sym = isSgFunctionSymbol(scope->lookup_function_symbol(funcname));
            if (sym == NULL)
            {
#if VERBOSE_DEBUG
                std::cout
                    << "was in scope " << (void *) scope;
#endif
             // If we didn't find the symbol, try the next enclosing scope.
                SgNode *p = scope->get_parent();
                while (p != NULL && !isSgScopeStatement(p))
                    p = p->get_parent();
                scope = isSgScopeStatement(p);
#if VERBOSE_DEBUG
                std::cout
                    << ", now trying scope " << (void *) scope
                    << std::endl;
#endif
            }
        }
    }
    sym = canonicalSymbol(sym);
#if VERBOSE_DEBUG
    std::cout
        << "findFunctionSymbolFromDeclaration: returning "
        << (void *) sym << std::endl;
#endif
    return sym;
}

PointsToAnalysis::Location *
PointsToAnalysis::Implementation::functionSymbol_location(
        SgFunctionSymbol *fsym)
{
    SgFunctionDeclaration *fd = canonicalSymbol(fsym)->get_declaration();
    Location *result = function_location(fd);
    return result;
}

SgFunctionSymbol *
PointsToAnalysis::Implementation::canonicalSymbol(SgFunctionSymbol *fsym)
{
 // ROSE does not always provide unique symbols for one and the same
 // function, even with AST merge. This function maps function symbols by
 // name to some canonical representative.
    SgFunctionSymbol *result = NULL;
    std::string fname = Utils::name(fsym);
    std::map<std::string, SgFunctionSymbol *>::iterator fpos;
    fpos = mainInfo->canonicalFSymbols.find(fname);
    if (fpos != mainInfo->canonicalFSymbols.end())
    {
     // We found the canonical symbol for this function.
        result = fpos->second;
     // If this function symbol for this function has not been seen before,
     // record it.
        if (fsym != result)
        {
            std::map<std::string, std::set<SgFunctionSymbol *> >::iterator
                redundantfpos;
            redundantfpos = mainInfo->redundantFunctionSymbols.find(fname);
            if (redundantfpos != mainInfo->redundantFunctionSymbols.end())
            {
             // We already have a set for these function symbols, insert our
             // symbol.
                std::set<SgFunctionSymbol *> &funcs = redundantfpos->second;
                funcs.insert(fsym);
            }
            else
            {
             // We do not yet have a set for these function symbols, insert
             // a new one containing our symbol.
                std::set<SgFunctionSymbol *> fsymset;
                fsymset.insert(fsym);
                mainInfo->redundantFunctionSymbols[fname] = fsymset;
            }
        }
    }
    else
    {
     // We do not yet have a canonical symbol for this function; the symbol
     // we got as argument will be the canonical one.
        mainInfo->canonicalFSymbols[fname] = fsym;
        result = fsym;
    }
    if (result == NULL)
    {
        std::cerr
            << "*** internal error: could not compute a canonical symbol "
            << "for function " << fname << std::endl;
        std::abort();
    }
    return result;
}

// This function returns the actual function location (*not* the
// pointer-to-function location) for the function with a given procnum.
PointsToAnalysis::Location *
PointsToAnalysis::Implementation::functionLocationForProcnum(int procnum)
{
    return mainInfo->procedureLocations[procnum]->baseLocation();
}

// Returns the pointer-to-function location for the named special function
// in the current info. Each info has its own collection of these. Since the
// name is not unique (there might be several references/call sites to the
// same function), we also use an address (of the FunctionRefExp, usually)
// to make the key unique.
PointsToAnalysis::Location *
PointsToAnalysis::Implementation::specialFunctionLocation(
        void *address, std::string name, size_t arity)
{
    Location *result = NULL;
    std::map<std::pair<void *, std::string>, Location *>::iterator pos;

    pos = info->specialFunctionLocations.find(std::make_pair(address, name));
    if (pos != info->specialFunctionLocations.end())
    {
        result = pos->second;
#if VERBOSE_DEBUG
        std::cout
            << "! stored location for func " << name << ": " << result->id
            << " base: " << result->baseLocation()->id
            << " ret: " << result->baseLocation()->return_location->id
            << std::endl;
#endif
    }
    else
    {
     // At position SgFunctionCallExp_args, there must be an argDummy
     // location with the correct number of args; newSpecialFunctionContext
     // returns the corresponding return location. Wrap all this in a
     // function location and store the pointer to it.
        Location *func_loc = createFunctionLocation();
        func_loc->arg_locations = std::vector<Location *>(arity);
        SynthesizedAttributesList args(SgFunctionCallExp_args + 1);
        args[SgFunctionCallExp_args] = func_loc;
        func_loc->return_location = newSpecialFunctionContext(name, args);
     // newSpecialFunctionContext returns dummies because they were
     // originally meant to be assigned right away; however, here we store
     // the location permanently, so we make it a non-dummy.
        if (func_loc->return_location != NULL)
            func_loc->return_location->dummy = false;
#if VERBOSE_DEBUG
        std::cout
            << "!!! special return location for func "
            << name << "@" << address << ": "
            << (func_loc->return_location != NULL
                    ? func_loc->return_location->id
                    : 0)
            << std::endl;
#endif

        result = createLocation(func_loc);
        info->specialFunctionLocations[std::make_pair(address, name)] = result;
    }

    return result;
}

PointsToAnalysis::Location * &
PointsToAnalysis::Implementation::specialFunctionAuxLocation(std::string name)
{
    std::map<std::string, Location *>::iterator pos;

    pos = mainInfo->specialFunctionAuxLocations.find(name);
    if (pos != mainInfo->specialFunctionAuxLocations.end())
        return pos->second;
    else
    {
        mainInfo->specialFunctionAuxLocations[name] = NULL;
        return mainInfo->specialFunctionAuxLocations[name];
    }
}



PointsToAnalysis::PointsToInformation::PointsToInformation()
  : rankMap(), parentMap(),
    disjointSets(RankMap(rankMap), ParentMap(parentMap)),
    integerConstantLocation(NULL),
    stringConstantLocation(NULL),
    functionSummaryNode(NULL),
    auxctr(1)
#if HAVE_PAG
    , context(NULL)
#endif
{
    specialFunctionNames.insert("__assert_fail");
    specialFunctionNames.insert("__ctype_b_loc");
    specialFunctionNames.insert("__errno_location");
    specialFunctionNames.insert("__fxstat");
    specialFunctionNames.insert("__lxstat");
    specialFunctionNames.insert("__xmknod");
    specialFunctionNames.insert("__xstat");
    specialFunctionNames.insert("access");
    specialFunctionNames.insert("atoi");
    specialFunctionNames.insert("close");
    specialFunctionNames.insert("exit");
    specialFunctionNames.insert("fclose");
    specialFunctionNames.insert("fcntl");
    specialFunctionNames.insert("fflush");
    specialFunctionNames.insert("fgets");
    specialFunctionNames.insert("fileno");
    specialFunctionNames.insert("fopen");
    specialFunctionNames.insert("fprintf");
    specialFunctionNames.insert("fputc");
    specialFunctionNames.insert("free");
    specialFunctionNames.insert("fsync");
    specialFunctionNames.insert("ftruncate");
    specialFunctionNames.insert("getcwd");
    specialFunctionNames.insert("getenv");
    specialFunctionNames.insert("getpid");
    specialFunctionNames.insert("getpwuid");
    specialFunctionNames.insert("gettimeofday");
    specialFunctionNames.insert("getuid");
    specialFunctionNames.insert("isatty");
    specialFunctionNames.insert("localtime");
    specialFunctionNames.insert("lseek");
    specialFunctionNames.insert("malloc");
    specialFunctionNames.insert("memcmp");
    specialFunctionNames.insert("memcpy");
    specialFunctionNames.insert("memset");
    specialFunctionNames.insert("open");
    specialFunctionNames.insert("printf");
    specialFunctionNames.insert("read");
    specialFunctionNames.insert("realloc");
    specialFunctionNames.insert("signal");
    specialFunctionNames.insert("sleep");
    specialFunctionNames.insert("sprintf");
    specialFunctionNames.insert("strcat");
    specialFunctionNames.insert("strcmp");
    specialFunctionNames.insert("strcpy");
    specialFunctionNames.insert("strlen");
    specialFunctionNames.insert("strncmp");
    specialFunctionNames.insert("strncpy");
    specialFunctionNames.insert("time");
    specialFunctionNames.insert("tolower");
    specialFunctionNames.insert("toupper");
    specialFunctionNames.insert("unlink");
    specialFunctionNames.insert("write");

 // Functions specific to sqlite3
 // First iteration: Memory management and error handling.
    specialFunctionNames.insert("sqlite3FreeX");
    specialFunctionNames.insert("sqlite3Malloc"); // two args
    specialFunctionNames.insert("sqlite3MallocX"); // one arg
    specialFunctionNames.insert("sqlite3MallocFailed");
    specialFunctionNames.insert("sqlite3MallocRaw");
    specialFunctionNames.insert("sqlite3StrDup");
    specialFunctionNames.insert("sqlite3StrNDup");
    specialFunctionNames.insert("sqlite3ErrorMsg");

 // Second iteration: Maybe OS-specific stuff? Or StrICmp?
}


} // namespace Analyses

} // namespace SATIrE

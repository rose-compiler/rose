
#include "MatlabTypeInference.h"

#include "rose.h"
#include "sageGeneric.h"

#include "MatlabParser.h"

#include "FunctionAnalyzer.h"
#include "VariableDeclarationBuilder.h"
#include "BottomUpTypeAnalysis.h"
#include "MatlabVariables.h"
#include "FastNumericsRoseSupport.h"
#include "utility/utils.h"

namespace si = SageInterface;
namespace sb = SageBuilder;
namespace ru = RoseUtils;

static
MatlabAnalysis::NameToDeclarationMap
collectNameToDeclarationPair(const Rose_STL_Container<SgFunctionDeclaration*>& matlabFunctions)
{
  return FastNumericsRoseSupport::collectNameToDeclarationPair(matlabFunctions);
}

MatlabAnalysis::NameToDeclarationMap
parseBuiltinDeclarations(std::string basedir)
{
  typedef Rose_STL_Container<SgFunctionDeclaration*> DeclContainer;

  std::string   mfile  = basedir + "/../../support/matlab/builtins.m";
  std::string   stubs  = basedir + "/../../support/";

  char*         argv[] = { const_cast<char*>(""),
                           const_cast<char*>("-I"),
                           const_cast<char*>(stubs.c_str()),
                           const_cast<char*>(mfile.c_str())
                         };
  int           argc   = sizeof(argv) / sizeof(char*);
  SgProject*    builtinProject  = MatlabParser::frontend(argc, argv);
  DeclContainer matlabFunctions = FastNumericsRoseSupport::getMatlabFunctions(builtinProject);

  return collectNameToDeclarationPair(matlabFunctions);
}


struct UnrealVarFinder : AstSimpleProcessing
{
    typedef MatlabAnalysis::NameToDeclarationMap NameToDeclarationMap;

    explicit
    UnrealVarFinder(NameToDeclarationMap funDecls)
    : ids(), matlabFunDecls(funDecls)
    {}

    virtual void visit(SgNode* n);

    operator std::vector<SgVarRefExp*>()
    {
      // return std::move(ids);
      return ids;
    }

  private:
    std::vector<SgVarRefExp*> ids;
    NameToDeclarationMap      matlabFunDecls;
};

struct UnrealVarHandler : sg::DispatchHandler<SgVarRefExp*>
{
    typedef sg::DispatchHandler<SgVarRefExp*>     base;
    typedef UnrealVarFinder::NameToDeclarationMap NameToDeclarationMap;

    explicit
    UnrealVarHandler(const NameToDeclarationMap& funDecls)
    : base(0), matlabFunDecls(funDecls)
    {}

    bool hasBuiltInFunction(std::string n)
    {
      NameToDeclarationMap::const_iterator pos = matlabFunDecls.find(n);

      // \pp \todo test for zero-arity
      return pos != matlabFunDecls.end();
    }

    void handle(SgNode&) {}

    void handle(SgVarRefExp& n)
    {
      if (  hasBuiltInFunction(ru::nameOf(n))
         && !isSgCallExpression(n.get_parent())
         )
      {
        res = &n;
      }
    }

  private:
    const NameToDeclarationMap& matlabFunDecls;
};


void UnrealVarFinder::visit(SgNode* n)
{
  SgVarRefExp* exp = sg::dispatch(UnrealVarHandler(matlabFunDecls), n);

  if (exp) ids.push_back(exp);
}


static
std::vector<SgVarRefExp*>
findUnrealVars(SgProject* proj, MatlabAnalysis::NameToDeclarationMap matlabFunDecls)
{
  UnrealVarFinder uvf(matlabFunDecls);

  uvf.traverse(proj, preorder);
  return uvf;
}

static
SgVarRefExp*
createFunctionSymbol(SgVarRefExp* n)
{
  SgVarRefExp* res = isSgVarRefExp(si::deepCopy(n));

  ROSE_ASSERT(res);
  return res;
}

static
void _makeFunCalls(SgVarRefExp* n)
{
  ROSE_ASSERT(n);

  SgFunctionCallExp* call = sb::buildFunctionCallExp(createFunctionSymbol(n));
  ROSE_ASSERT(call);

  std::cerr << "repl " << n->unparseToString() << " / " << call->unparseToString() << std::endl;
  si::replaceExpression(n, call);
}

static
void makeFunCalls(const std::vector<SgVarRefExp*>& unrealVars)
{
  std::for_each(unrealVars.begin(), unrealVars.end(), _makeFunCalls);
}


//
// function family to remove cloned declarations

static
void removeClonedFunctions_r(MatlabAnalysis::MatlabFunctionRec& fn)
{
  // if the original definition was not used
  if (!fn.second)
  {
    si::removeStatement(fn.first);
  }
}


static
void removeClonedFunctions_s(MatlabAnalysis::NameToDeclarationMap::value_type& overloads)
{
  std::for_each( overloads.second.begin(),
                 overloads.second.end(),
                 removeClonedFunctions_r
               );
}


/**
 * Removes those declarations that were cloned.
 * This means we don't need the original function any more.
 * Also builds variable declarations in all the functions
 */
static inline
void removeClonedFunctions(MatlabAnalysis::NameToDeclarationMap& nameToFuncDeclarations)
{
  std::for_each(nameToFuncDeclarations.begin(), nameToFuncDeclarations.end(), removeClonedFunctions_s);
}

static inline
void _buildVariableDeclarations(SgNode* n)
{
  MatlabAnalysis::buildVariableDeclarations(isSgFunctionDeclaration(n));
}


/**
 * insert variable declarations in the cloned functions.
 */
static inline
void insertVariablesIntoFunctions(SgProject* proj)
{
  Rose_STL_Container<SgNode*> fundecls = NodeQuery::querySubTree(proj, V_SgFunctionDeclaration);

  std::for_each(fundecls.begin(), fundecls.end(), _buildVariableDeclarations);
}


static inline
void revertForLoopChanges(SgProject *project)
{
  Rose_STL_Container<SgNode*> allMatlabFor = NodeQuery::querySubTree(project, V_SgMatlabForStatement);

  for(size_t i = 0; i < allMatlabFor.size(); ++i)
  {
    SgMatlabForStatement* matlabFor = isSgMatlabForStatement(allMatlabFor[i]);
    SgAssignOp*           assignOp = isSgAssignOp(matlabFor->get_range());
    SgExpression*         index = assignOp->get_lhs_operand();

    matlabFor->set_index(index);
    index->set_parent(matlabFor);

    SgExpression*         range = assignOp->get_rhs_operand();

    matlabFor->set_range(range);
    range->set_parent(matlabFor);
  }
}


namespace MatlabAnalysis
{
#if OBSOLETE_CODE
  
  // \todo check if this is needed
  void addAssignOpBeforeMatlabForStatement(SgProject *project)
  {
    Rose_STL_Container<SgNode*> allMatlabFor = NodeQuery::querySubTree(project, V_SgMatlabForStatement);

    for(size_t i = 0; i < allMatlabFor.size(); ++i)
    {
      SgMatlabForStatement* matlabFor = isSgMatlabForStatement(allMatlabFor[i]);
      SgExpression*         index = matlabFor->get_index();
      SgExpression*         range = matlabFor->get_range();
      SgAssignOp*           assignOp = sb::buildAssignOp(index, range);

      matlabFor->set_range(assignOp);
      assignOp->set_parent(matlabFor);
    }
  }

#endif /* OBSOLETE_CODE */

  static inline
  bool
  isPossibleEntryPoint(SgFunctionDeclaration* f)
  {
    ROSE_ASSERT(f);

    return (sg::deref(f->get_parameterList()).get_args().size() == 0);
  }


  static inline
  bool
  isPossibleEntryPoint(MatlabFunctionRec& rec)
  {
    return isPossibleEntryPoint(rec.first);
  }


  void typeAnalysis(SgProject* project)
  {
    typedef Rose_STL_Container<SgFunctionDeclaration*> DeclContainer;

    DeclContainer          matlabFunctions = FastNumericsRoseSupport::getMatlabFunctions(project);

    MatlabVariables::setTypes(project);

    // There should be at least one function
    if (matlabFunctions.size() == 0) return;

    // Create a map from function name to the corresponding defining function declarations
    // This will be used when cloning functions
    Ctx::nameToFunctionDeclaration = collectNameToDeclarationPair(matlabFunctions);

    // In a matlab file, the top function is always the main function.
    // As a common practice its name usually matches the filename
    SgFunctionDeclaration* mainFunction = matlabFunctions[0];
    MatlabOverloadSet&     overload = Ctx::nameToFunctionDeclaration[mainFunction->get_name()];

    ROSE_ASSERT(overload.size() == 1);
    MatlabFunctionRec&     mainRec  = overload.back();

    if (!hasBeenAnalyzed(mainRec))
    {
      ROSE_ASSERT(isPossibleEntryPoint(mainRec));

      setAnalyzed(mainRec);
      FunctionAnalyzer functionAnalyzer(project);

      functionAnalyzer.analyse_function(mainFunction);
    }

/*
 *  \todo go through all entry points in a Matlab module
 *
    NameToDeclarationMap::iterator aa = Ctx::nameToFunctionDeclaration.begin();
    NameToDeclarationMap::iterator zz = Ctx::nameToFunctionDeclaration.end();

    if (aa != zz)
    {
      MatlabOverloadSet::iterator osaa = (*aa).second.begin();
      MatlabOverloadSet::iterator oszz = (*aa).second.end();

      if (osaa != oszz)
      {
        MatlabFunctionRec& fn = (*osaa);



        ++osaa;
      }

      ++aa;
    }
*/
    // revertForLoopChanges(project);
    // generateDOT(*project);

    // The original functions from which we cloned need to be removed
    //  functionDeclarations
    removeClonedFunctions(Ctx::nameToFunctionDeclaration);

    // insert variables into cloned functions
    insertVariablesIntoFunctions(project);
  }

  void makeFunFromUnrealVars(SgProject* proj)
  {
    typedef Rose_STL_Container<SgFunctionDeclaration*> DeclContainer;

    NameToDeclarationMap  allMatlabFunctionDeclarations = Ctx::matlabBuiltins;
    const size_t          numBuiltIns = allMatlabFunctionDeclarations.size();

    DeclContainer         matlabFunctions = FastNumericsRoseSupport::getMatlabFunctions(proj);
    NameToDeclarationMap  projFunctionDeclarations = collectNameToDeclarationPair(matlabFunctions);
    const size_t          numUserFuns = projFunctionDeclarations.size();

    allMatlabFunctionDeclarations.insert( projFunctionDeclarations.begin(),
                                          projFunctionDeclarations.end()
                                        );

    //~ std::cerr << numBuiltIns << " + " << numUserFuns
              //~ << " == " << allMatlabFunctionDeclarations.size() << "?"
              //~ << std::endl;
    ROSE_ASSERT(numBuiltIns + numUserFuns == allMatlabFunctionDeclarations.size());
    makeFunCalls(findUnrealVars(proj, allMatlabFunctionDeclarations));
  }

  void loadMatlabBuiltins(std::string basedir)
  {
    Ctx::matlabBuiltins = parseBuiltinDeclarations(basedir);
  }
}

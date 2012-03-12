#include "satire.h"

#include "o_Location.h"

namespace SATIrE
{

Program::Program(AnalyzerOptions *o)
  : options(o), astRoot(createRoseAst(o)), icfg(NULL), prologTerm(NULL)
{
    analysisScheduler.registerProgram(this);

    fixVariableSymbols();

    if (options->buildIcfg())
        icfg = createICFG(this, options);

    if (options->getAralInputFileName() != "")
        attachAralInformation(this, options);

 // Run any provided analyzers that were specified on the command line.
 // Logically, this is part of program construction because the user asked
 // for an annotated program.
    const std::vector<DataFlowAnalysis *> &analyzers
        = o->getDataFlowAnalyzers();
    std::vector<DataFlowAnalysis *>::const_iterator a;
    for (a = analyzers.begin() ; a != analyzers.end(); ++a)
    {
      if (*a != NULL /* empty analyzer */) {
        (*a)->run(this);
        (*a)->processResultsWithPrefix(this, false);
      }
    }
}

Program::~Program()
{
    analysisScheduler.unregisterProgram(this);
}

SgVariableSymbol *
Program::get_symbol(SgInitializedName *initName)
{
    SgVariableSymbol *result = NULL;
    std::string name = initName->get_name().str();

    SgVariableDeclaration *vardecl
        = isSgVariableDeclaration(initName->get_declaration());
    if ((vardecl && isSgGlobal(vardecl->get_parent()))
     // SATIrE's tempvars are considered global
        || name.find("$") != std::string::npos)
    {
        assert(name != "");
        if (!initName->get_storageModifier().isStatic())
        {
            global_map_t::iterator g;
            g = global_map.find(name);
            assert(g != global_map.end());
            result = g->second.first;
        }
        else
        {
            static_map_t::iterator s;
            SgFile *file = traceBackToFileNode(vardecl);
            assert(file != NULL);
            s = static_map.find(std::make_pair(name, file));
            assert(s != static_map.end());
            result = s->second.first;
        }
        assert(result != NULL);
    }
    else
        result = isSgVariableSymbol(initName->get_symbol_from_symbol_table());

    if (vardecl && isSgGlobal(vardecl->get_parent()))
        assert(result != NULL);
    if (result == NULL && vardecl != NULL && name != "")
    {
        std::cerr
            << "*** error: failed to lookup symbol for variable "
            << name << "@" << (void *) initName
            << std::endl;
        assert(result != NULL);
    }
    return result;
}

SgVariableSymbol *
Program::get_symbol(SgVarRefExp *varRef)
{
    SgVariableSymbol *result = NULL;

 // Special case for "this" variables for C++ support (strictly speaking,
 // this breaks correct handling of global C variables named "this").
    if (varRef->get_symbol()->get_name().str() == std::string("this"))
        return varRef->get_symbol();

    SgInitializedName *initName = varRef->get_symbol()->get_declaration();
    result = get_symbol(initName);

    if (result == NULL)
    {
#if 0
        std::cout
            << "NULL symbol for varRef " << Ir::fragmentToString(varRef)
            << std::endl;
        std::cout
            << "varRef " << (void *) varRef << std::endl
            << "symbol " << (void *) varRef->get_symbol() << std::endl
            << "initName " << (void *) initName
            << std::endl;
#endif
     // The lookup above fails for struct members. Let's assume this is one
     // and fall back to the symbol stored in the struct...
        result = varRef->get_symbol();
    }

    assert(result != NULL);
    return result;
}

void
Program::fixVariableSymbols(void)
{
 // Visit all global variable declarations, and where symbols are present,
 // fill the maps. If there is already an entry in the map, do nothing; that
 // entry will be "canonical".
    class GlobalDeclarationCollector: public ROSE_VisitTraversal
    {
    public:
        GlobalDeclarationCollector(Program *program)
         : program(program)
        {
        }

        void visit(SgNode *node)
        {
            Program::global_map_t::const_iterator global;
            Program::static_map_t::const_iterator statc;

            SgVariableDeclaration *decl = isSgVariableDeclaration(node);
            assert(decl != NULL);
            SgInitializedName *initname = decl->get_variables().front();
            if (isSgGlobal(decl->get_scope()))
            {
                assert(initname != NULL);
                std::string name = initname->get_name().str();
                SgVariableSymbol *sym = isSgVariableSymbol(
                        initname->get_symbol_from_symbol_table());
                if (sym != NULL)
                {
                    if (!initname->get_storageModifier().isStatic())
                    {
                        global = program->global_map.find(name);
                        if (global == program->global_map.end())
                        {
                            program->global_map[name]
                                = std::make_pair(sym, initname);
                        }
                    }
                    else
                    {
                        SgFile *file = traceBackToFileNode(decl);
                        assert(file != NULL);
                        std::pair<std::string, SgFile *> p(name, file);
                        statc = program->static_map.find(p);
                        if (statc == program->static_map.end())
                        {
                            program->static_map[p]
                                = std::make_pair(sym, initname);
                        }
                    }
                }
            }
        }

    private:
        Program *program;
    };
    GlobalDeclarationCollector gdc(this);
    SgVariableDeclaration::traverseMemoryPoolNodes(gdc);
}

}

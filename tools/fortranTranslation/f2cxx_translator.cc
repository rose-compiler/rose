#include "rose.h"

#include "sageBuilder.h"
#include "sageGeneric.h"

#include "f2cxx_translator.hpp"

#include "f2cxx_analyzer.hpp"
#include "f2cxx_convenience.hpp"

namespace sb = SageBuilder;

#if 0
static
void mkDefiningFunction(SgFunctionDeclaration& dcl)
{
  ROSE_ASSERT(false);
  SgFunctionDefinition& def = sg::deref(new SgFunctionDefinition(&dcl, NULL));

  def.setCaseInsensitive(true);
  def.set_parent(&dcl);
  dcl.set_definition(&def);
}

static
void mkFunctionParameterScope(SgFunctionDeclaration& dcl)
{
  SgFunctionParameterScope& scp = sg::deref(SgFunctionParameterScope::build_node_from_nonlist_children());
}

struct ScopeGuard
{
  ScopeGuard(SgScopeStatement& scope)
  : safe(&scope)
  {
    sb::pushScopeStack(&scope);
  }

  ~ScopeGuard()
  {
    ROSE_ASSERT(sb::topScopeStack() == safe);

    sb::popScopeStack();
  }

  SgScopeStatement* safe;
};
#endif

namespace f2cxx
{
  namespace
  {
    struct FileInfoSetter
    {
      explicit
      FileInfoSetter(std::string fn)
      : filename(fn)
      {}

      void handle(SgNode& n) {}

      void handle(SgLocatedNode& n)
      {
        ROSE_ASSERT(filename != "");

        if (n.get_file_info() == NULL)
          n.set_file_info(new Sg_File_Info(filename, 0, 0));
      }

      std::string filename;
    };

    /// sets the symbols defining decl
    void link_decls(SgFunctionSymbol& funcsy, SgFunctionDeclaration& func)
    {
      SgFunctionDeclaration& sdcl = sg::deref(funcsy.get_declaration());

      sdcl.set_definingDeclaration(&func);
      func.set_firstNondefiningDeclaration(&sdcl);

      // \todo \pp is sdcl == func allowed in ROSE/C++?
    }

    //
    // make wrappers

    SgClassType&
    getClassType(SgDeclarationStatement& cldcl)
    {
      return sg::deref(SgClassType::createType(&cldcl));
    }

    SgClassType&
    getClassType(SgClassSymbol& sym)
    {
      return getClassType(sg::deref(sym.get_declaration()));
    }

    SgClassType&
    getClassType(SgScopeStatement& scope, std::string classname)
    {
      return getClassType(sg::deref(scope.lookup_class_symbol(classname)));
    }

    SgClassDeclaration& getClassDecl(SgNamedType& ty)
    {
      return *sg::assert_sage_type<SgClassDeclaration>(ty.get_declaration());
    }

    SgClassDeclaration& getClassDecl(SgType& ty)
    {
      return getClassDecl(*sg::assert_sage_type<SgNamedType>(&ty));
    }

    SgClassType&
    mkClassType(SgDeclarationStatement& cldcl)
    {
      return sg::deref(SgClassType::createType(&cldcl));
    }

    SgType&
    mkCst(SgType& base)
    {
      return sg::deref(sb::buildReferenceType(&base));
    }

    SgExpression&
    mkMemCall(SgExpression& receiver, std::string memfun, SgExprListExp& params)
    {
      SgClassDeclaration&     cldcl = getClassDecl(sg::deref(receiver.get_type()));
      SgClassDefinition&      cldef = get_defn(cldcl);
      SgSymbol*               symbl = cldef.lookup_function_symbol(memfun);
      SgMemberFunctionSymbol* funsy = sg::assert_sage_type<SgMemberFunctionSymbol>(symbl);
      SgMemberFunctionRefExp* fnref = sb::buildMemberFunctionRefExp(funsy, false, false);
      SgExpression*           dotex = sb::buildDotExp(&receiver, fnref);

      return sg::deref(sb::buildFunctionCallExp(dotex, &params));
    }

    SgExpression&
    mkMemCall(SgInitializedName& n, std::string memfun, SgExprListExp& params)
    {
      SgScopeStatement& dclscope = sg::ancestor<SgScopeStatement>(n);

      return mkMemCall(sg::deref(sb::buildVarRefExp(&n, &dclscope)), memfun, params);
    }

    SgExpression&
    mkCall(SgExpression& ref, SgExprListExp& params)
    {
      return sg::deref(sb::buildFunctionCallExp(&ref, &params));
    }

    SgExpression&
    mkCall(std::string callee, SgScopeStatement& scope, SgExprListExp& params)
    {
      SgFunctionSymbol&      sym = sg::deref(scope.lookup_function_symbol(callee));
      SgFunctionDeclaration& ref = sg::deref(sym.get_declaration());
      SgExpression&          tgt = sg::deref(sb::buildFunctionRefExp(&ref));

      return mkCall(tgt, params);
    }

    SgExprListExp&
    mkArgs(SgExpression& arg)
    {
      return sg::deref(sb::buildExprListExp(&arg));
    }

    SgExprListExp&
    mkArg(SgInitializedName& var)
    {
      SgScopeStatement& dclscope = sg::ancestor<SgScopeStatement>(var);

      return mkArgs(sg::deref(sb::buildVarRefExp(&var, &dclscope)));
    }

    SgFunctionDefinition&
    mkFunctionDefinition(SgFunctionDeclaration& dcl)
    {
      SgFunctionDefinition& def = *new SgFunctionDefinition(&dcl, NULL);

      dcl.set_definition(&def);
      def.set_parent(&dcl);
      return def;
    }

    SgBasicBlock&
    mkBasicBlock(SgFunctionDefinition& def)
    {
      SgBasicBlock& bdy = sg::deref(sb::buildBasicBlock());

      def.set_body(&bdy);
      bdy.set_parent(&def);
      return bdy;
    }

    SgType&
    mkConst(SgType& base)
    {
      return sg::deref(sb::buildConstType(&base));
    }

    SgType&
    mkRef(SgType& base)
    {
      return sg::deref(sb::buildReferenceType(&base));
    }

    SgInitializedName&
    mkVar(SgName nm, SgType& ty)
    {
      return sg::deref(sb::buildInitializedName(nm, &ty));
    }

    SgInitializedName&
    mkParm(SgName nm, SgType& ty)
    {
      return mkVar(nm, ty);
    }

    SgExpression&
    mkDummyExpr()
    {
      return sg::deref(sb::buildNullExpression());
    }

    //
    //

    void setInitializer(SgInitializedName& n, SgInitializer& i)
    {
      n.set_initializer(&i);
    }

    void setInitializer(SgInitializedName& n, SgExpression& e)
    {
      setInitializer(n, sg::deref(sb::buildAssignInitializer(&e)));
    }


    //
    // auxiliary functions

    void promoteToDefinition(SgFunctionDeclaration& func)
    {
      SgSymbol*             baseSy = func.search_for_symbol_from_symbol_table();
      SgFunctionSymbol&     funcSy = *sg::assert_sage_type<SgFunctionSymbol>(baseSy);

      link_decls(funcSy, func);
      func.set_definingDeclaration(&func);

      SgFunctionDefinition& fdef = mkFunctionDefinition(func);
      SgBasicBlock&         body = mkBasicBlock(fdef);

      sg::unused(body);
    }

    bool isParam(SgInitializedName* var)
    {
      return (getAttribute(sg::deref(var)).flags() & f2cxx::param) == f2cxx::param;
    }

    SgInitializedName& unique(std::set<SgInitializedName*>& assocs)
    {
      std::vector<SgInitializedName*> matching;

      std::copy_if(assocs.begin(), assocs.end(), std::back_inserter(matching), isParam);

      if (matching.size() < 1)
      {
        throw std::logic_error("no bounds array found");
      }

      if (matching.size() > 1)
      {
        std::cerr << "warning: multiple bounds";
      }

      // \todo do we need to check also for unique association of
      //       bounds with data;
      return sg::deref(matching.front());
    }

    void addParam(SgFunctionDeclaration& dcl, SgInitializedName& parm)
    {
      sg::deref(dcl.get_parameterList()).append_arg(&parm);
    }

    void addVar(SgFunctionDeclaration& dcl, SgInitializedName& var)
    {
      // sg::deref(dcl.get_parameterList()).append_arg(&parm);
    }

    struct ParamTL
    {
      ParamTL(SgScopeStatement& amrex, SgFunctionDeclaration& cxxfun)
      : amrexscope(&amrex), func(&cxxfun)
      {}

      void operator()(SgInitializedName* fparam)
      {
        AnalyzerAttribute& attr = getAttribute(sg::deref(fparam));

        if (attr.hasRole(static_cast<ParamAttr>(f2cxx::data | f2cxx::param)))
        {
          const std::string  fpname = fparam->get_name();
          // consistency check
          //   each 3D array has exactly one bounds array (checked in analyzer)
          SgInitializedName& datalb = unique(attr.associates(f2cxx::space_lower));
          SgInitializedName& dataub = unique(attr.associates(f2cxx::space_upper));

          //   each 3D array has exactly one loop bound array
          SgInitializedName& looplb = unique(attr.associates(f2cxx::loop_lower));
          SgInitializedName& loopub = unique(attr.associates(f2cxx::loop_upper));

          // create amrex box
          std::string        nmbox  = fpname + "_bx";
          SgType&            tybox  = mkRef(mkConst(getClassType(*amrexscope, "Box")));
          SgInitializedName& cxxbox = mkParm(nmbox, tybox);

          addParam(*func, cxxbox);

          // create amrex view
          std::string        nmfab  = fpname + "_fab";
          SgType&            tyfab  = mkRef(getClassType(*amrexscope, "FArrayBox"));
          SgInitializedName& cxxfab = mkParm(nmfab, tyfab);

          addParam(*func, cxxfab);

          // create substitute variables
          SgType&            tyauto = mkConst(getClassType(*amrexscope, "FArrayBox"));
          // mkFakeType("auto"));
          std::string        nmlen  = fpname + "_len";
          std::string        nmlow  = fpname + "_lo";
          std::string        nmdat  = fpname;
          SgInitializedName& cxxlen = mkVar(nmlen, tyauto);
          SgInitializedName& cxxlow = mkVar(nmlow, tyauto);
          SgInitializedName& cxxdat = mkVar(nmdat, tyauto);

          setInitializer(cxxlen, mkCall("length", *amrexscope, mkArg(cxxbox)));
          setInitializer(cxxlow, mkCall("lbound", *amrexscope, mkArg(cxxbox)));
          setInitializer(cxxdat, mkMemCall(cxxfab, "view", mkArg(cxxlow)));

          addVar(*func, cxxlen);
          addVar(*func, cxxlow);
          addVar(*func, cxxdat);

          getAttribute(*fparam).setTranslated(cxxdat);
          getAttribute(datalb).setTranslated(mkDummyExpr());
          getAttribute(dataub).setTranslated(mkDummyExpr());
          getAttribute(looplb).setTranslated(sg::deref(sb::buildIntVal(0)));
          getAttribute(loopub).setTranslated(cxxlen);

          return;
        }

        if (attr.hasRole(f2cxx::auxiliary))
        {
          // do not copy
          return;
        }

        // copy argument normally
      }

      SgScopeStatement*      amrexscope;
      SgFunctionDeclaration* func;
    };

    void translateArgumentList( SgProcedureHeaderStatement* proc,
                                SgFunctionDeclaration* func,
                                SgScopeStatement& amrexscope
                              )
    {
      SgFunctionParameterList& lst = sg::deref(proc->get_parameterList());

      std::for_each(begin(lst), end(lst), ParamTL(amrexscope, sg::deref(func)));
    }
  }


  void Translator::operator()(DeclMaker::mapping p)
  {
    SgProcedureHeaderStatement& proc = sg::deref(p.first);
    SgFunctionDeclaration&      func = sg::deref(p.second);

    promoteToDefinition(func);
    translateArgumentList(p.first, p.second, *amrexscope);

    sg::forAllNodes(f2cxx::FileInfoSetter(cxx->getFileName()), p.second);
  }

  void ::f2cxx::DeclMaker::operator()(SgProcedureHeaderStatement* n)
  {
    SgName                   nm  = n->get_name();
    SgType*                  ty  = sb::buildVoidType();
    SgFunctionParameterList* lst = sb::buildFunctionParameterList();
    SgFunctionDeclaration*   dcl =
        sb::buildNondefiningFunctionDeclaration(nm, ty, lst, glob, NULL /* decoratorList */);
        // sb::buildDefiningFunctionDeclaration(nm, ty, lst, glob, NULL /* decoratorList */);

    glob->append_statement(dcl);
    declmap.push_back(std::make_pair(n, dcl));
  }
}

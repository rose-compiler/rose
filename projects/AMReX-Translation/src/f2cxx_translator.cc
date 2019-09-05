#include "rose.h"

#include "sageBuilder.h"
#include "sageInterface.h"
#include "sageGeneric.h"

#include "f2cxx_translator.hpp"

#include "f2cxx_analyzer.hpp"
#include "f2cxx_convenience.hpp"

namespace sb = SageBuilder;
namespace si = SageInterface;

namespace f2cxx
{
  namespace
  {
    Sg_File_Info* dummyFileInfo()
    {
      // return Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode();
      return Sg_File_Info::generateDefaultFileInfoForTransformationNode();
    }

    void markCompilerGenerated(SgLocatedNode& n)
    {
      n.set_endOfConstruct(dummyFileInfo());
      n.set_startOfConstruct(dummyFileInfo());
      n.set_file_info(dummyFileInfo());
    }

    void markCompilerGenerated(SgLocatedNode* n)
    {
      markCompilerGenerated(sg::deref(n));
    }

    struct FileInfoChecker
    {
      explicit
      FileInfoChecker(bool finfoset)
      : requiresFileInfo(finfoset)
      {}

      void handle(SgNode& n) {}

      void handle(SgLocatedNode& n)
      {
        if (requiresFileInfo)
        {
          // \todo rm existing file infos
          markCompilerGenerated(n);
          return;
        }

        if (n.get_file_info() == NULL)
          std::cerr << "info not set on: " << typeid(n).name() << std::endl;

        if (n.get_startOfConstruct() == NULL)
          std::cerr << "start of construct not set on: " << typeid(n).name() << std::endl;

        if (n.get_endOfConstruct() == NULL)
          std::cerr << "start of construct not set on: " << typeid(n).name() << std::endl;
      }

      bool requiresFileInfo;
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
    tyClassType(SgDeclarationStatement& cldcl)
    {
      return sg::deref(SgClassType::createType(&cldcl));
    }

    SgClassType&
    tyClassType(SgClassSymbol& sym)
    {
      SgClassDeclaration& dcl = sg::deref(sym.get_declaration());

      return tyClassType(dcl);
    }

    SgClassType&
    tyClassType(SgScopeStatement& scope, std::string classname)
    {
      return tyClassType(sg::deref(scope.lookup_class_symbol(classname)));
    }

    static inline
    SgType&
    tyAuto(SgGlobal& scope)
    {
      return sg::deref(sb::buildOpaqueType("auto", &scope));
    }

    static inline
    SgType&
    tyVoid()
    {
      return sg::deref(sb::buildVoidType());
    }

    static inline
    SgType&
    tyInt()
    {
      return sg::deref(sb::buildIntType());
    }

    SgType&
    tyConst(SgType& base)
    {
      return sg::deref(sb::buildConstType(&base));
    }

    SgType&
    tyRef(SgType& base)
    {
      return sg::deref(sb::buildReferenceType(&base));
    }


    //
    // helper functions to get to the bottom of types
    template <class SageNode>
    struct TypeUnpealer : sg::DispatchHandler<SageNode*>
    {
      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SageNode& n) { this->res = &n; }

      void handle(SgModifierType& ty)
      {
        *this = sg::dispatch(*this, ty.get_base_type());
      }

      void handle(SgReferenceType& ty)
      {
        *this = sg::dispatch(*this, ty.get_base_type());
      }
    };

    template <class SageNode>
    SageNode& unpeal(SgType& ty)
    {
      return *sg::dispatch(TypeUnpealer<SageNode>(), &ty);
    }

    SgClassDeclaration& getClassDecl(SgNamedType& ty)
    {
      return *SG_ASSERT_TYPE(SgClassDeclaration, ty.get_declaration());
    }

    SgClassDeclaration& getClassDecl(SgType& ty)
    {
      return getClassDecl(unpeal<SgNamedType>(ty));
    }

    SgVarRefExp&
    mkVarRef(SgInitializedName& n)
    {
      SgScopeStatement& dclscope = sg::ancestor<SgScopeStatement>(n);
      SgVarRefExp&      varref   = sg::deref(sb::buildVarRefExp(&n, &dclscope));

      markCompilerGenerated(varref);
      return varref;
    }

    SgVarRefExp&
    mkVarRef(SgVariableSymbol& sy)
    {
      return mkVarRef(get_decl(sy));
    }

    SgFunctionCallExp&
    mkMemCall(SgExpression& receiver, std::string memfun, SgExprListExp& params)
    {
      SgClassDeclaration&     cldcl = getClassDecl(sg::deref(receiver.get_type()));
      SgClassDefinition&      cldef = get_defn(cldcl);
      SgSymbol*               symbl = cldef.lookup_function_symbol(memfun);
      SgMemberFunctionSymbol* funsy = SG_ASSERT_TYPE(SgMemberFunctionSymbol, symbl);
      SgMemberFunctionRefExp* fnref = sb::buildMemberFunctionRefExp(funsy, false, false);
      SgExpression*           dotex = sb::buildDotExp(&receiver, fnref);
      SgFunctionCallExp&      cllex = sg::deref(sb::buildFunctionCallExp(dotex, &params));

      markCompilerGenerated(fnref);
      markCompilerGenerated(dotex);
      markCompilerGenerated(cllex);
      return cllex;
    }

    SgFunctionCallExp&
    mkMemCall(SgInitializedName& n, std::string memfun, SgExprListExp& params)
    {
      return mkMemCall(mkVarRef(n), memfun, params);
    }

    SgExpression&
    mkMemAccess(SgVarRefExp& rec, SgVarRefExp& fld)
    {
      SgDotExp& fldacc = *new SgDotExp(&rec, &fld, fld.get_type());

      markCompilerGenerated(fldacc);
      return fldacc;
    }

    /** creates a fake member access expression.
     *
     *  fake, b/c @p memfld would normally be part of the @p receiver type,
     *  but here we take it from @p cldef */
    SgExpression&
    mkFakeMemberAccess(SgVarRefExp& receiver, std::string memfld, SgClassDefinition& cldef)
    {
      SgSymbol*         symbl = cldef.lookup_variable_symbol(memfld);
      SgVariableSymbol* fldsy = SG_ASSERT_TYPE(SgVariableSymbol, symbl);

      return mkMemAccess(receiver, mkVarRef(*fldsy));
    }

    SgExpression&
    mkCall(SgExpression& ref, SgExprListExp& params)
    {
      SgExpression& n = sg::deref(sb::buildFunctionCallExp(&ref, &params));

      markCompilerGenerated(n);
      return n;
    }

    SgExpression&
    mkCall(std::string callee, SgScopeStatement& scope, SgExprListExp& params)
    {
      SgFunctionSymbol&      sym = sg::deref(scope.lookup_function_symbol(callee));
      SgFunctionDeclaration& ref = sg::deref(sym.get_declaration());
      SgExpression&          tgt = sg::deref(sb::buildFunctionRefExp(&ref));
      SgExpression&          cll = mkCall(tgt, params);

      markCompilerGenerated(tgt);
      markCompilerGenerated(cll);
      return cll;
    }

    SgAssignInitializer&
    mkAssignInitializer(SgExpression& e)
    {
      SgAssignInitializer& res = sg::deref(sb::buildAssignInitializer(&e));

      markCompilerGenerated(res);
      return res;
    }

    SgExprListExp&
    mkArgs(SgExpression& arg)
    {
      SgExprListExp& res = sg::deref(sb::buildExprListExp(&arg));

      markCompilerGenerated(res);
      return res;
    }

    SgExprListExp&
    mkArg(SgInitializedName& var)
    {
      SgScopeStatement& dclscope = sg::ancestor<SgScopeStatement>(var);
      SgVarRefExp&      varref = sg::deref(sb::buildVarRefExp(&var, &dclscope));

      markCompilerGenerated(varref);
      return mkArgs(varref);
    }

    SgFunctionDefinition&
    mkFunctionDefinition(SgFunctionDeclaration& dcl)
    {
      SgFunctionDefinition& def = *new SgFunctionDefinition(&dcl, NULL);

      dcl.set_definition(&def);
      def.set_parent(&dcl);
      markCompilerGenerated(def);
      return def;
    }

    SgBasicBlock&
    mkBasicBlock()
    {
      SgBasicBlock& bdy = sg::deref(sb::buildBasicBlock());

      markCompilerGenerated(bdy);
      return bdy;
    }

    SgIntVal&
    mkIntVal(int v)
    {
      SgIntVal& res = sg::deref(sb::buildIntVal(v));

      markCompilerGenerated(res);
      return res;
    }

    template <class SageNode>
    SageNode&
    _mk(SageNode* (*mkr) (SgExpression*, SgExpression*), SgExpression& lhs, SgExpression& rhs)
    {
      SageNode& res = sg::deref(mkr(&lhs, &rhs));

      markCompilerGenerated(res);
      return res;
    }

    template <class SageNode>
    SageNode&
    _mk( SageNode* (*mkr) (SgExpression*),
         SgExpression& exp,
         SgUnaryOp::Sgop_mode mode = SgUnaryOp::prefix
       )
    {
      SageNode& res = sg::deref(mkr(&exp));

      res.set_mode(mode);
      markCompilerGenerated(res);
      return res;
    }

    SgAssignOp&
    mkAssign(SgExpression& lhs, SgExpression& rhs)
    {
      return _mk(sb::buildAssignOp, lhs, rhs);
    }

    SgAddOp&
    mkAdd(SgExpression& lhs, SgExpression& rhs)
    {
      return _mk(sb::buildAddOp, lhs, rhs);
    }

    SgLessThanOp&
    mkLessThan(SgExpression& lhs, SgExpression& rhs)
    {
      return _mk(sb::buildLessThanOp, lhs, rhs);
    }

    static inline
    SgPlusPlusOp&
    mkPostIncr(SgExpression& exp)
    {
      return _mk(sb::buildPlusPlusOp, exp, SgUnaryOp::postfix);
    }

    SgPlusPlusOp&
    mkPreIncr(SgExpression& exp)
    {
      return _mk(sb::buildPlusPlusOp, exp, SgUnaryOp::prefix);
    }

    SgExprStatement&
    mkStmt(SgExpression& exp)
    {
      SgExprStatement& res = sg::deref(sb::buildExprStatement(&exp));

      markCompilerGenerated(res);
      return res;
    }

    SgForStatement&
    mkForLoop(SgStatement& ini, SgStatement& tst, SgExpression& inc, SgStatement& bdy)
    {
      SgForStatement& res = sg::deref(sb::buildForStatement(&ini, &tst, &inc, &bdy));

      markCompilerGenerated(res);
      return res;
    }

    SgForStatement&
    mkForLoop(SgStatement& ini, SgExpression& tst, SgExpression& inc, SgStatement& bdy)
    {
      return mkForLoop(ini, mkStmt(tst), inc, bdy);
    }

    template <class SageNode>
    SgForStatement&
    mkForLoop(SgExpression& ini, SageNode& tst, SgExpression& inc, SgStatement& bdy)
    {
      return mkForLoop(mkStmt(ini), tst, inc, bdy);
    }

    SgInitializedName&
    dclParam(SgFunctionDeclaration& dcl, std::string nm, SgType& ty)
    {
      SgInitializedName&        prm = sg::deref(sb::buildInitializedName(nm, &ty));
      SgFunctionParameterList&  lst = sg::deref(dcl.get_parameterList());
      SgFunctionParameterScope& psc = sg::deref(dcl.get_functionParameterScope());
      SgSymbolTable&            tab = sg::deref(psc.get_symbol_table());

      markCompilerGenerated(prm);
      prm.set_scope(&psc);
      tab.insert(nm, new SgVariableSymbol(&prm));
      lst.append_arg(&prm);

      ROSE_ASSERT(prm.get_scope());
      ROSE_ASSERT(prm.search_for_symbol_from_symbol_table());
      return prm;
    }

    SgFunctionDeclaration&
    dclFunDecl(std::string nm, SgType& ty, SgGlobal& glob)
    {
      SgFunctionParameterList&  lst = sg::deref(sb::buildFunctionParameterList());
      SgFunctionDeclaration&    dcl =
            sg::deref(sb::buildNondefiningFunctionDeclaration(nm, &ty, &lst, &glob, NULL));
      SgFunctionParameterScope& psc =
            sg::deref(new SgFunctionParameterScope(dummyFileInfo()));

      markCompilerGenerated(lst);
      markCompilerGenerated(dcl);
      markCompilerGenerated(psc);
      dcl.set_functionParameterScope(&psc);
      glob.append_statement(&dcl);

      return dcl;
    }

    /** Returns a new dummy expressions. */
    SgExpression& mkDummyExpr()
    {
      SgExpression& exp = sg::deref(sb::buildNullExpression());

      markCompilerGenerated(exp);
      return exp;
    }

    //
    //

    void setInitializer(SgInitializedName& n, SgInitializer& i)
    {
      n.set_initializer(&i);
      i.set_parent(&n);
    }

    void setInitializer(SgInitializedName& n, SgExpression& e)
    {
      setInitializer(n, mkAssignInitializer(e));
    }

    //
    // auxiliary functions

    void promoteToDefinition(SgFunctionDeclaration& func)
    {
      SgSymbol*             baseSy = func.search_for_symbol_from_symbol_table();
      SgFunctionSymbol&     funcSy = *SG_ASSERT_TYPE(SgFunctionSymbol, baseSy);

      link_decls(funcSy, func);
      func.set_definingDeclaration(&func);
      func.unsetForward();

      SgFunctionDefinition& fdef = mkFunctionDefinition(func);
      SgBasicBlock&         body = mkBasicBlock();

      fdef.set_body(&body);
      body.set_parent(&fdef);
    }

    bool isParam(SgInitializedName* var)
    {
      return (getAttribute(sg::deref(var)).flags() & f2cxx::param) == f2cxx::param;
    }

    SgInitializedName& unique(std::set<SgInitializedName*>& assocs)
    {
      ROSE_ASSERT(assocs.size() > 0);

      std::vector<SgInitializedName*> matching;

      std::copy_if(assocs.begin(), assocs.end(), std::back_inserter(matching), isParam);

      sg::report_error_if(matching.size() < 1, "no bounds array found");

      if (matching.size() > 1)
      {
        std::cerr << "warning: multiple bounds" << std::endl;
      }

      // \todo do we need to check also for unique association of
      //       bounds with data;
      return sg::deref(matching.front());
    }

    /** Declares a new variable in a scope @p scp and returns the uniquely
     *  identifying node. */
    SgInitializedName&
    dclVar(SgScopeStatement& scp, std::string nm, SgType& ty)
    {
      SgVariableDeclaration& dcl = sg::deref(sb::buildVariableDeclaration(nm, &ty, NULL, &scp));
      SgInitializedName&     var = sg::deref(dcl.get_decl_item(nm));

      markCompilerGenerated(dcl);
      markCompilerGenerated(var);
      scp.append_statement(&dcl);
      var.set_scope(&scp);
      return var;
    }

    std::string dimName(size_t idx)
    {
      std::string res;

      switch (idx)
      {
        case 0:
          res = "x";
          break;

        case 1:
          res = "y";
          break;

        case 2:
          res = "z";
          break;

        default:
          sg::report_error("missing case label");
      }

      return res;
    }


    void convertToCxxSignature( SgInitializedName&     data,       // fortran argument
                                SgScopeStatement&      amrexscope, // cxx scope
                                SgFunctionDeclaration& func        // cxx decl
                              )
    {
      const std::string  fpname = data.get_name();
      AnalyzerAttribute& attr = getAttribute(data);

      // consistency check
      //   each 3D array has exactly one bounds array (checked in analyzer)
      SgInitializedName& datalb = unique(attr.associates(f2cxx::space_lower));
      SgInitializedName& dataub = unique(attr.associates(f2cxx::space_upper));

      // each 3D array has exactly one loop bound array
      //~ std::cerr << "a: " << fparam->get_name() << std::endl;
      //~ SgInitializedName& looplb = unique(attr.associates(f2cxx::loop_lower));
      //~ std::cerr << "b" << std::endl;
      //~ SgInitializedName& loopub = unique(attr.associates(f2cxx::loop_upper));

      // create amrex box
      std::string        nmbox  = fpname + "_bx";
      SgType&            tybox  = tyRef(tyConst(tyClassType(amrexscope, "Box")));
      SgInitializedName& cxxbox = dclParam(func, nmbox, tybox);

      // create amrex view
      std::string        nmfab  = fpname + "_fab";
      SgType&            tyfab  = tyRef(tyClassType(amrexscope, "FArrayBox"));
      SgInitializedName& cxxfab = dclParam(func, nmfab, tyfab);

      // create substitute variables
      SgGlobal&          glob   = sg::ancestor<SgGlobal>(func);
      SgType&            tydim  = tyClassType(amrexscope, "Dim");
      SgType&            tyauto = tyConst(tyAuto(glob));
      // mkFakeType("auto"));
      std::string        nmlen  = fpname + "_len";
      std::string        nmlow  = fpname + "_lo";
      std::string        nmdat  = fpname;
      SgBasicBlock&      funbdy = get_body(func);
      SgInitializedName& cxxlen = dclVar(funbdy, nmlen, tydim);
      SgInitializedName& cxxlow = dclVar(funbdy, nmlow, tyauto);
      SgInitializedName& cxxdat = dclVar(funbdy, nmdat, tyauto);

      setInitializer(cxxlen, mkCall("length", amrexscope, mkArg(cxxbox)));
      setInitializer(cxxlow, mkCall("lbound", amrexscope, mkArg(cxxbox)));
      setInitializer(cxxdat, mkMemCall(cxxfab, "view", mkArg(cxxlow)));

      getAttribute(data)
         .setTranslated(cxxdat)
         .setBounds(cxxlen);

      getAttribute(datalb).setTranslated(mkDummyExpr());
      getAttribute(dataub).setTranslated(mkDummyExpr());
    }

    struct ParamTL
    {
      ParamTL(SgScopeStatement& amrex, SgFunctionDeclaration& cxxfun)
      : amrexscope(&amrex), func(&cxxfun)
      {}

      void operator()(SgInitializedName* fparam)
      {
        AnalyzerAttribute& attr = getAttribute(sg::deref(fparam));

        if (!attr.hasRole(static_cast<ParamAttr>(f2cxx::data | f2cxx::param)))
          return;

        convertToCxxSignature(*fparam, *amrexscope, *func);
      }

      SgScopeStatement*      amrexscope;
      SgFunctionDeclaration* func;
    };

    void translateArgumentList( SgProcedureHeaderStatement& proc,
                                SgFunctionDeclaration& func,
                                SgScopeStatement& amrexscope
                              )
    {
      SgFunctionParameterList& lst = sg::deref(proc.get_parameterList());

      std::for_each(begin(lst), end(lst), ParamTL(amrexscope, func));
    }

    static
    SgType& tlType(SgType& ty)
    {
      return ty;
    }

    static inline
    bool isAmrexLoop(SgNode& n)
    {
      // extract info from analyzer
      return getAttribute(n).hasRole(f2cxx::amrex_loop);
    }

    template <class SageExpr>
    static
    SageExpr& clone(SageExpr& exp, bool requiresFileInfo = false)
    {
      SageExpr& res = sg::deref(si::deepCopy(&exp));

      sg::forAllNodes(FileInfoChecker(requiresFileInfo), &res);
      return res;
    }

    struct ExprMaker : sg::DispatchHandler<SgExpression*>
    {
      void handle(SgNode& n)            { SG_UNEXPECTED_NODE(n); }

      void handle(SgInitializedName& n) { res = &mkVarRef(n); }
    };

    bool hasRole(SgExpression& n, ParamAttr role)
    {
      SgVarRefExp&       varref = *SG_ASSERT_TYPE(SgVarRefExp, &n);
      SgInitializedName& var    = get_decl(varref);

      return getAttribute(var).hasRole(role);
    }

    struct ExprTranslator : sg::DispatchHandler<SgExpression*>
    {
      template <class SageExpr>
      SgExpression&
      binary_expr(SageExpr& (*mk) (SgExpression& lhs, SgExpression& rhs), SgBinaryOp& n)
      {
        SgExpression* lhs = sg::dispatch(*this, n.get_lhs_operand());
        SgExpression* rhs = sg::dispatch(*this, n.get_rhs_operand());

        return mk(*lhs, *rhs);
      }

      SgExpression&
      clone_expr(SgExpression& n)
      {
        return clone(n, true);
      }

      void handle(SgNode& n)       { SG_UNEXPECTED_NODE(n); }

      //
      // standard value expressions
      void handle(SgValueExp& n)   { res = &clone_expr(n); }

      //
      // standard binary expressions
      void handle(SgAssignOp& n)   { res = &binary_expr(mkAssign, n); }
      void handle(SgAddOp& n)      { res = &binary_expr(mkAdd, n);   }

      //
      // specialized translations
      void handle(SgVarRefExp& n)
      {
        SgInitializedName& dcl = get_decl(n);

        ROSE_ASSERT(getAttribute(dcl).translated() != NULL);
        res = sg::dispatch(ExprMaker(), getAttribute(dcl).translated());
      }

      void handle(SgPntrArrRefExp& n)
      {
        SgExpression& lhs = sg::deref(n.get_lhs_operand());

        // if the left hand side is an amrex type, we need to change this
        //   to a function call
        if (!hasRole(lhs, f2cxx::dataparam))
          ROSE_ASSERT(0);

        SgExpression&  obj  = *sg::dispatch(*this, &lhs);
        SgExprListExp& lst  = *SG_ASSERT_TYPE(SgExprListExp, n.get_rhs_operand());

        res = &mkCall(obj, clone(lst));
      }
    };

    static
    SgExpression& translate(SgExpression& n)
    {
      SgExpression* res = sg::dispatch(ExprTranslator(), &n);

      return sg::deref(res);
    }

    static
    SgVarRefExp& translate(SgVarRefExp& n)
    {
      SgExpression& exp = n;
      SgExpression& cxx = translate(exp);

      return *SG_ASSERT_TYPE(SgVarRefExp, &cxx);
    }

    SgExpression& genUpperBound(SgFortranDo& n, SgScopeStatement& amrexscope)
    {
      typedef std::pair<size_t, SgInitializedName*> AccessData;

      AnalyzerAttribute& attr      = getAttribute(n);
      AccessData         acc       = attr.array_access();
      AnalyzerAttribute& dataattr  = getAttribute(sg::deref(acc.second));
      SgInitializedName& upperbase = sg::deref(dataattr.bounds());
      SgClassSymbol&     clssy     = sg::deref(amrexscope.lookup_class_symbol("Dim"));
      SgClassDefinition& cldef     = get_defn(clssy);
      SgVarRefExp&       upperref  = mkVarRef(upperbase);
      size_t             idx       = acc.first;

      return mkFakeMemberAccess(upperref, dimName(idx), cldef);
    }

    struct StmtTranslator
    {
      StmtTranslator(SgScopeStatement& top, SgScopeStatement& amrex)
      : scopes()
      {
        scope_push(amrex);
        scope_push(top);
      }

      void handle(SgNode& n) { SG_UNEXPECTED_NODE(n); }

      void handle(SgVariableDeclaration& n)
      {
        sg::traverseChildren(*this, n);
      }

      void handle(SgInitializedName& n)
      {
        AnalyzerAttribute& attr = getAttribute(n);

        if (attr.hasRole(f2cxx::param))
        {
          if (attr.translated() == NULL)
            std::cerr << "skipped.. " << n.get_name() << std::endl;

          return;
        }

        std::cerr << "var: " << n.get_name() << std::endl;
        ROSE_ASSERT(!attr.translated());
        SgType&            cxxty  = tlType(sg::deref(n.get_type()));
        SgInitializedName& cxxvar = dclVar(scope_top(), n.get_name(), cxxty);

        attr.setTranslated(cxxvar);
      }

      void handle(SgExprStatement& n)
      {
        SgExpression&    exp = translate(sg::deref(n.get_expression()));

        scope_top().append_statement(&mkStmt(exp));
      }

      void handle(SgFortranDo& n)
      {
        ROSE_ASSERT(isAmrexLoop(n));

        // extract the loop variable and make it start from 0
        SgExpression*     init   = n.get_initialization();
        SgAssignOp*       asgn   = SG_ASSERT_TYPE(SgAssignOp, init);
        SgVarRefExp*      loop   = SG_ASSERT_TYPE(SgVarRefExp, asgn->get_lhs_operand());

        SgVarRefExp&      cxxvar = translate(*loop);
        SgExpression&     cxx0   = mkIntVal(0);
        SgExpression&     cxxini = mkAssign(cxxvar, cxx0);

        // create the new loop upper bound
        SgExpression&     cxxub  = genUpperBound(n, scope_amrex());
        SgExpression&     cxxtst = mkLessThan(clone(cxxvar), cxxub);

        // add increment
        SgExpression&     cxxinc = mkPreIncr(clone(cxxvar));

        // clone the body
        SgBasicBlock&     cxxblk = mkBasicBlock();
        SgForStatement&   cxxfor = mkForLoop(cxxini, cxxtst, cxxinc, cxxblk);

        scope_top().append_statement(&cxxfor);

        scope_push(cxxblk);
        ROSE_ASSERT(isSgBasicBlock(n.get_body()));
        sg::traverseChildren(*this, n.get_body());
        scope_pop(cxxblk);
      }

      void scope_push(SgScopeStatement& scope)
      {
        scopes.push_back(&scope);
      }

      void scope_pop(SgScopeStatement& scope)
      {
        ROSE_ASSERT(!scopes.empty());
        ROSE_ASSERT(scopes.back() == &scope);

        scopes.pop_back();
      }

      SgScopeStatement& scope_top()
      {
        ROSE_ASSERT(!scopes.empty());

        return sg::deref(scopes.back());
      }

      SgScopeStatement& scope_amrex()
      {
        return sg::deref(scopes.front());
      }

      std::vector<SgScopeStatement*> scopes;
    };

    void translateFunctionBody( SgProcedureHeaderStatement& proc,
                                SgFunctionDeclaration&      func,
                                SgScopeStatement&           amrexscope
                              )
    {
      SgBasicBlock& fxxbody = get_body(proc);
      SgBasicBlock& cxxbody = get_body(func);

      sg::traverseChildren(StmtTranslator(cxxbody, amrexscope), fxxbody);
    }
  }


  void Translator::operator()(DeclMaker::mapping p)
  {
    SgProcedureHeaderStatement& proc = sg::deref(p.first);
    SgFunctionDeclaration&      func = sg::deref(p.second);

    promoteToDefinition(func);
    std::cerr << "translate args.." << std::endl;
    translateArgumentList(proc, func, *amrexscope);
    std::cerr << "translate bodies.." << std::endl;
    translateFunctionBody(proc, func, *amrexscope);

    sg::forAllNodes(f2cxx::FileInfoChecker(false), &func);
  }

  void ::f2cxx::DeclMaker::operator()(SgProcedureHeaderStatement* n)
  {
    SgName                   nm  = n->get_name();
    SgType&                  ty  = tyVoid();
    SgFunctionDeclaration&   dcl = dclFunDecl(nm, ty, *glob);

    declmap.push_back(std::make_pair(n, &dcl));
  }
}

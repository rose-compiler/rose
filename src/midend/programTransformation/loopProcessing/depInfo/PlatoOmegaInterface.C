/********************************
*       FILE: PlatoOmegaInterface.C
*       AUTHOR: Mike Stiles
*       University of Texas at San Antonio 2007
*********************************/

#ifdef HAVE_CONFIG_H
//#include <config.h>
#endif

//#include <rose.h>
//#include <general.h>
 
#include <iostream>
#include <string.h>
#include <AstInterface.h>
#include <LoopInfoInterface.h>
#include <StmtInfoCollect.h>

#include <stdio.h>
#include <sstream>
#include <CommandOptions.h>
#include <fcntl.h>
#include <StmtDepAnal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "PlatoOmegaInterface.h"
#include <MathUtils.h>
#include <DDTypes.h>

extern bool DebugDep();
extern DepTestStatistics DepStats;
 
// DQ (3/8/2006): Since this is not used in a heade file it is OK here!

namespace PlatoOmegaInterface {

int     debug_determine_function_arity = 0,
                debug_GetStdVarU = 0,
                debug_RenameDupedVars = 0,              
                debug_AddSubscripts = 0,
                debug_AddCommonLoopIndices = 0,
                debug_AddLocalLoopIndices = 0,
                debug_AddLoopBounds = 0,
                debug_ComputeArrayDep = 0,
                debug_doHierarchy = 0,
                debug_VisitVar = 0,
                debug_VisitExpr = 0,
                debug_VisitConst = 0,
                debug_ParseVal = 0,
                debug_GetRationalExpression = 0,
                debug_GetNewRelationFromDV = 0,
                debug_AddConstraintsImposedByDV = 0;

DEP_TEST_CHOICE curr_DEP_TEST_CHOICE;

/**     This needs to be called before calling ComputeArrayDep.
*               Called in DepInfoPlato.C line 322.
*/

void SetDepChoice(DEP_TEST_CHOICE new_choice)
{
        switch (new_choice)
        {
                case PLATO :
                {
                        curr_DEP_TEST_CHOICE = PLATO;
                }
                break;
                case OMEGA :
                {
                        curr_DEP_TEST_CHOICE = OMEGA;
                }
                break;
                case ADHOC :
                {
                        curr_DEP_TEST_CHOICE = ADHOC;
                }
                break;
                default :
                {
                        curr_DEP_TEST_CHOICE = ADHOC;
                }
                break;
        }
}

/** Which one are we using? **/

bool UseOmega(void)
{
        if (curr_DEP_TEST_CHOICE == OMEGA)
        {
                return true;
        }
        else
        {
                return false;
        }
}

char *StringToChar(const std::string& str) {
   int i;
   std::string::size_type j;
   char *temp = (char *) calloc(str.size() + 1,sizeof(char));
   for (i=j=0;i<str.size();i++,j++)
      temp[i] = str[j];
   temp[str.size()] = '\0';
   return temp;
}

static char *StrDir[] = {
   "...",
   "<",
   "=",
   "<=",
   ">",
   "<>",
   ">=",
   "*"
};

static char * str_dir_vec (plato::dddir dv, int dvLevel)
{
   static char str_dv[5][20];
   static int count = 0;
   char * dirstr;
   int i;
   plato::dddir index;
    
   dirstr = (char *)str_dv[count];
   count = (count + 1) % 5;
    
   dirstr[0] = '(';
   dirstr[1] = '\0';

   for (i = 0; i < dvLevel; i++ ){
      index = dv & 0xf;
      sprintf (dirstr, (i < dvLevel - 1) ? "%s%s," : "%s%s",
               dirstr, StrDir[index]);
      dv = dv >> 4;
   }
   sprintf( dirstr, "%s)", dirstr );

   return dirstr;
}

void print_debug(std::string s1, std::string s2, int f) {
        if (f) {
                std::cerr << s1 << " " << s2 << std::endl;
        }
}

bool PlatoOmegaDepTesting::PlatoRelation::testDV(const plato::AbstractDirectionVector & dv, bool & accurate)
{
   bool accCond, coupling, feasible, ifIgnored;
   feasible = plato::RationalVITestProblem::testDV(dv, accCond, coupling, ifIgnored);
   accurate = accCond && !coupling && !ifIgnored;
   return feasible;
}

plato::RationalExpression PlatoOmegaDepTesting ::
      GetRationalExpression( AstInterface &fa, const AstNodePtr& exp, bool is_input)
{
   std::string name,fname = "";
   AstNodePtr scope;
   AstInterface::OperatorEnum opr;
   int val = 0;
   AstNodePtr s1, s2, curloop;
   AstInterface::AstNodeList l;
        PLATO_STD_VAR *tempVar;
        print_debug("GetRationalExpression","entering",debug_GetRationalExpression);
   if (fa.IsVarRef(exp, 0, &name, &scope))
        {
      tempVar = GetPlatoStdVar(name,is_input);
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return *tempVar;
   }
   else if (fa.IsConstInt(exp, &val)) {
      plato::RationalExpression v1(val);
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return v1;
   }
   else if (fa.IsBinaryOp(exp, &opr, &s1, &s2) && opr == AstInterface::BOP_TIMES) {
      plato::RationalExpression v1 = GetRationalExpression( fa, s1, is_input ),
                                v2 = GetRationalExpression(fa, s2, is_input);
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return v1 * v2;
   }
   else if (fa.IsBinaryOp(exp, &opr, &s1, &s2) && opr == AstInterface::BOP_PLUS) {
      plato::RationalExpression v1 = GetRationalExpression( fa, s1, is_input );
      plato::RationalExpression v2 = GetRationalExpression( fa, s2, is_input );
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return v1 +  v2;
   }
   else if (fa.IsBinaryOp(exp, &opr, &s1, &s2) && opr == AstInterface::BOP_MINUS) {
      plato::RationalExpression v1 = GetRationalExpression( fa, s1, is_input );
      plato::RationalExpression v2 = GetRationalExpression( fa, s2, is_input );
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return v1 - v2;
   }
   else if (fa.IsUnaryOp(exp, &opr, &s1) && opr == AstInterface::UOP_MINUS) {
      plato::RationalExpression v = GetRationalExpression( fa, s1, is_input);
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return (-1) * v;
   }
   else if (fa.IsFunctionCall(exp, &s1, &l) && fa.IsVarRef(s1,0,&name)) {      
                tempVar = GetPlatoStdVar(name,is_input);
                print_debug("GetRationalExpression","exiting",debug_GetRationalExpression);
      return *tempVar;
   }
}

/**     This function returns the loop level at which the
*               variable is being referenced. It does not (at present)
*               determine whether it is a write or a read, merely whether
*               if it has been referenced.
*
*               For omega, returns 0 or the level.
*               For plato, returns -1 or the level - but not 0.
*/

int PlatoOmegaDepTesting::ReturnVarianceLevel(const std::string name)
{
        int arity = (UseOmega()) ? 0 : -1;

        print_debug("_determine_function_arity","entering",debug_determine_function_arity);
        AstNodePtr curloop = _ref->commLoop;
        for (int i = _commLevel; i >= 1 && curloop != _fa->getNULL(); i--) {
                if (IsModified(curloop,name)) {
                        arity = i;
                        break;
                }
                curloop = GetEnclosingLoop(curloop,*_fa);
        }
        print_debug("_determine_function_arity","exiting",debug_determine_function_arity);
   return arity;
}

void PlatoOmegaDepTesting::print_omega_vars(void)
{
        std::cerr << "vars in input tuple " << _omega_relation->n_inp() << std::endl;
        std::cerr << "vars in output tuple " << _omega_relation->n_out() << std::endl;
        std::cerr << "\nparsing _omega_input_vars " << std::endl;
        STD_VAR_MAP::iterator iter = _omega_input_vars->begin();
        while (iter != _omega_input_vars->end())
        {
                std::cerr << "name " << iter->second->ReturnOmegaStdVar()->name();
                switch(iter->second->ReturnOmegaStdVar()->kind())
                {
                        case Input_Var :
                        {
                                std::cerr << " Input_Var, position " <<
                                                iter->second->ReturnOmegaStdVar()->get_position() << std::endl;
                        }
                        break;
                        case Output_Var :
                        {
                                std::cerr << " Ouput_Var, position " <<
                                                iter->second->ReturnOmegaStdVar()->get_position() << std::endl;
                        }
                        break;
                        case Set_Var :                          
                        {
                                std::cerr << " Set_Var " << std::endl;
                        }
                        break;
                        case Global_Var :
                        {
                                std::cerr << " Global_Var " << std::endl;
                        }
                        break;
                        case Forall_Var :
                        {
                                std::cerr << " Forall_Var " << std::endl;
                        }
                        break;
                        case Exists_Var :
                        {
                                std::cerr << " Exists_Var " << std::endl;
                        }
                        break;
                        case Wildcard_Var :
                        {
                                std::cerr << " Wildcard_Var " << std::endl;
                        }
                        break;
                        default :
                        {
                        }
                        break;
                        
                }
                iter++;
        }
        std::cerr << "\nparsing _omega_output_vars " << std::endl;
        iter = _omega_output_vars->begin();
        while (iter != _omega_output_vars->end())
        {
                std::cerr << "name " << iter->second->ReturnOmegaStdVar()->name();
                switch(iter->second->ReturnOmegaStdVar()->kind())
                {
                        case Input_Var :
                        {
                                std::cerr << " Input_Var, position " <<
                                                iter->second->ReturnOmegaStdVar()->get_position() << std::endl;
                        }
                        break;
                        case Output_Var :
                        {
                                std::cerr << " Ouput_Var, position " <<
                                                iter->second->ReturnOmegaStdVar()->get_position() << std::endl;
                        }
                        break;
                        case Set_Var :                          
                        {
                                std::cerr << " Set_Var " << std::endl;
                        }
                        break;
                        case Global_Var :
                        {
                                std::cerr << " Global_Var " << std::endl;
                        }
                        break;
                        case Forall_Var :
                        {
                                std::cerr << " Forall_Var " << std::endl;
                        }
                        break;
                        case Exists_Var :
                        {
                                std::cerr << " Exists_Var " << std::endl;
                        }
                        break;
                        case Wildcard_Var :
                        {
                                std::cerr << " Wildcard_Var " << std::endl;
                        }
                        break;
                        default :
                        {
                        }
                        break;
                        
                }
                iter++;
        }
        std::cerr << "\nprinting _omega_relation info " << std::endl;
        _omega_relation->print();
        std::cerr << std::endl;
        _omega_relation->print_with_subs(stdout);
        std::cerr << std::endl;
        _omega_relation->prefix_print();
        std::cerr << std::endl << std::endl;
}

/**     Returns the STD_VAR_U union which contains either an omega Variable_ID
*               or a plato Variable.
**/

STD_VAR_U *PlatoOmegaDepTesting::GetStdVarU(const std::string name, bool is_input)
{

    StandardVar *var_ref = 0;

         print_debug("GetStdVarU","entering",debug_GetStdVarU);

         if (UseOmega())
         {
                if (_omega_input_vars->count(name) > 0 && is_input)
                {
                        var_ref = (*_omega_input_vars)[name];
                        assert( var_ref );
                        print_debug("GetStdVarU","found omega input " + name,debug_GetStdVarU);
                }
                else if (_omega_output_vars->count(name) > 0 && !is_input)
                {
                        var_ref = (*_omega_output_vars)[name];
                        assert( var_ref );
                        print_debug("GetStdVarU","found omega output " + name,debug_GetStdVarU);
                }
                else
                {
                        int arity = ReturnVarianceLevel(name);
                        Const_String goofy_omega_string(StringToChar(name));

                        if (arity == 0)
                        {
                                print_debug("GetStdVarU","arity 0 creating omega free var " + name,debug_GetStdVarU);
                                FREE_VAR_U *free_var_u = new FREE_VAR_U();
                                assert(free_var_u);
                                free_var_u->_omega_free_var = new OMEGA_FREE_VAR(StringToChar(name));
                                assert(free_var_u->_omega_free_var);
                                FreeVar *free_var = new FreeVar(free_var_u);
                                assert(free_var);
                                _free_vars->insert(make_pair(name,free_var));

                                STD_VAR_U *std_var_u = new STD_VAR_U();
                                assert(std_var_u);
                                std_var_u->_omega_std_var = _omega_relation->get_local(free_var->ReturnOmegaFreeVar());
                                assert(std_var_u->_omega_std_var);
                                var_ref = new StandardVar(std_var_u);
                                assert(var_ref);
                                _omega_input_vars->insert(make_pair(name,new StandardVar(*var_ref)));
                                _omega_output_vars->insert(make_pair(name,var_ref));
                                
                                print_debug("GetStdVarU","finished creating omega free var " + name,debug_GetStdVarU);
                        }
                        else
                        {
                                print_debug("GetStdVarU","arity > 0 creating omega free var " + name,debug_GetStdVarU);
                                FREE_VAR_U *free_var_u = new FREE_VAR_U();
                                free_var_u->_omega_free_var = new OMEGA_FREE_VAR(goofy_omega_string,arity);
                                FreeVar *free_var = new FreeVar(free_var_u);
                                _free_vars->insert(make_pair(name,free_var));

                                STD_VAR_U *std_var_u = new STD_VAR_U();
                                std_var_u->_omega_std_var = _omega_relation->get_local(free_var->ReturnOmegaFreeVar(),
                                                Input_Tuple);
                                StandardVar *input_var_ref = new StandardVar(std_var_u);
                                _omega_input_vars->insert(make_pair(name, input_var_ref));

                                std_var_u = new STD_VAR_U();
                                std_var_u->_omega_std_var = _omega_relation->get_local(free_var->ReturnOmegaFreeVar(),
                                                Output_Tuple);
                                StandardVar *output_var_ref = new StandardVar(std_var_u);
                                _omega_output_vars->insert(make_pair(name, output_var_ref));
                                print_debug("GetStdVarU","finished creating omega free var " + name,debug_GetStdVarU);

                                if (is_input)
                                        var_ref = input_var_ref;
                                else
                                        var_ref = output_var_ref;
                        }
                }
         }
         else
         {
                if (_plato_input_vars->count(name) > 0 && is_input)
                {
                        print_debug("GetStdVarU","found plato input " + name,debug_GetStdVarU);
                        var_ref = (*_plato_input_vars)[name];
                        assert( (*_plato_input_vars)[name] );
                }
                else if (_plato_output_vars->count(name) > 0 && !is_input)
                {
                        print_debug("GetStdVarU","found plato output " + name,debug_GetStdVarU);
                        var_ref = (*_plato_output_vars)[name];
                        assert( (*_plato_output_vars)[name] );
                }
                else
                {
                        int arity = ReturnVarianceLevel(name);

                        /**     For plato, we set both the inner_var->_plato_var and
                        *               the inner_var_id->_plato_var_id to the same
                        *               PLATO_STD_VAR.
                        */

                        FREE_VAR_U *free_var_u = new FREE_VAR_U();
                        free_var_u->_plato_free_var = new PLATO_FREE_VAR(StringToChar(name));
                        FreeVar *free_var = new FreeVar(free_var_u);
                        _free_vars->insert(make_pair(name,free_var));

                        STD_VAR_U *std_var_u = new STD_VAR_U();
                        std_var_u->_plato_std_var = free_var->ReturnPlatoFreeVar();
                        var_ref = new StandardVar(std_var_u);
                        _plato_input_vars->insert(make_pair(name,new StandardVar(*var_ref)));
                        _plato_output_vars->insert(make_pair(name,var_ref));

                        _plato_relation->addSymbolicVariable(*free_var->ReturnPlatoFreeVar(),arity);
                }
         }
         print_debug("GetStdVarU","exiting",debug_GetStdVarU);
         assert( var_ref->ReturnStdVarU() );
    return var_ref->ReturnStdVarU();
}

void PlatoOmegaDepTesting::VisitConst( const SymbolicConst &v)
{
   int val = 0, frac = 0;
   v.GetIntVal(val,frac);
        print_debug("VisitConst","entering",debug_VisitConst);
        if (debug_VisitConst) {
                std::cerr << "val " << val << std::endl;
        }

        if (UseOmega())
        {
                assert(_constraint_handle);
                if (_omega_negate_expr) {
                        _constraint_handle->update_const( - val);
                }
                else {
                        _constraint_handle->update_const(val);
                }
        }
        else
        {
                plato::RationalExpression *v1 = new plato::RationalExpression(val);
                *_re = *v1;
        }
        print_debug("VisitConst","exiting",debug_VisitConst);
}

void PlatoOmegaDepTesting::VisitVar( const SymbolicVar &v)
{
        print_debug("VisitVar","entering " + v.GetVarName(),debug_VisitVar);
        if (UseOmega())
        {
                OMEGA_STD_VAR tempID = GetOmegaStdVar(v.GetVarName(),_is_input_expr);
                assert(_constraint_handle);
                if (_omega_negate_expr) {
                        _constraint_handle->update_coef(tempID,-1);
                }
                else {
                        _constraint_handle->update_coef(tempID,1);
                }
        }
        else
        {
                //define a new symbolic var
                PLATO_STD_VAR *tempID = GetPlatoStdVar(v.GetVarName(),_is_input_expr);
        }
        print_debug("VisitVar","exiting " + v.GetVarName(),debug_VisitVar);
}

void PlatoOmegaDepTesting::VisitExpr( const SymbolicExpr& v) {
   int operandsCount = 0;
   int value = 0, frac = 0;
   int varCount = 0;
   std::string name;

        print_debug("VisitExpr","entering",debug_VisitExpr);
        if (UseOmega())
                assert(_constraint_handle);
   SymbolicExpr::OpdIterator iter = v.GetOpdIterator();
   for (;!iter.ReachEnd(); ++iter)
   {
      SymbolicVal temp = v.Term2Val(iter.Current());
      switch (temp.GetValType())
                {
         case VAL_VAR : {
            name = temp.toString();
            operandsCount++;
            varCount++;
            switch (v.GetOpType())
                                {
               case SYMOP_MULTIPLY :
                                        {                  
                  if (operandsCount == 2 && varCount != 2)
                                                {
                                                        if (UseOmega())
                                                        {
                                                                OMEGA_STD_VAR tempID = GetOmegaStdVar(name,_is_input_expr);
                                                                if (_omega_negate_expr) {
                                                                        _constraint_handle->update_coef(tempID,-value);
                                                                }
                                                                else {
                                                                        _constraint_handle->update_coef(tempID,value);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                PLATO_STD_VAR *tempID = GetPlatoStdVar(name,_is_input_expr);
                                                                plato::RationalExpression *v1 = new plato::RationalExpression(*tempID);
                        plato::RationalExpression *v2 = new plato::RationalExpression(value);
                        *_re = *_re + *v1 * *v2;
                                                        }
                     operandsCount = 0;
                  }
                  else if (varCount == 2)
                                                {                     
                     _nonLinear = true;
                  }
               }
               break;
               case SYMOP_PLUS :
                                        {
                                                if (UseOmega())
                                                {
                                                        OMEGA_STD_VAR tempID = GetOmegaStdVar(name,_is_input_expr);
                                                        if (_omega_negate_expr)
                                                        {
                                                                _constraint_handle->update_coef(tempID,-1);
                                                        }
                                                        else
                                                        {
                                                                _constraint_handle->update_coef(tempID,1);
                                                        }
                                                }
                                                else
                                                {
                                                        PLATO_STD_VAR *tempID = GetPlatoStdVar(name,_is_input_expr);
                                                        plato::RationalExpression *v1 = new plato::RationalExpression(*tempID);
                        plato::RationalExpression *v2 = new plato::RationalExpression(value);
                        *_re = *_re + *v1 + *v2;
                                                }
               }
               break;
            }
         }
         break;
         case VAL_CONST :
                        {
            operandsCount++;
            temp.isConstInt(value,frac);
            switch (v.GetOpType())
                                {
               case SYMOP_MULTIPLY :
                                        {
                  if (operandsCount == 2)
                                                {
                                                        if (UseOmega())
                                                        {
                                                                if (_omega_negate_expr) {
                                                                        _constraint_handle->update_coef(GetOmegaStdVar(name,_is_input_expr),-value);
                                                                }
                                                                else {
                                                                        _constraint_handle->update_coef(GetOmegaStdVar(name,_is_input_expr),value);
                                                                }
                                                        }
                                                        else
                                                        {
                                                                plato::RationalExpression *v1 = new plato::RationalExpression(value);
                        *_re = *_re * *v1;
                                                        }
                     operandsCount = 0;
                  }                  
               }
               break;
               case SYMOP_PLUS :
                                        {
                                                if (UseOmega())
                                                {
                                                        if (_omega_negate_expr)
                                                        {
                                                                _constraint_handle->update_const( - value);
                                                        }
                                                        else {
                                                                _constraint_handle->update_const(value);
                                                        }
                                                }
                                                else
                                                {
                                                        plato::RationalExpression *v1 = new plato::RationalExpression(value);
                        *_re = *_re + *v1;
                                                }
               }
               break;
            }
         }
         break;
         case VAL_AST :
                        {
            temp.Visit(this);
         }
         break;
         case VAL_EXPR :
         case VAL_BASE :
         case VAL_FUNCTION :
                        {
            temp.Visit(this);
         }
         break;
         default :
                        {
            temp.Visit(this);
         }
         break;
      }
   }
        print_debug("VisitExpr","exiting",debug_VisitExpr);
}

void PlatoOmegaDepTesting::VisitFunction( const SymbolicFunction &v)
{
        if (UseOmega())
        {
                assert(_constraint_handle);
                if (_omega_negate_expr)
                {
                        _constraint_handle->update_const(negInfinity);
                }
                else
                {
                        _constraint_handle->update_const(posInfinity);
                }
        }
        else
        {
                if(_plato_is_bound)
                {
                        if (_plato_is_lower_bound) {
                                plato::RationalExpression *v1 = new plato::RationalExpression(MINUS_INF);
                                *_re = *v1;
                        }
                        else {
                                plato::RationalExpression *v1 = new plato::RationalExpression(PLUS_INF);
                                *_re = *v1;
                        }
                }
        }
}

void PlatoOmegaDepTesting::VisitAstWrap( const SymbolicAstWrap& v)
{
        if (UseOmega())
        {
                assert(_constraint_handle);
                if (_omega_negate_expr)
                {
                        _constraint_handle->update_const(negInfinity);
                }
                else
                {
                        _constraint_handle->update_const(posInfinity);
                }
        }
        else
        {
                if(_plato_is_bound)
                {
                        if (_plato_is_lower_bound) {
                                plato::RationalExpression *v1 = new plato::RationalExpression(MINUS_INF);
                                *_re = *v1;
                        }
                        else {
                                plato::RationalExpression *v1 = new plato::RationalExpression(PLUS_INF);
                                *_re = *v1;
                        }
                }
        }
}

plato::RationalExpression *PlatoOmegaDepTesting::ParseVal( const SymbolicVal &v, bool negate,
                                                                                                                        bool is_input, bool is_bound, bool is_low_bound)
{
        print_debug("ParseVal","entering",debug_ParseVal);
        _re = new plato::RationalExpression();
        _omega_negate_expr = negate;
        _is_input_expr = is_input;
        _plato_is_bound = is_bound;
        _plato_is_lower_bound;
        v.Visit(this);
        print_debug("ParseVal","exiting",debug_ParseVal);
        return(_re);
}

/**     Adds all the index variables in the common (to both array refs) loops.
*               This function makes a big assumption in that both the _info1->ivars
*               and _info2->ivars have exactly the same entries up to the
*               least dim - either _dim1 or _dim2. Then the loop stops
*               and further entries are picked up in the AddLocalLoopIndices.
**/
 
void PlatoOmegaDepTesting::AddCommonLoopIndices(void)
{
        int i = 1;
        STD_VAR_U *std_var_u;
        std::vector<SymbolicVar>::iterator iter1 = _info1->ivars.begin();
        std::vector<SymbolicVar>::iterator iter2 = _info2->ivars.begin();
        print_debug("AddCommonLoopIndices","entering",debug_AddCommonLoopIndices);
        while (iter1 != _info1->ivars.end() && iter2 != _info2->ivars.end())
        {
                const SymbolicVal &index = *iter1;

                //std::cerr << "level " << i << " var " << index.toString() << std::endl;
                if (UseOmega())
                //if (!added(_omega_input_vars,index.toString()) && UseOmega())
                {
                        std_var_u = new STD_VAR_U();
                        print_debug("AddCommonLoopIndices","creating omega input loop indice " + index.toString(),debug_AddCommonLoopIndices);
                        _omega_relation->name_input_var(i, StringToChar(index.toString()));
                        std_var_u->_omega_std_var = _omega_relation->input_var(i);
                        _omega_input_vars->insert(make_pair(index.toString(),new StandardVar(std_var_u)));
                //}
                //if (!added(_omega_output_vars,index.toString()) && UseOmega())
                //{
                        std_var_u = new STD_VAR_U();
                        print_debug("AddCommonLoopIndices","creating omega output loop indice " + index.toString(),debug_AddCommonLoopIndices);
                        _omega_relation->name_output_var(i, StringToChar(index.toString()));
                        std_var_u->_omega_std_var = _omega_relation->output_var(i);
                        _omega_output_vars->insert(make_pair(index.toString(),new StandardVar(std_var_u)));
                }
                //if (!added(_plato_input_vars,index.toString()) && !UseOmega())
                if (!UseOmega())
                {
                        print_debug("AddCommonLoopIndices","creating plato input loop indice " + index.toString(),debug_AddCommonLoopIndices);
                        /** We'll only create one var and use it in the inputs and outputs **/
                        STD_VAR_U *std_var_u_in = new STD_VAR_U();
                        std_var_u_in->_plato_std_var = new PLATO_STD_VAR(StringToChar(index.toString()));
                        _plato_input_vars->insert(make_pair(index.toString(),new StandardVar(std_var_u_in)));
                        //_plato_relation->addSymbolicVariable(*(std_var_u->_plato_std_var));
                //}
                //if (!added(_plato_output_vars,index.toString()) && !UseOmega())
                //{
                        print_debug("AddCommonLoopIndices","creating plato output loop indice " + index.toString(),debug_AddCommonLoopIndices);
                        STD_VAR_U *std_var_u_out = new STD_VAR_U();
                        std_var_u_out->_plato_std_var = std_var_u_in->_plato_std_var;
                        _plato_output_vars->insert(make_pair(index.toString(),new StandardVar(std_var_u_out)));
                        //_plato_relation->addSymbolicVariable(*(std_var_u->_plato_std_var));
                }                                       
                i++;
                iter1++;
                iter2++;
        }
        if(debug_AddCommonLoopIndices && UseOmega())
        {
                _omega_relation->print();
        }
        print_debug("AddCommonLoopIndices","exiting",debug_AddCommonLoopIndices);
}

/**     AddLocalLoopIndices adds all the loop indicies that surround the common
*               loops, and have not been added yet. The function makes the assumption
*               that if it hasn't been added yet, then that dimension is larger than the
*               other and a free variable will be made and it will be added to the
*               proper map.
**/

void PlatoOmegaDepTesting::AddLocalLoopIndices(void/*DepInfoAnal::LoopDepInfo info, bool is_input*/)
{
        //int i = 1;
        StandardVar *var_ref = 0;
        DepInfoAnal::LoopDepInfo *info;
        bool is_input;
        std::vector<SymbolicVar>::iterator iter;
        print_debug("AddLocalLoopIndices","entering",debug_AddLocalLoopIndices);
        for (int i = 0;i < 2;i++)
        {
                if (i == 0)
                {
                        info = _info1;
                        iter = _info1->ivars.begin();
                        is_input = true;
                }
                else
                {
                        info = _info2;
                        iter = _info2->ivars.begin();
                        is_input = false;
                }
                while (iter != info->ivars.end())
                {
                        const SymbolicVal &index = *iter;

                        if (UseOmega())
                        {
                                if ((!added(_omega_input_vars,index.toString()) && is_input) ||
                                                        (!added(_omega_output_vars,index.toString()) && !is_input))
                                {               
                                        FREE_VAR_U *free_var_u = new FREE_VAR_U();
                                        assert(free_var_u);
                                        free_var_u->_omega_free_var = new OMEGA_FREE_VAR(StringToChar(index.toString()));
                                        assert(free_var_u->_omega_free_var);
                                        FreeVar *free_var = new FreeVar(free_var_u);
                                        assert(free_var);
                                        _free_vars->insert(make_pair(index.toString(),free_var));
                                        
                                        STD_VAR_U *std_var_u = new STD_VAR_U();
                                        assert(std_var_u);
                                        std_var_u->_omega_std_var = _omega_relation->get_local(free_var->ReturnOmegaFreeVar());
                                        assert(std_var_u->_omega_std_var);
                                        var_ref = new StandardVar(std_var_u);
                                        assert(var_ref);

                                        if (is_input)
                                        {
                                                _omega_input_vars->insert(make_pair(index.toString(),var_ref));
                                        }
                                        else
                                        {
                                                _omega_output_vars->insert(make_pair(index.toString(),var_ref));
                                        }
                                }
                        }
                        /**     -------------- PLATO ONLY --------------
                        *               Here we are assuming that all of the common loop indices have
                        *               been added already, so any other enclosing loop indices
                        *               have to added as Symbolic Variables instead of common
                        *               loop variables.
                        **/
                        else if ((!added(_plato_input_vars,index.toString()) && is_input) ||
                                                        (!added(_plato_output_vars,index.toString()) && !is_input))
                        {
                                FREE_VAR_U *free_var_u = new FREE_VAR_U();
                                assert(free_var_u);
                                free_var_u->_plato_free_var = new PLATO_FREE_VAR(StringToChar(index.toString()));
                                assert(free_var_u->_plato_free_var);
                                FreeVar *free_var = new FreeVar(free_var_u);
                                assert(free_var);
                                _free_vars->insert(make_pair(index.toString(),free_var));

                                STD_VAR_U *std_var_u = new STD_VAR_U();
                                assert(std_var_u);
                                std_var_u->_plato_std_var = free_var->ReturnPlatoFreeVar();
                                assert(std_var_u->_plato_std_var);
                                var_ref = new StandardVar(std_var_u);
                                
                                if (is_input)
                                {
                                        print_debug("AddLocalLoopIndices","creating plato input loop indice " + index.toString(),debug_AddLocalLoopIndices);
                                        _plato_input_vars->insert(make_pair(index.toString(),var_ref));
                                }
                                else
                                {
                                        print_debug("AddLocalLoopIndices","creating plato output loop indice " + index.toString(),debug_AddLocalLoopIndices);
                                        _plato_output_vars->insert(make_pair(index.toString(),var_ref));
                                }

                                //_plato_relation->addSymbolicVariable(*free_var->ReturnPlatoFreeVar());
                        }
                        iter++;
                        //i++;
                }
        }
        print_debug("AddLocalLoopIndices","exiting",debug_AddLocalLoopIndices);
}

/** Adds the loop bounds of the common loop index variables **/

void PlatoOmegaDepTesting::AddLoopBounds(void /*DepInfoAnal::LoopDepInfo info, bool is_input*/)
{
   int stride = 1;
   int val;
   int dim;// = info.domain.NumOfLoops();
        bool is_input;
        DepInfoAnal::LoopDepInfo *info;

        print_debug("AddLoopBounds","entering ",debug_AddLoopBounds);
        for (int h = 0;h < 2;h++) //input then output
        {
                if (h == 0)
                {
                        info = _info1;
                        dim = _dim1;
                        is_input = true;
                }
                else
                {
                        info = _info2;
                        dim = _dim2;
                        is_input = false;
                }
                for (int i = 0;i < dim; i++)
                {
                        SymbolicVal symVal(info->ivars[i]);
                        SymbolicVal init(info->ivarbounds[i].lb);
                        SymbolicVal limit(info->ivarbounds[i].ub);
                        SymbolicVal *lb = &init;
                        SymbolicVal *ub = &limit;
                        if (stride < 0)
                        {
                                stride = - stride;
                                lb = &limit;
                                ub = &init;
                        }
                        if (UseOmega())
                        {
                                OMEGA_STD_VAR index;

                                index = GetOmegaStdVar(info->ivars[i].GetVarName(),is_input);

                                print_debug("AddLoopBounds","creating lower bound constraints for " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);

                                GEQ_Handle lb_constraint = _omega_constraints->add_GEQ();
                                _constraint_handle = &lb_constraint;
                                ParseVal(*lb,true,is_input,false,false);

                                lb_constraint.update_coef(index,1);

                                GEQ_Handle ub_constraint = _omega_constraints->add_GEQ();
                                _constraint_handle = &ub_constraint;
                                ParseVal(*ub,false,is_input,false,false);
                                ub_constraint.update_coef(index,-1);
                                print_debug("AddLoopBounds","finished with upper bound constraints",debug_AddLoopBounds);
                        }
                        else 
                        {
                                print_debug("AddLoopBounds","creating bound constraints for " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);
                                PLATO_STD_VAR *index;
                                plato::RationalExpression *lowBound;
                                plato::RationalExpression *upBound;

                                /**     Pick up any variables that might have been missed so far thru **/

                                index = GetPlatoStdVar(info->ivars[i].GetVarName(),is_input);

                                lowBound = ParseVal(*lb,false,is_input,true,true);
                                
                                upBound = ParseVal(*ub,false,is_input,true,false);

                                /**     here **/

                                /**     ---------------PLATO ONLY-------------
                                *               Here we add the common loop indices and bounds.
                                *               Any other enclosing loop indices were already
                                *               added in the AddLocalLoopIndices function.
                                **/
                                /*if (is_input && i < _commLevel)
                                {
                                        print_debug("AddLoopBounds","about to addCommonLoopVariable " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);
                                        _plato_relation->addCommonLoopVariable(*index,*lowBound,*upBound,1);
                                }
                                */
                                if (i < _commLevel && is_input) //add common vars only once
                                {
                                        print_debug("AddLoopBounds","about to add plato common loop variable " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);
                                        _plato_relation->addCommonLoopVariable(*index,*lowBound,*upBound,1);
                                }
                                else if (is_input && i >= _dim2)
                                {
                                        print_debug("AddLoopBounds","about to add plato local input loop variable " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);
                                        _plato_relation->addNest1LoopVariable(*index,*lowBound,*upBound,1);
                                }
                                else if (!is_input && i >= _dim1)
                                {
                                        print_debug("AddLoopBounds","about to add plato local output loop variable " + info->ivars[i].GetVarName(),
                                                                debug_AddLoopBounds);
                                        _plato_relation->addNest2LoopVariable(*index,*lowBound,*upBound,1);
                                } 
                        }
                }
        }
        if (debug_AddLoopBounds && UseOmega())
        {
                _omega_relation->print();
        }
        print_debug("AddLoopBounds","exiting",debug_AddLoopBounds);
}

/** Parses the array references to add the constraints to the relations **/

void PlatoOmegaDepTesting::AddSubscripts(void)
{
        print_debug("AddSubscripts","entering",debug_AddSubscripts);
   AstInterface::AstNodeList sub1, sub2;
        const AstNodePtr &ref1 = _ref->r1.ref;
        const AstNodePtr &ref2 = _ref->r2.ref;
   bool succ1 = _fa->IsArrayAccess(ref1, 0, &sub1);
   bool succ2 = _fa->IsArrayAccess(ref2, 0, &sub2);
   assert(succ1 && succ2);
        AstInterface::AstNodeList::reverse_iterator iter1 = sub1.rbegin();
        AstInterface::AstNodeList::reverse_iterator iter2 = sub2.rbegin();
   AstNodePtr s1, s2;
        for (;iter1 != sub1.rend() && iter2 != sub2.rend();iter1++,iter2++)
        {
                s1 = *iter1; s2 = *iter2;
                if (UseOmega())
                {
                        SymbolicVal val1 = SymbolicValGenerator::GetSymbolicVal(*_fa, s1);
                        SymbolicVal val2 = SymbolicValGenerator::GetSymbolicVal(*_fa, s2);
                        EQ_Handle sub_equal = _omega_constraints->add_EQ();
                        _constraint_handle = &sub_equal;

                        _constraint_handle = &sub_equal;
                        ParseVal(val1,false,true,false,false);

                        //Now do the other side of the equation.
                        _constraint_handle = &sub_equal;
                        ParseVal(val2,true,false,false,false);
                }
                else
                {
                        plato::RationalExpression val1 = GetRationalExpression(*_fa,s1,true);
        plato::RationalExpression val2 = GetRationalExpression(*_fa,s2,false);
                        print_debug("AddSubscripts","about to add plato subscripts",debug_AddSubscripts);
                        _plato_relation->addSubscript(val1,val2);
                }
   } 
        if (debug_AddSubscripts && UseOmega())
        {
                _omega_relation->print();
        }       
        print_debug("AddSubscripts","exiting",debug_AddSubscripts);
}

/**     ----------------OMEGA ONLY--------------
*               Adds constraints imposed by the given direction vector.
*               This routine is only used by the routines that use the
*               omega library.
**/

void PlatoOmegaDepTesting::AddConstraintsImposedByDV(Relation &relation,
                                      F_And &constraints,
                                      const plato::dddir &dv)
{
        int loop_depth = 0;
        plato::dddir dv_iter = dv;

        print_debug("AddConstraintsImposedByDV","entering",debug_AddConstraintsImposedByDV);
        for (loop_depth = 0;loop_depth < _commLevel;loop_depth++) {
                OMEGA_STD_VAR input_var = relation.input_var(loop_depth + 1);
                OMEGA_STD_VAR output_var = relation.output_var(loop_depth + 1);
                int loop_step = 1;//_common_loop_steps[loop_depth];
                plato::dddir dir = ddextract1(dv,loop_depth);

                /*if (loop_step < 0)
                        dir = invert_dir(dir);
                else if (loop_step == 0) {
                        // The actual loop step is unknown, so remove all constraints
                        // except for equality and unequality constraints.
                        /*
                        switch (dir) {
                                case _LT:   dir = _NEQ;  break;
                                case _GT:   dir = _NEQ;  break;
                                case _EQ:                break;
                                case _NEQ:               break;
                                default:    dir = _STAR; break;
                        }
                }
                */
                
                switch(dir)
                {
                        case (DDLES | DDEQU) : 
                        case DDLES:
                        {
                                GEQ_Handle lt_constraint = constraints.add_GEQ();
                                lt_constraint.update_coef(input_var, -1);
                                lt_constraint.update_coef(output_var, 1);

                                if (dir == (DDLES | DDEQU))
                                        lt_constraint.update_const(-1);                 
                        }
                        break;

                        case (DDGRT | DDEQU) :
                        case DDGRT:
                        {
                                GEQ_Handle gt_constraint = constraints.add_GEQ();
                                gt_constraint.update_coef(input_var, 1);
                                gt_constraint.update_coef(output_var, -1);

                                if (dir == (DDGRT | DDEQU))
                                        gt_constraint.update_const(-1);                 
                        }
                        break;

                        case DDEQU:
                        {
                                EQ_Handle eq_constraint = constraints.add_EQ();
                                eq_constraint.update_coef(input_var, 1);
                                eq_constraint.update_coef(output_var, -1);
                        }
                        break;

                        /*
                        case _NEQ: {
                                EQ_Handle neq_constraint = constraints.add_not()->add_and()->add_EQ();
                                neq_constraint.update_coef(input_var, 1);
                                neq_constraint.update_coef(output_var, -1);
                        }
                        break;
                        */
                        case DDALL:
                                // No constraints to build
                                break;

                        default:
                                //p_abort("_add_dv_constraints(): Unrecognized dependence direction.");
                        break;
                }
        }
        print_debug("AddConstraintsImposedByDV","exiting",debug_AddConstraintsImposedByDV);
}

/**     ----------------OMEGA ONLY--------------
*               Returns a relation with the same number of input and output variables
*               as my relation and with constraints imposed upon these variables by
*               the given direction vector.
**/

Relation PlatoOmegaDepTesting::GetNewRelationFromDV(const plato::dddir &dv)
{
        int i = 1;
        Relation relation(_dim1,_dim2);

        print_debug("GetNewRelationFromDV", "entering",debug_GetNewRelationFromDV);

        std::vector<SymbolicVar>::iterator iter1 = _info1->ivars.begin();
        std::vector<SymbolicVar>::iterator iter2 = _info2->ivars.begin();
        while (iter1 != _info1->ivars.end() && iter2 != _info2->ivars.end())
        {
                const SymbolicVal &index = *iter1;
                //std::cerr << "creating name " << index.toString() << " index " << i << std::endl;
                relation.name_input_var(i, StringToChar(index.toString()));
                relation.name_output_var(i, StringToChar(index.toString()));
                i++;
                iter1++;
                iter2++;
        }

        /*STD_VAR_MAP::iterator iter1 = _omega_input_vars->begin();
   for ( ; iter1 != _omega_input_vars->end(); ++iter1)
        {
                relation.name_input_var(i,StringToChar(iter1->first));
                i++;
   }
        i = 1;
   STD_VAR_MAP::iterator iter2 = _omega_output_vars->begin();
   for ( ; iter2 != _omega_output_vars->end(); ++iter2) {
                relation.name_output_var(i,StringToChar(iter2->first));
                i++;
   }
        */
        F_And *r_root = relation.add_and();
        AddConstraintsImposedByDV(relation, *r_root, dv);
        relation.finalize();

        print_debug("GetNewRelationFromDV", "exiting",debug_GetNewRelationFromDV);
        return relation;
}
/* From Polaris, might need to be used.

void SetBounds(int *lower_bound, int *upper_bound)
{
        //#define abs(x) ( ((x) > 0) ? (x) : -1*(x) )

        if(abs(*lower_bound) > 127)
                *lower_bound = INV_DDIST -1;
        else
                *lower_bound = -*lower_bound;
        if(abs(*upper_bound) > 127)
                *upper_bound = INV_DDIST -1;
        else
                *upper_bound = -*upper_bound;
}
*/
/**     ---------------OMEGA ONLY------------
*               This function tests all the direction vectors.
*               It is only used when the omega library is being used.
*               Omega   LowBnd  Up Bnd  ***     Rose    Align
*               =               0                       0                                       =               0
*               <                                       = 0                             
**/

void PlatoOmegaDepTesting::doHierarchy(SetDep& setdep, DepInfo &result, plato::dddir dv, int level, DepRel& rel)
{                                       // Recursive data dependence test
        static plato::dddir dirtab[] = {DDLES, DDEQU, DDGRT};
        plato::dddir newdv,savedv;
        int i, j, sharedLevel, precise; 
        DepRel tempRel = rel, *relPtr;
        int tempDim = (_dim1 > _dim2) ? _dim1 : _dim2;
        
        sharedLevel = _commLevel;

        /**     Recursively construct all of the direction vectors.
        *               After they are all constructed by calling this
        *               this algorithm recursively, then process
        *               each direction vector.
        **/

        if (level <= sharedLevel)
        {
                for (i = 0; i < 3; i++) /** Construct all three for each level **/
                {           // Test for '<', '=', and '>'
                        newdv = dv;
                        dddironly (newdv, dirtab[i], level );
                        doHierarchy(setdep,result,newdv, level + 1,rel);
                }
        }
        else /** Finished constructing, now test **/
        {
                if (DebugDep() || debug_doHierarchy)
                        std::cerr << "entering doHierarchy Testing Direction Vector " << str_dir_vec (dv, sharedLevel) << " level " << level <<std::endl;
                Relation  copy_rel = copy(*_omega_relation);
                Relation  dv_rel   = GetNewRelationFromDV(dv); //set relation to use new dv
                Relation dv_omega_relation = Intersection(copy_rel, dv_rel);
                int lower_bound, upper_bound;
                bool coupled;
                if (debug_doHierarchy)
                {
                        //dv_omega_relation.print();
                }
                for (i = 0;i < _commLevel;i++)
                {                     // Refine inner direction
                        j = i+tempDim;
                        OMEGA_STD_VAR input_var = dv_omega_relation.input_var(i+1);
                        OMEGA_STD_VAR output_var = dv_omega_relation.output_var(i+1);
                        /*if (DebugDep() || debug_doHierarchy)
                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " looping " << std::endl;
                        */
                        if (dv_omega_relation.is_upper_bound_satisfiable())
                        {
                                /*if (DebugDep() || debug_doHierarchy)
                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " is upper bound sat " << std::endl;
                                */
                                //dv_omega_relation.print();
                                dv_omega_relation.query_difference(input_var, output_var, lower_bound,
                                                                                        upper_bound, coupled);
                                if (lower_bound == 0 && upper_bound == 0 && level > sharedLevel)
                                {
                                        dddirset(_saved_dv,DDEQU,i+1);
                                        rel |= DepRel(DEPDIR_EQ,0);
                                        //rel.UnionUpdate(rel,DepRel(DEPDIR_EQ,0));
                                        setdep[i][j] = rel;
                                        precise = true;
                                        /*if (DebugDep() || debug_doHierarchy)
                                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " assigned DEPDIR_EQ " << std::endl;
                                        */
                                }
                                else if (lower_bound >= 0 && level > sharedLevel)
                                {
                                        dddirset(_saved_dv,DDGRT,i+1);                                  
                                        if (upper_bound != posInfinity)
                                        {
                                                rel |= DepRel(DEPDIR_EQ,lower_bound,upper_bound);
                                                //rel.UnionUpdate(rel,DepRel(DEPDIR_EQ,lower_bound,upper_bound));
                                                setdep[i][j] = rel;
                                                if (DebugDep() || debug_doHierarchy)
                                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " assigned DEPDIR_EQ " << lower_bound << " " << upper_bound << std::endl;                                              
                                                precise = true;
                                        }
                                        else
                                        {
                                                if (DebugDep() || debug_doHierarchy)
                                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " assigned DEPDIR_GE " << std::endl;
                                                rel |= DepRel(DEPDIR_GE,0);
                                                //rel.UnionUpdate(rel,DepRel(DEPDIR_GE,0));
                                                setdep[i][j] = rel;
                                                precise = false;
                                        }
                                        /*if (DebugDep() || debug_doHierarchy)
                                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " DDGRT loop " << std::endl;
                                        */
                                }
                                else if (upper_bound <= 0 && level > sharedLevel)
                                {
                                        dddirset(_saved_dv,DDLES,i+1);
                                        if (lower_bound != negInfinity) {
                                                rel |= DepRel(DEPDIR_EQ,lower_bound,upper_bound);
                                                //rel.UnionUpdate(rel,DepRel(DEPDIR_EQ,lower_bound,upper_bound));
                                                setdep[i][j] = rel;
                                                if (DebugDep() || debug_doHierarchy)
                                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel  << " assigned DEPDIR_EQ " << lower_bound << " " << upper_bound << std::endl;                                             
                                                precise = true;
                                        }
                                        else
                                        {
                                                if (DebugDep() || debug_doHierarchy)
                                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel  << " assigned DEPDIR_LE " << std::endl;
                                                rel |= DepRel(DEPDIR_LE,0);
                                                //rel.UnionUpdate(rel,DepRel(DEPDIR_LE,0));
                                                setdep[i][j] = rel;
                                                precise = false;
                                        }
                                        /*if (DebugDep() || debug_doHierarchy)
                                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " DDLES loop " << std::endl;
                                        */
                                }
                                else if (level > sharedLevel)
                                {
                                        dddirset(_saved_dv,DDALL,i+1);
                                        
                                        if (DebugDep() || debug_doHierarchy)
                                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel  << " assigned DEPDIR_ALL "<< std::endl;
                                        rel |= DepRel(DEPDIR_ALL);
                                        //rel.UnionUpdate(rel,DepRel(DEPDIR_ALL));
                                        setdep[i][j] = rel;
                                        precise = false;
                                        /*if (DebugDep() || debug_doHierarchy)
                                                std::cerr << "i " << i << " " << " _commLevel " << _commLevel  << " DDALL loop " << std::endl;
                                        */
                                        //PrintResults((std::string)_omega_relation->print_with_subs_to_string(false));
                                }
                                if (DebugDep() || debug_doHierarchy)
                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel << " has DV of " << str_dir_vec (_saved_dv, sharedLevel) << std::endl;
                        }
                        else
                        {
                                if (DebugDep() || debug_doHierarchy)
                                        std::cerr << "i " << i << " " << " _commLevel " << _commLevel  << " assigned DEPDIR_NONE " << std::endl;
                                dddirset(_saved_dv,0,i+1);
                                rel |= DepRel(DEPDIR_NONE);
                                //rel.UnionUpdate(rel,DepRel(DEPDIR_NONE));
                                setdep[i][j] = rel;
                        }
                }
        }
}

void PlatoOmegaDepTesting::CleanUp(void)
{
        if(UseOmega())
        {
                delete(_omega_relation);
                delete(_omega_input_vars);
                delete(_omega_output_vars);
        }
        else
        {
                delete(_plato_relation);
                delete(_plato_driver);
                delete(_plato_input_vars);
                delete(_plato_output_vars);
        }
        delete(_free_vars);
}

/**     This function is under construction
**/

void PlatoOmegaDepTesting::RenameDupedVars(void)
{

        int postfix = 0, i;
        AstInterface::AstNodeList sub1, sub2;
        std::stringstream varpostfix1, varpostfix2;
        ++postfix;
        varpostfix1 << "___depanal_" << postfix;
        ++postfix;
        varpostfix2 << "___depanal_" << postfix;

        print_debug("RenameDupedVars","entering",debug_RenameDupedVars);

        LoopTransformInterface fa((LoopTransformInterface &) *_fa);
        MakeUniqueVar::ReverseRecMap varmap;
        MakeUniqueVar varop(_anal->GetModifyVariableInfo(),varmap);
        MakeUniqueVarGetBound boundop(varmap, (LoopTransformInterface &) *_fa, *_anal);

        for (i = 0; i < _dim1; ++i)
                _bounds->push_back(_info1->ivarbounds[i]);
        for (i = 0 ; i < _dim2; ++i)
                _bounds->push_back(_info2->ivarbounds[i]);

        const AstNodePtr &ref1 = _ref->r1.ref;
        const AstNodePtr &ref2 = _ref->r2.ref;
   bool succ1 = _fa->IsArrayAccess(ref1, 0, &sub1);
   bool succ2 = _fa->IsArrayAccess(ref2, 0, &sub2);
   assert(succ1 && succ2);

        AstInterface::AstNodeList::reverse_iterator iter1 = sub1.rbegin();
        AstInterface::AstNodeList::reverse_iterator iter2 = sub2.rbegin();
   AstNodePtr s1, s2;
        std::vector <std::vector<SymbolicVal> > analMatrix;
        print_debug("RenameDupedVars","fixing to parse vals",debug_RenameDupedVars);
        for (;iter1 != sub1.rend() && iter2 != sub2.rend();iter1++,iter2++)
        {
                print_debug("RenameDupedVars","starting loop",debug_RenameDupedVars);
                s1 = *iter1; s2 = *iter2;
                SymbolicVal val1 = SymbolicValGenerator::GetSymbolicVal(*_fa, s1);
        SymbolicVal val2 = SymbolicValGenerator::GetSymbolicVal(*_fa, s2);
                std::vector<SymbolicVal> cur;
                SymbolicVal left1 = DecomposeAffineExpression(fa, val1, _info1->ivars, cur,_dim1);
        SymbolicVal left2 = DecomposeAffineExpression(fa, -val2, _info2->ivars,cur,_dim2);
                for (i = 0; i < _dim1; ++i)
                {
                        print_debug("RenameDupedVars","fixing to varop",debug_RenameDupedVars);
                        //assert(*_ref->commLoop);
                        cur[i] = varop(_ref->commLoop, _ref->r1.ref, cur[i], varpostfix1.str());
                        print_debug("RenameDupedVars","finished varoping",debug_RenameDupedVars);
                }
                print_debug("RenameDupedVars","finished first dim",debug_RenameDupedVars);
                //left1 = varop(_ref->commLoop, _ref->r1.ref, left1, varpostfix1.str());
                for (; i < _dim1 + _dim2; ++i)
                {
                        cur[i] = varop(_ref->commLoop, _ref->r2.ref, cur[i], varpostfix2.str());
                }
                print_debug("RenameDupedVars","finished other dim",debug_RenameDupedVars);
                //left2 = varop(_ref->commLoop, _ref->r2.ref, left2, varpostfix2.str());
                //SymbolicVal leftVal = -left2 - left1;
                //cur.push_back(leftVal);
                //if (DebugDep())
                {
                        //assert(dim+1 == cur.size());
                        //std::cerr << "coefficients for induction variables (" << _dim1 << " + " << _dim2 << "+ 1)\n";
                        //for (int i = 0; i < _dim1 + _dim2; ++i)
                                //std::cerr << cur[i].toString() << _bounds[i]->toString() << " " ;
                        //std::cerr << cur[_dim1+_dim2].toString() << std::endl;
                }
                for ( int i = 0; i < _dim1 + _dim2; ++i) {
        SymbolicVal cut = cur[i];
        if (cut == 1 || cut == 0 || cut == -1)
             continue;
        std::vector<SymbolicVal> split;
        if (SplitEquation( fa, cur, cut, *_bounds, boundop, split)) 
             analMatrix.push_back(split);
                }
                analMatrix.push_back(cur);
                print_debug("RenameDupedVars","finisishing loop body",debug_RenameDupedVars);
        }
        print_debug("RenameDupedVars","exiting",debug_RenameDupedVars);
}

double my_time()
{
        struct rusage ruse;
        getrusage(RUSAGE_SELF, &ruse);
        return( (double)(ruse.ru_utime.tv_sec+ruse.ru_utime.tv_usec*1.0e-6) );
}

DepInfo 
PlatoOmegaDepTesting::ComputeArrayDep(LoopTransformInterface &fa, DepInfoAnal& anal,
                       const DepInfoAnal::StmtRefDep& ref, DepType deptype)
{       
   if ((DebugDep() || debug_ComputeArrayDep) && UseOmega())
        {
      std::cerr << "OmegaDependenceTesting compute array dep between " << AstToString(ref.r1.ref) << " and " << \
         AstToString(ref.r2.ref) << std::endl;
        }
        else if ((DebugDep() || debug_ComputeArrayDep) && !UseOmega())
        {
      std::cerr << "PlatoDependenceTesting compute array dep between " << AstToString(ref.r1.ref) << " and " << \
         AstToString(ref.r2.ref) << std::endl;
        }

   /**  info1 and info2 contains both a vector of the induction variables of
   *            the surrounding loops and a vector of the bounds of the induction variables
   *            See definition for LoopDepInfo in DepInfoAnal.h
   **/
  
   const DepInfoAnal::LoopDepInfo& info1 = anal.GetStmtInfo(fa,ref.r1.stmt);
   const DepInfoAnal::LoopDepInfo& info2 = anal.GetStmtInfo(fa,ref.r2.stmt);
        _info1 = (DepInfoAnal::LoopDepInfo*) &info1;
        _info2 = (DepInfoAnal::LoopDepInfo*) &info2;
        _fa = (AstInterface *) &fa;
        _anal = &anal;
        _ref = (DepInfoAnal::StmtRefDep *) &ref;

        _bounds = new std::vector<SymbolicBound>;

        MakeUniqueVar::ReverseRecMap varmap;
        MakeUniqueVar varop(anal.GetModifyVariableInfo(),varmap);
        MakeUniqueVarGetBound boundop(varmap, fa, anal);

        _varmap = &varmap;
        _varop = &varop;
        _boundop = &boundop;

   /**  info1.domain and info2.domain are matrices of direction entries
   *            that model the relations of the induction variables surrounding each stmt.
   *            _dim1 and _dim2 are the numbers of loops surrounding r1 and r2 respectively
   **/
        
   _dim1 = info1.domain.NumOfLoops(), _dim2 = info2.domain.NumOfLoops();

        _commLevel = _ref->commLevel;   
        
        if (!UseOmega())
        {
                PLATO_STD_VAR::resetVars();
        }

   /**  'precise' is set to be true to indicate a 'yes' answer from testing.
   *            should be reset to false if the testing result is 'maybe'
   **/
        
   int precise = true;

        /**     result contains a new DepInfo object that will remember the dependence
   *            relations between r1 and r2
   **/
   DepInfo result =DepInfoGenerator::GetDepInfo(_dim1, _dim2, deptype, ref.r1.ref, ref.r2.ref, false, ref.commLevel);
    
   /** setdep should be the interface one uses to set entries of result **/
        
   SetDep setdep( info1.domain, info2.domain, &result);

        if (_commLevel > 0)
        {
                _free_vars = new FREE_VAR_MAP;
                
                if (UseOmega())
                {
                        _omega_relation = new Relation(_dim1,_dim2);
                        _omega_constraints = _omega_relation->add_and();
                        _omega_input_vars = new STD_VAR_MAP;
                        _omega_output_vars = new STD_VAR_MAP;
                        assert( _omega_relation && _omega_constraints && _omega_input_vars && _omega_output_vars && _free_vars );
                }
                else
                {
                        _plato_relation = new PlatoRelation(_commLevel,DebugDep());
                        _plato_driver = new PlatoDriver(*_plato_relation,DebugDep());
                        _plato_input_vars = new STD_VAR_MAP;
                        _plato_output_vars = new STD_VAR_MAP;
                        assert( _plato_relation && _plato_driver && _plato_input_vars && _plato_input_vars &&  _free_vars );
                }

                //RenameDupedVars();

                /** Find and create all the common loop indices **/
                
                AddCommonLoopIndices();

                /** Find and create any remaining enclosing loop indices **/

                AddLocalLoopIndices();

                /** Add all constraints given by the loop bounds to the relation. */

                AddLoopBounds();

                /** Now parse the subscripts and add them **/

                AddSubscripts();

                if (UseOmega())
                {
                        _omega_relation->finalize();
                        _omega_relation->simplify();
                }

/*
                if (DebugDep() && !_nonLinear) {
                        _omega_relation->print();
                        _omega_relation->print_with_subs(stdout);
                        _omega_relation->prefix_print();
                        if (_omega_relation->is_upper_bound_satisfiable()) {
                                std::cerr << "is upper bound satisfiable" << std::endl;
                        }
                        else {
                                std::cerr << "is not upper bound satisfiable" << std::endl;
                        }
                        if (_omega_relation->is_obvious_tautology()) {
                                std::cerr << "is tautology" << std::endl;
                        }
                        else {
                                std::cerr << "is not tautology" << std::endl;
                        }
                }
*/
                if (_nonLinear)
                        precise = false;
                
                plato::dddir dv = DDALLDV;
                int tempDim = (_dim1 >= _dim2) ? _dim1 : _dim2;
                _saved_dv = 0;  
                if (UseOmega())
                {
                        DepRel rel = DepRel(DEPDIR_NONE);
                        DepStats.InitOmegaTime();
                        doHierarchy(setdep,result,dv,1,rel);
                        DepStats.SetOmegaTime();
                }
                else
                {
                        DepStats.InitPlatoTime();
                        _plato_driver->doDDTest();
                        DepStats.SetPlatoTime();
                        _saved_dv = _plato_driver->GetDirectionVector();
                }

                dv = 0;

                /** We need to set the DepInfo result with our dependency results **/

                for (int i = 0;i < ref.commLevel;i++) {
                        dv = ddextract1(_saved_dv,i+1);
                        int j = i+tempDim;
                        switch (dv) {
                                case DDLES :
                                {
                                        setdep[i][j] = DepRel(DEPDIR_LE);
                                }
                                break;
                                case DDLES | DDEQU :
                                {
                                        setdep[i][j] = (DepRel(DEPDIR_LE) | DepRel(DEPDIR_EQ,0));
                                }
                                break;
                                case DDEQU :
                                {
                                        setdep[i][j] = DepRel(DEPDIR_EQ,0);
                                }
                                break;
                                case DDGRT :
                                {
                                        setdep[i][j] = DepRel(DEPDIR_GE);
                                }
                                break;
                                case DDGRT | DDEQU :
                                {
                                        setdep[i][j] = (DepRel(DEPDIR_GE) | DepRel(DEPDIR_EQ,0));
                                }
                                break;
                                case DDGRT | DDLES :
                                {
                                        setdep[i][j] = (DepRel(DEPDIR_GE) | DepRel(DEPDIR_LE));
                                }
                                break;
                                case DDALL :
                                {
                                        setdep[i][j] = DepRel(DEPDIR_ALL);
                                }                               
                                break;
                                case 0 :
                                {
                                        setdep[i][j] = DepRel(DEPDIR_NONE);
                                }
                                break;
                                default :
                                {
                                        //setdep[i][j] = rel * DepRel(DEPDIR_NONE);
                                }
                                break;
                        }
                }

                if ((DebugDep() || debug_ComputeArrayDep) && UseOmega())
                        std::cerr << "Omega Returned Direction Vector " << str_dir_vec (_saved_dv, ref.commLevel) << std::endl;
                else if ((DebugDep() || debug_ComputeArrayDep) && !UseOmega())
                        std::cerr << "Plato Returned Direction Vector " << str_dir_vec (_saved_dv, ref.commLevel) << std::endl;

                CleanUp();
                
                if (!setdep)
                        return DepInfo();
                if (precise)
                        result.set_precise();
        }

   /**  the finalize function is called to impose as much restrictions as possible
   *            to the DepInfo result
   **/
   setdep.finalize();
   
   if (DebugDep() || debug_ComputeArrayDep)
        std::cerr << "after restrictions from stmt domain, result =: \n" << result.toString() << std::endl;

   return result;
}

}

/********************************
*       FILE: PlatoOmegaInterface.h
*       AUTHOR: Mike Stiles
*       University of Texas at San Antonio 2007
*********************************/

#ifndef PLATO_OMEGA_INTERFACE
#define PLATO_OMEGA_INTERFACE

#include <CountRefHandle.h>
#include <FunctionObject.h>
#include <AstInterface.h>
#include <assert.h>
#include <string>
#include <set>
#include <map>
#include <DepInfoAnal.h>
#include <SymbolicVal.h>
#include <SymbolicBound.h>
#include <SymbolicExpr.h>
#include <omega.h>
#include <DDTypes.h>
#include <DepRel.h>
#include <fstream>
#include <DDTypes.h>
#include <DepTestStatistics.h>

#include "RationalExpression.h"
#include "RationalVITestProblem.h"
#include "DDTesting.h"

/**
*       This Header file defines the interface to the Plato and Omega
*       dependency testing libraries.
*
*       Variables or functions that are specific to one of the libraries
*       will have Plato or Omega in the name.
*       
*       Classes and functions have the first letter of each word
*       capitalized with no underscores.
*
*       typedefs are normally all capitalized with underscores between
*       each word.
*
*       Class variables start with an underscore and have no capitals.
*
*       Function variables do not start with an underscore but still
*       have them between words, and there is no capitals.
**/

namespace PlatoOmegaInterface
{
        class PlatoOmegaDepTesting;

        typedef enum {ADHOC = 0x1,PLATO = 0x2,OMEGA = 0x4} DEP_TEST_CHOICE;

        bool UseOmega(void);

        void SetDepChoice(DEP_TEST_CHOICE new_choice);

        typedef Variable_ID OMEGA_STD_VAR;
        typedef Free_Var_Decl OMEGA_FREE_VAR;
        typedef plato::Variable PLATO_STD_VAR;
        typedef plato::Variable PLATO_FREE_VAR;

        /**     These structures hold pointers to the actual library variables.
        *               They only hold one or the other depending on the 
        *               test being utilized (Plato or Omega).
        *               Since they are pointers, a union is not recommended.
        **/

        typedef struct VAR_U { OMEGA_FREE_VAR *_omega_free_var; PLATO_FREE_VAR  *_plato_free_var; } FREE_VAR_U;
        typedef struct VAR_ID_U { OMEGA_STD_VAR _omega_std_var; PLATO_STD_VAR *_plato_std_var; } STD_VAR_U;

        /**     FreeVar is an abstraction for the free variables in the
        *               Plato and Omega libraries.
        **/

        class FreeVar
        {
                public:
                        inline FreeVar(FREE_VAR_U *free_var_u);
                        inline FreeVar(const FreeVar &other);
                        inline virtual ~FreeVar();

                        inline FREE_VAR_U *ReturnFreeVarU();
                        inline OMEGA_FREE_VAR *ReturnOmegaFreeVar() {
                                assert(_free_var_u->_omega_free_var);
                                return _free_var_u->_omega_free_var;
                        };
                        inline PLATO_FREE_VAR *ReturnPlatoFreeVar() {
                                assert(_free_var_u->_plato_free_var);
                                return _free_var_u->_plato_free_var;
                        };

                private:
                        FREE_VAR_U *_free_var_u;
        };

        /**     StandardVar is an abstraction for the induction
        *               variables in the Plato and Omega libraries.
        **/
        
        class StandardVar
        {
                public:
                        inline StandardVar(STD_VAR_U *std_var_u);
                        inline StandardVar(const StandardVar &other);
                        inline virtual ~StandardVar();

                        inline STD_VAR_U *ReturnStdVarU() const;
                        inline OMEGA_STD_VAR ReturnOmegaStdVar() {
                                assert( _std_var_u->_omega_std_var );
                                return _std_var_u->_omega_std_var;
                        };
                        inline PLATO_STD_VAR *ReturnPlatoStdVar() {
                                assert( _std_var_u->_plato_std_var );
                                return _std_var_u->_plato_std_var;
                        };
                private:
                        STD_VAR_U *_std_var_u;
        };
        
        class PlatoOmegaDepTesting : public DependenceTesting, public SymbolicVisitor
        {
                public:
                        virtual DepInfo ComputeArrayDep(LoopTransformInterface &fa, DepInfoAnal& anal,
                                                                const DepInfoAnal::StmtRefDep& ref, DepType deptype);

                        STD_VAR_U *GetStdVarU(const std::string name, bool is_input);
                        OMEGA_STD_VAR GetOmegaStdVar(const std::string name, bool is_input)
                        {
                                assert(GetStdVarU(name,is_input)->_omega_std_var);
                                return GetStdVarU(name,is_input)->_omega_std_var;
                        };
                        PLATO_STD_VAR *GetPlatoStdVar(const std::string name, bool is_input){
                                assert(GetStdVarU(name,is_input)->_plato_std_var);
                                return GetStdVarU(name,is_input)->_plato_std_var;
                        };

                private:

                        typedef std::map<std::string, StandardVar*> STD_VAR_MAP;
                        typedef std::map<std::string, FreeVar*> FREE_VAR_MAP;

                        /**     PlatoRelation is analogous to the Omega Relation
                        *               in that it is what is used to add all of the
                        *               dependency constraints for the Plato library.
                        **/

                        class PlatoRelation : public plato::RationalVITestProblem
                        {
                                private:
                                        int _loop_nest_size;

                                public :
                                        PlatoRelation( int loop_nest_size,bool aFlag = false) : _loop_nest_size(loop_nest_size),
                                                                                RationalVITestProblem(aFlag) { };
                                        virtual ~PlatoRelation() { };
                                        virtual bool testDV (const plato::AbstractDirectionVector & dv, bool & accurate);                                       
                                        virtual int getLoopNestSize() const { return _loop_nest_size; };
                        };

                        /**     PlatoDriver is the class that actually runs the
                        *               Plato dependency tests after the relation has been
                        *               built.
                        **/

                        class PlatoDriver : public plato::DVHierarchyDriver {
                                private:
                                        plato::dddir _saved_dv;

                                public :
                                        PlatoDriver(PlatoRelation & aProblem, bool aFlag = false) :
                                                plato::DVHierarchyDriver(aProblem,aFlag), _saved_dv(0) { };
                                        virtual void StoreDependence (plato::dddir dv) {
                                                _saved_dv = (_saved_dv | dv);                                           
                                        };
                                        inline plato::ddep doDDTest() { return doHierarchy (DDALLDV, 1); };
                                        inline plato::dddir GetDirectionVector() { return _saved_dv; };
                        };

                        /** General loop information variables **/
                        int _dim1,_dim2,_commLevel;
                        AstInterface *_fa;
                        DepInfoAnal *_anal;
                        DepInfoAnal::StmtRefDep *_ref;
                        DepInfoAnal::LoopDepInfo *_info1;
                        DepInfoAnal::LoopDepInfo *_info2;
                        MakeUniqueVar::ReverseRecMap *_varmap;
                        MakeUniqueVar *_varop;
                        MakeUniqueVarGetBound *_boundop;
                        std::vector<SymbolicBound> *_bounds;

                        /** Dependence relation variables **/
                        Relation *_omega_relation;
                        PlatoRelation *_plato_relation;
                        PlatoDriver *_plato_driver;
                        F_And *_omega_constraints;
                        STD_VAR_MAP *_omega_input_vars;
                        STD_VAR_MAP *_omega_output_vars;
                        STD_VAR_MAP *_plato_input_vars;
                        STD_VAR_MAP *_plato_output_vars;
                        FREE_VAR_MAP *_free_vars;

                        /** Variable parsing and new constraint variables **/
                        Constraint_Handle *_constraint_handle;
                        plato::RationalExpression *_re;
                        bool _omega_negate_expr;
                        bool _is_input_expr;
                        bool _plato_is_bound;
                        bool _plato_is_lower_bound;                     

                        /** Dependence testing results variables **/
                        bool _nonLinear;
                        plato::dddir _saved_dv; //plato direction vector

                        /** Building the relations functions **/
                        void AddCommonLoopIndices(void);
                        void AddLocalLoopIndices(void);
                        void AddSubscripts(void);
                        void AddLoopBounds(void);

                        /** Building the relations functions -- Helper functions **/
                        void RenameDupedVars(void);
                        int ReturnVarianceLevel(const std::string name);
                        #define added(a,b) (a->count(b) > 0)
                        bool IsModified(AstNodePtr loop,std::string name) {return _anal->GetModifyVariableInfo().Modify(loop,name);}

                        /** Dependence testing functions **/
                        void AddConstraintsImposedByDV(Relation &relation,F_And &constraints,const plato::dddir &dv);
                        Relation GetNewRelationFromDV(const plato::dddir &dv);          
                        void doHierarchy(SetDep& setdep, DepInfo &result, plato::dddir dv, int level, DepRel& rel);

                        /** Parsing the variables functions **/
                        void VisitConst( const SymbolicConst &v);
                        void VisitVar( const SymbolicVar &v);
                        void VisitExpr( const SymbolicExpr& v);
                        void VisitFunction( const SymbolicFunction &v);
                        void VisitAstWrap( const SymbolicAstWrap& v);
                        plato::RationalExpression *ParseVal( const SymbolicVal &v, bool negate,
                                                                                                                        bool is_input, bool is_bound, bool is_low_bound);
                        plato::RationalExpression GetRationalExpression( AstInterface &fa,
                                        const AstNodePtr& exp, bool is_input);

                        /** misc **/
                        void print_omega_vars(void);
                        void CleanUp(void);
        };

        static char *StringToChar(const std::string& str);
        
        /** FreeVar **/
        
        inline FreeVar::FreeVar(FREE_VAR_U *free_var_u)
        {
                assert(free_var_u);
                _free_var_u = free_var_u;
        }

        inline FreeVar::FreeVar(const FreeVar &other)
        {
                if (UseOmega())
                {
                        assert(other._free_var_u->_omega_free_var);
                        _free_var_u->_omega_free_var = new OMEGA_FREE_VAR(*(other._free_var_u->_omega_free_var));
                        assert(_free_var_u->_omega_free_var);
                }
                else
                {
                        assert(other._free_var_u->_plato_free_var);
                        _free_var_u->_plato_free_var = new PLATO_FREE_VAR(*(other._free_var_u->_plato_free_var));
                        assert(_free_var_u->_plato_free_var);
                }
        }

        inline FreeVar::~FreeVar()
        {
                if (UseOmega())
                {
                        delete _free_var_u->_omega_free_var;
                }
                else
                {
                        delete _free_var_u->_plato_free_var;
                }
        }

        inline FREE_VAR_U *FreeVar::ReturnFreeVarU()
        {
                assert(_free_var_u);
                return _free_var_u;
        }

        /** StandardVar **/


        inline StandardVar::StandardVar(STD_VAR_U *std_var_u)
        {
                assert(std_var_u);
                _std_var_u = std_var_u;
        }

        inline StandardVar::StandardVar(const StandardVar &other)
        {
                _std_var_u = new STD_VAR_U();
                assert(_std_var_u);
                if (UseOmega())
                {
                        assert(other._std_var_u->_omega_std_var);
                        _std_var_u->_omega_std_var = other._std_var_u->_omega_std_var;
                }
                else
                {
                        assert(other._std_var_u->_plato_std_var);
                        _std_var_u->_plato_std_var = other._std_var_u->_plato_std_var;
                }               
        }

        inline StandardVar::~StandardVar()
        {
                // Nothing to do.
        }

        inline STD_VAR_U *StandardVar::ReturnStdVarU() const
        {
                assert(_std_var_u);
                return _std_var_u;
        }
}
#endif


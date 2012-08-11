#include "pointerAliasAnalysis.h"
#include <algorithm>



// **********************************************************************
//                     pointerAliasLattice
// **********************************************************************
void pointerAliasLattice::initialize()
{}
 

// Returns a copy of this lattice
Lattice* pointerAliasLattice::copy() const
{ return new pointerAliasLattice(*this); }


// Copies that lattice into this
void pointerAliasLattice::copy(Lattice* that_arg)
{
    pointerAliasLattice *that = dynamic_cast<pointerAliasLattice*>(that_arg);
    Dbg::dbg<<"Entering COPY : That:" <<that<<" -- "<< that->str(" ") << "This :"<< endl << " -- " << str(" ")<<endl ;
    this->level = that->level;
    this->aliases = that->aliases;
    this->aliasRelations = that->aliasRelations;
}

// Checks if that equals this
bool pointerAliasLattice::operator==(Lattice* that_arg)
{
        pointerAliasLattice* that = dynamic_cast<pointerAliasLattice*>(that_arg);
        return aliases == that->aliases;
}

//Required function for printing debug information
string pointerAliasLattice::str(string indent)
{
        ostringstream oss;
        oss<< "Aliases:{ ";
        for(set<varID>::iterator al = aliases.begin(); al!=aliases.end(); ){             
             oss << *al;
             al++;
             if(al != aliases.end())
                oss<<",";
        }      
        oss<<" }";

        oss << "{";        
        for(set< std::pair<AliasRelationNode, AliasRelationNode> >::iterator alRel = aliasRelations.begin(); alRel!=aliasRelations.end(); ){
            if((alRel->first).var != NULL && (alRel->second).var !=NULL) 
                oss << "(" << isSgVariableSymbol((alRel->first).var)->get_name() 
                    << "," <<(alRel->first).vID
                    << "," << (alRel->first).derefLevel
                    << ") ("
                    << isSgVariableSymbol((alRel->second).var)->get_name() 
                    << "," <<(alRel->second).vID
                    << "," <<(alRel->second).derefLevel 
                    << ")";
            alRel++;
        }
        oss << "}";
        return oss.str();
}


//Add a new Alias
void pointerAliasLattice::setAliases(varID al)
{
    aliases.insert(al);
}


//Add a new Alias relation pair
void pointerAliasLattice::setAliasRelation(std::pair < AliasRelationNode, AliasRelationNode > alRel)
{
    aliasRelations.insert(alRel);
}


//Union of that lattice with this lattice
bool pointerAliasLattice::meetUpdate(Lattice* that_arg)
{
    bool modified=false;
    pointerAliasLattice *that = dynamic_cast<pointerAliasLattice*>(that_arg);
    Dbg::dbg<<"IN MEETTPDATE That:" << that->str(" ") << "This :"<< str(" ")<<endl ;
    
    //Union of Aliasrelations
    set< std::pair<AliasRelationNode, AliasRelationNode> > thisAliasRelations= aliasRelations;
    set< std::pair<AliasRelationNode, AliasRelationNode> > thatAliasRelations= that->getAliasRelations();
    for(set< std::pair<AliasRelationNode, AliasRelationNode> >::iterator alRel = thatAliasRelations.begin(); 
        alRel!=thatAliasRelations.end();alRel++ )
    {
       //set::find() doesnt work well on pairs for some reason. Adding a search function for now
       if(!search(thisAliasRelations,*alRel)){
          this->setAliasRelation(*alRel);
          modified = true;
        }
    }

    //Union of Aliases (Compact Representation Graphs)
    set< varID > thisAlias= aliases;
    set< varID > thatAlias= that->getAliases();
    Dbg::dbg<<"This alias Size :"<<thisAlias.size() << " That alias Size :"<<thatAlias.size();
    for(set< varID >::iterator al = thatAlias.begin(); al!=thatAlias.end();al++ )
    {
       if(thisAlias.find(*al) == thisAlias.end()){
         this->setAliases(*al);
         modified = true;
        }
    }
return modified;
}



template <typename T>
bool pointerAliasLattice::search(set<T> thisSet, T value)
{
    for(typename set< T >::iterator item = thisSet.begin(); item!=thisSet.end(); item++ )
    {
        if(*item == value)
          return true;
    }
    return false;
}


//Getter for AliasRelations
set< std::pair<AliasRelationNode, AliasRelationNode> > pointerAliasLattice::getAliasRelations() 
{
    return aliasRelations;
}

//Getter for Aliases
set<varID> pointerAliasLattice::getAliases()
{
    return aliases;
}


//Clear Aliases - used for must-aliases
void pointerAliasLattice::clearAliases()
{
    aliases.clear();
}



// **********************************************************************
//                     pointerAliasAnalysisTransfer
// **********************************************************************

/*pointerAliasAnalysisTransfer::pointerAliasAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   : IntraDFTransferVisitor(func, n, state, dfInfo)
{}
*/


int pointerAliasAnalysisDebugLevel = 2;

pointerAliasAnalysisTransfer::pointerAliasAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
   : VariableStateTransfer<pointerAliasLattice>(func, n, state, dfInfo, pointerAliasAnalysisDebugLevel)
{}


void pointerAliasAnalysisTransfer::visit(SgAssignOp *sgn)
{
      ROSE_ASSERT(sgn != NULL);
      pointerAliasLattice *resLat = getLattice(sgn);

      SgExpression *lhs = NULL;
      SgExpression *rhs = NULL;
      AliasRelationNode leftARNode, rightARNode;

      SageInterface::isAssignmentStatement(sgn,&lhs, &rhs);
      Dbg::dbg << "AssignOP Stement"<<lhs->variantT()<<"and"<<rhs->variantT()<<"\n"; 
      processLHS(lhs,leftARNode);
      processRHS(rhs,rightARNode);

      //Establish the per CFG-node alias relations
      if((leftARNode.var !=NULL) && (rightARNode.var !=NULL))
        resLat->setAliasRelation(make_pair(leftARNode,rightARNode)); 
        
      //Update the Aliases(Compact Representation Graph)
      updateAliases(resLat->getAliasRelations(),1);
}



void pointerAliasAnalysisTransfer::visit(SgFunctionCallExp *sgn)
{
    pointerAliasLattice *resLat = getLattice(sgn);
    updateAliases(resLat->getAliasRelations(),1);
}


void pointerAliasAnalysisTransfer::visit(SgAssignInitializer *sgn)
{
    
    SgAssignInitializer *assgn_i = isSgAssignInitializer(sgn);
    assert(assgn_i != NULL);
    pointerAliasLattice *resLat = getLattice(sgn);

    SgExpression *lhs = static_cast<SgExpression *> (assgn_i->get_parent());
    SgExpression *rhs = assgn_i->get_operand();
    AliasRelationNode leftARNode, rightARNode;

    processLHS(lhs,leftARNode);
    processRHS(rhs,rightARNode);

    //Establish the per CFG-node alias relations
    if((leftARNode.var !=NULL) && (rightARNode.var !=NULL))
        resLat->setAliasRelation(make_pair(leftARNode,rightARNode)); 
      
    //Update the Aliases(Compact Representation Graph)
    updateAliases(resLat->getAliasRelations(),1);
}


void pointerAliasAnalysisTransfer::visit(SgConstructorInitializer *sgn)
{
    pointerAliasLattice *resLat = getLattice(sgn);
    updateAliases(resLat->getAliasRelations(),1);
}


bool pointerAliasAnalysisTransfer::finish()
{
  return modified;
}



//Update the Aliases (compact representation graph)
/*
    aliasRelations : Set of Alias Relations that hold at the CFG-node
    isMust : Is Must Alias or not

    - For each alias relation pair compute aliases at the left and right expressions in the pair
    - If is must alias, remove existing aliases of left variable lattice
    - Add a alias between each left variable and all their right variable aliases 
*/
bool pointerAliasAnalysisTransfer::updateAliases(set< std::pair<AliasRelationNode, AliasRelationNode> > aliasRelations, int isMust)
{
    bool modified = false;
    pointerAliasLattice *toLat, *fromLat;
    set <varID>  rightResult, leftResult;

    for(set< std::pair<AliasRelationNode, AliasRelationNode> >::iterator alRel = aliasRelations.begin();
            alRel!=aliasRelations.end();alRel++ )
    {
        computeAliases(getLattice((alRel->first).vID), (alRel->first).vID, (alRel->first).derefLevel, leftResult);
        computeAliases(getLattice((alRel->second).vID), (alRel->second).vID, (alRel->second).derefLevel+1 , rightResult); 
        Dbg::dbg<<"LEFT ALIAS SIZE:" <<leftResult.size() <<"RIGHT ALIAS SIZE :"<<rightResult.size(); 

        if(isMust)
        {
            for(set<varID>::iterator leftVar = leftResult.begin(); leftVar != leftResult.end(); leftVar++ )
            {
                toLat = getLattice(*leftVar);
                //if((toLat->getAliases()).size()==1)
                //{   
                    toLat->clearAliases();
                    modified = true; 
                //}
            }
        }
       
        for(set<varID>::iterator leftVar = leftResult.begin(); leftVar != leftResult.end(); leftVar++ ) {
            toLat = getLattice(*leftVar);        
            for(set<varID>::iterator rightVar = rightResult.begin(); rightVar != rightResult.end(); rightVar++ ) {
               toLat->setAliases(*rightVar); 
               modified = true; 
            }
        }  
    }  
return modified; 
}


//Compute Aliases by recursively walking through the compact representation of per-variable lattices
void pointerAliasAnalysisTransfer::computeAliases(pointerAliasLattice *lat, varID var, int derefLevel, set<varID> &result)
{
   if(derefLevel==0)
    result.insert(var);
   else
    {
        set<varID> outS = lat->getAliases();
        for(set<varID>::iterator outVar = outS.begin(); outVar != outS.end(); outVar++)
        {
            computeAliases(getLattice(*outVar),*outVar,derefLevel-1,result);
        }
    }
}

 
//- process left hand side of expressions(node) and calculate the dereference count for pointer and reference variables
//- The newly found alias relation is placed in the arNode. 
//- Alias relations are established only for pointer and references.
void pointerAliasAnalysisTransfer::processLHS(SgNode *node,struct AliasRelationNode &arNode) {
    if(node == NULL)
        return;

    SgVariableSymbol *sym = NULL;
    SgVarRefExp *var_exp;
    varID var;
    int derefLevel = 0;
    switch (node->variantT()) {
        case V_SgInitializedName:
        {
            SgInitializedName *init_exp = isSgInitializedName(node);
            ROSE_ASSERT(init_exp != NULL);
            sym = static_cast<SgVariableSymbol *>(init_exp->get_symbol_from_symbol_table());
            var = SgExpr2Var(init_exp->get_initializer());
        }
        break;

        case V_SgVarRefExp:
        {
             var_exp = isSgVarRefExp(node);
             ROSE_ASSERT(var_exp != NULL);
            sym = var_exp->get_symbol();
            var = SgExpr2Var(var_exp);
        }
        break;

        case V_SgPointerDerefExp:
        {
            SgPointerDerefExp *tmp = isSgPointerDerefExp(node);
            SgPointerDerefExp *deref_exp = NULL;
            ROSE_ASSERT(tmp != NULL);
            while(tmp != NULL) {
                deref_exp = tmp;
                tmp = isSgPointerDerefExp(deref_exp->get_operand());
                derefLevel++;
            }
            processLHS(deref_exp->get_operand(), arNode);
            arNode.derefLevel += derefLevel;
            return;
        }
        break;
        case V_SgDotExp:
        case V_SgArrowExp:
        {
            SgBinaryOp *bin_exp = isSgBinaryOp(node);
            if(bin_exp != NULL) {
                processLHS(bin_exp->get_rhs_operand(), arNode);
                return;
            }
        }
        break;

        default:
            sym = NULL;
    };


    //Maintain alias relation for Pointer/Reference types only
    if(sym != NULL &&
            (SageInterface::isPointerType(sym->get_type()) == false && SageInterface::isReferenceType(sym->get_type())==false) )
                sym = NULL;

    arNode.var = sym;
    arNode.vID = var;
    arNode.derefLevel = derefLevel;
}


//- Process aliases for Right hand side of expression and calculate derefernce count for pointer and reference variables
//- The newly found alias relation is placed in the arNode. 
//- Alias relations are established only for pointer and references.
void pointerAliasAnalysisTransfer::processRHS(SgNode *node, struct AliasRelationNode &arNode) {
    if(node == NULL)
        return;

    SgVariableSymbol *sym = NULL;
    SgVarRefExp *var_exp;
    varID var;
    int derefLevel = 0;
    static int new_index;
    static map<SgExpression*, SgVariableSymbol *> new_variables;

    switch (node->variantT()) {

        // = a
        case V_SgVarRefExp:
        {
            var_exp = isSgVarRefExp(node);
            ROSE_ASSERT(var_exp != NULL);
            // get the variable symbol
            sym = var_exp->get_symbol();
            var = SgExpr2Var(var_exp);
        }
        break;

        // = *a or = **a or = ***a  etc
        case V_SgPointerDerefExp:
        {
            SgPointerDerefExp *tmp = isSgPointerDerefExp(node);
            SgPointerDerefExp *deref_exp = NULL;
            ROSE_ASSERT(tmp != NULL);
            while(tmp != NULL) {
                deref_exp = tmp;
                tmp = isSgPointerDerefExp(deref_exp->get_operand());
                derefLevel++;
            }
            processRHS(deref_exp->get_operand(), arNode);
            arNode.derefLevel += derefLevel;
            return;
        }
        break;
        //  = &b
        case   V_SgAddressOfOp:
        {
            SgAddressOfOp *add_exp = isSgAddressOfOp(node);
            assert(add_exp != NULL);
            processRHS(add_exp->get_operand(), arNode);
            sym = arNode.var;
            var = arNode.vID;
            derefLevel = arNode.derefLevel-1;
        }
       break;
       // a.b or a->b or this->b
        case V_SgDotExp:
        case V_SgArrowExp:
        {
            SgBinaryOp *bin_exp = isSgBinaryOp(node);
            if(bin_exp != NULL) {
                processRHS(bin_exp->get_rhs_operand(), arNode);
                return;
            }
        }
        break;
        // Case where *a = *b = *c ...etc
       case V_SgAssignOp:
       {
            SgExpression *lhs = NULL;
            SgExpression *rhs = NULL;
            if(SageInterface::isAssignmentStatement(node,&lhs, &rhs)){
                ROSE_ASSERT(rhs != NULL);
                processRHS(rhs, arNode);
                return;
            }
       }
       break;

       case V_SgCastExp:
       {
           SgCastExp *cast_exp = isSgCastExp(node);
           processRHS(cast_exp->get_operand(), arNode);
           return;
       }
       break;
       // C *c = new C or
       // c = new C
       //return new C
       case V_SgNewExp:
       {
        SgNewExp *new_exp = isSgNewExp(node);
        if(new_variables.count(new_exp) == 0){

            SgScopeStatement *scope = NULL;
           SgNode *parent=new_exp->get_parent();
           //isSgStatement(parent)->get_scope()
           SgStatement *stmt = NULL;

           // find the nearest parent which is a statement
           while(!(stmt = isSgStatement(parent)))
                parent = parent->get_parent();

           scope = stmt->get_scope();
           SgType *type = new_exp->get_type()->dereference();
           std::stringstream ss;
           ss << "__tmp_VFA__" << new_index;
           std::string name;
           ss >> name;
           SgName var_name = name;
           new_index++;
           assert(scope != NULL);

           // Create a temporary variable so that every memory location has an assigned name
           SageBuilder::buildVariableDeclaration_nfi(var_name, type, NULL, scope);

           //SageInterface::prependStatement(var_decl, scope);
           sym = scope->lookup_variable_symbol(var_name);
           ROSE_ASSERT(sym != NULL);
           new_variables[new_exp] = sym;
        }
        else
            sym = new_variables[new_exp];
           
        var =  SgExpr2Var(new_exp);
        derefLevel = derefLevel - 1;
       }
       break;
       default:
            sym = NULL;
    }

    arNode.derefLevel = derefLevel;
    arNode.var = sym;
    arNode.vID = var;
}



// **********************************************************************
//                     pointerAliasAnalysis
// **********************************************************************
void pointerAliasAnalysis::genInitState(const Function& func, const DataflowNode& n, const NodeState& state,std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts)
{
     map<varID, Lattice*> emptyM;
     initLattices.push_back(new FiniteVarsExprsProductLattice((Lattice*) new pointerAliasLattice(), emptyM, (Lattice*)NULL,NULL, n, state) );

}

pointerAliasAnalysis::pointerAliasAnalysis(LiveDeadVarsAnalysis* ldva)   
{
     this->ldva = ldva;   
}


bool pointerAliasAnalysis::transfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
{
    assert(0);
    return false;
}

boost::shared_ptr<IntraDFTransferVisitor>
pointerAliasAnalysis::getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo)
{
    return boost::shared_ptr<IntraDFTransferVisitor>(new pointerAliasAnalysisTransfer(func, n, state, dfInfo));
}




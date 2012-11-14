// Tainted flow analysis.  See taintAnalysis.h for details.
#include "sage3basic.h"
#include "taintAnalysis.h"
#include "stringify.h"

/******************************************************************************************************************************
 *                                      Taint Lattice
 ******************************************************************************************************************************/

bool
TaintLattice::set_vertex(Vertex v) {
    bool retval = vertex!=v;
    vertex = v;
    return retval;
}

void
TaintLattice::copy(/*const*/ Lattice *other_) /*override*/ {
    TaintLattice *other = dynamic_cast<TaintLattice*>(other_);
    assert(other!=NULL);
    *this = *other;
}

bool
TaintLattice::operator==(/*const*/ Lattice *other_) /*const*/ /*override*/ {
    TaintLattice *other = dynamic_cast<TaintLattice*>(other_);
    assert(other!=NULL);
    return vertex == other->vertex;
}

std::string
TaintLattice::to_string() const {
    std::string s = stringifyTaintLatticeVertex(vertex, "VERTEX_");
    for (size_t i=0; i<s.size(); ++i)
        s[i] = tolower(s[i]);
    return s;
}

bool
TaintLattice::meetUpdate(/*const*/ Lattice *other_) /*override*/ {
    TaintLattice *other = dynamic_cast<TaintLattice*>(other_);
    assert(other!=NULL);
    Vertex new_vertex = std::max(vertex, other->vertex);
    bool retval = new_vertex != vertex;
    vertex = new_vertex;
    return retval;
}

std::ostream& operator<<(std::ostream &o, const TaintLattice &lattice) {
    o <<lattice.to_string();
    return o;
}



/******************************************************************************************************************************
 * Taint Flow Analysis
 ******************************************************************************************************************************/

void
TaintAnalysis::genInitState(const Function& func, const DataflowNode& node, const NodeState& state,
                            std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts) {
    if (debug) {
        *debug <<"TaintAnalysis::genInitState(func=" <<func.get_name() <<",\n"
               <<"                            node={" <<StringUtility::makeOneLine(node.toString()) <<"},\n"
               <<"                            state={" <<StringUtility::makeOneLine(state.str(this, "")) <<"},\n"
               <<"                            initLattice[" <<initLattices.size() <<"]={...},\n"
               <<"                            initFacts[" <<initFacts.size() <<"]={...})\n";
    }
    map<varID, Lattice*> emptyM;
    FiniteVarsExprsProductLattice *prodLat = new FiniteVarsExprsProductLattice(new TaintLattice, emptyM, (Lattice*)NULL,
                                                                                   ldv_analysis, node, state);
    assert(prodLat!=NULL);
    magic_tainted(node.getNode(), prodLat); // certain names are considered to be always tainted
    initLattices.push_back(prodLat);
}

bool
TaintAnalysis::transfer(const Function& func, const DataflowNode& node_, NodeState& state, const std::vector<Lattice*>& dfInfo) {
    static size_t ncalls = 0;
    if (debug) {
        *debug <<"TaintAnalysis::transfer-" <<++ncalls <<"(func=" <<func.get_name() <<",\n"
               <<"                        node={" <<StringUtility::makeOneLine(node_.toString()) <<"},\n"
               <<"                        state={" <<state.str(this, "                            ") <<",\n"
               <<"                        dfInfo[" <<dfInfo.size() <<"]={...})\n";
    }

    SgNode *node = node_.getNode();
    assert(!dfInfo.empty());
    FiniteVarsExprsProductLattice *prodLat = dynamic_cast<FiniteVarsExprsProductLattice*>(dfInfo.front());
    bool modified = magic_tainted(node, prodLat); // some values are automatically tainted based on their name

    // Process AST nodes that transfer taintedness.  Most of these operations have one or more inputs from which a result
    // is always calculated the same way.  So we just gather up the inputs and do the calculation at the very end of this
    // function.  The other operations are handled individually within their "if" bodies.
    TaintLattice *result = NULL;                    // result pointer into the taint lattice
    std::vector<TaintLattice*> inputs;              // input pointers into the taint lattice
    if (isSgAssignInitializer(node)) {
        // as in "int a = b"
        SgAssignInitializer *xop = isSgAssignInitializer(node);
        TaintLattice *in1 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(SgExpr2Var(xop->get_operand())));
        inputs.push_back(in1);

    } else if (isSgAggregateInitializer(node)) {
        // as in "int a[1] = {b}"
        SgAggregateInitializer *xop = isSgAggregateInitializer(node);
        const SgExpressionPtrList &exprs = xop->get_initializers()->get_expressions();
        for (size_t i=0; i<exprs.size(); ++i) {
            varID in_id = SgExpr2Var(exprs[i]);
            TaintLattice *in = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in_id));
            inputs.push_back(in);
        }

    } else if (isSgInitializedName(node)) {
        SgInitializedName *xop = isSgInitializedName(node);
        if (xop->get_initializer()) {
            varID in1_id = SgExpr2Var(xop->get_initializer());
            TaintLattice *in1 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in1_id));
            inputs.push_back(in1);
        }

    } else if (isSgValueExp(node)) {
        // numeric and character constants
        SgValueExp *xop = isSgValueExp(node);
        result = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(SgExpr2Var(xop)));
        if (result)
            modified = result->set_vertex(TaintLattice::VERTEX_UNTAINTED);
        
    } else if (isSgAddressOfOp(node)) {
        // as in "&x".  The result taintedness has nothing to do with the value in x.
        /*void*/

    } else if (isSgBinaryOp(node)) {
        // as in "a + b"
        SgBinaryOp *xop = isSgBinaryOp(node);
        varID in1_id = SgExpr2Var(isSgExpression(xop->get_lhs_operand()));
        TaintLattice *in1 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in1_id));
        inputs.push_back(in1);
        varID in2_id = SgExpr2Var(isSgExpression(xop->get_rhs_operand()));
        TaintLattice *in2 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in2_id));
        inputs.push_back(in2);

        if (isSgAssignOp(node)) { // copy the rhs lattice to the lhs lattice (as well as the entire '=' expression result)
            assert(in1 && in2);
            modified = in1->meetUpdate(in2);
        }

    } else if (isSgUnaryOp(node)) {
        // as in "-a"
        SgUnaryOp *xop = isSgUnaryOp(node);
        varID in1_id = SgExpr2Var(xop->get_operand());
        TaintLattice *in1 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in1_id));
        inputs.push_back(in1);

    } else if (isSgReturnStmt(node)) {
        // as in "return a".  The result will always be dead, so we're just doing this to get some debugging output.  Most
        // of our test inputs are functions, and the test examines the function's returned taintedness.
        SgReturnStmt *xop = isSgReturnStmt(node);
        varID in1_id = SgExpr2Var(xop->get_expression());
        TaintLattice *in1 = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(in1_id));
        inputs.push_back(in1);

    }


    // Update the result lattice (unless dead) with the inputs (unless dead) by using the meedUpdate() method.  All this
    // means is that the new result will be the maximum of the old result and all inputs, where "maximum" is defined such
    // that "tainted" is greater than "untainted" (and both of them are greater than bottom/unknown).
    for (size_t i=0; i<inputs.size(); ++i)
        if (debug)
            *debug <<"TaintAnalysis::transfer: input " <<(i+1) <<" is " <<lattice_info(inputs[i]) <<"\n";
    if (!result && varID::isValidVarExp(node)) {
        varID result_id(node); // NOTE: constructor doesn't handle all SgExpression nodes, thus the next "if"
        result = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(result_id));
    }
    if (!result && isSgExpression(node)) {
        varID result_id = SgExpr2Var(isSgExpression(node));
        result = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(result_id));
    }
    if (result) {
        for (size_t i=0; i<inputs.size(); ++i) {
            if (inputs[i])
                modified = result->meetUpdate(inputs[i]) || modified;
        }
    }
    if (debug)
        *debug <<"TaintAnalysis::transfer: result is " <<lattice_info(result) <<(modified?" (modified)":" (not modified)") <<"\n";

    return modified;
}

bool
TaintAnalysis::magic_tainted(SgNode *node, FiniteVarsExprsProductLattice *prodLat) {
    if (isSgInitializedName(node)) {
        SgInitializedName *iname = isSgInitializedName(node);
        std::string vname = iname->get_name().getString();
        TaintLattice *tlat = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(varID(iname)));
        if (tlat && 0==vname.compare(0, 7, "TAINTED")) {
            bool modified = tlat->set_vertex(TaintLattice::VERTEX_TAINTED);
            if (debug) {
                *debug <<"TaintAnalysis::magic_tainted: lattice is magically " <<tlat->to_string()
                       <<(modified?" (modified)":" (not modified)") <<"\n";
            }
            return modified;
        }
    } else if (isSgVarRefExp(node)) {
        SgVarRefExp *vref = isSgVarRefExp(node);
        std::string vname = vref->get_symbol()->get_name().getString();
        TaintLattice *tlat = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(varID(vref)));
        if (tlat && 0==vname.compare(0, 7, "TAINTED")) {
            bool modified = tlat->set_vertex(TaintLattice::VERTEX_TAINTED);
            if (debug) {
                *debug <<"TaintAnalysis::magic_tainted: lattice is magically " <<tlat->to_string()
                       <<(modified?" (modified)":" (not modified)") <<"\n";
            }
            return modified;
        }
    } else if (isSgVariableDeclaration(node)) {
        SgVariableDeclaration *vdecl = isSgVariableDeclaration(node);
        const SgInitializedNamePtrList &inames = vdecl->get_variables();
        for (size_t i=0; i<inames.size(); ++i) {
            std::string vname = inames[i]->get_name().getString();
            TaintLattice *tlat = dynamic_cast<TaintLattice*>(prodLat->getVarLattice(varID(inames[i])));
            if (tlat && 0==vname.compare(0, 7, "TAINTED")) {
                bool modified = tlat->set_vertex(TaintLattice::VERTEX_TAINTED);
                if (debug) {
                    *debug <<"TaintAnalysis::magic_tainted: lattice is magically " <<tlat->to_string()
                           <<(modified?" (modified)":" (not modified)") <<"\n";
                }
                return modified;
            }
        }
    }

    return false;
}

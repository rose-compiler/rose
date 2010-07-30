// #include "cfgView.h"
// #include "rose.h"
// #include "loopHelpers.h"
// #include <sstream>
// #include <iomanip>
// #include <stdint.h>

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "virtualBinCFG.h"

using namespace std;

namespace VirtualBinCFG {

    string CFGNode::toString() const {
        if (isSgAsmFunctionDeclaration(node)) {
            return "BinaryFunctionDefinition";
        }
        return "";
    }

    string CFGNode::toStringForDebugging() const {
        ostringstream s;
        if (node == NULL) {
            s << "End of procedure";
        } else {
            string nodeText;
        }
        return s.str();
    }

    string CFGNode::id() const {
        ostringstream s;
        s << "n_" << hex << uintptr_t(node) << "_" << dec ;
        return s.str();
    }

    string CFGEdge::toString() const {
        return toStringForDebugging();
    }

    string CFGEdge::toStringForDebugging() const {
        ostringstream s;
        // s << src.id() << " -> " << tgt.id();
        bool anyNonEmpty = false;
        EdgeConditionKind cond = condition();
        if (cond != eckUnconditional) {
            if (anyNonEmpty) s << " "; // For consistency
            s << "key(";
            switch (cond) {
                case eckTrue:
                    s << "true";
                    break;
                case eckFalse:
                    s << "false";
                    break;
                case eckCaseLabel:
	            // s << caseLabel()->unparseToString();
                    break;
                case eckDefault:
                    s << "default";
                    break;
                default:
                    s << "unknown";
                    break;
            }
            s << ")";
            anyNonEmpty = true;
        }
        return s.str();
    }

    string CFGEdge::id() const {
        ostringstream s;
        s << src.id() << "__" << tgt.id();
        return s.str();
    }


    EdgeConditionKind CFGEdge::condition() const {
#if 0
        SgAsmNode* srcNode = src.getNode();
        unsigned int srcIndex = src.getIndex();
        SgAsmNode* tgtNode = tgt.getNode();
        unsigned int tgtIndex = tgt.getIndex();
        if (isSgAsmMov(srcNode) ) {
            SgAsmMov* ifs = isSgAsmMov(srcNode);
#if 0
            if (ifs->get_true_body() == tgtNode) {
                return eckTrue;
            } else if (ifs->get_false_body() == tgtNode) {
                return eckFalse;
            } else ROSE_ASSERT (!"Bad successor in if statement");
#endif
        }
#if 0
        else if (isSgWhileStmt(srcNode) && srcIndex == 1) {
            if (srcNode == tgtNode) {
                // False case for while test
                return eckFalse;
            } else {
                return eckTrue;
            }
        } else if (isSgDoWhileStmt(srcNode) && srcIndex == 2) {
            // tgtIndex values are 0 for true branch and 3 for false branch
            if (tgtIndex == 0) {
                return eckTrue;
            } else {
                return eckFalse;
            }
        } else if (isSgForStatement(srcNode) && srcIndex == 2) {
            if (srcNode == tgtNode) {
                // False case for test
                return eckFalse;
            } else {
                return eckTrue;
            }
        } else if (isSgSwitchStatement(srcNode) && isSgCaseOptionStmt(tgtNode)) {
            return eckCaseLabel;
        } else if (isSgSwitchStatement(srcNode) && isSgDefaultOptionStmt(tgtNode)){
            return eckDefault;
        } else if (isSgConditionalExp(srcNode) && srcIndex == 1) {
            SgConditionalExp* ce = isSgConditionalExp(srcNode);
            if (ce->get_true_exp() == tgtNode) {
                return eckTrue;
            } else if (ce->get_false_exp() == tgtNode) {
                return eckFalse;
            } else ROSE_ASSERT (!"Bad successor in conditional expression");
        } else if (isSgAndOp(srcNode) && srcIndex == 1) {
            if (srcNode == tgtNode) {
                // Short-circuited false case
                return eckFalse;
            } else {
                return eckTrue;
            }
        } else if (isSgOrOp(srcNode) && srcIndex == 1) {
            if (srcNode == tgtNode) {
                // Short-circuited true case
                return eckTrue;
            } else {
                return eckFalse;
            }
        } 
#endif
        else {
            // No key
            return eckUnconditional;
        }
#else
    // DQ (11/28/2009): This function was already commented out, but must return a value for use in MSVC.
       return eckFalse;
#endif
    }

    /** Makes a CFG edge, adding appropriate labels. */
    void makeEdge(SgAsmInstruction* from, SgAsmInstruction* to, const AuxiliaryInformation* info, vector<CFGEdge>& result) {
#if 0
        SgAsmNode* fromNode = from.getNode();
        unsigned int fromIndex = from.getIndex();
        SgAsmNode* toNode = to.getNode();
        unsigned int toIndex = to.getIndex();
#if 0
        // Exit early if the edge should not exist because of a control flow discontinuity
        if (fromIndex == 1 && (isSgGotoStatement(fromNode) || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
            return;
        }
        if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
            SgReturnStmt* rs = isSgReturnStmt(fromNode);
            if (fromIndex == 1 || fromIndex == 0 && !rs->get_expression()) return;
        }
        if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
            isSgSwitchStatement(fromNode)->get_body() == toNode) return;
#endif
#endif
        // Create the edge
        result.push_back(CFGEdge(CFGNode(from, info), CFGNode(to, info), info));
    }

    vector<CFGEdge> CFGNode::outEdges() const {
        ROSE_ASSERT (node);
        return node->cfgBinOutEdges(info);
    }

    vector<CFGEdge> CFGNode::inEdges() const {
        ROSE_ASSERT (node);
        return node->cfgBinInEdges(info);
    }

    const std::set<uint64_t>& AuxiliaryInformation::getPossibleSuccessors(SgAsmInstruction* insn) const {
        static const std::set<uint64_t> emptySet;
        std::map<SgAsmInstruction*, std::set<uint64_t> >::const_iterator succsIter = indirectJumpTargets.find(insn);
        if (isSgAsmx86Instruction(insn) && isSgAsmx86Instruction(insn)->get_kind() == x86_ret) {
            SgNode* f = insn;
            while (f && !isSgAsmBlock(f) && !isSgAsmFunctionDeclaration(f)) f = f->get_parent();
            std::map<SgAsmStatement*, std::set<uint64_t> >::const_iterator retIter = returnTargets.find(isSgAsmStatement(f));
            if (retIter == returnTargets.end()) {
                return emptySet;
            } else {
                return retIter->second;
            }
        } else if (succsIter == indirectJumpTargets.end()) {
            return emptySet;
        } else {
            // rose translator has trouble in unparsing it correctly.
            return succsIter->second;
        }
    }

    AuxiliaryInformation::AuxiliaryInformation(SgNode* top)
        : addressToInstructionMap(), indirectJumpTargets(), returnTargets(), incomingEdges()
    {

        struct AuxInfoTraversal: public AstSimpleProcessing {
            AuxiliaryInformation* info;
            AuxInfoTraversal(AuxiliaryInformation* info): info(info) {}
            virtual void visit(SgNode* n) {
                SgAsmInstruction* insn = isSgAsmInstruction(n);
                if (!insn) return;
                info->addressToInstructionMap[insn->get_address()] = insn;
            }
        };

        struct AuxInfoTraversal2: public AstSimpleProcessing {
            AuxiliaryInformation* info;
            AuxInfoTraversal2(AuxiliaryInformation* info): info(info) {}
            virtual void visit(SgNode* n) {
                SgAsmx86Instruction* insn = isSgAsmx86Instruction(n);
                if (!insn) return;
                if (insn->get_kind() != x86_call) return;
                //cerr << "Found call xxx at " << hex << insn->get_address() << endl;
                uint64_t tgtAddr;
                if (!x86GetKnownBranchTarget(insn, tgtAddr)) return;
                //cerr << "Found call at " << hex << insn->get_address() << " with known target " << hex << tgtAddr << endl;
                SgAsmInstruction* tgt = info->getInstructionAtAddress(tgtAddr);
                if (!tgt) return;
                //cerr << "Found target insn" << endl;
                SgNode* f = tgt;
                while (f && !isSgAsmBlock(f) && !isSgAsmFunctionDeclaration(f)) f = f->get_parent();
                if (!f) return;
                //cerr << "Found function of target" << endl;
                uint64_t next = insn->get_address() + insn->get_raw_bytes().size();
                info->returnTargets[isSgAsmStatement(f)].insert(next);
            }
        };

        struct AuxInfoTraversal3: public AstSimpleProcessing {
            AuxiliaryInformation* info;
            AuxInfoTraversal3(AuxiliaryInformation* info): info(info) {}
            virtual void visit(SgNode* n) {
                SgAsmInstruction* insn = isSgAsmInstruction(n);
                if (!insn) return;
                vector<CFGEdge> outEdgesSoFar = insn->cfgBinOutEdges(info);
                for (size_t i = 0; i < outEdgesSoFar.size(); ++i) {
                    info->incomingEdges[outEdgesSoFar[i].target().getNode()].insert(insn->get_address());
                }
            }
        };

        AuxInfoTraversal trav(this);
        trav.traverse(top, preorder);
        AuxInfoTraversal2 trav2(this);
        trav2.traverse(top, preorder);
        AuxInfoTraversal3 trav3(this);
        trav3.traverse(top, preorder);
    }
}

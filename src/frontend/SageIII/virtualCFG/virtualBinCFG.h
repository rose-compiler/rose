#ifndef VIRTUAL_BIN_CFG_H
#define VIRTUAL_BIN_CFG_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <stdint.h>
#include "rosedll.h"

//class AttachedPreprocessingInfoType;
class SgAsmInstruction;
class SgAsmStatement;
 
namespace VirtualBinCFG {

    class CFGEdge;

    enum EdgeConditionKind 
        {
        eckUnconditional,       // Normal, unconditional edge
        eckTrue,                // True case of a two-way branch
        eckFalse,               // False case of a two-way branch
        eckCaseLabel,           // Case label (constant is given by caseLabel())
        eckDefault              // Default label
    };
    
    typedef std::set<Rose::BinaryAnalysis::Address> AddressSet;
    typedef std::map<Rose::BinaryAnalysis::Address, SgAsmInstruction*> AddressToInstructionMap;
    typedef std::map<SgAsmInstruction*, AddressSet> InstructionToAddressesMap;
    typedef std::map<SgAsmStatement*, AddressSet> StatementToAddressesMap;

    struct ROSE_DLL_API AuxiliaryInformation {
        AddressToInstructionMap    addressToInstructionMap;
        InstructionToAddressesMap  indirectJumpTargets;
        StatementToAddressesMap    returnTargets; /* statement is SgAsmBlock or SgAsmFunction */
        InstructionToAddressesMap  incomingEdges;

    public:

        AuxiliaryInformation() {}
        AuxiliaryInformation(SgNode* top);

        /** Returns the instruction (if any) disassembled at the specified address. */
        SgAsmInstruction *getInstructionAtAddress(Rose::BinaryAnalysis::Address addr) const {
            AddressToInstructionMap::const_iterator i = addressToInstructionMap.find(addr);
            if (i == addressToInstructionMap.end()) return NULL;
            return i->second;
        }

        /* NOTE: this is not the transpose of getPossiblePredecessors()! */
        const AddressSet& getPossibleSuccessors(SgAsmInstruction* insn) const;

        const AddressSet& getPossiblePredecessors(SgAsmInstruction* insn) const {
            static const AddressSet emptySet;
            InstructionToAddressesMap::const_iterator predsIter = incomingEdges.find(insn);
            if (predsIter == incomingEdges.end()) {
                return emptySet;
            } else {
                return predsIter->second;
            }
        }
    };

    class CFGNode {
        SgAsmInstruction *node;
#if 0 // [Robb Matzke 2021-03-17]: unused
        const AuxiliaryInformation *info;
#endif
    public:
        explicit CFGNode(SgAsmInstruction *node, const AuxiliaryInformation* /*info*/ = NULL)
            : node(node)
#if 0 // [Robb Matzke 2021-03-17]: unused
              , info(info)
#endif
            {
#ifdef _MSC_VER
//#define __builtin_constant_p(exp) (0)
#endif
            assert(node);
        }
        std::string toString() const;
        // String for debugging graphs
        std::string toStringForDebugging() const;
        // ID to use for Dot, etc.
        std::string id() const;

        SgAsmInstruction *getNode() const {
            return node;
        }
  
        std::vector<CFGEdge> outEdges() const;
        std::vector<CFGEdge> inEdges() const;
        bool operator==(const CFGNode& o) const {
            return node == o.node;
        }
        bool operator!=(const CFGNode& o) const {
            return !(*this == o);
        }
        bool operator<(const CFGNode& o) const {
            return node < o.node;
        }
    };

    class CFGEdge {
        CFGNode src, tgt;
#if 0 // [Robb Matzke 2021-03-17]: unused
        const AuxiliaryInformation *info;
#endif
    public:
        CFGEdge(CFGNode src, CFGNode tgt, const AuxiliaryInformation* /*info*/ = NULL)
            : src(src), tgt(tgt)
#if 0 // [Robb Matzke 2021-03-17]: unused
              , info(info)
#endif
            {}
        std::string toString() const;                   // Pretty string for Dot node labels, etc.
        std::string toStringForDebugging() const;       // String for debugging graphs
        std::string id() const;                         // ID to use for Dot, etc.
        CFGNode source() const {
            return src;
        }
        CFGNode target() const {
            return tgt;
        }
        EdgeConditionKind condition() const;
        //SgExpression* caseLabel() const;
        //SgExpression* conditionBasedOn() const;
        //std::vector<SgInitializedName*> scopesBeingExited() const;
        //std::vector<SgInitializedName*> scopesBeingEntered() const;
        bool operator==(const CFGEdge& o) const {
            return src == o.src && tgt == o.tgt;
        }
        bool operator!=(const CFGEdge& o) const {
            return src != o.src || tgt != o.tgt;
        }
        bool operator<(const CFGEdge& o) const {
            return src < o.src || (src == o.src && tgt < o.tgt);
        }
    };

    // Used in inEdges() and outEdges() methods
    void makeEdge(SgAsmInstruction *from, SgAsmInstruction *to, const AuxiliaryInformation *info, std::vector<CFGEdge> &result);
}

#endif
#endif

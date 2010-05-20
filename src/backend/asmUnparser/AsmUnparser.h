#ifndef ROSE_ASM_UNPARSER_H
#define ROSE_ASM_UNPARSER_H

//#include "rose.h"
#include <ostream>

/** Converts assembly instruction nodes to ASCII and sends them to a supplied output stream. Properties in the class influnce the
 *  function used for output. The user can also easily subclass to override particular output functions. */
class AsmUnparser {
public:
    AsmUnparser()
        : insn_show_bytes(true), insn_linefeed(true),
          blk_detect_noop_seq(false), blk_show_noop_seq(false), blk_remove_noop_seq(false), blk_show_noop_warning(true),
          blk_show_successors(true), 
          func_show_title(true), 
          interp_show_title(true) {}
    virtual ~AsmUnparser() {}

    /*======================================== Properties ========================================*/

    /* Instruction properties */
    bool insn_show_bytes;               /**< Show bytes of instruction? */
    bool insn_linefeed;                 /**< Emit line feed after each instruction before calling post()? */

    /* Basic block properties */
    bool blk_detect_noop_seq;           /**< Run no-op sequence detection and pass info via insn_is_noop_seq data member. If any
                                         *   of the following no-op sequence features are enabled then this is implicitely on. */
    bool blk_show_noop_seq;             /**< Show no-op sequence information before instructions? */
    bool blk_remove_noop_seq;           /**< Remove sequences of instructions that are no-ops? */
    bool blk_show_noop_warning;         /**< Show warning message if no-op sequences were removed? */
    bool blk_show_successors;           /**< Show list of successors after instructions? */

    /* Function properties */
    bool func_show_title;               /**< Show function name, entry, etc before basic blocks? */

    /* Interpretation properties */
    bool interp_show_title;             /**< Show interpretation title before listing functions? */

    /*======================================== Context ========================================*/
    /* These data members are set during unparsing in order to provide context to the more deeply
     * nested methods so that unparse methods don't need so many arguments. */
    bool insn_is_noop_seq;              /**< Set if instruction is part of a no-op. */




    /* These get called before each object */
    virtual void pre(std::ostream&, SgAsmInstruction*) {};
    virtual void pre(std::ostream&, SgAsmBlock*) {};
    virtual void pre(std::ostream&, SgAsmFunctionDeclaration*) {};
    virtual void pre(std::ostream&, SgAsmInterpretation*) {};

    /* These get called after each object */
    virtual void post(std::ostream&, SgAsmInstruction*) {};
    virtual void post(std::ostream&, SgAsmBlock*) {};
    virtual void post(std::ostream&, SgAsmFunctionDeclaration*) {};
    virtual void post(std::ostream&, SgAsmInterpretation*) {};
    
    /* Main functions for unparsing various objects */
    virtual void unparse(std::ostream&, SgAsmInstruction*);
    virtual void unparse(std::ostream&, SgAsmBlock*);
    virtual void unparse(std::ostream&, SgAsmFunctionDeclaration*);
    virtual void unparse(std::ostream&, SgAsmInterpretation*);
};

#endif

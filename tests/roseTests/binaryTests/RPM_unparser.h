#include <ostream>

class RPM_unparser {
public:
    RPM_unparser()
        : insn_show_bytes(true), insn_linefeed(true),
          blk_detect_noop_seq(false), blk_show_noop_seq(false), blk_remove_noop_seq(false), blk_show_noop_warning(true),
          blk_show_successors(true), 
          func_show_title(true), 
          interp_show_title(true) {}
    virtual ~RPM_unparser() {}

    /* Properties */
    bool insn_show_bytes;               /**< Show bytes of instruction? */
    bool insn_linefeed;                 /**< Emit line feed after each instruction before calling post()? */

    bool blk_detect_noop_seq;           /**< Run no-op sequence detection and pass info via insn_is_noop_seq data member. If any
                                         *   of the following no-op sequence features are enabled then this is implicitely on. */
    bool blk_show_noop_seq;             /**< Show no-op sequence information before instructions? */
    bool blk_remove_noop_seq;           /**< Remove sequences of instructions that are no-ops? */
    bool blk_show_noop_warning;         /**< Show warning message if no-op sequences were removed? */
    bool blk_show_successors;           /**< Show list of successors after instructions? */

    bool func_show_title;               /**< Show function name, entry, etc before basic blocks? */

    bool interp_show_title;             /**< Show interpretation title before listing functions? */

    /* Additional node attributes set during the unparse() for that node */
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

    

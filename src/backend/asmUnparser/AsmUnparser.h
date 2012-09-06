#ifndef ROSE_AsmUnparser_H
#define ROSE_AsmUnparser_H

#include <ostream>

#include "callbacks.h"          /* Needed for ROSE_Callbacks::List<> */
#include "BinaryControlFlow.h"
#include "BinaryFunctionCall.h"
#include "Disassembler.h"

class SgAsmInstruction;
class SgAsmBlock;
class SgAsmFunction;
class SgAsmInterpretation;


/** Unparses binary AST into text.
 *
 *  This class operates on part of an AST corresponding to a binary file (executable, library, etc) and generates output. It is
 *  intended to be highly customizable and has been used to generate both assembly listings and Graphviz files.
 *
 *  To generate an assembly listing, use the unparse() method, like this:
 *  @code
 *  SgProject *project = ...;
 *  AsmUnparser().unparse(std::cout, project);
 *  @endcode
 *
 *  The unparser can organize the output either by address (like traditional assembly listings) or by the organization found in
 *  the AST (by interpretation, then function, then basic block).  The AST organization is often more useful and is therefore
 *  the default.  Some of the callbacks mentioned below apply only to one organizational style or the other.
 *
 *  Instead of passing a SgProject node to the unparse() method, one may also pass lower-level nodes, such as
 *  SgAsmInterpretation, SgAsmFunction, SgAsmBlock, or even a single SgAsmInstruction. The unparser operates finding the list
 *  of top-level, unparsable nodes (see find_unparsable_nodes()) reachable from the specified node and then unparsing each 
 *  one.  For each node, an appropriate unparse_TYPE() is called, where TYPE is "insn", "basicblock", "staticdata",
 *  "datablock", "function", or "interpretation".  Each of these unparse functions invokes three lists of callbacks:
 *  <ul>
 *     <li>The TYPE_pre callback list outputs header information for the TYPE object.  E.g., for instructions, it might emit
 *         an address or the raw bytes.</li>
 *     <li>The TYPE_unparse callback list outputs the body of the object.  E.g., for basic blocks this would be the list
 *         of instructions belonging to the block.</li>
 *     <li>The TYPE_post callback list outputs footer information for the object.  E.g., for functions this could be a blank
 *         line to separate one function from another.</li>
 *  </ul>
 *
 *  The callback lists are initialized to contain the following functor types.  Some callbacks are no-ops for certain
 *  organizations of the output, and these are noted.
 *  <ul>
 *     <li>Instruction callbacks (SgAsmInstruction).
 *        <ol>
 *           <li>InsnBlockSeparation (pre): emits inter-block spacing when output is organized by address.</li>
 *           <li>InsnSkipBackStart (pre): prints skip/back info when output is organized by address.</li>
 *           <li>InsnFuncEntry (pre): emits function info at entry points when output is organized by address.</li>
 *           <li>InsnRawBytes (pre): emits address and raw bytes of instruction in a hexdump-like format.</li>
 *           <li>InsnBlockEntry (pre): emits info about first instruction of each block when output is organized by address.</li>
 *           <li>InsnBody (unparse): emits the instruction mnemonic and arguments.</li>
 *           <li>InsnNoEffect (post): emits an indication when an instruction is part of a no-effect sequence of
 *               instructions.  This functor only has an effect if the BasicBlockNoopUpdater functor is run as
 *               part of the pre-callbacks for basic blocks.</li>
 *           <li>InsnComment (post): emits any comment associated with the instruction.</li>
 *           <li>InsnLineTermination (post): emits a line feed at the end of the instruction.</li>
 *           <li>InsnSkipBackEnd (post): updates skip/back info at the end of the instruction.</li>
 *        </ol>
 *     </li>
 *     <li>Basic block callbacks (SgAsmBlock objects containing only instruction objects).  These callbacks are not invoked
 *     when output is organized by address.
 *        <ol>
 *           <li>BasicBlockReasons (pre): emits the reasons why this block is part of the function.</li>
 *           <li>BasicBlockPredecessors (pre): emits addresses of basic block control flow predecessors.</li>
 *           <li>BasicBlockBody (unparse): unparses each instruction of the basic block.</li>
 *           <li>BasicBlockSuccessors (post): emits addresses of basic block control flow successors.</li>
 *           <li>BasicBlockLineTermination (post): emits a linefeed at the end of each basic block.</li>
 *           <li>BasicBlockCleanup (post): cleans up analysis from BasicBlockNoopUpdater and future built-in functors.</li>
 *        </ol>
 *     </li>
 *     <li>StaticData (SgAsmStaticData):
 *        <ol>
 *           <li>StaticDataBlockSeparation (pre): emits inter-block spacing when output is organized by address.</li>
 *           <li>StaticDataSkipBackStart (pre): prints skip/back info when output is organized by address.</li>
 *           <li>StaticDataRawBytes (pre): emits a hexdump-like output of the value.</li>
 *           <li>StaticDataBlockEntry (pre): emits info about the first data node of each data block when output is
 *               organzied by address.</li>
 *           <li>StaticDataDetails (unparse): info about starting address, size, type, etc. We will probably split this
 *               callback into smaller parts once we have something interesting to denote.</li>
 *           <li>StaticDataComment (unparse): emits any comment associated with the data.</li>
 *           <li>StaticDataLineTermination (post): emits a linefeed at the end of each data object.</li>
 *           <li>StaticDataDisassembler (post): disassembles data as if it were code.</li>
 *           <li>StaticDataSkipBackEnd (post): updates skip/back info at the end of a static data block.</li>
 *        </ol>
 *     </li>
 *     <li>Data blocks (SgAsmBlock objects containing only data objects):
 *        <ol>
 *           <li>DataBlockBody (unparse): unparse each data object in the block.</li>
 *           <li>DataBlockLineTermination (post): emits a linefeed at the end of each data block.</li>
 *        </ol>
 *     </li>
 *     <li>Functions:
 *        <ol>
 *           <li>FunctionEntryAddress (pre): emits the function's entry virtual address.</li>
 *           <li>FunctionSeparator (pre): emits a bunch of "=" characters to make the function stand out.</li>
 *           <li>FunctionReasons (pre): emits the reasons why this address is considered the start of a function.</li>
 *           <li>FunctionName (pre): emits the name of the function in angle brackets, or "no name".</li>
 *           <li>FunctionLineTermination (pre): emits a linefeed for functions.</li>
 *           <li>FunctionComment (pre): emits function comments followed by a linefeed if necessary.</li>
 *           <li>FunctionPredecessors (pre): emits information about what calls this function
 *           <li>FunctionSuccessors (pre): emits information about what functions are called by this function.</li>
 *           <li>FunctionAttributes (pre): emits additional information about the function, such as whether it returns to the
 *               caller.</li>
 *           <li>FunctionLineTermination (pre): blank line before first block.<li>
 *           <li>FunctionBody (unparse): unparses the basic blocks of a function.</li>
 *        </ol>
 *     </li>
 *     <li>Interpretations:
 *        <ol>
 *           <li>InterpName (pre): emits the name of the interpratation based on the file headers present.</li>
 *           <li>InterpBody (pre): unparses teh functions making up an interpretation.</li>
 *        </ol>
 *     </li>
 *  </ul>
 *
 *  Additional functors are defined within the AsmUnparser class but are not installed into the callback lists by
 *  default. They're only provided because they might be useful to end users.
 *
 *  @section AsmUnparser_SDD Static Data Disassembly
 *
 *  Sometimes ROSE mistakenly assumes that certain parts of the address space are static data. This can happen, for example,
 *  when code is unreachable.  One common place this happens is when a call is made to a function that never returns, but the
 *  compiler thought it might return.  It's often useful to be able to disassemble these static data areas and present them as
 *  instructions (in addition to the usual data presentation).  Originally, this had to be done by hand using a derived
 *  AsmUnparser class and registering a static data unparser that did the disassembly, but now the AsmUnparser class
 *  defines a staticDataDisassembler callback (of type StaticDataDisassembler) for this purpose.  In order to disassemble
 *  static data, one needs to provide the staticDataDisassembler with a disassembler.  One way to do that is to use the default
 *  disassembler for the interpretation that's being unparsed.  One could also use the same disassembler that was originally
 *  used to disassemble the entire specimen. In either case, setting the disassembler's recursion properties is probably wise
 *  since static data is often meaningless as instructions.
 *
 *  @code
 *  SgAsmInterpretation *interp = ...;
 *  Disassembler *disassembler = Disassembler::lookup(interp)->clone();
 *  disassembler->set_search(Disassembler::SEARCH_DEFAULT | Disassembler::SEARCH_DEADEND |
 *                           Disassembler::SEARCH_UNKNOWN | Disassembler::SEARCH_UNUSED);
 *  AsmUnparser unparser;
 *  unparser.staticDataDisassembler.init(disassembler);
 *  unparser.unparse(std::cout, interp);
 *  delete disassembler; // ok to delete since we won't be using unparser anymore
 *  @endcode
 *
 *  The output will look something like this:
 *
 *  @verbatim
 0x0804828f: 00 ff 35 a0 95 04 08 ff |..5.....|
 0x08048297: 25 a4 95 04 08 00 00 00 |%.......|
 0x0804829f: 00                      |.       | 17 bytes untyped data beginning at 0x0804828f
 0x0804828f: 00 ff                   |..      | (data)   add    bh, bh
 0x08048291: 35 a0 95 04 08          |5....   | (data)   xor    eax, 0x080495a0
 0x08048296: ff 25 a4 95 04 08       |.%....  | (data)   jmp    DWORD PTR ds:[0x080495a4]
 0x0804829c: 00 00                   |..      | (data)   add    BYTE PTR ds:[eax], al
 0x0804829e: 00 00                   |..      | (data)   add    BYTE PTR ds:[eax], al @endverbatim
 *
 *  @section AsmUnparser_Examples Examples
 *
 *  This example shows how to escape entire instructions for HTML output, and surround each instruction with an HTML table row.
 *  We could have just as easily combined all three parts into a single functor, but doing it this way makes the example
 *  a bit more interesting.
 *
 *  @code
 *  class UnparseForHTML: public AsmUnparser {
 *
 *      // Callback to emit the beginning of the table row.
 *      class BB_Prologue: public UnparserCallback {
 *      public:
 *          virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
 *              if (enabled)
 *                  args.output <<"<tr><td>";
 *              return enabled;
 *          }
 *      } bb_prologue;
 *
 *      // Callback to output the instructions into a string and then escape
 *      // any characters that are special to HTML.
 *      class BB_Body: public UnparserCallback {
 *      public:
 *          virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
 *              if (enabled) {
 *                  for (size_t i=0; i<args.insns.size(); i++) {
 *                      std::ostringstream ss;
 *                      args.unparser->unparse_insn(ss, args.insns[i], i);
 *                      args.output <<StringUtility::htmlEscape(ss.str());
 *                  }
 *              }
 *              return enabled;
 *          }
 *      } bb_body;
 *
 *      // Callback to emit the end of the table row.
 *      class BB_Epilogue: public UnparserCallback {
 *      public:
 *          virtual bool operator()(bool enabled, const BasicBlockArgs &args) {
 *              if (enabled)
 *                  args.output <<"</tr></td>";
 *              return enabled;
 *          }
 *      } bb_epilogue;
 *
 *      // The constructor makes some adjustments to the callback lists.  The
 *      // basicBlockBody is the functor that would normally emit the instructions
 *      // of the basic block.
 *      UnparseForHTML() {
 *          basicblock_callbacks.pre.prepend(&bb_prologue);
 *          basicblock_callbacks.unparse.replace(&basicBlockBody, &bb_body);
 *          basicblock_callbacks.post.append(&bb_epilogue);
 *      }
 *  };
 *
 *  void
 *  unparse_binary(std::ostream &output, SgNode *ast)
 *  {
 *      output <<"<table>";
 *      UnparseForHTML().unparse(output, ast);
 *      output <<"</table>";
 *  }
 *  @endcode
 */
class AsmUnparser {
public:
    enum Organization {
        ORGANIZED_BY_AST,               /**< Output follows the AST organization.  In other words, the instructions and data
                                         *    for a block appear consecutively, the blocks of a function appear consecutively,
                                         *    and the functions of an interpretation appear consecutively.  This is the default
                                         *    format since it provides the most information about the organization of the
                                         *    binary file. */
        ORGANIZED_BY_ADDRESS            /**< Output of instructions and data are organized by address.  Specifically, in the
                                         *    case of overlapping instructions and/or data, the starting address is used to
                                         *    sort the output.  This organization is typical of more traditional
                                         *    disassemblers. */
    };

    /** Control Flow Graph type.  The unparser supports the standard binary control flow graph data type.  This could be
     *  templatized, but we're planning to move to a non-template graph type in the near future [RPM 2012-04-18]. */
    typedef BinaryAnalysis::ControlFlow::Graph CFG;
    typedef boost::graph_traits<CFG>::vertex_descriptor CFG_Vertex;
    typedef std::map<SgAsmBlock*, CFG_Vertex> CFG_BlockMap;
    typedef BinaryAnalysis::FunctionCall::Graph CG;
    typedef boost::graph_traits<CG>::vertex_descriptor CG_Vertex;
    typedef std::map<SgAsmFunction*, CG_Vertex> CG_FunctionMap;

    class UnparserCallback {
    public:
        /** Arguments common to all unparser callback lists. */
        struct GeneralArgs {
            GeneralArgs(AsmUnparser *unparser, std::ostream &output)
                : unparser(unparser), output(output) {}
            AsmUnparser *unparser;      /**< The object doing the unparsing, from which this callback is invoked. */
            std::ostream &output;       /**< Where output should be sent. */
        };
            
        /** Arguments passed to instruction unparsing callbacks. */
        struct InsnArgs: public GeneralArgs {
            InsnArgs(AsmUnparser *unparser, std::ostream &output, SgAsmInstruction *insn, size_t position_in_block)
                : GeneralArgs(unparser, output), insn(insn), position_in_block(position_in_block) {}
            SgAsmInstruction *get_node() const { return insn; } /**< Return the node being unparsed. */
            SgAsmInstruction *insn;     /**< The instruction being unparsed. */
            size_t position_in_block;   /**< The index position of the instruction within the basic block, or -1 if unknown. */
        };

        /** Arguments passed to basic block unparsing callbacks. */
        struct BasicBlockArgs: public GeneralArgs {
            BasicBlockArgs(AsmUnparser *unparser, std::ostream &output, SgAsmBlock *block,
                           const std::vector<SgAsmInstruction*> &insns)
                : GeneralArgs(unparser, output), block(block), insns(insns) {}
            SgAsmBlock *get_node() const { return block; } /**< Return the node being unparsed. */
            SgAsmBlock *block;          /**< The basic block being unparsed. */
            const std::vector<SgAsmInstruction*> &insns; /**< The instructions contained in this basic block. */
        };

        /** Arguments passed to data unparsing callbacks. */
        struct StaticDataArgs: public GeneralArgs {
            StaticDataArgs(AsmUnparser *unparser, std::ostream &output, SgAsmStaticData *data, size_t position_in_block)
                : GeneralArgs(unparser, output), data(data), position_in_block(position_in_block) {}
            SgAsmStaticData *get_node() const { return data; } /**< Return the node being unparsed. */
            SgAsmStaticData *data;      /**< The data being unparsed. */
            size_t position_in_block;   /**< The index position of the data within the data block, or -1 if unknown. */
        };

        /** Arguments passed to data block unparsing callbacks. */
        struct DataBlockArgs: public GeneralArgs {
            DataBlockArgs(AsmUnparser *unparser, std::ostream &output, SgAsmBlock *block,
                          const std::vector<SgAsmStaticData*> &datalist)
                : GeneralArgs(unparser, output), block(block), datalist(datalist) {}
            SgAsmBlock *get_node() const { return block; } /**< Return the node being unparsed. */
            SgAsmBlock *block;          /**< The block of data being unparsed. */
            const std::vector<SgAsmStaticData*> &datalist; /**< The data items contained in this data block. */
        };

        /** Arguments passed to function unparsing callbacks. */
        struct FunctionArgs: public GeneralArgs {
            FunctionArgs(AsmUnparser *unparser, std::ostream &output, SgAsmFunction *func)
                : GeneralArgs(unparser, output), func(func) {}
            SgAsmFunction *get_node() const { return func; } /**< Return the node being unparsed. */
            SgAsmFunction *func;        /**< The function being unparsed. */
        };

        /** Arguments passed to interpretation unparsing callbacks. */
        struct InterpretationArgs: public GeneralArgs {
            InterpretationArgs(AsmUnparser *unparser, std::ostream &output, SgAsmInterpretation *interp)
                : GeneralArgs(unparser, output), interp(interp) {}
            SgAsmInterpretation *get_node() const { return interp; } /**< Return the node being unparsed. */
            SgAsmInterpretation *interp; /**< The interpretation being unparsed. */
        };
        
        virtual ~UnparserCallback() {}

        /** Default callbacks. The default is to abort, because if these are called it means one of three things:
         *  <ol>
         *     <li>The arguments are wrong in the subclass and the subclass therefore isn't providing an implementation
         *         when it thinks it is.</li>
         *     <li>The wrong type was used for the second argument.  Instruction callbacks use InsnArgs, basic block
         *         callbacks use BasicBlockArgs, etc.</li>
         *     <li>The functor was added to the wrong callback list.  For instance, an instruction functor was accidently
         *         added to one of the function callback lists.</li>
         *  </ol>
         *  @{ */
        virtual bool operator()(bool enabled, const InsnArgs&)           { abort(); }
        virtual bool operator()(bool enabled, const BasicBlockArgs&)     { abort(); }
        virtual bool operator()(bool enabled, const StaticDataArgs&)     { abort(); }
        virtual bool operator()(bool enabled, const DataBlockArgs&)      { abort(); }
        virtual bool operator()(bool enabled, const FunctionArgs&)       { abort(); }
        virtual bool operator()(bool enabled, const InterpretationArgs&) { abort(); }
        /** @} */
    };

    /**************************************************************************************************************************
     *                                  Instruction Callbacks
     **************************************************************************************************************************/

    /** Functor to print skip/back information when an instruction is entered. Skip/back information can be enabled/disabled
     *  across the entire unparser by calling the set_skipback_reporting() method. This callback can be removed per object type
     *  without confusing other objects that define a similar callbacks (for example, to disable skip/back reporting for
     *  instructions but leave it in place for static data). */
    class InsnSkipBackBegin: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit basic block separation in output organized by address.  This does nothing if the output is organized by
     *  AST since the basic block callbacks handle it in that case. */
    class InsnBlockSeparation: public UnparserCallback {
    public:
        SgAsmBlock *prev_block;
        InsnBlockSeparation(): prev_block(NULL) {}
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit function information at entry points.  This does nothing if the output is organized by AST since
     * function callbacks will handle it in that case. */
    class InsnFuncEntry: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit instruction address. This isn't necessary if you use the InsnRawBytes functor. */
    class InsnAddress: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit instruction bytes. The output is similar to the hexdump utility. */
    class InsnRawBytes: public UnparserCallback {
    public:
        HexdumpFormat fmt;
        InsnRawBytes() {
            fmt.width = 8;              /* Max instruction bytes per line of output. */
            fmt.pad_chars = true;       /* Show ASCII characters as well as bytes. */
        }
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit info about the first instruction of a block.  If the instruction is the first instruction of a block,
     *  then certain information is printed.  The output for all instructions is always the same width so that things line up
     *  properly between instructions that are first in a block and those that aren't.  This callback is a no-op unless the
     *  output is organized by address. */
    class InsnBlockEntry: public UnparserCallback {
    public:
        bool show_function;             /**< If true (the default) show entry address of function owning block. */
        bool show_reasons;              /**< If true (the default) show block reason bits. */
        InsnBlockEntry(): show_function(true), show_reasons(true) {}
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit the entire instruction.  Output includes the mnemonic, arguments, and comments. */
    class InsnBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit a note about instructions that have no effect.  If the instruction is part of a sequence of
     * instructions that has no effect, then this callback prints " !EFFECT" or something similar. */
    class InsnNoEffect: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit instruction comment, if any. */
    class InsnComment: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Functor to emit instruction line termination. */
    class InsnLineTermination: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /** Update instruction end address for skip/back reporting.  This callback should probably not be removed if skip/back
     *  reporting is enabled and output is organized by address. Removing it could cause other object types to loose track of
     *  where we are in the address space and thus make incorrect skip/back reports. */
    class InsnSkipBackEnd: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InsnArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Basic Block Callbacks
     **************************************************************************************************************************/

    /** Functor to emit reasons this block is part of a function. */
    class BasicBlockReasons: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit control flow predecessor addresses.  This functor outputs a line containing the addresses of all known
     *  predecessors according to the unparser's control flow graph. */
    class BasicBlockPredecessors: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to update unparser's is_noop array. */
    class BasicBlockNoopUpdater: public UnparserCallback {
    public:
        bool debug;             /**< If set, then emit information about the no-op subsequences. */
        BasicBlockNoopUpdater(): debug(false) {}
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit a warning if the block contains any no-effect sequences. */
    class BasicBlockNoopWarning: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit the instructions that belong to a basic block.  This is a no-op except when output is organized by
     *  AST. */
    class BasicBlockBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit block successor list.  If the unparser's control flow graph is not empty, then we use it to find
     *  successors, otherwise we consult the successors cached in the AST.  The AST-cached successors were probably cached by
     *  the instruction partitioner (see Partitioner class), which does fairly extensive analysis -- certainly more than just
     *  looking at the last instruction of the block.  */
    class BasicBlockSuccessors: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit a blank line after every basic block. */
    class BasicBlockLineTermination: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to clean up after basic block.  This doesn't produce any output; it just resets some unparser state back to ane
     *  values. */
    class BasicBlockCleanup: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled/*ignored*/, const BasicBlockArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Static Data Callbacks
     **************************************************************************************************************************/

    /** Functor to print skip/back information when a static data block is entered. Skip/back information can be
     *  enabled/disabled across the entire unparser by calling the set_skipback_reporting() method. This callback can be
     *  removed per object type without confusing other objects that define a similar callbacks (for example, to disable
     *  skip/back reporting for static data blocks but leave it in place for instructions). */
    class StaticDataSkipBackBegin: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit data block separation in output organized by address.  This does nothing if the output is organized by
     *  AST since the data block callbacks handle it in that case. */
    class StaticDataBlockSeparation: public UnparserCallback {
    public:
        SgAsmBlock *prev_block;
        StaticDataBlockSeparation(): prev_block(NULL) {}
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit the bytes of the data block. */
    class StaticDataRawBytes: public UnparserCallback {
    public:
        bool show_address;              /* Should we show the address in the left margin? */
        bool show_offset;               /* If show_address is true, then should we display START+OFFSET or just an address? */

        HexdumpFormat fmt;
        StaticDataRawBytes() {
            show_address = true;
            show_offset = false;
            fmt.prefix = NULL;          /* Adjusted by the callback each time called. */
            fmt.multiline = false;      /* Do not emit prefix before first line and linefeed after last line. */
            fmt.width = 8;              /* Max data bytes per line of output. */
            fmt.pad_chars = true;       /* Show ASCII characters as well as bytes. */
        }
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit info about the first data node of a block.  If the node is the first data node of a data block,
     *  then certain information is printed.  The output for all nodes is always the same width so that things line up
     *  properly between nodes that are first in a block and those that aren't.  This callback is a no-op unless the
     *  output is organized by address. */
    class StaticDataBlockEntry: public UnparserCallback {
    public:
        bool show_function;             /**< If true (the default) show entry address of function owning block. */
        bool show_reasons;              /**< If true (the default) show block reason bits. */
        StaticDataBlockEntry(): show_function(true), show_reasons(true) {}
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit details about static data. */
    class StaticDataDetails: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit optional static data comment. */
    class StaticDataComment: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit a blank line after every data block. */
    class StaticDataLineTermination: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Disassembles static data as if it were code.   This callback only does something if it is first initialized with a
     *  disassembler. It can also be initialized with an unparser, but that is optional (the default unparser will be the same
     *  as a default-constructed AsmUnparser except that a pre-instruction callback is added to print the string "(data)" in
     *  front of every instruction as a reminder that the instruction came from what ROSE considered to be static data. */
    class StaticDataDisassembler: public UnparserCallback {
    public:
        class DataNote: public UnparserCallback {
        public:
            virtual bool operator()(bool enabled, const InsnArgs &args) {
                if (enabled)
                    args.output <<" (data)";
                return enabled;
            }
        };

        DataNote data_note;
        Disassembler *disassembler;
        AsmUnparser *unparser;
        bool unparser_allocated_here;
        StaticDataDisassembler(): disassembler(NULL), unparser(NULL), unparser_allocated_here(false) {}
        ~StaticDataDisassembler() { reset(); }
        virtual void reset();
        virtual void init(Disassembler *disassembler, AsmUnparser *unparser=NULL);
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Update static data end address for skip/back reporting.  This callback should probably not be removed if skip/back
     *  reporting is enabled and output is organized by address. Removing it could cause other object types to loose track of
     *  where we are in the address space and thus make incorrect skip/back reports. */
    class StaticDataSkipBackEnd: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Data Block Callbacks
     **************************************************************************************************************************/

    /** Functor to print some information at the beginning of a data block. */
    class DataBlockTitle: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit each data statement of the block.  This is a no-op except when output is organized by AST. */
    class DataBlockBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const DataBlockArgs &args);
    };

    /** Functor to emit a blank line after every data block. */
    class DataBlockLineTermination: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const DataBlockArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Function Callbacks
     **************************************************************************************************************************/

    /** Functor to emit function entry address. */
    class FunctionEntryAddress: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to emit function separator. */
    class FunctionSeparator: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to emit function reasons. */
    class FunctionReasons: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to emit function name. */
    class FunctionName: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to emit function line termination. */
    class FunctionLineTermination: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to print function comments followed by a linefeed if necessary. */
    class FunctionComment: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to print caller addresses. Callers are only shown if a control flow graph is present (see
     * add_control_flow_graph()). */
    class FunctionPredecessors: public UnparserCallback {
    public:
        std::string prefix;
        FunctionPredecessors();
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to print callee addresses. Prints a list of functions called by this function.  Callees are only shown if a
     *  control flow graph is present (see add_control_flow_graph()). */
    class FunctionSuccessors: public UnparserCallback {
    public:
        std::string prefix;
        FunctionSuccessors();
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to emit function attributes.  Attributes are emitted one per line and each line is prefixed with a user
     *  supplied string.  The string is a printf format string and may contain one integer specifier for the function entry
     *  address.   The default is "0x%08llx: ". */
    class FunctionAttributes: public UnparserCallback {
    public:
        std::string prefix;
        FunctionAttributes();
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /** Functor to unparse the function body.  This is a no-op except when output is organized by AST. */
    class FunctionBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const FunctionArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Interpretation Callbacks
     **************************************************************************************************************************/

    /** Functor to emit interpretation name. */
    class InterpName: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InterpretationArgs &args);
    };

    /** Functor to emit the functions in an interpretation.  This is a no-op except when output is organized by AST. */
    class InterpBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const InterpretationArgs &args);
    };

    /**************************************************************************************************************************
     *                                  Built-in Callbacks
     * Don't forget to modify init() to actually add these to callback lists if desired.
     **************************************************************************************************************************/

    /** @name Functors
     * 
     *  Functors used by the base class.  We declare these as public so users can have an address by which to search
     *  through the various callback lists.  For instance, if the user doesn't want to see instruction raw bytes, they just
     *  remove the insnRawBytes callback from the appropriate list(s).
     *
     *  @{
     */
    InsnSkipBackBegin insnSkipBackBegin;
    InsnBlockSeparation insnBlockSeparation;
    InsnFuncEntry insnFuncEntry;
    InsnAddress insnAddress;
    InsnRawBytes insnRawBytes;
    InsnBlockEntry insnBlockEntry;
    InsnBody insnBody;
    InsnNoEffect insnNoEffect;
    InsnComment insnComment;
    InsnLineTermination insnLineTermination;
    InsnSkipBackEnd insnSkipBackEnd;

    BasicBlockReasons basicBlockReasons;
    BasicBlockPredecessors basicBlockPredecessors;
    BasicBlockNoopUpdater basicBlockNoopUpdater;
    BasicBlockNoopWarning basicBlockNoopWarning;
    BasicBlockBody basicBlockBody;
    BasicBlockSuccessors basicBlockSuccessors;
    BasicBlockLineTermination basicBlockLineTermination;
    BasicBlockCleanup basicBlockCleanup;

    StaticDataSkipBackBegin staticDataSkipBackBegin;
    StaticDataBlockSeparation staticDataBlockSeparation;
    StaticDataRawBytes staticDataRawBytes;
    StaticDataBlockEntry staticDataBlockEntry;
    StaticDataDetails staticDataDetails;
    StaticDataComment staticDataComment;
    StaticDataLineTermination staticDataLineTermination;
    StaticDataDisassembler staticDataDisassembler;
    StaticDataSkipBackEnd staticDataSkipBackEnd;

    DataBlockBody dataBlockBody;
    DataBlockLineTermination dataBlockLineTermination;

    FunctionEntryAddress functionEntryAddress;
    FunctionSeparator functionSeparator;
    FunctionReasons functionReasons;
    FunctionName functionName;
    FunctionLineTermination functionLineTermination;
    FunctionComment functionComment;
    FunctionPredecessors functionPredecessors;
    FunctionSuccessors functionSuccessors;
    FunctionAttributes functionAttributes;
    FunctionBody functionBody;

    InterpName interpName;
    InterpBody interpBody;
    /** @} */

    /**************************************************************************************************************************
     *                                  Public methods
     **************************************************************************************************************************/

    /** Constructor that intializes the "unparser" callback lists with some useful functors. */
    AsmUnparser() {
        init();
    }

    virtual ~AsmUnparser() {}

    /** Get/set how output is organized.
     *
     *  The unparse() method organizes output by one of the methods described for the Organization enum.
     *
     *  @{ */
    virtual Organization get_organization() const { return organization; }
    virtual void set_organization(Organization organization) { this->organization = organization; }
    /** @} */

    /** Determines if a node can be unparsed.
     *
     *  The AsmUnparser only handles certain node types, namely those that correspond to binary functions, blocks, and
     *  instructions.  This method should return true if the specified node is one that can be parsed.  See also,
     *  find_unparsable_node(). */
    virtual bool is_unparsable_node(SgNode *node);

    /** Finds first unparsable node.
     *
     *  Traverses the specified AST to find a node that can be unparsed, and returns it.  The traversal is a depth-first
     *  traversal with pre-order visiting.  The traversal terminates as soon as a node is found.  Returns false if no suitable
     *  node can be found. */
    virtual SgNode *find_unparsable_node(SgNode *ast);

    /** Finds top unparsable nodes.
     *
     *  Traverses the specified AST to find nodes that can be unparsed, and returns a vector of such nodes.  Once a node is
     *  discovered, the subtree rooted at that node is not searched. */
    virtual std::vector<SgNode*> find_unparsable_nodes(SgNode *ast);
    
    /** Optional information about no-op sequences.
     *
     *  When a basic block is unparsed, the BasicBlockNoopUpdater callback (if present and enabled) will analyze its
     *  instructions and update the insn_is_noop vector.  The vector is indexed by position of instruction within the block and
     *  indicates whehter that instruction is part of a no-op sequence.  Note that if BasicBlockNoopUpdater is not called for a
     *  block, then the insn_is_noop vector will not be initialized.  Even if it is initialized, the size of the vector may be
     *  less then the number of instructions because we don't store trailing false values. The vector is cleared by the
     *  BasicBlockCleanup callback. */
    std::vector<bool> insn_is_noop;

    /** Unparse part of the AST.
     *
     *  This is the primary method for this class. It traverses the specified @p ast and prints some kind of assembly language
     *  output to the specified output stream.  The specifics of what is produced depend on how the AsmUnparser is configured.
     *
     *  The return value is the number of unparsable nodes encountered and has various meanings depending on the output
     *  organization.  For output organized by AST it is the number of "top-level unparsable nodes", the number of nodes,
     *  R, such that is_unparsable_node(R) is true, and there exists no node C such that C is a proper ancestor of R and
     *  contained in the specified AST.  For output organized by address, it is the number of instructions and data objects
     *  unparsed.  In any case, a return value of zero means that nothing was unparsed and no output was produced. */
    virtual size_t unparse(std::ostream&, SgNode *ast);

    /** Unparse a single node if possible.
     *
     *  Tries to unparse the given AST node directly, without traversing the AST to find a starting point.  This is basically a
     *  big switch statement that calls one of the unparse_WHATEVER() methods.
     *
     *  Returns true if the node was unparsed, false otherwise. */
    virtual bool unparse_one_node(std::ostream&, SgNode*);

    /** Unparse an object. These are called by unparse_one_node(), but might also be called by callbacks.
     *
     *  @{ */
    virtual bool unparse_insn(bool enabled, std::ostream&, SgAsmInstruction*, size_t position_in_block=(size_t)-1);
    virtual bool unparse_basicblock(bool enabled, std::ostream&, SgAsmBlock*);
    virtual bool unparse_staticdata(bool enabled, std::ostream&, SgAsmStaticData*, size_t position_in_block=(size_t)-1);
    virtual bool unparse_datablock(bool enabled, std::ostream&, SgAsmBlock*);
    virtual bool unparse_function(bool enabled, std::ostream&, SgAsmFunction*);
    virtual bool unparse_interpretation(bool enabled, std::ostream&, SgAsmInterpretation*);
    /** @} */

    /** Maps integers to labels. */
    typedef std::map<uint64_t, std::string> LabelMap;

    /** Adds function labels to the label map.  This method traverses the specified AST looking for function symbols, and adds
     *  their address and name to the label map for the unparser.  The map is used by some callbacks to convert numeric values
     *  to more human friendly labels.   If integer values are relative to other AST nodes, then one doesn't need to populate
     *  the LabelMap (see documentation for the AsmUnparser::labels data member. */
    void add_function_labels(SgNode *ast);

    /** Associates a control flow graph with this unparser.  If a control flow graph is present then certain output callbacks
     *  will be able to use that information.  For instance, the basicBlockPredecessors will emit a list of all the
     *  predecessors of a block.  Passing an empty graph will remove control flow information. */
    void add_control_flow_graph(const BinaryAnalysis::ControlFlow::Graph &cfg);

    /** Controls printing of skip/back messages during linear output.  Each callback that prints an object that occupies
     *  address space should call start_of_object() and end_of_object() before and after printing the object.  If output is
     *  organized in the linear fashion and the start of an object does not coincide with the end of the previous object, and
     *  if skip/back reporting is enabled, then a message about skipping ahead or backward is inserted into the unparse
     *  output at that point.
     *  @{ */
    void set_skipback_reporting(bool b=true) { skipback.active=b; skipback.triggered=false; }
    void clear_skipback_reporting() { set_skipback_reporting(false); }
    bool get_skipback_reporting() const { return skipback.active; }
    void reset_skipback() { skipback.triggered=false; skipback.va=0; }
    void start_of_object(rose_addr_t, std::ostream&);
    void end_of_object(rose_addr_t);
    /** @} */

protected:
    struct CallbackLists {
        ROSE_Callbacks::List<UnparserCallback> unparse;                 /**< The main unparsing callbacks. */
        ROSE_Callbacks::List<UnparserCallback> pre;                     /**< Callbacks invoked before 'unparse' callbacks. */
        ROSE_Callbacks::List<UnparserCallback> post;                    /**< Callbacks invoked after 'unparse' callbacks. */

        /** Clears all the callback lists. */
        void clear() {
            unparse.clear();
            pre.clear();
            post.clear();
        }
    };

    CallbackLists insn_callbacks;                       /**< Callbacks for instruction unparsing. */
    CallbackLists basicblock_callbacks;                 /**< Callbacks for basic block unparsing. */
    CallbackLists staticdata_callbacks;                 /**< Callbacks for static data unparsing. */
    CallbackLists datablock_callbacks;                  /**< Callbacks for data block unparsing. */
    CallbackLists function_callbacks;                   /**< Callbacks for function unparsing. */
    CallbackLists interp_callbacks;                     /**< Callbacks for interpretation unparsing. */

    /** This map is consulted whenever a constant is encountered. If the constant is defined as a key of the map, then that
     *  element's string is used as a label.  Populating this map is not as important as it once was, because now integers can
     *  be associated with other addressable AST nodes and labels are generated from them.  For example, if 0x08042000 is the
     *  entry address of a function named "init", then it can be added to the LabelMap and the unparser will generate this
     *  assembly code:
     *
     * @code
     *  call 0x08042000<init>
     * @endcode
     *
     * If the SgAsmDoubleWordValueExpression that represents the 0x08042000 is associated with the SgAsmFunction node for the
     * "init" function, then the same output is generated when the LabelMap is not populated.  In fact, the new method can also
     * generate code like this, where the integer is an offset from the entry point:
     *
     * @code
     *  je 0x08042080<init+0x80>
     * @endcode
     *
     * which isn't possible in general with the LabelMap mechanism. */
    LabelMap labels;

    /** How output will be organized. */
    Organization organization;

    /** Initializes the callback lists.  This is invoked by the default constructor. */
    virtual void init();

    /** Control flow graph. If non-empty, then it is used for things like listing CFG predecessors of each basic block. */
    CFG cfg;

    /** A mapping from SgAsmBlock to control flow graph vertex. This map is updated when the control flow graph is modified by
     *  the add_control_flow_graph() method. */
    CFG_BlockMap cfg_blockmap;

    /** Function call graph.  This graph is built from the control flow graph whenever add_control_flow_graph() is called. */
    CG cg;

    /** A mapping from SgAsmFunction to call graph vertex.  This map is updated when the control flow graph is modified by the
     *  add_control_flow_graph() method. */
    CG_FunctionMap cg_functionmap;

    /** Details for skip/back reporting. See set_skipback_reporting(). */
    struct SkipBack {
        SkipBack(): active(true), triggered(false), va(0) {}
        bool active;                    /** Make reports? */
        bool triggered;                 /** Have we seen the first object yet? Is the 'va' member valid? */
        rose_addr_t va;                 /** Virtual address for previous end_of_object() call. */
    } skipback;
};

#endif

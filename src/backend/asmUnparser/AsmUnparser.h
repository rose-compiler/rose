#ifndef ROSE_AsmUnparser_H
#define ROSE_AsmUnparser_H

#include <ostream>

#include "callbacks.h"          /* Needed for ROSE_Callbacks::List<> */

class SgAsmInstruction;
class SgAsmBlock;
class SgAsmFunction;
class SgAsmInterpretation;


/** Unparses binary AST into text.
 *
 *  This class operates on part of an AST corresponding to a binary file (executable, library, etc) and generates output. It is
 *  intended to be highly customizable and has been used to generate both assembly listings and Graphviz files.
 *
 *  To generate a basic assembly listing, use the unparse() method, like this:
 *  @code
 *  SgProject *project = ...;
 *  AsmUnparser().unparse(std::cout, project);
 *  @endcode
 *
 *  The unparser operates finding the list of top-level, unparsable nodes (see find_unparsable_nodes()) and unparsing each
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
 *  The callback lists are initialized to contain the following functor types.
 *  <ul>
 *     <li>Instructions (SgAsmInstruction):
 *        <ol>
 *           <li>InsnRawBytes (pre): emits address and raw bytes of instruction in a hexdump-like format.</li>
 *           <li>InsnBody (unparse): emits the instruction mnemonic and arguments.</li>
 *           <li>InsnNoEffect (post): emits an indication when an instruction is part of a no-effect sequence of
 *               instructions.  This functor only has an effect if the BasicBlockNoopUpdater functor is run as
 *               part of the pre-callbacks for basic blocks.</li>
 *           <li>InsnComment (post): emits any comment associated with the instruction.</li>
 *           <li>InsnLineTermination (post): emits a line feed at the end of the instruction.</li>
 *        </ol>
 *     </li>
 *     <li>Basic blocks (SgAsmBlock objects containing only instruction objects):
 *        <ol>
 *           <li>BasicBlockReasons (pre): emits the reasons why this block is part of the function.</li>
 *           <li>BasicBlockBody (unparse): unparses each instruction of the basic block.</li>
 *           <li>BasicBlockSuccessors (post): emits addresses of basic block successors.</li>
 *           <li>BasicBlockLineTermination (post): emits a linefeed at the end of each basic block.</li>
 *           <li>BasicBlockCleanup (post): cleans up analysis from BasicBlockNoopUpdater and future built-in functors.</li>
 *        </ol>
 *     </li>
 *     <li>StaticData (SgAsmStaticData):
 *        <ol>
 *           <li>StaticDataTitle (pre): emits a title line.</li>
 *           <li>StaticDataRawBytes (unparse): emits a hexdump-like output of the value.</li>
 *           <li>StaticDataLineTermination (post): emits a linefeed at the end of each data object.</li>
 *        </ol>
 *     </li>
 *     <li>Data blocks (SgAsmBlock objects containing only data objects):
 *        <ol>
 *           <li>DataBlockBody (unparse): unparse each data object in the block.</li>
 *        </ol>
 *     </li>
 *     <li>Functions:
 *        <ol>
 *           <li>FunctionEntryAddress (pre): emits the function's entry virtual address.</li>
 *           <li>FunctionSeparator (pre): emits a bunch of "=" characters to make the function stand out.</li>
 *           <li>FunctionReasons (pre): emits the reasons why this is address is considered the start of a function.</li>
 *           <li>FunctionName (pre): emits the name of the function in angle brackets, or "no name".</li>
 *           <li>FunctionLineTermination (pre): emits a linefeed for functions.</li>
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
            SgAsmInstruction *insn;     /**< The instruction being unparsed. */
            size_t position_in_block;   /**< The index position of the instruction within the basic block, or -1 if unknown. */
        };

        /** Arguments passed to basic block unparsing callbacks. */
        struct BasicBlockArgs: public GeneralArgs {
            BasicBlockArgs(AsmUnparser *unparser, std::ostream &output, SgAsmBlock *block,
                           const std::vector<SgAsmInstruction*> &insns)
                : GeneralArgs(unparser, output), block(block), insns(insns) {}
            SgAsmBlock *block;          /**< The basic block being unparsed. */
            const std::vector<SgAsmInstruction*> &insns; /**< The instructions contained in this basic block. */
        };

        /** Arguments passed to data unparsing callbacks. */
        struct StaticDataArgs: public GeneralArgs {
            StaticDataArgs(AsmUnparser *unparser, std::ostream &output, SgAsmStaticData *data, size_t position_in_block)
                : GeneralArgs(unparser, output), data(data), position_in_block(position_in_block) {}
            SgAsmStaticData *data;      /**< The data being unparsed. */
            size_t position_in_block;   /**< The index position of the data within the data block, or -1 if unknown. */
        };

        /** Arguments passed to data block unparsing callbacks. */
        struct DataBlockArgs: public GeneralArgs {
            DataBlockArgs(AsmUnparser *unparser, std::ostream &output, SgAsmBlock *block,
                          const std::vector<SgAsmStaticData*> &datalist)
                : GeneralArgs(unparser, output), block(block), datalist(datalist) {}
            SgAsmBlock *block;          /**< The block of data being unparsed. */
            const std::vector<SgAsmStaticData*> &datalist; /**< The data items contained in this data block. */
        };

        /** Arguments passed to function unparsing callbacks. */
        struct FunctionArgs {
            FunctionArgs(AsmUnparser *unparser, std::ostream &output, SgAsmFunction *func)
                : unparser(unparser), output(output), func(func) {}
            AsmUnparser *unparser;      /**< The object doing the unparsing, from which this callback is invoked. */
            std::ostream &output;       /**< Where output should be sent. */
            SgAsmFunction *func;        /**< The function being unparsed. */
        };

        /** Arguments passed to interpretation unparsing callbacks. */
        struct InterpretationArgs {
            InterpretationArgs(AsmUnparser *unparser, std::ostream &output, SgAsmInterpretation *interp)
                : unparser(unparser), output(output), interp(interp) {}
            AsmUnparser *unparser;      /**< The object doing the unparsing, from which this callback is invoked. */
            std::ostream &output;       /**< Where output should be sent. */
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
            fmt.width = 6;              /* Max instruction bytes per line of output. */
            fmt.pad_chars = true;       /* Show ASCII characters as well as bytes. */
        }
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

    /**************************************************************************************************************************
     *                                  Basic Block Callbacks
     **************************************************************************************************************************/

    /** Functor to emit reasons this block is part of a function. */
    class BasicBlockReasons: public UnparserCallback {
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

    /** Functor to emit the instructions that belong to a basic block. */
    class BasicBlockBody: public UnparserCallback {
    public:
        virtual bool operator()(bool enabled, const BasicBlockArgs &args);
    };

    /** Functor to emit block successor list.  These are the successors that were probably cached by the instruction
     * partitioner (see Partitioner class), which does fairly extensive analysis -- certainly more than just looking at the
     * last instruction of the block. */
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

    /** Functor to print some information at the beginning of a data item. */
    class StaticDataTitle: public UnparserCallback {
    public:
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
            fmt.multiline = true;       /* Also emit prefix before first line and linefeed after last line. */
            fmt.width = 8;             /* Max data bytes per line of output. */
            fmt.pad_chars = true;       /* Show ASCII characters as well as bytes. */
        }
        virtual bool operator()(bool enabled, const StaticDataArgs &args);
    };

    /** Functor to emit a blank line after every data block. */
    class StaticDataLineTermination: public UnparserCallback {
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

    /** Functor to emit each data statement of the block. */
    class DataBlockBody: public UnparserCallback {
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

    /** Functor to unparse the function body. */
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

    /** Functor to emit the functions in an interpretation. */
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
    InsnAddress insnAddress;
    InsnRawBytes insnRawBytes;
    InsnBody insnBody;
    InsnNoEffect insnNoEffect;
    InsnComment insnComment;
    InsnLineTermination insnLineTermination;

    BasicBlockReasons basicBlockReasons;
    BasicBlockNoopUpdater basicBlockNoopUpdater;
    BasicBlockNoopWarning basicBlockNoopWarning;
    BasicBlockBody basicBlockBody;
    BasicBlockSuccessors basicBlockSuccessors;
    BasicBlockLineTermination basicBlockLineTermination;
    BasicBlockCleanup basicBlockCleanup;

    StaticDataTitle staticDataTitle;
    StaticDataRawBytes staticDataRawBytes;
    StaticDataLineTermination staticDataLineTermination;

    DataBlockBody dataBlockBody;

    FunctionEntryAddress functionEntryAddress;
    FunctionSeparator functionSeparator;
    FunctionReasons functionReasons;
    FunctionName functionName;
    FunctionLineTermination functionLineTermination;
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
     *  The return value is the number of top-level unparsable nodes encountered.  A "top-level unparsable node" is any node,
     *  R, such that is_unparsable_node(R) is true, and there exists no node C such that C is a proper ancestor of R and
     *  contained in the specified AST.  A return value of zero means that nothing was unparsed and no output was produced. */
    virtual size_t unparse(std::ostream&, SgNode *ast);

    /** Unparse an object. These are called by unparse(), but might also be called by callbacks.
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
     *  to more human friendly labels. */
    void add_function_labels(SgNode *ast);

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
     *  element's string is used as a label. */
    LabelMap labels;

    /** Initializes the objects callback lists.  This is invoked by the default constructor. */
    virtual void init();

};

#endif

// Basic types for binary unparsing.
#ifndef ROSE_BinaryAnalysis_Unparser_H
#define ROSE_BinaryAnalysis_Unparser_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Sawyer/CommandLine.h>
#include <BaseSemantics2.h>
#include <BinaryEdgeArrows.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** Settings that control unparsing.
 *
 *  This struct holds all settings that control the output of the binary instruction unparser.  Particular subclasses of the
 *  basic unparser may have settings that derive from these base settings. The @ref commandLineSwitches function can be used to
 *  control these settings from a command-line. */
struct Settings {
    virtual ~Settings() {}

    struct {
        bool showingSourceLocation;                     /**< Show source file name and line number when available. */
        bool showingReasons;                            /**< Show reasons for function existing. */
        bool showingDemangled;                          /**< Show demangled name in preference to mangled name. */
        struct {
            bool showing;                               /**< Show function call graph? */
        } cg;                                           /**< Settings for function call graphs. */
        struct {
            bool showing;                               /**< Show stack delta? */
            bool concrete;                              /**< Show concrete or symbolic deltas. */
        } stackDelta;                                   /**< Settings for function stack deltas. */
        struct {
            bool showing;                               /**< Show calling convention? */
        } callconv;                                     /**< Settings for function calling convention. */
        struct {
            bool showing;                               /**< Show no-op analysis results? */
        } noop;                                         /**< Settings for no-op analysis. */
        struct {
            bool showing;                               /**< Show results of may-return analysis? */
        } mayReturn;                                    /**< Settings for may-return analysis. */
    } function;                                         /**< Settings for functions. */

    struct {
        bool showingSourceLocation;                     /**< Show source file name and line number when available. */
        struct {
            bool showingPredecessors;                   /**< Show basic block predecessors? */
            bool showingSuccessors;                     /**< Show basic block successors? */
            bool showingSharing;                        /**< Show functions when block is shared? */
            bool showingArrows;                         /**< Draw arrows from one block to another. */
        } cfg;                                          /**< Settings for control flow graphs. */
        struct {
            bool showingReachability;                   /**< Show code reachability in the basic block prologue area. */
        } reach;                                        /**< Reachability analysis results. */
    } bblock;                                           /**< Settings for basic blocks. */

    struct {
        bool showingSourceLocation;                     /**< Show source file ane line number when available. */
    } dblock;

    struct {
        struct {
            bool showing;                               /**< Show instruction addresses? */
            size_t fieldWidth;                          /**< Min characters to use per insn address. */
        } address;                                      /**< Settings for instruction starting addresses. */

        struct {
            bool showing;                               /**< Show instruction bytes? */
            size_t perLine;                             /**< Max number of bytes to show per line of output. */
            size_t fieldWidth;                          /**< Min characters to use for the bytes field. */
        } bytes;                                        /**< Settings for the bytes that make up an instruction. */

        struct {
            bool showing;                               /**< Show stack deltas? */
            size_t fieldWidth;                          /**< Min characters to use for the stack delta field. */
        } stackDelta;                                   /**< Settings for stack deltas. */

        struct {
            size_t fieldWidth;                          /**< Min characters to use for the instruction mnemonic. */
            std::string semanticFailureMarker;          /**< Mark instruction if it had semantic failures. */
        } mnemonic;                                     /**< Settings for instruction mnemonics. */

        struct {
            std::string separator;                      /**< How to separate one operand from another. */
            size_t fieldWidth;                          /**< Min characters to use for the operand list. */
            bool showingWidth;                          /**< Show width of all expression terms in square brackets. */
        } operands;                                     /**< Settings for the operand list. */

        struct {
            bool showing;                               /**< Show instruction comments? */
            bool usingDescription;                      /**< Lacking comment, use instruction description as comment? */
            std::string pre;                            /**< String to introduce a comment. */
            std::string post;                           /**< String to terminate a comment. */
            size_t fieldWidth;                          /**< Min characters to use for the comment field. */
        } comment;                                      /**< Settings for instruction comments. */

        struct {
            bool showing;                               /**< Show instruction semantics? */
            InstructionSemantics2::BaseSemantics::Formatter formatter; /**< How to format the semantic state output. */
            bool tracing;                               /**< Show instruction semantics traces when showing semantics. */
        } semantics;
    } insn;                                             /**< Settings for instructions. */
    
    struct {
        EdgeArrows::ArrowStylePreset style;             /**< One of the arrow style presets. */
    } arrow;                                            /**< How to render arrows along the left margin. */

    Settings();
    static Settings full();
    static Settings minimal();
};

// Forward declarations.
class Base;
class State;
typedef Sawyer::SharedPointer<Base> BasePtr;

/** Command-line switches for unparser settings. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings &settings);

} // namespace
} // namespace
} // namespace

#endif
#endif

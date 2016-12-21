// Basic types for binary unparsing.
#ifndef ROSE_BinaryAnalysis_Unparser_H
#define ROSE_BinaryAnalysis_Unparser_H

#include <Sawyer/CommandLine.h>

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

/** Settings that control unparsing.
 *
 *  This struct holds all settings that control the output of the binary instruction unparser.  Particular subclasses of the
 *  basic unparser may have settings that derive from these base settings. The @ref commandLineSwitches function can be used to
 *  control these settings from a command-line. */
struct SettingsBase {
    virtual ~SettingsBase() {}

    struct {
        bool showingReasons;                            /**< Show reasons for function existing. */
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
        struct {
            bool showingPredecessors;                   /**< Show basic block predecessors? */
            bool showingSuccessors;                     /**< Show basic block successors? */
            bool showingSharing;                        /**< Show functions when block is shared? */
        } cfg;                                          /**< Settings for control flow graphs. */
    } bblock;                                           /**< Settings for basic blocks. */

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
        } mnemonic;                                     /**< Settings for instruction mnemonics. */

        struct {
            std::string separator;                      /**< How to separate one operand from another. */
            size_t fieldWidth;                          /**< Min characters to use for the operand list. */
        } operands;                                     /**< Settings for the operand list. */

        struct {
            bool showing;                               /**< Show instruction comments? */
            std::string pre;                            /**< String to introduce a comment. */
            std::string post;                           /**< String to terminate a comment. */
            size_t fieldWidth;                          /**< Min characters to use for the comment field. */
        } comment;                                      /**< Settings for instruction comments. */
    } insn;                                             /**< Settings for instructions. */

    SettingsBase() {
        function.showingReasons = true;
        function.cg.showing = true;
        function.stackDelta.showing = true;
        function.stackDelta.concrete = true;
        function.callconv.showing = true;
        function.noop.showing = true;
        function.mayReturn.showing = true;

        bblock.cfg.showingPredecessors = true;
        bblock.cfg.showingSuccessors = true;
        bblock.cfg.showingSharing = true;

        insn.address.showing = true;
        insn.address.fieldWidth = 10;
        insn.bytes.showing = true;
        insn.bytes.perLine = 8;
        insn.bytes.fieldWidth = 25;
        insn.stackDelta.showing = true;
        insn.stackDelta.fieldWidth = 2;
        insn.mnemonic.fieldWidth = 1;
        insn.operands.separator = ", ";
        insn.operands.fieldWidth = 40;
        insn.comment.showing = true;
        insn.comment.pre = "; ";
        insn.comment.fieldWidth = 1;
    }
};

// Forward declarations.
class UnparserBase;
typedef Sawyer::SharedPointer<UnparserBase> UnparserBasePtr;

/** Command-line switches for unparser settings. */
Sawyer::CommandLine::SwitchGroup commandLineSwitches(SettingsBase &settings);

} // namespace
} // namespace
} // namespace

#endif

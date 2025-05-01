// Basic types for binary unparsing.
#ifndef ROSE_BinaryAnalysis_Unparser_Settings_H
#define ROSE_BinaryAnalysis_Unparser_Settings_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Color.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/Unparser/EdgeArrows.h>
#include <Sawyer/CommandLine.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** %Style of text. */
struct Style {
    Sawyer::Optional<Color::HSV> foreground;            /**< Optional foreground color. */
    Sawyer::Optional<Color::HSV> background;            /**< Optional background color. */

    /** Construct an empty style using default colors. */
    Style() {}

    /** Construct a new style with the specified foreground and background colors. */
    Style(const Sawyer::Optional<Color::HSV> &fg, const Sawyer::Optional<Color::HSV> &bg = Sawyer::Nothing())
        : foreground(fg), background(bg) {}

    /** Generate the ANSI escape for the style. */
    std::string ansiStyle() const;
};

/** %Settings that control unparsing.
 *
 *  This struct holds all settings that control the output of the binary instruction unparser.  Particular subclasses of the
 *  basic unparser may have settings that derive from these base settings. The @ref commandLineSwitches function can be used to
 *  control these settings from a command-line. */
struct Settings {
    virtual ~Settings() {}

    Color::Colorization colorization;                   /**< Overrides for global color settings. */

    struct {
        struct {
            Style style;
        } line;                                         /**< Comment occupying an entire line. */
        struct {
            Style style;
        } trailing;                                     /**< Comment extending to the end of the line. */
    } comment;

    struct {
        Style separatorStyle;                           /**< Style of the line separating functions. */
        Style titleStyle;                               /**< Style for title line of function. */
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
            Style arrowStyle;                           /**< Arrow style. */
            bool showingNormalFallThroughEdges;         /**< Show normal fall through edges from one block to the next. */
            bool showingCallReturnFallThroughEdges;     /**< Show call-return fall through edges from one block to the next. */
        } cfg;                                          /**< Settings for control flow graphs. */
        struct {
            bool showingReachability;                   /**< Show code reachability in the basic block prologue area. */
        } reach;                                        /**< Reachability analysis results. */
        bool showingPostBlock;                          /**< Show info about what happens after the last instruction. */
    } bblock;                                           /**< Settings for basic blocks. */

    struct {
        bool showingSourceLocation;                     /**< Show source file ane line number when available. */
    } dblock;

    struct {
        struct {
            bool showing;                               /**< Show instruction addresses or labels? */
            bool useLabels;                             /**< Generate labels and use them instead of addresses? */
            size_t fieldWidth;                          /**< Min characters to use per insn address. */
            Style style;                                /**< Style for printing the address. */
        } address;                                      /**< Settings for instruction starting addresses. */

        struct {
            bool showing;                               /**< Show instruction bytes? */
            size_t perLine;                             /**< Max number of bytes to show per line of output. */
            size_t fieldWidth;                          /**< Min characters to use for the bytes field. */
            Style style;                                /**< Style for printing the insn bytes. */
        } bytes;                                        /**< Settings for the bytes that make up an instruction. */

        struct {
            bool showing;                               /**< Show stack deltas? */
            size_t fieldWidth;                          /**< Min characters to use for the stack delta field. */
            Style style;                                /**< Style for the stack delta. */
        } stackDelta;                                   /**< Settings for stack deltas. */

        struct {
            bool showing;                               /**< Show frame pointer w.r.t. stack pointer. */
            size_t fieldWidth;                          /**< Min characters to use for the frame delta field. */
            Style style;                                /**< Style for the frame delta. */
        } frameDelta;                                   /**< Settings for frame deltas. */

        struct {
            size_t fieldWidth;                          /**< Min characters to use for the instruction mnemonic. */
            std::string semanticFailureMarker;          /**< Mark instruction if it had semantic failures. */
            Style semanticFailureStyle;                 /**< Style for the semantic failure indicator. */
            Style style;                                /**< Style for mnemonic. */
        } mnemonic;                                     /**< Settings for instruction mnemonics. */

        struct {
            std::string separator;                      /**< How to separate one operand from another. */
            size_t fieldWidth;                          /**< Min characters to use for the operand list. */
            bool showingWidth;                          /**< Show width of all expression terms in square brackets. */
            Style style;                                /**< Output style for instruction operands. */
        } operands;                                     /**< Settings for the operand list. */

        struct {
            bool showing;                               /**< Show instruction comments? */
            bool usingDescription;                      /**< Lacking comment, use instruction description as comment? */
            std::string pre;                            /**< String to introduce a comment. */
            std::string post;                           /**< String to terminate a comment. */
            size_t fieldWidth;                          /**< Min characters to use for the comment field. */
            // Style style;                             // see `comment.trailing.style` above
        } comment;                                      /**< Settings for instruction comments. */

        struct {
            bool showing;                               /**< Show instruction semantics? */
            InstructionSemantics::BaseSemantics::Formatter formatter; /**< How to format the semantic state output. */
            bool tracing;                               /**< Show instruction semantics traces when showing semantics. */
            Style style;                                /**< Style for showing instruction semantics. */
        } semantics;
    } insn;                                             /**< Settings for instructions. */
    
    struct {
        EdgeArrows::ArrowStylePreset style;             /**< One of the arrow style presets. */
    } arrow;                                            /**< How to render arrows along the left margin. */

    std::string linePrefix;

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

#ifndef ROSE_BinaryAnalysis_Unparser_Base_H
#define ROSE_BinaryAnalysis_Unparser_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2/ControlFlowGraph.h>
#include <Rose/BinaryAnalysis/Partitioner2/FunctionCallGraph.h>
#include <Rose/BinaryAnalysis/Reachability.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/BinaryAnalysis/Unparser/EdgeArrows.h>
#include <Rose/BinaryAnalysis/Unparser/Settings.h>
#include <Rose/BitFlags.h>
#include <Rose/Progress.h>

#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedObject.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

/** Map from address to label. */
typedef std::map<uint64_t, std::string> LabelMap;

/** Diagnostic output for unparsing. */
extern Sawyer::Message::Facility mlog;

// used internally to initialize mlog
void initDiagnostics();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Constructs a string to describe the invalid register.
 *
 *  May also optionally emit a diagnostic message. */
std::string invalidRegister(SgAsmInstruction*, RegisterDescriptor, const RegisterDictionaryPtr&)
    ROSE_DEPRECATED("not called by anything");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Margins containing arrows
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** State associated with printing arrows in the margin. */
class ArrowMargin {
public:
    /** Flags controlling the finer aspects of margin arrows. */
    enum Flags {
        /** Set this flag when you want the emitLinePrefix function to treat the next possible line as the start of a pointable
         *  entity. The actual start is delayed until an appropriate state is reached. When the entity does finally start, the
         *  emitLinePrefix function will clear this flag and set the corresponding EMIT flag instead.  The start of a pointable
         *  entity serves as the line to which arrows point. */
        POINTABLE_ENTITY_START = 0x00000001,

        /** Set this flag when you want the emitLinePrefix function to treat the next line as the end of a pointable entity.
         * The end of an entity is from whence arrows emanate.  The emitLinePrefix will clear this flag at the next
         * opportunity. */
        POINTABLE_ENTITY_END    = 0x00000002,

        /** This flag is modified automatically by the emitLinePrefix function. There is no need to adjust it. */
        POINTABLE_ENTITY_INSIDE = 0x00000004,

        /** If set, then emit the prefix area even if we seem to be generating output that the unparser would otherwise
         *  consider to be before or after the set of arrows. This flag is not adjusted by the renderer. */
        ALWAYS_RENDER           = 0x00000008
    };

    EdgeArrows arrows;                                  /**< The arrows to be displayed. */
    BitFlags<Flags> flags;                              /**< Flags that hold and/or control the output state. */
    Sawyer::Optional<EdgeArrows::EndpointId> latestEntity; /**< Latest pointable entity that was encountered in the output. */

    /** Reset the marging arrow state.
     *
     *  This should be called near the end of emitting a function, probably just before emitting the function epilogue. */
    void reset() {
        arrows.reset();
        flags = 0;
        latestEntity = Sawyer::Nothing();
    }

    /** Generate the string to print in the margin.
     *
     *  The @p currentEntity values are the vertex IDs used to initialize the @ref arrows method of this object. For control flow
     *  graphs, that's usually the entry address of a basic block. However, the unparser doesn't really care what kind of entities
     *  are being pointed at by the arrows. */
    std::string render(Sawyer::Optional<EdgeArrows::EndpointId> currentEntity);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Output style
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Stack of styles. */
class StyleStack {
    std::vector<Style> stack_;
    Style current_;
    Color::Colorization colorization_;

public:
    StyleStack() {}

    /** Property: Colorization settings.
     *
     * @{ */
    Color::Colorization colorization() const { return colorization_; }
    void colorization(const Color::Colorization c) { colorization_ = c; }
    /** @} */

    /** Push style onto stack.
     *
     *  Returns the old size of the stack that can be passed to popTo. */
    size_t push(const Style&);

    /** Pop top style from stack.
     *
     *  The stack must not be empty. */
    void pop();

    /** Pop until stack is a certain size. */
    void popTo(size_t);

    /** Clear the stack. */
    void reset();

    /** Number of styles on the stack. */
    size_t size() const;

    /** Merged style.
     *
     *  This returns a style with as many data members filled in as possible by looking at the top item and subsequent items
     *  as necessary.  For instance, if the top style specifies a foreground color but no background color, then we look at
     *  the next style (or deeper) to get a background color. */
    const Style& current() const;

private:
    void merge(const Style &style);                     // merge style into current_
    void mergeAll();                                    // recalculate current_ based on stack_
};

/** Pushes a style and arranges for it to be popped later. */
class StyleGuard {
    StyleStack &stack_;
    size_t n_;
    Style current_;
    Style previous_;
public:
    /** Push style onto stack.
     *
     *  The destructor will pop the stack back to its current size, removing the pushed style along with everything that
     *  was pushed after it. */
    StyleGuard(StyleStack &stack, const Style &style)
        : stack_(stack) {
        previous_ = stack_.current();
        n_ = stack_.push(style);
        current_ = stack_.current();
    }

    StyleGuard(StyleStack &stack, const Style &first, const Style &second)
        : stack_(stack) {
        previous_ = stack_.current();
        n_ = stack_.push(first);
        stack_.push(second);
        current_ = stack_.current();
    }

    ~StyleGuard() {
        stack_.popTo(n_);
    }

    /** Render style entry. */
    std::string render() const;

    /** Render style exit. */
    std::string restore() const;

    /** Current merged style. */
    const Style& current() const {
        return current_;
    }

    /** Style before pushing. */
    const Style& previous() const {
        return previous_;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// State of the unparser (the unparser itself is const during unparsing)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** State for unparsing.
 *
 *  This object stores the current state for unparsing. The state is kept separate from the unparser class so that (1) the
 *  unparser can be a const reference, and (2) multiple threads can be unparsing with the same unparser object. */
class State {
public:
    typedef Sawyer::Container::Map<rose_addr_t, std::string> AddrString; /**< Map from address to string. */

private:
    Partitioner2::PartitionerConstPtr partitioner_;
    Partitioner2::FunctionCallGraph cg_;
    Partitioner2::FunctionPtr currentFunction_;
    Partitioner2::BasicBlockPtr currentBasicBlock_;
    Sawyer::Optional<EdgeArrows::EndpointId> currentPredSuccId_;
    SgAsmExpression *currentExpression_;
    std::string nextInsnLabel_;
    AddrString basicBlockLabels_;
    RegisterNames registerNames_;
    const Base &frontUnparser_;
    std::vector<Reachability::ReasonFlags> cfgVertexReachability_;
    Sawyer::Container::Map<Reachability::ReasonFlags::Vector, std::string> reachabilityNames_; // map reachability value to name
    ArrowMargin intraFunctionCfgArrows_;                              // arrows for the intra-function control flow graphs
    ArrowMargin intraFunctionBlockArrows_;                            // user-defined intra-function arrows to/from blocks
    ArrowMargin globalBlockArrows_;                                   // user-defined global arrows to/from blocks
    bool cfgArrowsPointToInsns_;                                      // arrows point to insns? else predecessor/successor lines
    StyleStack styleStack_;                                           // styles
    bool isPostInstruction_ = false;                                  // show extra information appearing after an instruction

public:
    State(const Partitioner2::PartitionerConstPtr&, const Architecture::BaseConstPtr&, const Settings&, const Base &frontUnparser);
    virtual ~State();

    /** Property: Partitioner, which may be null. */
    Partitioner2::PartitionerConstPtr partitioner() const;

    /** Property: Call grap, which may be empty. */
    const Partitioner2::FunctionCallGraph& cg() const;

    /** Property: Reachability analysis results.
     *
     *  This property stores a vector indexed by CFG vertex IDs that holds information about whether the vertex is reachable
     *  and why.
     *
     * @{ */
    const std::vector<Reachability::ReasonFlags> cfgVertexReachability() const;
    void cfgVertexReachability(const std::vector<Reachability::ReasonFlags>&);
    /** @} */

    /** Returns reachability based on the @ref cfgVertexReachability property.
     *
     *  If the property has more elements than vertices in the CFG, then the extras are ignored; if the CFG is larger then the
     *  missing reachability values are assumed to be zero. */
    Reachability::ReasonFlags isCfgVertexReachable(size_t vertexId) const;

    /** Control flow graph arrows within a function.
     *
     *  This property holds information about how and when to draw arrows in the left margin to represent the edges of a control
     *  flow graph whose endpoints are both within the same function. The object is initialized each time a function is entered (see
     *  @ref Base::emitFunction) just before emitting the first basic block, but only if the unparser settings indicate that these
     *  margin arrows should be displayed.  The object is reset just after printing the basic blocks. The object should be in a
     *  default state when printing the function prologue and epilogue information, otherwise those parts of the output would be
     *  unecessarily indented.
     *
     *  See also, @ref intraFunctionBlockArrows, @ref globalBlockArrows.
     *
     * @{ */
    const ArrowMargin& intraFunctionCfgArrows() const { return intraFunctionCfgArrows_; }
    ArrowMargin& intraFunctionCfgArrows() { return intraFunctionCfgArrows_; }
    /** @} */

    /** User-defined intra-function margin arrows.
     *
     *  This object holds information about user-defined arrows in the margin. These arrows point to/from basic blocks
     *  such that both basic blocks are in the same function.
     *
     *  To update these arrows during unparsing, the user should provide an unparser updateIntraFunctionArrows either by class
     *  derivation or chaining.  The actual information about the arrows will be in that function's @c state argument in @c
     *  state.intraFunctionBlockArrows().arrows. Although it will have already been computed, the @p
     *  state.intraFunctionCfgArrows can be adjusted at the same time.
     *
     *  See also, @ref intraFunctionCfgArrows, @ref globalBlockArrows.
     *
     * @{ */
    const ArrowMargin& intraFunctionBlockArrows() const { return intraFunctionBlockArrows_; }
    ArrowMargin& intraFunctionBlockArrows() { return intraFunctionBlockArrows_; }
    /** @} */

    /** User-defined arrows to basic blocks across entire output.
     *
     *  This object holds information about user-defined arrows in the margin. These arrows point to/from basic blocks
     *  anywhere in the program (not just within functions as with @ref intraFunctionBlockArrows).
     *
     *  See also, @ref intraFunctionCfgArrows, @ref intraFunctionBlockArrows.
     *
     * @{ */
    const ArrowMargin& globalBlockArrows() const { return globalBlockArrows_; }
    ArrowMargin& globalBlockArrows() { return globalBlockArrows_; }
    /** @} */

    /** Call this when you're about to output the first instruction of a basic block. */
    void thisIsBasicBlockFirstInstruction();

    /** Call this when you're about to output the last instruction of a basic block. */
    void thisIsBasicBlockLastInstruction();

    /** Property: ID for CFG edge arrow endpoint.
     *
     *  When generating margin arrows that point to the "predecessor:" and "successor:" lines of the output (instead of arrows
     *  that point to the basic block instructions), this property holds the ID number for the arrow endpoint. See @ref
     *  EdgeArrows::computeCfgEdgeLayout.
     *
     * @{ */
    Sawyer::Optional<EdgeArrows::EndpointId> currentPredSuccId() const { return currentPredSuccId_; }
    void currentPredSuccId(Sawyer::Optional<EdgeArrows::EndpointId> id) { currentPredSuccId_ = id; }
    /** @} */

    /** Property: Whether CFG margin arrows point to instructions.
     *
     *  If set, then the CFG arrows in the left margin origin from and point to instructions of basic blocks. If false, they
     *  originate from "successor:" lines and point to "predecessor:" lines.  If there are no CFG margin arrows then the
     *  value of this property doesn't matter.
     *
     * @{ */
    bool cfgArrowsPointToInsns() const { return cfgArrowsPointToInsns_; }
    void cfgArrowsPointToInsns(bool b) { cfgArrowsPointToInsns_ = b; }
    /** @} */

    /** Property: Stack of styles.
     *
     * @{ */
    const StyleStack& styleStack() const { return styleStack_; }
    StyleStack& styleStack() { return styleStack_; }
    /** @} */

    /** Assign a reachability name to a reachability value.
     *
     *  The two-argument version of this function associates a name with a value. An empty name clears the association.
     *
     *  The one-argument version returns the name corresponding to the value, or generates a name on the fly. To generate a name,
     *  the value is first looked up in the mapping and that name is used if present. Otherwise, the value is broken down into
     *  individual bits and the resulting string is the comma-separated names for each of the bits. If the mapping has a name for a
     *  bit then it's used. Otherwise, if the bit is greater than or equal to @ref
     *  BinaryAnalysis::Reachability::Reason @c USER_DEFINED_0 the string will be "user-defined-x" where @p x is the number of bits to
     *  right shift the flag to make it equal to USER_DEFINED_0. Otherwise, the name of the bit is obtained by treating the value as
     *  an enum and obtaining the enum name. If that fails due to the fact that not all bits have corresponding enum constants, the
     *  name is the hexadecimal string for the bit.
     *
     * @{ */
    void reachabilityName(Reachability::Reason value, const std::string &name);
    std::string reachabilityName(Reachability::ReasonFlags value) const;
    /** @} */

    Partitioner2::FunctionPtr currentFunction() const;
    void currentFunction(const Partitioner2::FunctionPtr&);

    Partitioner2::BasicBlockPtr currentBasicBlock() const;
    void currentBasicBlock(const Partitioner2::BasicBlockPtr&);

    SgAsmExpression* currentExpression() const;
    void currentExpression(SgAsmExpression*);

    const std::string& nextInsnLabel() const;
    void nextInsnLabel(const std::string&);

    const RegisterNames& registerNames() const;
    void registerNames(const RegisterNames &r);

    const AddrString& basicBlockLabels() const;
    AddrString& basicBlockLabels();

    bool isPostInstruction() const;
    void isPostInstruction(bool);

    /** First unparser in the chained list of unparsers. */
    const Base& frontUnparser() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base unparser
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Abstract base class for unparsers.
 *
 *  This defines the interface only. All data that's used during unparsing is provided to each function so that this interface
 *  is thread-safe. Only the configuration settings are stored in the unparser itself.
 *
 *  End users generally invoke the high-level output methods, which are provided by non-virtual function operators that are
 *  overloaded on the type of object being unparsed.
 *
 *  The high-level function operators each call a corresponding overloaded virtual @ref unparse function that kicks things off by
 *  creating an unparse @ref Unparser::State object, setting the front parser to the one whose @ref unparse method was called, and
 *  invoking an appropriate mid-level "emit" function in that front parser.
 *
 *  Mid-level "emit" functions use a combination of C++ virtual functions and unparser object chaining as described in the
 *  documentation for those functions. This permits two ways to override or augment behavior: behavior modification based on
 *  architecture (virtual functions) and behavior modification based on a filtering concept to support things like tables and
 *  HTML (chaining).
 *
 *  The following code is a starting point for creating your own unparser:
 *
 * @code
 *  class MyUnparser: public BinaryAnalysis::Unparser::Base {
 *  protected:
 *      explicit MyUnparser(const BinaryAnalysis::Unparser::Base::Ptr &next)
 *          : BinaryAnalysis::Unparser::Base(next) {
 *          ASSERT_not_null(next);
 *      }
 *
 *  public:
 *      typedef Sawyer::SharedPointer<MyUnparser> Ptr;
 *
 *      static Ptr instance(const BinaryAnalysis::Unparser::Base::Ptr &next) { return Ptr(new MyUnparser(next)); }
 *      virtual BinaryAnalysis::Unparser::Base::Ptr copy() const override { return Ptr(new MyUnparser(nextUnparser()->copy())); }
 *      virtual const BinaryAnalysis::Unparser::Settings& settings() const override { return nextUnparser()->settings(); }
 *      virtual BinaryAnalysis::Unparser::Settings& settings() override { return nextUnparser()->settings(); }
 *
 *      // specialized output functions here...
 *  };
 * @endcode
 *
 *  To instantiate this parser from a @p Partitioner2::Partitioner object named @c partitioner, do this:
 *
 * @code
 *  BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner->unparser();
 *  unparser->settings() = settings.unparser; // See Rose::BinaryAnalysis::Unparser::Settings for starters
 *  unparser = MyUnparser::instance(unparser);
 * @endcode */
class Base: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<Base> Ptr;

private:
    Architecture::BaseConstPtr architecture_;           // non-null architecture
    Ptr nextUnparser_;

protected:
    explicit Base(const Architecture::BaseConstPtr&);
    explicit Base(const Ptr &nextUnparser);

public:
    virtual Ptr copy() const = 0;
    virtual ~Base();

    /** Property: Settings associated with this unparser.
     *
     *  Most of these settings can also be configured from the command-line. They control features of the unparser
     *  directly without the programmer having to invervene by creating a subclss or chaining a new parser. Most of
     *  the switches simply turn things on and off.
     *
     *  Copying an unparser also copies its settings.
     *
     * @{ */
    virtual const Settings& settings() const = 0;
    virtual Settings& settings() = 0;
    void settings(const Settings &s) {
        settings() = s;
    }
    /** @} */

    /** Property: Next parser in chain.
     *
     *  Parsers can be subclass and/or chained.  Nearly all functions check for chaining and delegate to the next parser
     *  in the chain.  When one function calls some other function, it uses the first parser of the chain.  The chain can
     *  be created by supplying an argument to the constructor, or it can be constructed later by setting this property.
     *
     * @{ */
    Ptr nextUnparser() const { return nextUnparser_; }
    void nextUnparser(Ptr next) { nextUnparser_ = next; }
    /** @} */

    /** Property: Architecture.
     *
     *  A non-null pointer to the architecture that this unparser unparses. */
    Architecture::BaseConstPtr architecture() const;

    /** Emit the entity to an output stream.
     *
     *  Renders the third argument as text and sends it to the stream indicated by the first argument.  The @p partitioner
     *  argument provides additional details about the thing being printed.  The version of this function that takes only two
     *  arguments causes all functions to be emitted.
     *
     *  @{ */
    void operator()(std::ostream&, const Partitioner2::PartitionerConstPtr&) const /*final*/;
    void operator()(std::ostream&, const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) const /*final*/;
    void operator()(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    void operator()(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::DataBlockPtr&) const /*final*/;
    void operator()(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&) const /*final*/;
    void operator()(std::ostream&, SgAsmInstruction*) const /*final*/;
    /** @} */

    /** Emit the entity to a string.
     *
     *  This is just a convenience wrapper around the three-argument form.
     *
     * @{ */
    std::string operator()(const Partitioner2::PartitionerConstPtr&, const Progress::Ptr& = Progress::Ptr()) const /*final*/;
    std::string operator()(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) const /*final*/;
    std::string operator()(const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    std::string operator()(const Partitioner2::PartitionerConstPtr&, const Partitioner2::DataBlockPtr&) const /*final*/;
    std::string operator()(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&) const /*final*/;
    std::string operator()(SgAsmInstruction*) const /*final*/;
    /** @} */

public:
    /** High-level unparsing function.
     *
     *  This function does the same thing as the function operator that has the same arguments.
     *
     * @{ */
    void unparse(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Progress::Ptr& = Progress::Ptr()) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::DataBlockPtr&) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&) const /*final*/;
    void unparse(std::ostream&, SgAsmInstruction*) const /*final*/;

    std::string unparse(const Partitioner2::PartitionerConstPtr&, const Progress::Ptr& = Progress::Ptr()) const /*final*/;
    std::string unparse(const Partitioner2::PartitionerConstPtr&, SgAsmInstruction*) const /*final*/;
    std::string unparse(const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    std::string unparse(const Partitioner2::PartitionerConstPtr&, const Partitioner2::DataBlockPtr&) const /*final*/;
    std::string unparse(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&) const /*final*/;
    std::string unparse(SgAsmInstruction*) const /*final*/;
    /** @} */

public:
    /** Mid-level unparser function.
     *
     *  This function emits some entity to the specified output stream. All call-specific state is supplied in the State
     *  reference so that this function can be called with a const parser references and is thread safe per state object. The
     *  high-level functions create the state -- states are not intended for end users -- before calling this mid-level
     *  function.
     *
     *  This mid-level function uses a combination of C++ virtual function calls and object chaining. The virtual functions
     *  allow subclasses to override or augment behavior from the base class, so that, e.g., an x86 operand expression can be
     *  output in a different format than an m68k expression.  The object chaining allows one unparser to override or augment
     *  the behavior of another function and is used, for example, to provide HTML wrapping around various entities.
     *
     *  The object chaining is implemented in two parts: every unparser object has a @ref nextUnparser pointer and the final
     *  unparser in the list has a null pointer for this property; the @ref Unparser::State object has a @ref State::frontUnparser
     *  "frontUnparser" method that returns the first unparser in this list.  This mid-level function is expected to always
     *  invoke functions on the front unparser in order to give every parser in the chain a chance to influence behavior. The
     *  base implementation of this mid-level function defers to the next parser in the chain if there is one, otherwise it
     *  performs some default output that's hopefully appropriate for most unparsers.
     *
     *  All such chained and virtual emitters begin with the letters "emit".
     *
     * @{ */
    virtual void emitFunction(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionPrologue(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionBody(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionEpilogue(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;

    virtual void emitFunctionSourceLocation(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionReasons(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallers(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallees(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionComment(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionStackDelta(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallingConvention(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionNoopAnalysis(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionMayReturn(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;

    virtual void emitDataBlockSourceLocation(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlock(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockPrologue(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockBody(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockEpilogue(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;

    virtual void emitBasicBlock(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockPrologue(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockBody(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockEpilogue(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;

    virtual void emitBasicBlockSourceLocation(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockComment(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockSharing(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockPredecessors(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockSuccessors(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockReachability(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;

    virtual void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionPrologue(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionBody(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionEpilogue(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitInstructionAddress(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionBytes(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionStackDelta(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionFrameDelta(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionOperands(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionComment(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitInstructionSemantics(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitOperand(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandPrologue(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandEpilogue(std::ostream&, SgAsmExpression*, State&) const;

    virtual void emitExpression(std::ostream&, SgAsmExpression*, State&) const;
#ifdef ROSE_ENABLE_ASM_AARCH32
    virtual std::vector<std::string> emitAarch32Coprocessor(std::ostream&, SgAsmAarch32Coprocessor*, State&) const;
#endif
#ifdef ROSE_ENABLE_ASM_AARCH64
    virtual std::vector<std::string> emitAarch64AtOperand(std::ostream&, SgAsmAarch64AtOperand*, State&) const;
    virtual std::vector<std::string> emitAarch64PrefetchOperand(std::ostream&, SgAsmAarch64PrefetchOperand*, State&) const;
    virtual std::vector<std::string> emitAarch64PState(std::ostream&, SgAsmAarch64PState*, State&) const;
    virtual std::vector<std::string> emitAarch64SysMoveOperand(std::ostream&, SgAsmAarch64SysMoveOperand*, State&) const;
    virtual std::vector<std::string> emitAarch64CImmediateOperand(std::ostream&, SgAsmAarch64CImmediateOperand*, State&) const;
    virtual std::vector<std::string> emitAarch64BarrierOperand(std::ostream&, SgAsmAarch64BarrierOperand*, State&) const;
#endif
    virtual std::vector<std::string> emitBinaryAdd(std::ostream&, SgAsmBinaryAdd*, State&) const;
    virtual std::vector<std::string> emitBinarySubtract(std::ostream&, SgAsmBinarySubtract*, State&) const;
    virtual std::vector<std::string> emitBinaryMultiply(std::ostream&, SgAsmBinaryMultiply*, State&) const;
    virtual std::vector<std::string> emitBinaryPreupdate(std::ostream&, SgAsmBinaryPreupdate*, State&) const;
    virtual std::vector<std::string> emitBinaryPostupdate(std::ostream&, SgAsmBinaryPostupdate*, State&) const;
    virtual std::vector<std::string> emitMemoryReferenceExpression(std::ostream&, SgAsmMemoryReferenceExpression*, State&) const;
    virtual std::vector<std::string> emitDirectRegisterExpression(std::ostream&, SgAsmDirectRegisterExpression*, State&) const;
    virtual std::vector<std::string> emitIndirectRegisterExpression(std::ostream&, SgAsmIndirectRegisterExpression*, State&) const;
    virtual std::vector<std::string> emitIntegerValueExpression(std::ostream&, SgAsmIntegerValueExpression*, State&) const;
    virtual std::vector<std::string> emitFloatValueExpression(std::ostream&, SgAsmFloatValueExpression*, State&) const;
    virtual std::vector<std::string> emitUnaryUnsignedExtend(std::ostream&, SgAsmUnaryUnsignedExtend*, State&) const;
    virtual std::vector<std::string> emitUnarySignedExtend(std::ostream&, SgAsmUnarySignedExtend*, State&) const;
    virtual std::vector<std::string> emitUnaryTruncate(std::ostream&, SgAsmUnaryTruncate*, State&) const;
    virtual std::vector<std::string> emitBinaryAsr(std::ostream&, SgAsmBinaryAsr*, State&) const;
    virtual std::vector<std::string> emitBinaryRor(std::ostream&, SgAsmBinaryRor*, State&) const;
    virtual std::vector<std::string> emitBinaryLsr(std::ostream&, SgAsmBinaryLsr*, State&) const;
    virtual std::vector<std::string> emitBinaryLsl(std::ostream&, SgAsmBinaryLsl*, State&) const;
    virtual std::vector<std::string> emitBinaryMsl(std::ostream&, SgAsmBinaryMsl*, State&) const;
    virtual std::vector<std::string> emitByteOrder(std::ostream&, SgAsmByteOrder*, State&) const;
    virtual std::vector<std::string> emitRegisterNames(std::ostream&, SgAsmRegisterNames*, State&) const;
    virtual std::vector<std::string> emitBinaryConcat(std::ostream&, SgAsmBinaryConcat*, State&) const;

    virtual void emitRegister(std::ostream&, RegisterDescriptor, State&) const;
    virtual std::vector<std::string> emitUnsignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitSignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitInteger(std::ostream&, const Sawyer::Container::BitVector&, State&,
                                                 bool isSigned) const;
    virtual bool emitAddress(std::ostream&, rose_addr_t, State&, bool always=true) const;
    virtual bool emitAddress(std::ostream&, const Sawyer::Container::BitVector&, State&, bool always=true) const;
    virtual void emitCommentBlock(std::ostream&, const std::string&, State&, const std::string &prefix = ";;; ") const;
    virtual void emitTypeName(std::ostream&, SgAsmType*, State&) const;

    virtual void emitLinePrefix(std::ostream&, State&) const;
    /** @} */

    //----- Other overrridable things -----
public:
    /** Finish initializing the unparser state.
     *
     *  This gets called by the @ref unparse methods just after the state object is created. It can be used to adjust the state
     *  before any unparsing actually starts. One common use is to initialize the global margin arrows.  The base implementation
     *  does nothing except chain to the next unparser. */
    virtual void initializeState(State&) const;

    /** Calculate intra-function arrows.
     *
     *  This is the oppurtunity for the subclass to calculate the intra-function arrows that should appear in the left margin
     *  of the output. This function is invoked by the base parser after emitting the function prologue and after possibly
     *  calculating CFG intra-function arrows but before emitting any basic blocks or data blocks for the function. */
    virtual void updateIntraFunctionArrows(State&) const;

    //-----  Utility functions -----
public:
    /** Render a string left justified. */
    static std::string leftJustify(const std::string&, size_t nchars);

    /** Render a table row.
     *
     *  Given a row of table data as a vector of cell contents, each of which could be multiple lines, return a string, also
     *  possibly multiple lines, that renders the row into columns. The @p colorEscapes are the pair of strings that should be
     *  emitted before and after each column and do not contribute to the width of the column. */
    static std::string juxtaposeColumns(const std::vector<std::string> &content, const std::vector<size_t> &minWidths,
                                        const std::vector<std::pair<std::string, std::string> > &colorEscapes,
                                        const std::string &columnSeparator = " ");

    /** Return true if edges are in order by source address.
     *
     *  If edge @p a has a source address that's less than the address of @p b, or if @ a has a source address and @p b has
     *  no source address, then return true; otherwise return false. Both edges must be valid edges, not end iterators. This
     *  defines the order that block prefixes are emitted. Source addresses are the last address of the source vertex. */
    static bool ascendingSourceAddress(Partitioner2::ControlFlowGraph::ConstEdgeIterator a,
                                       Partitioner2::ControlFlowGraph::ConstEdgeIterator b);

    /** Return true if edges are in order by target address.
     *
     *  If edge @p a has a target address that's less than the address of @p b, or if @ a has a target address and @p b has
     *  no target address, then return true; otherwise return false. Both edges must be valid edges, not end iterators. This
     *  defines the order that block suffixes are emitted. Target addresses are the first address of the target vertex. */
    static bool ascendingTargetAddress(Partitioner2::ControlFlowGraph::ConstEdgeIterator a,
                                       Partitioner2::ControlFlowGraph::ConstEdgeIterator b);

    /** Ordered incoming CFG edges.
     *
     *  Returns the incoming CFG edges for the specified basic block in the order that they should be displayed in the listing.
     *  The order is defined by @ref ascendingSourceAddress. */
    static std::vector<Partitioner2::ControlFlowGraph::ConstEdgeIterator>
    orderedBlockPredecessors(const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&);

    /** Ordered outgoing CFG edges.
     *
     *  Returns the outgoing CFG edges for the specified basic block in the order that they should be displayed in the listing.
     *  The order is defined by @ref ascendingTargetAddress. */
    static std::vector<Partitioner2::ControlFlowGraph::ConstEdgeIterator>
    orderedBlockSuccessors(const Partitioner2::PartitionerConstPtr&, const Partitioner2::BasicBlockPtr&);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Python API wrappers and functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_PYTHON_API
class PythonBase {
    Base::Ptr base_;

public:
    PythonBase() {}

    explicit PythonBase(const Base::Ptr &base)
        : base_(base) {
        ASSERT_not_null(base);
    }

    std::string unparse(const Partitioner2::PartitionerConstPtr &p) const {
        return base_->unparse(p);
    }

    void print(const Partitioner2::PartitionerConstPtr &p) const {
        base_->unparse(std::cout, p);
    }
};
#endif

} // namespace
} // namespace
} // namespace

#endif
#endif

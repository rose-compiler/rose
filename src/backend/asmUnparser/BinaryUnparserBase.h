#ifndef ROSE_BinaryAnalysis_UnparserBase_H
#define ROSE_BinaryAnalysis_UnparserBase_H

#include <BinaryEdgeArrows.h>
#include <BinaryReachability.h>
#include <BinaryUnparser.h>
#include <BitFlags.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>
#include <Partitioner2/FunctionCallGraph.h>
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/SharedObject.h>
#include <Progress.h>
#include <Registers.h>

namespace Rose {
namespace BinaryAnalysis {

/** Generates pseudo-assembly listings. */
namespace Unparser {

/** Diagnostic output for unparsing. */
extern Sawyer::Message::Facility mlog;

// used internally to initialize mlog
void initDiagnostics();

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
    Sawyer::Optional<EdgeArrows::VertexId> latestEntity;/**< Latest pointable entity that was encountered in the output. */

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
     *  The @p currentEntity values are the vertex IDs used to initialize the @ref arrows method of this object. For
     *  control flow graphs, that's usually the entry address of a basic block. However, the unparser doesn't really
     *  care what kind of entities are being pointed at by the arrows. */
    std::string render(Sawyer::Optional<EdgeArrows::VertexId> currentEntity);
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
    typedef Sawyer::Container::Map<rose_addr_t, std::string> AddrString;/**< Map from address to string. */

private:
    const Partitioner2::Partitioner &partitioner_;
    Partitioner2::FunctionCallGraph cg_;
    Partitioner2::FunctionPtr currentFunction_;
    Partitioner2::BasicBlockPtr currentBasicBlock_;
    Sawyer::Optional<EdgeArrows::VertexId> currentPredSuccId_;
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

public:
    State(const Partitioner2::Partitioner&, const Settings&, const Base &frontUnparser);
    virtual ~State();

    const Partitioner2::Partitioner& partitioner() const;

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
     *  This property holds information about how and when to draw arrows in the left margin to represent the edges of a
     *  control flow graph whose endpoints are both within the same function. The object is initialized each time a function is
     *  entered (see @code emitFunction) just before emitting the first basic block, but only if the unparser settings indicate
     *  that these margin arrows should be displayed.  The object is reset just after printing the basic blocks. The object
     *  should be in a default state when printing the function prologue and epilogue information, otherwise those parts
     *  of the output would be unecessarily indented.
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
    Sawyer::Optional<EdgeArrows::VertexId> currentPredSuccId() const { return currentPredSuccId_; }
    void currentPredSuccId(Sawyer::Optional<EdgeArrows::VertexId> id) { currentPredSuccId_ = id; }
    /** @} */

    /** Proerty: Whether CFG margin arrows point to instructions.
     *
     *  If set, then the CFG arrows in the left margin origin from and point to instructions of basic blocks. If false, they
     *  originate from "successor:" lines and point to "predecessor:" lines.  If there are no CFG margin arrows then the
     *  value of this property doesn't matter.
     *
     * @{ */
    bool cfgArrowsPointToInsns() const { return cfgArrowsPointToInsns_; }
    void cfgArrowsPointToInsns(bool b) { cfgArrowsPointToInsns_ = b; }
    /** @} */
    
    /** Assign a reachability name to a reachability value.
     *
     *  The two-argument version of this function associates a name with a value. An empty name clears the association.
     *
     *  The one-argument version returns the name corresponding to the value, or generates a name on the fly. To generate a
     *  name, the value is first looked up in the mapping and that name is used if present. Otherwise, the value is broken down
     *  into individual bits and the resulting string is the comma-separated names for each of the bits. If the mapping has a
     *  name for a bit then it's used. Otherwise, if the bit is greater than or equal to @ref
     *  Reachability::Reason::USER_DEFINED_0 the string will be "user-defined-x" where @p x is the number of bits to right
     *  shift the flag to make it equal to USER_DEFINED_0. Otherwise, the name of the bit is obtained by treating the value as
     *  an enum and obtaining the enum name. If that fails due to the fact that not all bits have corresponding enum constants,
     *  the name is the hexadecimal string for the bit.
     *
     * @{ */
    void reachabilityName(Reachability::Reason value, const std::string &name);
    std::string reachabilityName(Reachability::ReasonFlags value) const;
    /** @} */

    Partitioner2::FunctionPtr currentFunction() const;
    void currentFunction(const Partitioner2::FunctionPtr&);

    Partitioner2::BasicBlockPtr currentBasicBlock() const;
    void currentBasicBlock(const Partitioner2::BasicBlockPtr&);

    const std::string& nextInsnLabel() const;
    void nextInsnLabel(const std::string&);

    const RegisterNames& registerNames() const;
    void registerNames(const RegisterNames &r);

    const AddrString& basicBlockLabels() const;
    AddrString& basicBlockLabels();

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
 *  End users generally inovke the high-level output methods, which are provided by non-virtual function operators that are
 *  overloaded on the type of object being unparsed.
 *
 *  The high-level function operators each call a corresponding overloaded virtual @ref unparse function that kicks things off
 *  by creating an unparse @ref State object, setting the front parser to the one whose @ref unparse method was called, and
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
 *      virtual BinaryAnalysis::Unparser::Base::Ptr copy() const ROSE_OVERRIDE { return Ptr(new MyUnparser(nextUnparser()->copy())); }
 *      virtual const BinaryAnalysis::Unparser::Settings& settings() const ROSE_OVERRIDE { return nextUnparser()->settings(); }
 *      virtual BinaryAnalysis::Unparser::Settings& settings() ROSE_OVERRIDE { return nextUnparser()->settings(); }
 *
 *      // specialized output functions here...
 *  };
 * @endcode
 *
 *  To instantiate this parser from a @p Partitioner2::Partitioner object named @c partitioner, do this:
 *
 * @code
 *  BinaryAnalysis::Unparser::Base::Ptr unparser = partitioner.unparser();
 *  unparser->settings() = settings.unparser; // See Rose::BinaryAnalysis::Unparser::Settings for starters
 *  unparser = MyUnparser::instance(unparser);
 * @endcode */
class Base: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<Base> Ptr;

private:
    Ptr nextUnparser_;

protected:
    Base();
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

    /** Emit the entity to an output stream.
     *
     *  Renders the third argument as text and sends it to the stream indicated by the first argument.  The @p partitioner
     *  argument provides additional details about the thing being printed.  The version of this function that takes only two
     *  arguments causes all functions to be emitted.
     *
     *  @{ */
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p) const /*final*/ {
        unparse(out, p);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    SgAsmInstruction *insn) const /*final*/ {
        unparse(out, p, insn);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::BasicBlockPtr &bb) const /*final*/{
        unparse(out, p, bb);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::DataBlockPtr &db) const /*final*/ {
        unparse(out, p, db);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::FunctionPtr &f) const /*final*/ {
        unparse(out, p, f);
    }
    /** @} */

    /** Emit the entity to a string.
     *
     *  This is just a convenience wrapper around the three-argument form.
     *
     * @{ */
    std::string operator()(const Partitioner2::Partitioner &p, const Progress::Ptr &progress = Progress::Ptr()) const /*final*/ {
        return unparse(p, progress);
    }
    std::string operator()(const Partitioner2::Partitioner &p, SgAsmInstruction *insn) const /*final*/ {
        return unparse(p, insn);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::BasicBlockPtr &bb) const /*final*/ {
        return unparse(p, bb);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::DataBlockPtr &db) const /*final*/ {
        return unparse(p, db);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::FunctionPtr &f) const /*final*/ {
        return unparse(p, f);
    }
    /** @} */




public:
    /** High-level unparsing function.
     *
     *  This function does the same thing as the function operator that has the same arguments.
     *
     * @{ */
    void unparse(std::ostream&, const Partitioner2::Partitioner&, const Progress::Ptr &progress = Progress::Ptr()) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::Partitioner&, SgAsmInstruction*) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::DataBlockPtr&) const /*final*/;
    void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&) const /*final*/;

    std::string unparse(const Partitioner2::Partitioner&, const Progress::Ptr &progress = Progress::Ptr()) const /*final*/;
    std::string unparse(const Partitioner2::Partitioner&, SgAsmInstruction*) const /*final*/;
    std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::BasicBlockPtr&) const /*final*/;
    std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::DataBlockPtr&) const /*final*/;
    std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&) const /*final*/;
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
     *  unparser in the list has a null pointer for this property; the state object has a @ref State::frontUnparser
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
    virtual void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionOperands(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionComment(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitInstructionSemantics(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitOperand(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandPrologue(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandEpilogue(std::ostream&, SgAsmExpression*, State&) const;

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
     *  This gets called by the @ref unparse and @ref operator() methods just after the state object is created. It
     *  can be used to adjust the state before any unparsing actually starts. One common use is to initialize the
     *  global margin arrows.  The base implementation does nothing except chain to the next unparser. */
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
     *  Given a row of table data as a vector of cell contents, each of which could be multiple lines, return a
     *  string, also possibly multiple lines, that renders the row into columns. */
    static std::string juxtaposeColumns(const std::vector<std::string> &content, const std::vector<size_t> &minWidths,
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
    orderedBlockPredecessors(const Partitioner2::Partitioner&, const Partitioner2::BasicBlock::Ptr&);

    /** Ordered outgoing CFG edges.
     *
     *  Returns the outgoing CFG edges for the specified basic block in the order that they should be displayed in the listing.
     *  The order is defined by @ref ascendingTargetAddress. */
    static std::vector<Partitioner2::ControlFlowGraph::ConstEdgeIterator>
    orderedBlockSuccessors(const Partitioner2::Partitioner&, const Partitioner2::BasicBlock::Ptr&);
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

    std::string unparse(const Partitioner2::Partitioner &p) const {
        return base_->unparse(p);
    }

    void print(const Partitioner2::Partitioner &p) const {
        base_->unparse(std::cout, p);
    }
};
#endif

} // namespace
} // namespace
} // namespace

#endif

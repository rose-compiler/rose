#ifndef ROSE_TaintAnalysis_H
#define ROSE_TaintAnalysis_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tainted flow analysis.
//
// The original version of this tainted flow analysis was written 2012-09 by someone other than the author of the
// genericDataflow framework.  It is based on the sign analysis (sgnAnalysis.[Ch]) in this same directory since documentation
// for the genericDataflow framework is fairly sparse: 5 pages in the tutorial, not counting the code listings) and no doxygen
// documentation.
//
// This file contains two types of comments:
//    1. Comments that try to document some of the things I've discovered through playing with the genericDataflow framework.
//    2. Comments and suggestions about usability, consistency, applicability to binary analysis, etc.
//
// [RPM 2012-09]
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// USABILITY: Names of header files aren't consistent across the genericDataflow files. E.g., in the "lattice" directory we
// have "lattice.h" that defines the Lattice class, but "ConstrGraph.h" that defines "ConstrGraph" (and apparently no
// documentation as to what "Constr" means).
#include "lattice.h"
#include "dataflow.h"
#include "liveDeadVarAnalysis.h"        // misspelled? Shouldn't it be liveDeadVarsAnalysis or LiveDeadVarsAnalysis?


// USABILITY: The abundant use of dynamic_cast makes it seem like something's wrong with the whole dataflow design.  And I
//            couldn't find any documentation about when it's okay to cast from Lattice to one of its subclasses, so I've made
//            the assumption throughout that when the dynamic_cast returns null, the node in question points to a variable or
//            expression that the live/dead analysis has determined to be dead.
//
// USABILITY: No doxygen comments throughout genericDataflow framework?!?  But it looks like there's some doxygen-like stuff
//            describing a few function parameters, so is it using some other documenting system?  I at least added the headers
//            to the docs/Rose/rose.cfg file so doxygen picks up the structure.
//
// USABILITY: The genericDataflow framework always produces files named "index.html", "summary.html", and "detail.html" and
//            a directory named "dbg_imgs" regardless of any debug settings.  These are apprently the result of the Dbg::init()
//            call made from the main program, and this call is required (omitting it results in segmentation faults).  The
//            file names are constant and therefore one should expect tests to clobber each other's outputs when run in
//            parallel.  The contents of the files cannot be trusted.  Furthermore, since these are HTML files, an aborted test
//            will generate only a partial HTML file which some browsers will choke on.


/******************************************************************************************************************************
 * Taint Lattice
 ******************************************************************************************************************************/

/** A pointer to a vertex of the static taint lattice.
 *
 *  The taint lattice is static, consisting of only the vertices defined by the Vertex enum.  A TaintLattice object is
 *  slightly misnamed but follows existing convention: it's not really the whole lattice, but a pointer to one of the static
 *  latice's vertices. */
class TaintLattice: public FiniteLattice {
public:

    /** The differet vertices of this static lattice.  They're defined in a particular order so that the meetUpdate() method
     *  only needs to compute the maximum when merging two vertex pointers.  A value is considered to be tainted if it points
     *  to the VERTEX_TAINTED or the VERTEX_TOP vertex of the static taint lattice. */
    enum Vertex {
        VERTEX_BOTTOM,                  /**< No information is known about the value of the variable. */
        VERTEX_UNTAINTED,               /**< Value is not tainted. */
        VERTEX_TAINTED,                 /**< Value is tainted. */
        // no need for a top since that would imply that the value is tainted. I.e., VERTEX_TAINTED *is* our top.
    };

protected:
    Vertex vertex;                      /**< The vertex of the static taint lattice to which this object points. */

public:

    /** Default initializer makes this object point to the lattice's bottom vertex. */
    TaintLattice(): vertex(VERTEX_BOTTOM) {}

    /** Same as default constructor. */
    virtual void initialize() /*override*/ {
        *this = TaintLattice();
    }

    /** Accessor for this node's vertex in the lattice.  The set_level() mutator also returns true if the new value is different
     *  than the old value, and false if there was no change.
     * @{ */
    Vertex get_vertex() const { return vertex; }
    bool set_vertex(Vertex v);
    /** @} */


    /** Returns a new copy of this vertex pointer. */
    virtual Lattice *copy() const /*override*/ {
        return new TaintLattice(*this);
    }

    // USABILITY: The base class defines copy() without a const argument, so we must do the same here.
    /** Assignment-like operator. Makes this object point to the same lattice vertex as the @p other object. The other object
     *  must also be a TaintLattice object. */
    virtual void copy(/*const*/ Lattice *other_) /*override*/;


    // USABILITY: The base class defines '==' with non-const argument and "this", so we must do the same here.
    // USABILITY: This is not a real equality predicate since it's not reflexive.  In other words, (A==B) does not imply (B==A)
    //            for all values of A and B.
    /** Equality predicate, sort of. Beware that this is not true equality since it is not symmetric. */
    virtual bool operator==(/*const*/ Lattice *other_) /*const*/ /*override*/;

    // USABILITY: The base class defines str() with non-const "this", so we must do the same here.  That means that if we want
    //            to use this functionality from our own methods (that have const "this") we have to distill it out to some
    //            other place.
    // USABILITY: The "prefix" argument is pointless. Why not just use StringUtility::prefixLines() in the base class rather
    //            than replicate this functionality all over the place?
    /** String representation of the lattice vertex to which this object points.  The return value is the name of the vertex to
     *  which this object points, sans "VERTEX_" prefix, and converted to lower case.  The @p prefix is prepended to the
     *  returned string. */
    virtual std::string str(/*const*/ std::string /*&*/prefix) /*const*/ /*override*/ {
        return prefix + to_string();
    }

    // USABILITY: We define this only because of deficiencies with the "str" signature in the base class.  Otherwise our
    //            printing method (operator<<) could just use str().  We're trying to avoid evil const_cast.
    /** String representation of a lattice vertex.  Returns the name of the taint lattice vertex to which this object
     *  points. The returned string is one of the Vertex enum constants sans the "VERTEX_" prefix and converted to lower case. */
    std::string to_string() const;

    // USABILITY: The base class defines meetUpdate() with a non-const argument, so we must do the same here.
    /** Merges this lattice node with another and stores the result in this node.  Returns true iff this node changed. */
    virtual bool meetUpdate(/*const*/ Lattice *other_) /*override*/;

    friend std::ostream& operator<<(std::ostream &o, const TaintLattice &lattice);
};

/******************************************************************************************************************************
 * Taint Flow Analysis
 ******************************************************************************************************************************/

class TaintAnalysis: public IntraFWDataflow {
protected:
    LiveDeadVarsAnalysis* ldv_analysis;
    std::ostream *debug;

public:
    // USABILITY: Documentation as to why a live/dead analysis is used in SgnAnalysis would be nice. I tried doing it without
    //            originally to make things simpler, but it seems that the FiniteVarsExprProductLattice depends on it even
    //            though I saw commented out code and comments somewhere(?) that indicated otherwise.
    TaintAnalysis(LiveDeadVarsAnalysis *ldv_analysis)
        : ldv_analysis(ldv_analysis), debug(NULL) {}

    /** Accessor for debug settings.  If a non-null output stream is supplied, then debugging information will be sent to that
     *  stream; otherwise debugging information is suppressed.  Debugging is disabled by default.
     * @{ */
    std::ostream *get_debug() const { return debug; }
    void set_debug(std::ostream *os) { debug = os; }
    /** @} */

    // BINARIES:  The "Function" type is a wrapper around SgFunctionDeclaration and the data flow traversals depend on this
    //            fact.  Binaries don't have SgFunctionDeclaration nodes (they have SgAsmFunction, which is a bit different).
    //
    // NOTE:      The "DataflowNode" is just a VirtualCFG::DataflowNode that contains a VirtualCFG::CFGNode pointer and a
    //            "filter". I didn't find any documentation for how "filter" is used.
    //
    // USABILITY: The "initLattices" and "initFacts" are not documented. They're apparently only outputs for this function
    //            since they seem to be empty on every call and are not const.  They're apparently not parallel arrays since
    //            the examples I was looking at don't push the same number of items into each vector.
    //
    // USABILITY: Copied from src/midend/programAnalysis/genericDataflow/simpleAnalyses/sgnAnalysis.C.  I'm not sure what
    //            it's doing yet since there's no doxygen documentation for FiniteVarsExprsProductLattice or any of its
    //            members.
    //
    // BINARIES:  This might not work for binaries because FiniteVarsExprsProductLattice seems to do things in terms of
    //            variables.  Variables are typically lacking from binary specimens and most existing binary analysis
    //            describes things in terms of static register names or dynamic memory locations.
    /** Generate initial lattice state.  Generates the initial lattice state for the given dataflow node, in the given
     *  function, with the given node state. */
    void genInitState(const Function& func, const DataflowNode& node, const NodeState& state,
                      std::vector<Lattice*>& initLattices, std::vector<NodeFact*>& initFacts);

    // USABILITY: Not documented in doxygen, so I'm more or less copying from the SgnAnalysis::transfer() method defined in
    //            src/midend/programAnalysis/genericDataflow/sgnAnalysis.C.
    /** Adjust a result vertex pointer.  This function has an opportunity to adjust the result lattice vertex pointer based on
     *  input lattice vertices at a particular AST node at a particular time in the data flow.  For instance, if the AST node
     *  is an SgAddOp binry operation then we can obtain the vertices to which the two operands point (say, VERTEX_TAINTED and
     *  VERTEX_UNTAINTED) and adjust the result so it points to a particular vertex (say, VERTEX_TAINTED).
     *
     *  This method returns true if it changed where the result points, and false otherwise.  For example, if the result
     *  originally pointed to VERTEX_BOTTOM but now points to VERTEX_TAINTED then we would return true. */
    bool transfer(const Function& func, const DataflowNode& node_, NodeState& state, const std::vector<Lattice*>& dfInfo);

protected:
    /** Helps print lattice pointers. Like std::cout <<"lhs lattice is " <<lattice_info(lhs) <<"\n".  Convenient because the
     * live/dead analysis causes some lattices to be missing (null). */
    static std::string lattice_info(const TaintLattice *lattice) {
        return lattice ? lattice->to_string() : "dead";
    }

    /** Make certain variables always tainted.
     *
     *  Variables whose names begin with the string "TAINTED" are assumed to be tainted regardless of their situation. This is
     *  our currently kludgy way of introducing taint into a specimen -- the user modifies the specimen and introduces taint
     *  with these specially named variables.  E.g.,
     *
     * @code
     *   ORIGINAL                               MODIFIED
     *   ------------------------------------   ------------------------------------
     *   int specimen() {                       int specimen() {
     *                                              extern int TAINTED;
     *       int a = 2 * foo();                     int a = 2 * foo() + TAINTED;
     *       return a;                              return a;
     *   }                                      }
     * @endcode
     */
    bool magic_tainted(SgNode *node, FiniteVarsExprsProductLattice *prodLat);
};

#endif

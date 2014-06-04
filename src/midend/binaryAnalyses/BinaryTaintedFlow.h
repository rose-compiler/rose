#ifndef ROSE_BinaryAnalysis_TaintedFlow_H
#define ROSE_BinaryAnalysis_TaintedFlow_H

#include "BinaryDataFlow.h"
#include "Diagnostics.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace BinaryAnalysis {

/** Various tools for performing tainted flow analysis.
 *
 *  Example usage can be found in tests/roseTests/binaryTests/taintedFlow.C */
class TaintedFlow {
public:
    /** Taint values.
     *
     *  These values form a lattice where <code>NOT_TAINTED</code> and <code>TAINTED</code> are children of <code>TOP</code>
     *  and parents of <code>BOTTOM</code>. */
    enum Taintedness { BOTTOM, NOT_TAINTED, TAINTED, TOP };

    /** Merges two taint values.
     *
     *  Given two taint values that are part of a taintedness lattice, return the least common ancestor. */
    static Taintedness merge(Taintedness, Taintedness);

    /** Variable-Taintedness pair. */
    typedef std::pair<DataFlow::Variable, Taintedness> VariableTaint;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  State
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Taint state.
     *
     *  This class represents the variables being tracked by dataflow and maps each of those variables to a taintedness value.
     *  States are reference counted, so use either @ref instance or @ref copy to create new states. */
    class State {
        typedef std::list<VariableTaint> VarTaintList;
        VarTaintList taints_;

    public:
        /** Smart pointer for taint states. */
        typedef boost::shared_ptr<State> Ptr;

    protected:
        // Initialize taintedness for all variables; this is protected because this is a reference-counted object
        State(const DataFlow::VariableList &variables, Taintedness taint) {
            BOOST_FOREACH (const DataFlow::Variable &variable, variables)
                taints_.push_back(std::make_pair(variable, taint));
        }

    public:
        /** Allocating constructor.
         *
         *  Allocates a new instance of a taint state, initializing all variables to the specified @p taint.  Returns a pointer
         *  to the new reference-counted object. */
        static State::Ptr instance(const DataFlow::VariableList &variables, Taintedness taint = BOTTOM) {
            return State::Ptr(new State(variables, taint));
        }

        /** Virtual copy constructor.
         *
         *  Creates a new taint state that has the same variables and taintedness as this state. */
        virtual State::Ptr copy() const {
            return State::Ptr(new State(*this));
        }

        virtual ~State() {}

        /** Find the taintedness for some variable.
         *
         * The specified variable must exist in this state according to <code>Variable::mustAlias</code>. Returns a reference
         * to the variables taintedness value. */
        Taintedness& lookup(const DataFlow::Variable&);

        /** Set taintedness if the variable exists.
         *
         *  Returns true if the variable exists and false otherwise. */
        bool setIfExists(const DataFlow::Variable&, Taintedness);

        /** Merge other state into this state.
         *
         *  Merges the specified state into this state and returns true if this state changed in any way. */
        bool merge(const State::Ptr&);

        /** Print this state. */
        void print(std::ostream&) const;
    };

    /** Reference counting pointer to @ref State.
     *
     *  State objects are reference counted and should not be deleted explicitly. */
    typedef State::Ptr StatePtr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Transfer function
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    class TransferFunction {
        const DataFlow::VertexFlowGraphs &index_; // maps CFG vertex to data flow graph
    public:
        explicit TransferFunction(const DataFlow::VertexFlowGraphs &index): index_(index) {}

        template<class CFG>
        StatePtr operator()(const CFG &cfg, size_t cfgVertex, const StatePtr &in) {
            return (*this)(cfgVertex, in);
        }

        StatePtr operator()(size_t cfgVertex, const StatePtr &in);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //                                  Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static Sawyer::Message::Facility mlog;
    DataFlow dataFlow_;
    DataFlow::VertexFlowGraphs vertexFlowGraphs_;
    DataFlow::VariableList variableList_;
    bool vlistInitialized_;
    std::vector<StatePtr> results_;

public:
    /** Constructs a tainted flow analysis.
     *
     *  The @p userDispatcher is a CPU instruction dispatcher for instruction semantics and may use any semantic domain.  The
     *  semantic domain will be used to identify variables in the analyzed specimen.  The symbolic domain is the usual choice.
     *  The dispatcher need not have a valid state at this time; however, the state must be initialized before calling @ref
     *  computeFlowGraphs (if that method is called). */
    explicit TaintedFlow(const InstructionSemantics2::BaseSemantics::DispatcherPtr &userDispatcher)
        : dataFlow_(userDispatcher), vlistInitialized_(false) {}

    /** Initialize diagnostics.
     *
     *  This is called by rose::Diagnostics::initialize. */
    static void initDiagnostics();

    /** Compute data flow graphs.
     *
     *  This method computes a data flow graph for each reachable vertex of the control flow graph, and as a result also
     *  obtains the list of variables over which the tainted flow analysis will operate.  It uses whatever algorithm is
     *  implemented in @ref BinaryAnalysis::DataFlow::buildGraphPerVertex. */
    template<class CFG>
    void computeFlowGraphs(const CFG &cfg, size_t cfgStartVertex) {
        using namespace rose::Diagnostics;
        ASSERT_this();
        ASSERT_require(cfgStartVertex < cfg.nVertices());
        Stream mesg(mlog[WHERE] <<"computeFlowGraphs starting at CFG vertex " <<cfgStartVertex);
        vertexFlowGraphs_ = dataFlow_.buildGraphPerVertex(cfg, cfgStartVertex);
        variableList_ = dataFlow_.getUniqueVariables(vertexFlowGraphs_);
        results_.clear();
        vlistInitialized_ = true;
        mesg <<"; found " <<StringUtility::plural(variableList_.size(), "variables") <<"\n";
    }

    /** Property: data flow graphs.
     *
     *  The taint analysis stores data flow graph for each CFG vertex.  This information is used by the data flow engine's
     *  transfer function whenever it processes a CFG vertex.  The user can provide his own information by setting this
     *  property, or have the property's value calculated by calling @ref computeFlowGraphs.
     *
     *  @{ */
    const DataFlow::VertexFlowGraphs& vertexFlowGraphs() const {
        ASSERT_this();
        return vertexFlowGraphs_;
    }
    void vertexFlowGraphs(const DataFlow::VertexFlowGraphs &graphMap) {
        using namespace rose::Diagnostics;
        ASSERT_this();
        vertexFlowGraphs_ = graphMap;
        variableList_ = dataFlow_.getUniqueVariables(vertexFlowGraphs_);
        vlistInitialized_ = true;
        results_.clear();
        mlog[WHERE] <<"vertexFlowGraphs set by user with " <<StringUtility::plural(variableList_.size(), "variables") <<"\n";
    }
    /** @} */

    /** List of variables.
     *
     *  Returns the list of variables over which tainted flow analysis is operating.  The variables are in no particular
     *  order. The @ref vertexFlowGraphs property must have already been set or calculated. */
    const DataFlow::VariableList& variables() const {
        ASSERT_this();
        ASSERT_require2(vlistInitialized_, "TaintedFlow::computeFlowGraphs must be called before TaintedFlow::variables");
        return variableList_;
    }

    /** Creates a new state.
     *
     *  Creates a new state with all variables initialized to the specified taintedness value.  The @ref vertexFlowGraphs
     *  property must have alraeady been set or calculated. */
    StatePtr stateInstance() const {
        ASSERT_this();
        ASSERT_require2(vlistInitialized_, "TaintedFlow::computeFlowGraphs must be called before TaintedFlow::stateInstance");
        return State::instance(variableList_);
    }

    /** Run data flow.
     *
     *  Runs the tainted data flow analysis until it converges to a fixed point. */
    template<class CFG>
    void runToFixedPoint(const CFG &cfg, size_t cfgStartVertex, const StatePtr &initialState) {
        using namespace rose::Diagnostics;
        ASSERT_this();
        ASSERT_require(cfgStartVertex < cfg.nVertices());
        ASSERT_not_null(initialState);
        Stream mesg(mlog[WHERE] <<"runToFixedPoint starting at CFG vertex " <<cfgStartVertex);
        results_.clear();
        TransferFunction xfer(vertexFlowGraphs_);
        DataFlow::Engine<CFG, StatePtr, TransferFunction> dfEngine(cfg, xfer);
        dfEngine.runToFixedPoint(cfgStartVertex, initialState);
        results_ = dfEngine.getFinalStates();
        mesg <<"; results for " <<StringUtility::plural(results_.size(), "vertices", "vertex") <<"\n";
    }

    /** Query results.
     *
     *  Returns a the taint state at the specified control flow graph vertex.  The state is that which exists at the end of the
     *  specified vertex. */
    StatePtr getFinalState(size_t cfgVertexId) const {
        ASSERT_this();
        ASSERT_require(cfgVertexId < results_.size());
        return results_[cfgVertexId];
    }
};

std::ostream& operator<<(std::ostream &out, const TaintedFlow::State &state);

} // namespace

#endif

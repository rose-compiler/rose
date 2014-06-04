#ifndef ROSE_BinaryAnalysis_TaintedFlow_H
#define ROSE_BinaryAnalysis_TaintedFlow_H

#include "BinaryDataFlow.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>

namespace BinaryAnalysis {

/** Various tools for performing tainted flow analysis. */
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
    
    typedef std::pair<DataFlow::Variable, Taintedness> VariableTaint;


    /** Taint state.
     *
     *  This class represents the variables being tracked by dataflow and maps each of those variables to a taintedness value.
     *  States are reference counted, so use either @ref instance or @ref copy to create new states. */
    class State {
        typedef std::list<VariableTaint> VarTaintList;
        VarTaintList taints_;

    public:
        typedef boost::shared_ptr<State> Ptr;

    protected:
        // Initialize taintedness for all variables.
        explicit State(const DataFlow::VariableList &variables) {
            BOOST_FOREACH (const DataFlow::Variable &variable, variables)
                taints_.push_back(std::make_pair(variable, BOTTOM));
        }

    public:
        // Allocating constructor
        static State::Ptr instance(const DataFlow::VariableList &variables) {
            return State::Ptr(new State(variables));
        }

        // Virtual copy constructor
        virtual State::Ptr copy() const {
            return State::Ptr(new State(*this));
        }

        virtual ~State() {}

        // Find the taintedness for some variable.  The specified variable must exist in this state.  Memory addresses exist in
        // an arbitrary user domain and we can't assume that relational operators exist (not even "==" and "!=").  Therefore we
        // must do a linear search with the mustAlias operator.  We might be able to speed things up a little by realizing that
        // this example uses the symbolic domain -- symbolic expressions provide a 64-bit hash value that we could use in a
        // hashmap, but whenever the key is a different expression than the address stored in the state we would be relegated
        // to a linear search anyway.
        Taintedness& lookup(const DataFlow::Variable&);

        // Set taintedness if the variable exists and return true, else return false.
        bool setIfExists(const DataFlow::Variable&, Taintedness);

        // Merge other state into this state and return true if this state changed in any way.
        // This method is the only one required by the data flow engine.
        bool merge(const State::Ptr&);

        // Print the state
        void print(std::ostream&) const;
    };

    typedef State::Ptr StatePtr;

    // Data flow transfer functor.
    class TransferFunction {
        const DataFlow::VertexFlowIndex &index_; // maps CFG vertex to data flow graph
    public:
        explicit TransferFunction(const DataFlow::VertexFlowIndex &index): index_(index) {}

        template<class CFG>
        StatePtr operator()(const CFG &cfg, size_t cfgVertex, const StatePtr &in) {
            return (*this)(cfgVertex, in);
        }

        StatePtr operator()(size_t cfgVertex, const StatePtr &in);
    };

private:
    DataFlow dataFlow_;
    DataFlow::VertexFlowIndex vertexFlowIndex_;
    DataFlow::VariableList variableList_;
    bool vlistInitialized_;
    std::vector<StatePtr> results_;

public:
    explicit TaintedFlow(const InstructionSemantics2::BaseSemantics::DispatcherPtr &userDispatcher)
        : dataFlow_(userDispatcher), vlistInitialized_(false) {}

    template<class CFG>
    void computeFlowGraphs(const CFG &cfg, size_t cfgStartVertex) {
        ASSERT_this();
        ASSERT_require(cfgStartVertex < cfg.nVertices());
        vertexFlowIndex_ = dataFlow_.buildGraphPerVertex(cfg, cfgStartVertex);
        variableList_ = dataFlow_.getUniqueVariables(vertexFlowIndex_);
        results_.clear();
        vlistInitialized_ = true;
    }

    const DataFlow::VertexFlowIndex& vertexFlowIndex() const {
        ASSERT_this();
        return vertexFlowIndex_;
    }

    void vertexFlowIndex(const DataFlow::VertexFlowIndex &index) {
        ASSERT_this();
        vertexFlowIndex_ = index;
        variableList_ = dataFlow_.getUniqueVariables(vertexFlowIndex_);
        vlistInitialized_ = true;
        results_.clear();
    }

    const DataFlow::VariableList& variables() const {
        ASSERT_this();
        ASSERT_require2(vlistInitialized_, "TaintedFlow::computeFlowGraphs must be called before TaintedFlow::variables");
        return variableList_;
    }

    StatePtr stateInstance() const {
        ASSERT_this();
        ASSERT_require2(vlistInitialized_, "TaintedFlow::computeFlowGraphs must be called before TaintedFlow::stateInstance");
        return State::instance(variableList_);
    }

    template<class CFG>
    void runToFixedPoint(const CFG &cfg, size_t cfgStartVertex, const StatePtr &initialState) {
        ASSERT_this();
        ASSERT_require(cfgStartVertex < cfg.nVertices());
        ASSERT_not_null(initialState);
        results_.clear();
        TransferFunction xfer(vertexFlowIndex_);
        DataFlow::Engine<CFG, StatePtr, TransferFunction> dfEngine(cfg, xfer);
        dfEngine.runToFixedPoint(cfgStartVertex, initialState);
        results_ = dfEngine.getFinalStates();
    }

    StatePtr getFinalState(size_t cfgVertexId) const {
        ASSERT_this();
        ASSERT_require(cfgVertexId < results_.size());
        return results_[cfgVertexId];
    }
};

std::ostream& operator<<(std::ostream &out, const TaintedFlow::State &state);

} // namespace

#endif

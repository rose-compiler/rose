#include <rose.h>
#include "DistributedMemoryAnalysis.h"
#include "programVerification.h"

/* DistributedProgramVerifier: The core class for program verification in
 * distributed memory.  It inherits from the DistributedMemoryAnalysis class
 * that takes care of the actual distribution of the work; the interface each
 * subclass must implement is given by the four functions in the protected
 * section. */
class DistributedProgramVerifier
    : public DistributedMemoryAnalysis<Conditions *> {
public:
    DistributedProgramVerifier(): success(true), report("") {
    }

    bool verificationSucceeded() const {
        return success;
    }

    void printReport(std::ostream &outputStream) const {
        outputStream << report;
    }

protected:
    /* This function is called (in different instances of this class!) for
     * every defining function declaration in the AST. It is responsible for
     * doing the actual (typically very expensive) analysis work. */
    void analyzeSubtree(SgFunctionDeclaration *func,
                        Conditions *conditions) {
        /* Call the existing sequential program verification framework, note
         * whether it fails and the report it generates. */
        ProgramVerifier verifier;
        if (!verifier.verify(func, conditions))
            success = false;
        report += verifier.report();
    }

    /* This function is called after all functions have been analyzed. Its job
     * is to pack the relevant analyzer state into a buffer that can be
     * communicated to another processor and unpacked there. It returns a pair
     * consisting of the buffer length in bytes and a pointer to the buffer
     * itself. */
    std::pair<int, void *> serialize() {
        int stateSize = sizeof success + report.size() + 1;
        char *stateBuffer = new char[stateSize];
        /* Unfortunately, some low-level pointer operations are necessary. */
        std::memcpy(stateBuffer, &success, sizeof success);
        std::memcpy(stateBuffer + sizeof success, report.c_str(),
                    report.size() + 1);
        return std::make_pair(stateSize, stateBuffer);
    }

    /* This function combines a state object received from another processor
     * with the state of this instance of the analyzer. There is one
     * distinguished process in which this function will be called with each
     * of the states that the other processes sent. */
    void addSerializedState(std::pair<int, void *> state) {
        bool otherSuccess;
        std::memcpy(&otherSuccess, state.second, sizeof otherSuccess);
        if (otherSuccess == false)
            success = false;
        const char *otherReport
            = (const char *) state.second + sizeof otherSuccess;
        report += otherReport;
    }

    /* This function can be implemented to clean up after the serialized state
     * after it has been communicated; typically, this will free any dynamic
     * memory allocated for serialization. */
    void cleanupSerializedState(std::pair<int, void *> state) {
        delete[] (char *) state.second;
    }

private:
    bool success;
    std::string report;
};

/* ConditionTraversal: An auxiliary top-down traversal that will be run on
 * those parts of the AST that are outside defining function declarations. The
 * inherited attributes it computes for defining function declarations will be
 * passed as the second argument of the analyzeSubtree() function in
 * DistributedProgramVerifier, providing context information for the actual
 * verification process. */
class ConditionTraversal: public AstTopDownProcessing<Conditions *> {
protected:
    Conditions *evaluateInheritedAttribute(SgNode *node,
                                           Conditions *inheritedConditions) {
        Conditions *conditions = NULL;
        if (isSgClassDefinition(node)) {
            /* Propagate class invariants downwards. */
            conditions = classInvariants(isSgClassDefinition(node));
        } else if (isSgFunctionDeclaration(node)) {
            /* Compute the function's pre and post conditions and combine them
             * with the class invariants we may have inherited. */
            Conditions *prePostConditions
                = functionPrePostConditions(isSgFunctionDeclaration(node));
            conditions = combineConditions(prePostConditions,
                                           inheritedConditions);
        }
        return conditions;
    }
};

int main(int argc, char **argv) {
    // Initialize and check compatibility. See rose::initialize
    ROSE_INITIALIZE;

    SgProject *project = frontend(argc, argv);

    /* Must be called before any distributed memory analyses are performed. */
    initializeDistributedMemoryProcessing(&argc, &argv);

    /* The auxiliary traversal that is run outside defining function
     * declarations before the distributed part of the analysis. */
    ConditionTraversal conditionTraversal;

    /* The initial inherited attribute the auxiliary traversal is started
     * with. */
    Conditions *rootConditions = NULL;

    /* The distributed analyzer itself. */
    DistributedProgramVerifier distributedVerifier;

    /* This runs the analysis: First, the auxiliary traversal is run and the
     * inherited attributes it computes are saved; then the distributed
     * analyzer's analyzeSubtree() function is called for each defining
     * function declaration with the corresponding inherited attribute. */
    distributedVerifier.performAnalysis(project, rootConditions,
                                        &conditionTraversal);
    
    int status = EXIT_SUCCESS;
    /* The analyzer instance in exactly one of the distributed processes will
     * contain all the information that it gathered from the other processes;
     * the containsFinalResults() function in this process will return true,
     * its results can be used for further processing. Where this function
     * does not return true, the analyzer state is non complete and should not
     * be used. */
    if (distributedVerifier.containsFinalResults()) {
        if (!distributedVerifier.verificationSucceeded())
            status = EXIT_FAILURE;
        distributedVerifier.printReport(std::cout);
    }

    /* Must be called after all distributed analyses are finished. */
    finalizeDistributedMemoryProcessing();

    return status;
}

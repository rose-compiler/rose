#ifndef bROwSE_WPartitioner_H
#define bROwSE_WPartitioner_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/WHexValueEdit.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

/** Settings for running the partitioner. */
class WPartitioner: public Wt::WContainerWidget {
public:
    enum State {
        InitialState,                                   /**< Partitioner is in its initial state. No specimens are parsed or
                                                         *   loaded, the memory map is empty, the partitioner proper is in a
                                                         *   default unusable state with a null disassembler. */
        ParsedSpecimen,                                 /**< Specimen ELF/PE containers are parsed but not yet loaded into
                                                         *   memory or linked.  The memory map is empty and the partitioner
                                                         *   proper is in a default unusable state with a null disassembler. */
        LoadedSpecimen,                                 /**< Memory maps are initialized but not partitioner or disassembler
                                                         *   has been created yet. */
        PartitionedSpecimen,                            /**< Specimen has been disassembled and partitioned. */

        NStates
    };

    enum GhostEdgeFollowing { FOLLOW_NEVER, FOLLOW_NOW, FOLLOW_LATER };

private:
    Context &ctx_;
    State state_;                                       // current state
    std::vector<Wt::WPanel*> panels_;                   // panels created by init()

    Wt::WCheckBox *wUseConfiguration_;                  // use configuration files from command-line?
    Wt::WComboBox *wIsaName_;                           // instruction set architecture
    Wt::WText *wIsaError_;                              // error message for ISA problems
    Wt::WCheckBox *wUseSemantics_;                      // use symbolic semantics?
    Wt::WComboBox *wFollowGhostEdges_;                  // how to follow ghost edges from opaque predicates?
    Wt::WText *wFollowGhostEdgesWarning_;               // warning message for wFollowGhostEdges
    Wt::WCheckBox *wAllowDiscontiguousBlocks_;          // can basic blocks be discontiguous?
    Wt::WCheckBox *wFindDeadCode_;                      // look for unreachable basic blocks?
    Wt::WCheckBox *wDefeatPeScrambler_;                 // descramble PEScrambler binaries?
    Wt::WLineEdit *wPeScramblerDispatchVa_;             // address for PEScrambler dispatcher
    Wt::WCheckBox *wAssumeFunctionsReturn_;             // how do unknown functions behave?
    Wt::WComboBox *wInterpretation_;                    // which interpretation to unparse
    Wt::WSlider *wStackDeltaDepth_;                     // max depth for interprocedural stack delta analysis
    Wt::WCheckBox *wInterruptVector_;                   // whether to use an interrupt vector
    Wt::WLineEdit *wInterruptVectorVa_;                 // address of an interrupt vector
    Wt::WText *wInterruptVectorMessage_;                // messages from parsing the interrupt vector
    Wt::WCheckBox *wFindThunks_;                        // whether to search for thunks not in the CFG
    Wt::WCheckBox *wSplitThunks_;                       // whether to split thunks into their own functions
    Wt::WCheckBox *wCallingConvention_;                 // whether to perform calling convention analysis on the whole CG

    Wt::WPushButton *wParseSpecimen_;                   // causes ELF/PE files to be parsed
    Wt::WPushButton *wLoadSpecimen_;                    // causes specimen to be loaded into memory, and possibly linked
    Wt::WPushButton *wPartitionSpecimen_;               // causes specimen to be disassembled and partitioned

    Wt::Signal<bool> specimenParsed_;                   // emitted when a specimen is parsed (false means undo parse)
    Wt::Signal<bool> specimenLoaded_;                   // emitted when a specimen is loaded (false means undo load)
    Wt::Signal<bool> specimenPartitioned_;              // emitted after the partitioner runs

    WMemoryMap *wMemoryMap_;                            // optional object from which to obtain memory map

public:
    explicit WPartitioner(Context &ctx, Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx), state_(InitialState),
          wUseConfiguration_(NULL), wIsaName_(NULL), wIsaError_(NULL), wUseSemantics_(NULL), wFollowGhostEdges_(NULL),
          wFollowGhostEdgesWarning_(NULL), wAllowDiscontiguousBlocks_(NULL), wFindDeadCode_(NULL), wDefeatPeScrambler_(NULL),
          wPeScramblerDispatchVa_(NULL), wAssumeFunctionsReturn_(NULL), wInterpretation_(NULL), wStackDeltaDepth_(NULL),
          wInterruptVector_(NULL), wInterruptVectorVa_(NULL), wInterruptVectorMessage_(NULL), wFindThunks_(NULL),
          wSplitThunks_(NULL), wCallingConvention_(NULL), wParseSpecimen_(NULL), wLoadSpecimen_(NULL),
          wPartitionSpecimen_(NULL) {
        init();
    }

    /** Optional provider of memory map.
     *
     *  Immediately prior to disassembling and partitioning, the partitioning engine's memory map is obtained from a
     *  provider. A null provider causes the default memory map to be used. */
    void memoryMapProvider(WMemoryMap *w) { wMemoryMap_ = w; }

    std::vector<std::string> useConfiguration() const;
    std::string isaName() const;
    bool useSemantics() const;
    GhostEdgeFollowing followGhostEdges() const;
    bool allowDiscontiguousBlocks() const;
    bool findDeadCode() const;
    bool defeatPeScrambler() const;
    rose_addr_t peScramblerDispatcherVa() const;
    bool assumeFunctionsReturn() const;
    bool interruptVector() const;
    rose_addr_t interruptVectorVa() const;
    bool findingThunks() const;
    bool splittingThunks() const;
    bool analyzingCalls() const;

    State currentState() const { return state_; }
    void changeState(State);
    void redoState(State);

    Wt::Signal<bool>& specimenParsed() { return specimenParsed_; }
    Wt::Signal<bool>& specimenLoaded() { return specimenLoaded_; }
    Wt::Signal<bool>& specimenPartitioned() { return specimenPartitioned_; }

private:
    void init();
    Wt::WContainerWidget *makePanel(State, const std::string &title, Wt::WContainerWidget *parent);
    void adjustPanelBorders();

    // called when the useSemantics checkbox is checked or unchecked in order to adjust some other widgets that depend on
    // having semantics available.
    void handleUseSemantics(bool);

    bool parseSpecimen();
    void undoParseSpecimen();

    void chooseInterpretation(const Wt::WString&);

    bool loadSpecimen();
    void undoLoadSpecimen();

    void clearIsaError();

    bool partitionSpecimen();
    void undoPartitionSpecimen();

};

} // namespace
#endif

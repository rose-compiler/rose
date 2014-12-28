#ifndef bROwSE_WPartitioner_H
#define bROwSE_WPartitioner_H

#include <bROwSE/bROwSE.h>
#include <bROwSE/WHexValueEdit.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WMemoryMap;

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
    };

private:
    Context &ctx_;
    State state_;                                       // current state

    Wt::WComboBox *wIsaName_;                           // instruction set architecture
    Wt::WText *wIsaError_;                              // error message for ISA problems
    Wt::WCheckBox *wUseSemantics_;                      // use symbolic semantics?
    Wt::WCheckBox *wFollowGhostEdges_;                  // do we ignore opaque predicates?
    Wt::WCheckBox *wAllowDiscontiguousBlocks_;          // can basic blocks be discontiguous?
    Wt::WCheckBox *wFindDeadCode_;                      // look for unreachable basic blocks?
    Wt::WCheckBox *wDefeatPeScrambler_;                 // descramble PEScrambler binaries?
    WHexValueEdit *wPeScramblerDispatchVa_;             // address for PEScrambler dispatcher
    Wt::WCheckBox *wIntraFunctionCode_;                 // suck up unused addresses as intra-function code?
    Wt::WCheckBox *wAssumeFunctionsReturn_;             // how do unknown functions behave?
    Wt::WComboBox *wInterpretation_;                    // which interpretation to unparse

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
          wIsaName_(NULL), wIsaError_(NULL), wUseSemantics_(NULL), wFollowGhostEdges_(NULL), wAllowDiscontiguousBlocks_(NULL),
          wFindDeadCode_(NULL), wDefeatPeScrambler_(NULL), wPeScramblerDispatchVa_(NULL), wIntraFunctionCode_(NULL),
          wAssumeFunctionsReturn_(NULL), wInterpretation_(NULL), wParseSpecimen_(NULL), wLoadSpecimen_(NULL),
          wPartitionSpecimen_(NULL) {
        init();
    }

    /** Optional provider of memory map.
     *
     *  Immediately prior to disassembling and partitioning, the partitioning engine's memory map is obtained from a
     *  provider. A null provider causes the default memory map to be used. */
    void memoryMapProvider(WMemoryMap *w) { wMemoryMap_ = w; }

    std::string isaName() const;
    bool useSemantics() const;
    bool followGhostEdges() const;
    bool allowDiscontiguousBlocks() const;
    bool findDeadCode() const;
    bool defeatPeScrambler() const;
    rose_addr_t peScramblerDispatcherVa() const;
    bool intraFunctionCode() const;
    bool assumeFunctionsReturn() const;

    State currentState() const { return state_; }
    void changeState(State);

    Wt::Signal<bool>& specimenParsed() { return specimenParsed_; }
    Wt::Signal<bool>& specimenLoaded() { return specimenLoaded_; }
    Wt::Signal<bool>& specimenPartitioned() { return specimenPartitioned_; }

private:
    void init();

    void parseSpecimen();
    void undoParseSpecimen();

    void chooseInterpretation(const Wt::WString&);

    void loadSpecimen();
    void undoLoadSpecimen();

    void clearIsaError();

    void partitionSpecimen();
    void undoPartitionSpecimen();

};

} // namespace
#endif

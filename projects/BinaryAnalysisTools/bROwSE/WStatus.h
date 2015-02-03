#ifndef bROwSE_WStatus_H
#define bROwSE_WStatus_H

#include <bROwSE/bROwSE.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WContainerWidget>

namespace bROwSE {

// Models a table of messages.
class MessageModel: public Wt::WAbstractTableModel {
public:
    enum Column {
        SequenceColumn,                                 // sequence number of message (not the message ID)
        ArrivalColumn,                                  // Time the message arrived
        FacilityColumn,                                 // like "rose::BinaryAnalysis::Partitioner2"
        ImportanceColumn,                               // DEBUG, INFO, WARN, ERROR, etc.
        MessageColumn,                                  // The message text
        NColumns
    };

    struct Message {
        unsigned id;                                    // unique message ID (non sequential, from Sawyer)
        size_t sequence;                                // sequence of arrival
        std::string facility;                           // name of facility that produced the message
        Sawyer::Message::Importance importance;         // one of about 7 importance levels
        std::string arrivalTime;                        // arrival time as a string
        std::string message;                            // the message text

        Message(unsigned id, size_t sequence, const std::string &facility, Sawyer::Message::Importance importance,
                const std::string &arrivalTime, const std::string &message)
            : id(id), sequence(sequence), facility(facility), importance(importance), arrivalTime(arrivalTime),
              message(message) {}
    };

    typedef std::vector<Message> Messages;

private:
    Messages messages_;                                 // messages in the order they arrived

public:
    void insertMessage(const Message&);

    int rowCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE;
    int columnCount(const Wt::WModelIndex &parent = Wt::WModelIndex()) const ROSE_OVERRIDE;
    boost::any headerData(int column, Wt::Orientation orientation = Wt::Horizontal,
                          int role = Wt::DisplayRole) const ROSE_OVERRIDE;
    boost::any data(const Wt::WModelIndex &index, int role = Wt::DisplayRole) const ROSE_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Full status info
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** System status.
 *
 *  This displays full information about the status of the system and serves as the contents of the main "Status" tab. */
class WStatus: public Wt::WContainerWidget {
    Context &ctx_;

    Wt::WTreeTable *wStats_;
    Wt::WText *wProcessMemory_, *wProcessTime_, *wPartitionerInsns_, *wPartitionerBBlocks_, *wPartitionerFunctions_;
    Wt::WText *wPartitionerHullSize_, *wPartitionerBytes_, *wSemanticsAllocated_, *wSemanticsReserved_;
    Wt::WText *wRoseProject_, *wRoseTotal_;
    
    MessageModel *model_;
    Wt::WTableView *tableView_;

    Wt::Signal<std::string> messageArrived_;            // emitted when a message arrives from ROSE

public:
    explicit WStatus(Context &ctx, Wt::WContainerWidget *parent = NULL)
        : Wt::WContainerWidget(parent), ctx_(ctx),
          wStats_(NULL), wProcessMemory_(NULL), wProcessTime_(NULL), wPartitionerInsns_(NULL), wPartitionerBBlocks_(NULL),
          wPartitionerFunctions_(NULL), wPartitionerHullSize_(NULL), wPartitionerBytes_(NULL),
          wSemanticsAllocated_(NULL), wSemanticsReserved_(NULL), wRoseProject_(NULL), wRoseTotal_(NULL),
          model_(NULL), tableView_(NULL) {
        init();
    }

    /** Signal emitted whenever a diagnostic message arrives from ROSE. */
    Wt::Signal<std::string>& messageArrived() { return messageArrived_; }

    /** Update statistics. */
    void redraw();


private:
    friend class MessageTransfer;
    void init();
    void handleMessageArrival(const MessageModel::Message&);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Simple status bar
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** One-line status bar.
 *
 *  This normally appears across the bottom of the screen. */
class WStatusBar: public Wt::WContainerWidget {
    Wt::WText *wLatestMessage_;
public:

    explicit WStatusBar(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), wLatestMessage_(NULL) {
        init();
    }

    void appendMessage(const std::string &mesg);

private:
    void init();
};

} // namespace
#endif

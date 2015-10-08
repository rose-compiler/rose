#include <rose.h>
#include <bROwSE/WStatus.h>

#include <BinarySymbolicExpr.h>                         // ROSE
#include <boost/algorithm/string/trim.hpp>
#include <Color.h>                                      // ROSE
#include <Diagnostics.h>                                // ROSE
#include <Sawyer/SharedPointer.h>
#include <sys/resource.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WTableView>
#include <Wt/WText>
#include <Wt/WTree>
#include <Wt/WTreeTable>
#include <Wt/WTreeTableNode>
#include <Wt/WVBoxLayout>

using namespace rose;

namespace bROwSE {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Support functions, etc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static std::string
facilityBaseName(const std::string &s) {
    size_t colon = s.find_last_of(':');
    if (colon!=std::string::npos && colon+1<s.size())
        return s.substr(colon+1);
    return s;
}

// Sits between Sawyer::Message and WStatusModel to transfer messages from Sawyer (originally from ROSE, etc) into the message
// model which can then be displayed in a table.
class MessageTransfer: public Sawyer::Message::Destination {
    Wt::WApplication *application_;
    MessageModel *model_;
    WStatus *wStatus_;
    size_t sequence_;

protected:
    MessageTransfer(Wt::WApplication *application, MessageModel *model, WStatus *wStatus)
        : application_(application), model_(model), wStatus_(wStatus), sequence_(0) {
        ASSERT_not_null(model);
        ASSERT_not_null(wStatus);
    }

public:
    typedef Sawyer::SharedPointer<MessageTransfer> Ptr;

    static Ptr instance(Wt::WApplication *application, MessageModel *model, WStatus *wStatus) {
        return Ptr(new MessageTransfer(application, model, wStatus));
    }

    void post(const Sawyer::Message::Mesg &mesg, const Sawyer::Message::MesgProps &props) ROSE_OVERRIDE {
        if (mesg.isComplete()) {
            std::string s = boost::trim_copy(mesg.text());
            if (!s.empty()) {
                std::string facility = facilityBaseName(*props.facilityName);

                char arrivalTime[64];
                time_t now = time(NULL);
                const struct tm *tm = localtime(&now);
                snprintf(arrivalTime, sizeof arrivalTime, "%02d:%02d:%02d %s",
                         tm->tm_hour, tm->tm_min, tm->tm_sec, tm->tm_zone);
                
                MessageModel::Message m(mesg.id(), ++sequence_, facility, *props.importance, arrivalTime, mesg.text());

                // Message may be coming from any thread, so be careful
                Wt::WApplication::UpdateLock lock(application_);
                if (lock) {
                    model_->insertMessage(m);
                    wStatus_->handleMessageArrival(m);
#if 0 // [Robb P. Matzke 2014-12-31]: is this necessary?
                    application_->triggerUpdate();
#endif
                }
            }
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Message model
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
MessageModel::insertMessage(const Message &m) {
    layoutAboutToBeChanged().emit();
    messages_.push_back(m);
    layoutChanged().emit();
}

int
MessageModel::rowCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : messages_.size();
}

int
MessageModel::columnCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : NColumns;
}

boost::any
MessageModel::headerData(int column, Wt::Orientation orientation, int role) const {
    if (Wt::Horizontal == orientation) {
        if (Wt::DisplayRole == role) {
            ASSERT_require(column>=0 && (size_t)column < NColumns);
            switch ((Column)column) {
                case SequenceColumn:
                    return Wt::WString("Seq");
                case ArrivalColumn:
                    return Wt::WString("Time");
                case FacilityColumn:
                    return Wt::WString("Facility");
                case ImportanceColumn:
                    return Wt::WString("Imp");
                case MessageColumn:
                    return Wt::WString("Message");
                default:
                    ASSERT_not_reachable("invalid message column");
            }
        }
    }
    return boost::any();
}
    
boost::any
MessageModel::data(const Wt::WModelIndex &index, int role) const {
    ASSERT_require(index.isValid());
    ASSERT_require(index.row() >= 0 && (size_t)index.row() < messages_.size());
    ASSERT_require(index.column() >= 0 && (size_t)index.column() < NColumns);
    const Message &mesg = messages_[index.row()];
    if (Wt::DisplayRole == role) {
        switch ((Column)index.column()) {
            case SequenceColumn:
                return Wt::WString(StringUtility::numberToString(index.row()+1));
            case ArrivalColumn:
                return Wt::WString(mesg.arrivalTime);
            case FacilityColumn:
                return Wt::WString(mesg.facility);
            case ImportanceColumn:
                return Wt::WString(stringifyImportance(mesg.importance));
            case MessageColumn:
                return Wt::WString(mesg.message);
            default:
                ASSERT_not_reachable("invalid message column");
        }
    } else if (Wt::StyleClassRole == role) {
        if (ImportanceColumn == index.column()) {
            switch (messages_[index.row()].importance) {
                case Diagnostics::INFO:
                    return Wt::WString("status_info");
                case Diagnostics::WARN:
                    return Wt::WString("status_warn");
                case Diagnostics::ERROR:
                    return Wt::WString("status_error");
                case Diagnostics::FATAL:
                    return Wt::WString("status_fatal");
                default:
                    break;
            }
        }
        if (index.row() % 2)
            return Wt::WString("status_oddrow");
    }
    return boost::any();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      WStatus (the big status)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static Wt::WText*
addNode(Wt::WTreeTableNode *parent, const std::string &name, const std::string &units) {
    Wt::WTreeTableNode *node = new Wt::WTreeTableNode(name, 0, parent);
    Wt::WText *wValue = new Wt::WText("0");
    node->setColumnWidget(1, wValue);
    node->setColumnWidget(2, new Wt::WText(units));
    return wValue;
}

void
WStatus::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    //----------------------------------------------
    // Statistics from various parts of the system.
    //----------------------------------------------

    // We need the hbox to prevent the treetable from spaning the entire width of the browser
    Wt::WContainerWidget *statusContainer = new Wt::WContainerWidget;
    vbox->addWidget(statusContainer);
    Wt::WHBoxLayout *statusHBox = new Wt::WHBoxLayout;
    statusContainer->setLayout(statusHBox);

    wStats_ = new Wt::WTreeTable;
    wStats_->tree()->setSelectionMode(Wt::SingleSelection);
    wStats_->addColumn("Value", Wt::WLength(6, Wt::WLength::FontEm));
    wStats_->addColumn("Units", Wt::WLength(6, Wt::WLength::FontEm));
    wStats_->resize(650, Wt::WLength::Auto);
    statusHBox->addWidget(wStats_);

    Wt::WTreeTableNode *root = new Wt::WTreeTableNode("System statistics");
    wStats_->setTreeRoot(root, "System statistics");

    Wt::WTreeTableNode *processGroup = new Wt::WTreeTableNode("Process", 0, root);
    wProcessMemory_ = addNode(processGroup, "memory", "MB");
    wProcessTime_   = addNode(processGroup, "time",   "seconds");

    Wt::WTreeTableNode *partitionerGroup = new Wt::WTreeTableNode("Partitioner", 0, root);
    wPartitionerInsns_     = addNode(partitionerGroup, "InsnCache",  "insns");
    wPartitionerBBlocks_   = addNode(partitionerGroup, "CFG",        "bblocks");
    wPartitionerFunctions_ = addNode(partitionerGroup, "CFG",        "functions");
    wPartitionerBytes_     = addNode(partitionerGroup, "AUM actual", "bytes");
    wPartitionerHullSize_  = addNode(partitionerGroup, "AUM hull",   "bytes");

    Wt::WTreeTableNode *semanticsGroup = new Wt::WTreeTableNode("Semantics", 0, root);
    wSemanticsAllocated_ = addNode(semanticsGroup, "allocated", "nodes");
    wSemanticsReserved_  = addNode(semanticsGroup, "reserved",  "nodes");

    Wt::WTreeTableNode *roseGroup = new Wt::WTreeTableNode("ROSE AST", 0, root);
    wRoseProject_ = addNode(roseGroup, "project", "nodes");
    wRoseTotal_   = addNode(roseGroup, "total",   "nodes");

    root->expand();
    processGroup->expand();
    partitionerGroup->expand();
    semanticsGroup->expand();
    roseGroup->expand();

    statusHBox->addWidget(new Wt::WText(""), 1 /*stretch*/);// prevent WTreeTable from spanning entire width

    //-------------------------------
    // Diagnostic messages from ROSE
    //-------------------------------

    vbox->addWidget(new Wt::WBreak());
    vbox->addWidget(new Wt::WBreak());
    vbox->addWidget(new Wt::WText("<b>Diagnostics from ROSE</b>"));

    Wt::WContainerWidget *wMessageTable = new Wt::WContainerWidget;
    vbox->addWidget(wMessageTable, 1 /*stretch*/);
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
    wMessageTable->setLayout(hbox);                     // so table view scrolls horizontally

    model_ = new MessageModel;

    tableView_ = new Wt::WTableView;
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1);                   // this must be first property set
    tableView_->resize(Wt::WLength::Auto, Wt::WLength(5, Wt::WLength::FontEx));
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(false);
    tableView_->setAlternatingRowColors(false);         // we choose our own
    tableView_->setColumnResizeEnabled(true);
    tableView_->setSelectionMode(Wt::NoSelection);
    tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    tableView_->setColumnWidth(MessageModel::SequenceColumn, Wt::WLength(3, Wt::WLength::FontEm));
    tableView_->setColumnWidth(MessageModel::ArrivalColumn, Wt::WLength(6, Wt::WLength::FontEm));
    tableView_->setColumnWidth(MessageModel::FacilityColumn, Wt::WLength(6, Wt::WLength::FontEm));
    tableView_->setColumnWidth(MessageModel::ImportanceColumn, Wt::WLength(4, Wt::WLength::FontEm));
    tableView_->setColumnWidth(MessageModel::MessageColumn, Wt::WLength(50, Wt::WLength::FontEm));
    hbox->addWidget(tableView_);

    // Wire up ROSE diagnostics to the MessageModel
    MessageTransfer::Ptr xfer = MessageTransfer::instance(ctx_.application, model_, this);
    Sawyer::Message::MultiplexerPtr mplex = rose::Diagnostics::destination.dynamicCast<Sawyer::Message::Multiplexer>();
    ASSERT_not_null(mplex);
    mplex->addDestination(xfer);
}

void
WStatus::handleMessageArrival(const MessageModel::Message &mesg) {
    std::string s = "#" + StringUtility::numberToString(mesg.sequence) + " " + mesg.arrivalTime + ": " +
                    mesg.facility + "[" + stringifyImportance(mesg.importance) + "]: " + mesg.message;
    messageArrived_.emit(s);
}

// Do not call this from handleMessage: the partitioner doesn't expect us to be messing with it while it's deep inside printing
// a message.
void
WStatus::redraw() {
    using namespace StringUtility;

    // Process info
    struct rusage ru;
    if (0==getrusage(RUSAGE_SELF, &ru)) {
        wProcessMemory_->setText(numberToString(ru.ru_maxrss/1024.0));
        wProcessTime_->setText(numberToString(ru.ru_utime.tv_sec + ru.ru_utime.tv_usec*1e-6));
    } else {
        wProcessMemory_->setText("N/A");
        wProcessTime_->setText("N/A");
    }

    // Partitioner info
    if (!ctx_.partitioner.isDefaultConstructed()) {
        wPartitionerInsns_->setText(numberToString(ctx_.partitioner.instructionProvider().nCached()));
        wPartitionerBBlocks_->setText(numberToString(ctx_.partitioner.nPlaceholders()));
        wPartitionerFunctions_->setText(numberToString(ctx_.partitioner.nFunctions()));
        wPartitionerBytes_->setText(numberToString(ctx_.partitioner.aum().size()));
        wPartitionerHullSize_->setText(numberToString(ctx_.partitioner.aum().hull().size()));
    } else {
        wPartitionerInsns_->setText("0");
        wPartitionerBBlocks_->setText("0");
        wPartitionerFunctions_->setText("0");
        wPartitionerBytes_->setText("0");
        wPartitionerHullSize_->setText("0");
    }

    // Semantics info
    {
        Sawyer::SynchronizedPoolAllocator &exprPool = BinaryAnalysis::SymbolicExpr::TreeNode::poolAllocator();
        std::pair<size_t, size_t> exprAllocs = exprPool.nAllocated();
        wSemanticsAllocated_->setText(numberToString(exprAllocs.first));
        wSemanticsReserved_->setText(numberToString(exprAllocs.second));
    }
    
    // ROSE info
    if (SgProject *project = SageInterface::getProject()) {
        struct T1: AstSimpleProcessing {
            size_t nNodes;
            T1(): nNodes(0) {}
            void visit(SgNode*) { ++nNodes; }
        } t1;
        t1.traverse(project, preorder);
        wRoseProject_->setText(numberToString(t1.nNodes));
    } else {
        wRoseProject_->setText("0");
    }
    {
        VariantVector allVariants = V_SgNode;
        wRoseTotal_->setText(numberToString(NodeQuery::queryMemoryPool(allVariants).size()));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      WStatusBar (the little status)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
WStatusBar::init() {
    wLatestMessage_ = new Wt::WText(this);
    wLatestMessage_->decorationStyle().setBackgroundColor(toWt(Color::HSV(0, 0, 0.9)));
}

void
WStatusBar::appendMessage(const std::string &mesg) {
    wLatestMessage_->setText(mesg);
}

} // namespace

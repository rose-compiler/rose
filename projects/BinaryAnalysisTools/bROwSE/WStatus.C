#include <bROwSE/WStatus.h>

#include <boost/algorithm/string/trim.hpp>
#include <Color.h>                                      // ROSE
#include <Diagnostics.h>                                // ROSE
#include <sawyer/SharedPointer.h>
#include <sys/resource.h>
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WTableView>
#include <Wt/WText>
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
    MessageModel *model_;
    WStatus *wStatus_;
    size_t sequence_;

protected:
    MessageTransfer(MessageModel *model, WStatus *wStatus)
        : model_(model), wStatus_(wStatus), sequence_(0) {
        ASSERT_not_null(model);
        ASSERT_not_null(wStatus);
    }

public:
    typedef Sawyer::SharedPointer<MessageTransfer> Ptr;

    static Ptr instance(MessageModel *model, WStatus *wStatus) {
        return Ptr(new MessageTransfer(model, wStatus));
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
                model_->insertMessage(m);
                wStatus_->handleMessageArrival(m);
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

void
WStatus::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    wPartitionerStats_ = new Wt::WText("Partitioner stats:", Wt::PlainText);
    vbox->addWidget(wPartitionerStats_);

    wProcessStats_ = new Wt::WText("Process stats:", Wt::PlainText);
    vbox->addWidget(wProcessStats_);

    wRoseStats_ = new Wt::WText("ROSE stats:", Wt::PlainText);
    vbox->addWidget(wRoseStats_);

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
    MessageTransfer::Ptr xfer = MessageTransfer::instance(model_, this);
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
    if (!ctx_.partitioner.isDefaultConstructed()) {
        std::ostringstream ss;
        ss <<"Partitioner: insns=" <<ctx_.partitioner.instructionProvider().nCached()
           <<"; blocks=" <<ctx_.partitioner.nPlaceholders()
           <<"; functions=" <<ctx_.partitioner.nFunctions();
        wPartitionerStats_->setText(ss.str());
        wPartitionerStats_->show();
    } else {
        wPartitionerStats_->hide();
    }

    struct rusage ru;
    if (0==getrusage(RUSAGE_SELF, &ru)) {
        std::ostringstream ss;
        ss <<"Process: mem=" <<(ru.ru_maxrss/1024.0) <<" MB";
        ss <<"; time=" <<(ru.ru_utime.tv_sec + ru.ru_utime.tv_usec*1e-6) <<" seconds";
        wProcessStats_->setText(ss.str());
        wProcessStats_->show();
    } else {
        wProcessStats_->hide();
    }

    // FIXME[Robb P. Matzke 2014-12-30]: how do we count parts of the AST that aren't linked to the project yet?
    if (SgProject *project = SageInterface::getProject()) {
        struct T1: AstSimpleProcessing {
            size_t nNodes;
            T1(): nNodes(0) {}
            void visit(SgNode*) { ++nNodes; }
        } t1;
        t1.traverse(project, preorder);
        std::ostringstream ss;
        ss <<"ROSE: project=" <<StringUtility::plural(t1.nNodes, "nodes");
        wRoseStats_->setText(ss.str());
        wRoseStats_->show();
    } else {
        wRoseStats_->hide();
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

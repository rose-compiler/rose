#include <rose.h>
#include <bROwSE/WMemoryMap.h>

#include <boost/algorithm/string/trim.hpp>
#include <bROwSE/FunctionUtil.h>
#include <bROwSE/WHexValueEdit.h>
#include <rose_strtoull.h>
#include <SRecord.h>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WButtonGroup>
#include <Wt/WCheckBox>
#include <Wt/WImage>
#include <Wt/WInPlaceEdit>
#include <Wt/WLineEdit>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WText>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

namespace bROwSE {

void
WMemoryMap::init() {

    new Wt::WText("The memory map represents the addresses that are mapped as part of the specimen. ROSE's partitioner "
                  "will assume that values stored at non-writable addresses are constant for the life of the specimen, "
                  "and that all writable data could change.  Instructions must be executable, but ROSE will assume that "
                  "they don't change even if an instruction's segment is writable (it is relatively common for old "
                  "specimens to have writable .text sections even though they have no self-modifying code). If you "
                  "haven't yet started the disassembly process, the memory layout is editable.  For instance, one "
                  "might want to adjust dynamic linking tables to be read-only if linking has already been performed.", this);
    new Wt::WBreak(this);
    new Wt::WBreak(this);

    wTable_ = new Wt::WTable(this);

    // Table headers
    wTable_->setHeaderCount(1);
    wTable_->elementAt(0, IdentColumn      )->addWidget(new Wt::WText("ID"));
    wTable_->elementAt(0, LeastVaColumn    )->addWidget(new Wt::WText("Low"));
    wTable_->elementAt(0, GreatestVaColumn )->addWidget(new Wt::WText("High"));
    wTable_->elementAt(0, SizeColumn       )->addWidget(new Wt::WText("Size"));
    wTable_->elementAt(0, ReadableColumn   )->addWidget(new Wt::WText("Read"));
    wTable_->elementAt(0, WritableColumn   )->addWidget(new Wt::WText("Write"));
    wTable_->elementAt(0, ExecutableColumn )->addWidget(new Wt::WText("Exec"));
    wTable_->elementAt(0, NameColumn       )->addWidget(new Wt::WText("Name"));

    // Table column widths
    wTable_->columnAt(IdentColumn     )->setWidth(Wt::WLength(2,  Wt::WLength::FontEm));
    wTable_->columnAt(LeastVaColumn   )->setWidth(Wt::WLength(8,  Wt::WLength::FontEm));
    wTable_->columnAt(GreatestVaColumn)->setWidth(Wt::WLength(8,  Wt::WLength::FontEm));
    wTable_->columnAt(SizeColumn      )->setWidth(Wt::WLength(8,  Wt::WLength::FontEm));
    wTable_->columnAt(ReadableColumn  )->setWidth(Wt::WLength(2,  Wt::WLength::FontEm));
    wTable_->columnAt(WritableColumn  )->setWidth(Wt::WLength(2,  Wt::WLength::FontEm));
    wTable_->columnAt(ExecutableColumn)->setWidth(Wt::WLength(2,  Wt::WLength::FontEm));
    wTable_->columnAt(NameColumn      )->setWidth(Wt::WLength(30, Wt::WLength::FontEm));

    synchronize();

    // Download raw data
    wDownloadPanel_ = new Wt::WPanel(this);
    wDownloadPanel_->setTitle("Download mapped data");
    wDownloadPanel_->setHidden(!allowDownloads_);
    {
        Wt::WContainerWidget *container = new Wt::WContainerWidget;
        wDownloadPanel_->setCentralWidget(container);

        new Wt::WText("Download from ", container);
        wDownloadFrom_ = new Wt::WLineEdit(container);
        wDownloadFrom_->enterPressed().connect(boost::bind(&WMemoryMap::prepareDownload, this));
        wDownloadFrom_->setToolTip("Starting address as decimal, octal, or hexadecimal.");

        new Wt::WText(" to ", container);
        wDownloadTo_ = new Wt::WLineEdit(container);
        wDownloadTo_->enterPressed().connect(boost::bind(&WMemoryMap::prepareDownload, this));
        wDownloadTo_->setToolTip("Ending address, inclusive; or a plus sign followed by a size.  The value can be "
                                 "decimal, octal, or hexadecimal.");

        new Wt::WBreak(container);
        new Wt::WText("Format as ", container);
        wDownloadFormat_ = new Wt::WButtonGroup(container);
        Wt::WRadioButton *button;
        ASSERT_require(BinaryFormat==0);
        ASSERT_require(SRecordFormat==1);
        wDownloadFormat_->addButton((button=new Wt::WRadioButton("binary", container)));
        button->setToolTip("Download as a raw binary file. This only works for selections where all memory addresses are mapped");
        wDownloadFormat_->addButton((button=new Wt::WRadioButton("S-Records", container)));
        button->setToolTip("Download as Motorola S-Records, an ASCII format that supports downloading discontiguous "
                           "memory regions.");
        wDownloadFormat_->setSelectedButtonIndex(BinaryFormat);

        new Wt::WText("&nbsp;&nbsp;&nbsp;", container);
        wDownloadPrepare_ = new Wt::WPushButton("Prepare", container);
        wDownloadPrepare_->clicked().connect(boost::bind(&WMemoryMap::prepareDownload, this));
        wDownloadPrepare_->setToolTip("Prepare the download on the server.");

        new Wt::WBreak(container);
        wDownload_ = new Wt::WAnchor("", "Download", container);
        wDownload_->setTarget(Wt::TargetNewWindow);
        wDownload_->hide();

        wDownloadMessage_ = new Wt::WText(container);
    }
}

void
WMemoryMap::isEditable(bool b) {
    if (b != isEditable_) {
        isEditable_ = b;
        if (!rowGroups_.empty()) {
            // Make sure widgets are enabled or disabled. All edit rows are hidden (either by this call or because we were
            // previously not editable) therefore the RowGroup passed to synchronizeEdits is irrelevant.
            synchronizeEdits(rowGroups_.front(), ZeroColumn);
        }
    }
}

void
WMemoryMap::allowDownloads(bool b) {
    allowDownloads_ = b;
    wDownloadPanel_->setHidden(!b);
}

void
WMemoryMap::memoryMap(const MemoryMap::Ptr &map) {
    memoryMap_ = map;
    synchronize();
    mapChanged_.emit();
}

// Synchronize the RowGroup list and the WTable rows with the MemoryMap
void
WMemoryMap::synchronize() {
    ASSERT_not_null(memoryMap_);
    memoryMap_->checkConsistency();
    MemoryMap::NodeIterator mmNode = memoryMap_->nodes().begin();
    RowGroups::iterator rg = rowGroups_.begin();
    size_t rgIdx = 0;

    for (/*void*/; mmNode != memoryMap_->nodes().end(); ++mmNode, ++rg, ++rgIdx) {
        size_t tableIdx = rowGroupTableIndex(rgIdx);
        if (rg == rowGroups_.end()) {
            // Append a RowGroup and the corresponding table rows
            rg = rowGroups_.insert(rg, RowGroup(mmNode->key().least()));
            instantiateTableWidgets(*rg, tableIdx);
        } else {
            // Make sure the existing RowGroup is linked to the correct MemoryMap node.  Since MemoryMap::NodeIterator is not
            // stable over insert and erase, we need to store a lookup key rather than an iterator.
            ASSERT_require2(rowGroupTableIndex(rgIdx+1)-1 < (size_t)wTable_->rowCount(), "table rows must already exist");
            rg->segmentVa = mmNode->key().least();
        }

        // Make sure the table is showing correct data.
        rg->editingColumn = ZeroColumn;
        rg->wId->setText(StringUtility::numberToString(rgIdx+1));
        updateRowGroupWidgets(*rg, mmNode);
    }

    // Remove extra RowGroup entries and their corresponding WTable rows.
    rowGroups_.erase(rg, rowGroups_.end());
    size_t tableIdx = rowGroupTableIndex(rgIdx);
    while ((size_t)wTable_->rowCount() > tableIdx)
        wTable_->deleteRow(wTable_->rowCount()-1);
}

// Synchronize edit rows of the table with the RowGroups
void
WMemoryMap::synchronizeEdits(RowGroup &rg, ColumnNumber toEdit) {
    ASSERT_not_null(memoryMap_);
    rg.editingColumn = toEdit;
    MemoryMap::NodeIterator mmNode = memoryMap_->nodes().begin();
    RowGroups::iterator rgIter = rowGroups_.begin();
    for (/*void*/; mmNode != memoryMap_->nodes().end(); ++mmNode, ++rgIter) {
        ASSERT_require(rgIter != rowGroups_.end());
        ASSERT_require(mmNode->key().least() == rgIter->segmentVa);
        if (rgIter->wId != rg.wId || !isEditable_)
            rgIter->editingColumn = ZeroColumn;
        updateRowGroupEditWidgets(*rgIter, mmNode);
    }
}

// Convert a RowGroup index to a WTable row index
size_t
WMemoryMap::rowGroupTableIndex(size_t rowGroupIdx) const {
    return NHeaderRows + rowGroupIdx * NRowsPerSegment;
}

// Instantiate table widgets for a RowGroup. This doesn't give a value to any of the widgets, it only creates them.
void
WMemoryMap::instantiateTableWidgets(RowGroup &rg, size_t tableIdx) {
    Wt::WCssDecorationStyle addressDecor;
    addressDecor.setFont(Wt::WFont(Wt::WFont::Monospace));

    // This widget typically serves as a key for looking up the RowGroup corresponding to a WTable row.
    rg.wId = new Wt::WText;
    wTable_->elementAt(tableIdx+DataRow, IdentColumn)->addWidget(rg.wId);

    //------------- 
    // Table data
    //------------- 

    rg.wDelete = new Wt::WImage("/images/delete-24x24.png");
    rg.wDelete->setToolTip("Delete this segment.");
    rg.wDelete->resize(Wt::WLength(1, Wt::WLength::FontEm), Wt::WLength(1, Wt::WLength::FontEm));
    rg.wDelete->clicked().connect(boost::bind(&WMemoryMap::startDeleteSegment, this, rg.wId));
    wTable_->elementAt(tableIdx+DataRow, DeleteColumn)->addWidget(rg.wDelete);

    rg.wMove = new Wt::WImage("/images/move-24x24.png");
    rg.wMove->setToolTip("Move this segment to a new address.");
    rg.wMove->resize(Wt::WLength(1, Wt::WLength::FontEm), Wt::WLength(1, Wt::WLength::FontEm));
    rg.wMove->clicked().connect(boost::bind(&WMemoryMap::startMoveSegment, this, rg.wId));
    wTable_->elementAt(tableIdx+DataRow, MoveColumn)->addWidget(rg.wMove);

    rg.wSplit = new Wt::WImage("/images/separate-24x24.png");
    rg.wSplit->setToolTip("Split this segment into two segments.");
    rg.wSplit->resize(Wt::WLength(1, Wt::WLength::FontEm), Wt::WLength(1, Wt::WLength::FontEm));
    rg.wSplit->clicked().connect(boost::bind(&WMemoryMap::startSplitSegment, this, rg.wId));
    wTable_->elementAt(tableIdx+DataRow, SplitColumn)->addWidget(rg.wSplit);
        
    rg.wMerge = new Wt::WImage("/images/join-24x24.png");
    rg.wMerge->setToolTip("Merge this segment with the following one.");
    rg.wMerge->resize(Wt::WLength(1, Wt::WLength::FontEm), Wt::WLength(1, Wt::WLength::FontEm));
    rg.wMerge->clicked().connect(boost::bind(&WMemoryMap::startMergeSegments, this, rg.wId));
    wTable_->elementAt(tableIdx+DataRow, MergeColumn)->addWidget(rg.wMerge);
        
    rg.wLeastVa = new Wt::WText(StringUtility::addrToString(0));
    rg.wLeastVa->setDecorationStyle(addressDecor);
    rg.wLeastVa->clicked().connect(boost::bind(&WMemoryMap::startHexValueEdit, this, rg.wId, LeastVaColumn));
    wTable_->elementAt(tableIdx+DataRow, LeastVaColumn)->addWidget(rg.wLeastVa);

    rg.wGreatestVa = new Wt::WText(StringUtility::addrToString(0));
    rg.wGreatestVa->setDecorationStyle(addressDecor);
    rg.wGreatestVa->clicked().connect(boost::bind(&WMemoryMap::startHexValueEdit, this, rg.wId, GreatestVaColumn));
    wTable_->elementAt(tableIdx+DataRow, GreatestVaColumn)->addWidget(rg.wGreatestVa);

    rg.wSize = new Wt::WText(StringUtility::addrToString(0));
    rg.wSize->setDecorationStyle(addressDecor);
    rg.wSize->clicked().connect(boost::bind(&WMemoryMap::startHexValueEdit, this, rg.wId, SizeColumn));
    wTable_->elementAt(tableIdx+DataRow, SizeColumn)->addWidget(rg.wSize);

    rg.wReadable = new Wt::WCheckBox;
    rg.wReadable->clicked().connect(boost::bind(&WMemoryMap::toggleAccess, this, rg.wId, rg.wReadable, MemoryMap::READABLE));
    wTable_->elementAt(tableIdx+DataRow, ReadableColumn)->addWidget(rg.wReadable);

    rg.wWritable = new Wt::WCheckBox;
    rg.wWritable->clicked().connect(boost::bind(&WMemoryMap::toggleAccess, this, rg.wId, rg.wWritable, MemoryMap::WRITABLE));
    wTable_->elementAt(tableIdx+DataRow, WritableColumn)->addWidget(rg.wWritable);

    rg.wExecutable = new Wt::WCheckBox;
    rg.wExecutable->clicked().connect(boost::bind(&WMemoryMap::toggleAccess, this,
                                                  rg.wId, rg.wExecutable, MemoryMap::EXECUTABLE));
    wTable_->elementAt(tableIdx+DataRow, ExecutableColumn)->addWidget(rg.wExecutable);

    rg.wName = new Wt::WInPlaceEdit;
    rg.wName->setPlaceholderText("(no name)");
    wTable_->elementAt(tableIdx+DataRow, NameColumn)->addWidget(rg.wName);

    //--------------- 
    // Table editing
    //--------------- 
    rg.wEditStack = new Wt::WStackedWidget;

    // editing a hex value
    rg.wHexValueEdit = new WHexValueEdit;
    rg.wHexValueEdit->canceled().connect(boost::bind(&WMemoryMap::cancelEdit, this, rg.wId));
    rg.wHexValueEdit->saved().connect(boost::bind(&WMemoryMap::finishHexValueEdit, this, rg.wId));
    rg.wEditStack->addWidget(rg.wHexValueEdit);

    // deleting a segment
    rg.wDeleteConfirm = new Wt::WContainerWidget;
    new Wt::WText("Delete segment? ", rg.wDeleteConfirm);
    Wt::WPushButton *wYes = new Wt::WPushButton("Delete", rg.wDeleteConfirm);
    wYes->clicked().connect(boost::bind(&WMemoryMap::finishDeleteSegment, this, rg.wId));
    Wt::WPushButton *wNo = new Wt::WPushButton("Cancel", rg.wDeleteConfirm);
    wNo->clicked().connect(boost::bind(&WMemoryMap::cancelEdit, this, rg.wId));
    rg.wEditStack->addWidget(rg.wDeleteConfirm);

    // moving a segment
    rg.wMoveSegment = new WHexValueEdit;
    rg.wMoveSegment->canceled().connect(boost::bind(&WMemoryMap::cancelEdit, this, rg.wId));
    rg.wMoveSegment->saved().connect(boost::bind(&WMemoryMap::finishMoveSegment, this, rg.wId));
    rg.wMoveSegment->changed().connect(boost::bind(&WMemoryMap::checkMoveSegment, this, rg.wId, _1));
    rg.wEditStack->addWidget(rg.wMoveSegment);

    // merging two segments
    rg.wMergeConfirm = new Wt::WContainerWidget;
    new Wt::WText("Merge segment with following segment? ", rg.wMergeConfirm);
    wYes = new Wt::WPushButton("Merge", rg.wMergeConfirm);
    wYes->clicked().connect(boost::bind(&WMemoryMap::finishMergeSegments, this, rg.wId));
    wNo = new Wt::WPushButton("Cancel", rg.wMergeConfirm);
    wNo->clicked().connect(boost::bind(&WMemoryMap::cancelEdit, this, rg.wId));
    rg.wEditStack->addWidget(rg.wMergeConfirm);

    wTable_->elementAt(tableIdx+EditRow, EditColumn)->addWidget(rg.wEditStack);
    wTable_->elementAt(tableIdx+EditRow, EditColumn)->setColumnSpan(NColumns-EditColumn);
}

void
WMemoryMap::updateRowGroupWidgets(RowGroup &rg, MemoryMap::NodeIterator mmNode) {
    updateRowGroupDataWidgets(rg, mmNode);
    updateRowGroupEditWidgets(rg, mmNode);
}

void
WMemoryMap::updateRowGroupDataWidgets(RowGroup &rg, MemoryMap::NodeIterator mmNode) {
    ASSERT_not_null(memoryMap_);
    ASSERT_require(mmNode != memoryMap_->nodes().end());
    const AddressInterval &interval = mmNode->key();
    ASSERT_forbid(interval.isEmpty());
    ASSERT_require(rg.segmentVa == interval.least());

    rg.wSplit->setHidden(!isEditable_ || rg.editingColumn==SplitColumn || !canSplit(rg, mmNode));
    rg.wMerge->setHidden(!isEditable_ || rg.editingColumn==MergeColumn || !canMerge(rg, mmNode));

    rg.wLeastVa->setText(StringUtility::addrToString(interval.least()));
    rg.wGreatestVa->setText(StringUtility::addrToString(interval.greatest()));

    if (interval.isWhole()) {
        rg.wSize->setText("whole");                    // since size would overflow back to zero
    } else {
        rg.wSize->setText(StringUtility::addrToString(interval.size()));
    }

    const MemoryMap::Segment &segment = mmNode->value();
    rg.wReadable->setChecked(0 != (segment.accessibility() & MemoryMap::READABLE));
    rg.wWritable->setChecked(0 != (segment.accessibility() & MemoryMap::WRITABLE));
    rg.wExecutable->setChecked(0 != (segment.accessibility() & MemoryMap::EXECUTABLE));

    rg.wName->setText(StringUtility::cEscape(segment.name()));
}

void
WMemoryMap::updateRowGroupEditWidgets(RowGroup &rg, MemoryMap::NodeIterator mmNode) {
    if (isEditable_) {
        rg.wDelete->setHidden(rg.editingColumn==DeleteColumn);
        rg.wMove->setHidden(rg.editingColumn==MoveColumn);
        rg.wSplit->setHidden(rg.editingColumn==SplitColumn || !canSplit(rg, mmNode));
        rg.wMerge->setHidden(rg.editingColumn==MergeColumn || !canMerge(rg, mmNode));
        rg.wReadable->setEnabled(true);
        rg.wWritable->setEnabled(true);
        rg.wExecutable->setEnabled(true);

        switch (rg.editingColumn) {
            case ZeroColumn:
                rg.wEditStack->hide();
                break;
            case DeleteColumn:
                rg.wEditStack->setCurrentWidget(rg.wDeleteConfirm);
                rg.wEditStack->show();
                break;
            case MoveColumn:
                rg.wEditStack->setCurrentWidget(rg.wMoveSegment);
                rg.wEditStack->show();
                break;
            case MergeColumn:
                rg.wEditStack->setCurrentWidget(rg.wMergeConfirm);
                rg.wEditStack->show();
                break;
            case SplitColumn:
            case LeastVaColumn:
            case GreatestVaColumn:
            case SizeColumn:
                rg.wEditStack->setCurrentWidget(rg.wHexValueEdit);
                rg.wEditStack->show();
                break;
            default:
                ASSERT_not_reachable("don't know how to edit column " + StringUtility::numberToString(rg.editingColumn));
        }
    } else {
        rg.wDelete->hide();
        rg.wMove->hide();
        rg.wSplit->hide();
        rg.wMerge->hide();
        rg.wReadable->setEnabled(false);
        rg.wWritable->setEnabled(false);
        rg.wExecutable->setEnabled(false);
    }
}

bool
WMemoryMap::canSplit(const RowGroup &rg, MemoryMap::NodeIterator mmNode) {
    ASSERT_not_null(memoryMap_);
    ASSERT_require(mmNode != memoryMap_->nodes().end());
    return !mmNode->key().isEmpty() && mmNode->key().size() > 1;
}

bool
WMemoryMap::canMerge(const RowGroup &rg, MemoryMap::NodeIterator mmNode) {
    ASSERT_not_null(memoryMap_);
    ASSERT_require(mmNode != memoryMap_->nodes().end());
    MemoryMap::NodeIterator mmNext = mmNode; ++mmNext;
    return mmNext!=memoryMap_->nodes().end() && mmNode->key().greatest()+1 == mmNext->key().least();
}

MemoryMap::NodeIterator
WMemoryMap::findMapNode(const RowGroup &rg) {
    ASSERT_not_null(memoryMap_);
    MemoryMap::NodeIterator mmNode = memoryMap_->at(rg.segmentVa).findNode();
    ASSERT_require(mmNode != memoryMap_->nodes().end());
    ASSERT_require(mmNode->key().least() == rg.segmentVa);
    return mmNode;
}

WMemoryMap::RowGroup&
WMemoryMap::rowGroup(Wt::WText *wId) {
    BOOST_FOREACH (RowGroup &rg, rowGroups_) {
        if (rg.wId == wId)
            return rg;
    }
    ASSERT_not_reachable("RowGroup not found");
}

WMemoryMap::RowGroup&
WMemoryMap::rowGroup(rose_addr_t segmentVa) {
    BOOST_FOREACH (RowGroup &rg, rowGroups_) {
        if (rg.segmentVa == segmentVa)
            return rg;
    }
    ASSERT_not_reachable("RowGroup not found");
}

void
WMemoryMap::startDeleteSegment(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    if (rg.editingColumn != DeleteColumn)
        synchronizeEdits(rowGroup(wId), DeleteColumn);
}

void
WMemoryMap::finishDeleteSegment(Wt::WText *wId) {
    ASSERT_not_null(memoryMap_);
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    ASSERT_require(rg.editingColumn == DeleteColumn);
    memoryMap_->erase(findMapNode(rg)->key());
    synchronize();
    mapChanged_.emit();
}

void
WMemoryMap::startMoveSegment(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    if (rg.editingColumn != MoveColumn) {
        MemoryMap::NodeIterator mmNode = findMapNode(rg);
        rose_addr_t hi = AddressInterval::whole().greatest() - mmNode->key().size() + 1;
        rg.wMoveSegment->value(rg.segmentVa, AddressInterval::hull(0, hi));
        rg.wMoveSegment->label("Move to");
        synchronizeEdits(rg, MoveColumn);
    }
}

void
WMemoryMap::checkMoveSegment(Wt::WText *wId, rose_addr_t destinationVa) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    MemoryMap::NodeIterator mmNode = findMapNode(rg);
    if (destinationVa != mmNode->key().least()) {
        AddressInterval newInterval = AddressInterval::baseSize(destinationVa, mmNode->key().size());
        AddressIntervalSet usedAddresses(*memoryMap_);
        usedAddresses.erase(mmNode->key());
        if (usedAddresses.isOverlapping(newInterval))
            rg.wMoveSegment->setError("Destination would collide with another segment.");
    }
}

void
WMemoryMap::finishMoveSegment(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    ASSERT_require(rg.editingColumn == MoveColumn);
    MemoryMap::NodeIterator mmNode = findMapNode(rg);
    AddressInterval newInterval = AddressInterval::baseSize(rg.wMoveSegment->value(), mmNode->key().size());
    if (newInterval != mmNode->key()) {
        AddressIntervalSet usedAddresses(*memoryMap_);
        usedAddresses.erase(mmNode->key());
        if (usedAddresses.isOverlapping(newInterval)) {
            // Just comment this check out if you want to allow this.  You'll also probably need to comment out the same check
            // in the checkMoveSegment method. [Robb P. Matzke 2014-12-26]
            std::cerr <<"  Move fails because it overlaps with something else.\n";
        } else {
            MemoryMap::Segment newSegment = mmNode->value();
            memoryMap_->erase(mmNode->key());
            memoryMap_->insert(newInterval, newSegment);
            mapChanged_.emit();
        }
    }
    synchronize();
}

void
WMemoryMap::startSplitSegment(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    if (rg.editingColumn != SplitColumn) {
        MemoryMap::NodeIterator mmNode = findMapNode(rg);
        ASSERT_require(canSplit(rg, mmNode));
        AddressInterval limits = AddressInterval::hull(mmNode->key().least()+1, mmNode->key().greatest());
        rg.wHexValueEdit->value(limits.least(), limits);
        rg.wHexValueEdit->label("Split point: ");
        synchronizeEdits(rg, SplitColumn);
    }
}

void
WMemoryMap::startMergeSegments(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    if (rg.editingColumn != MergeColumn)
        synchronizeEdits(rg, MergeColumn);
}

void
WMemoryMap::finishMergeSegments(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rgCurrent = rowGroup(wId);
    MemoryMap::NodeIterator mmCurrent = findMapNode(rgCurrent);
    const RowGroup rgNext = rowGroup(mmCurrent->key().greatest() + 1);// copied so it doesn't dangle after deleteSegment
    MemoryMap::NodeIterator mmNext = findMapNode(rgNext);

    AddressInterval newInterval = AddressInterval::hull(mmCurrent->key().least(), mmNext->key().greatest());
    MemoryMap::Segment newSegment = mmCurrent->value();

    // If the following node and this node's buffers cannot be merged then create a new buffer.
    if (mmCurrent->value().buffer() != mmNext->value().buffer() ||
        mmCurrent->value().offset() + mmCurrent->key().size() != mmNext->value().offset()) {
        MemoryMap::Buffer::Ptr newBuffer = MemoryMap::AllocatingBuffer::instance(newInterval.size());

        const uint8_t *src = mmCurrent->value().buffer()->data();
        ASSERT_not_null2(src, "fast copying will not work here");
        newBuffer->write(src + mmCurrent->value().offset(), 0, mmCurrent->key().size());

        src = mmNext->value().buffer()->data();
        ASSERT_not_null2(src, "fast copying will not work here");
        newBuffer->write(src + mmNext->value().offset(), mmCurrent->key().size(), mmNext->key().size());

        newSegment.buffer(newBuffer);
        newSegment.offset(0);
    }

    // Insert the new segment over the top of the two we're replacing
    memoryMap_->insert(newInterval, newSegment);
    mapChanged_.emit();
    synchronize();
}

void
WMemoryMap::startHexValueEdit(Wt::WText *wId, ColumnNumber column) {
    if (!isEditable_)
        return;
    ASSERT_forbid(column==ZeroColumn);                  // can't edit column zero
    RowGroup &rg = rowGroup(wId);
    if (rg.editingColumn == column)
        return;                                         // already editing this column

    // Find memory map node, prior node and next node if possible, or end nodes.
    MemoryMap::NodeIterator mmNode = findMapNode(rg);
    MemoryMap::NodeIterator mmPrior = mmNode;
    if (mmPrior != memoryMap_->nodes().begin()) {
        --mmPrior;
    } else {
        mmPrior = memoryMap_->nodes().end();
    }
    MemoryMap::NodeIterator mmNext = mmNode;
    if (mmNext != memoryMap_->nodes().end())
        ++mmNext;

    // Get the initial value and limits for the edit.
    std::string label;
    rose_addr_t value=0, lo=1, hi=0;
    switch (column) {
        case LeastVaColumn: {
            label = "Low address: ";
            value = mmNode->key().least();
            rose_addr_t lo1 = mmPrior==memoryMap_->nodes().end() ? 0 : mmPrior->key().greatest()+1;
            rose_addr_t lo2 = rg.segmentVa - std::min(rg.segmentVa, mmNode->value().offset());
            lo = std::max(lo1, lo2);
            hi = mmNode->key().greatest();
            break;
        }
        case GreatestVaColumn: {
            label = "High address: ";
            value = mmNode->key().greatest();
            lo = mmNode->key().least();
            rose_addr_t avail = mmNode->value().buffer()->available(mmNode->value().offset());
            if (mmNext != memoryMap_->nodes().end())
                avail = std::min(avail, mmNext->key().least() - mmNode->key().least());
            ASSERT_require(avail > 0);
            hi = lo + avail - 1;
            break;
        }
        case SizeColumn: {
            label = "Size: ";
            value = mmNode->key().size();
            lo = 1;
            hi = mmNode->value().buffer()->available(mmNode->value().offset());
            if (mmNext != memoryMap_->nodes().end())
                hi = std::min(hi, mmNext->key().least() - mmNode->key().least());
            ASSERT_require(hi > 0);
            break;
        }
        default:
            ASSERT_not_reachable("not a hexadecimal column");
    }
    ASSERT_require(lo <= hi);
    AddressInterval valueLimits = AddressInterval::hull(lo, hi);
    ASSERT_require(valueLimits.isContaining(value));
    rg.wHexValueEdit->value(value, valueLimits);
    rg.wHexValueEdit->label(label);

    synchronizeEdits(rg, column);
}

void
WMemoryMap::finishHexValueEdit(Wt::WText *wId) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    MemoryMap::NodeIterator mmNode = findMapNode(rg);
    rose_addr_t value = rg.wHexValueEdit->value();
    switch (rg.editingColumn) {
        case LeastVaColumn:
            if (value > rg.segmentVa) {
                AddressInterval toErase = AddressInterval::hull(rg.segmentVa, value-1);
                memoryMap_->erase(toErase);
                mapChanged_.emit();
            } else if (value < rg.segmentVa) {
                MemoryMap::Segment newSegment = mmNode->value();
                rose_addr_t negDelta = rg.segmentVa - value;
                ASSERT_require(mmNode->value().offset() >= negDelta);
                newSegment.offset(mmNode->value().offset() - negDelta);
                AddressInterval newInterval = AddressInterval::hull(value, mmNode->key().greatest());
                memoryMap_->insert(newInterval, newSegment);
                mapChanged_.emit();
            }
            break;
        case GreatestVaColumn:
            if (value > mmNode->key().greatest()) {
                MemoryMap::Segment newSegment = mmNode->value();
                rose_addr_t newSize = value - rg.segmentVa + 1;
                ASSERT_require(newSegment.buffer()->available(newSegment.offset()) >= newSize);
                AddressInterval newInterval = AddressInterval::baseSize(rg.segmentVa, newSize);
                memoryMap_->insert(newInterval, newSegment);
                mapChanged_.emit();
            } else if (value < mmNode->key().greatest()) {
                AddressInterval toErase = AddressInterval::hull(value+1, mmNode->key().greatest());
                memoryMap_->erase(toErase);
                mapChanged_.emit();
            }
            break;
        case SizeColumn:
            if (value > mmNode->key().size()) {
                MemoryMap::Segment newSegment = mmNode->value();
                rose_addr_t newSize = value;
                ASSERT_require(newSegment.buffer()->available(newSegment.offset()) >= newSize);
                AddressInterval newInterval = AddressInterval::baseSize(rg.segmentVa, newSize);
                memoryMap_->insert(newInterval, newSegment);
                mapChanged_.emit();
            } else if (value < mmNode->key().size()) {
                AddressInterval toErase = AddressInterval::baseSize(rg.segmentVa, value);
                memoryMap_->erase(toErase);
                mapChanged_.emit();
            }
            break;
        case SplitColumn: {
            // Split segment [a..c] into [a..b-1] union [b..c]. In order to prevent MemoryMap from re-joining adjacent
            // segments, we need to make them different by causing them to point to two different buffers. (Or perhaps we
            // should just add "(copy N)" to the name?)
            rose_addr_t delta = value - mmNode->key().least();
            MemoryMap::Buffer::Ptr newBuffer = mmNode->value().buffer()->copy();
            MemoryMap::Segment newSegment = mmNode->value();
            newSegment.buffer(newBuffer);
            newSegment.offset(newSegment.offset() + delta);
            AddressInterval newInterval = AddressInterval::hull(value, mmNode->key().greatest());
            memoryMap_->insert(newInterval, newSegment);
            mapChanged_.emit();
            break;
        }
        default:
            ASSERT_not_reachable("not a hex value column");
    }
    synchronize();
}

void
WMemoryMap::toggleAccess(Wt::WText *wId, Wt::WCheckBox *wCheckBox, unsigned accessBits) {
    ASSERT_require(isEditable_);
    RowGroup &rg = rowGroup(wId);
    MemoryMap::NodeIterator mmNode = findMapNode(rg);
    unsigned oldBits = mmNode->value().accessibility();
    unsigned newBits = oldBits;
    if (wCheckBox->checkState() == Wt::Checked) {
        newBits |= accessBits;
    } else {
        newBits &= ~accessBits;
    }
    if (newBits != oldBits) {
        mmNode->value().accessibility(newBits);
        synchronize();
        mapChanged_.emit();
    }
}

void
WMemoryMap::cancelEdit(Wt::WText *wId) {
    synchronizeEdits(rowGroup(wId), ZeroColumn);
}

void
WMemoryMap::prepareDownload() {
    wDownloadMessage_->setText("");
    DownloadFormat fmt = (DownloadFormat)wDownloadFormat_->checkedId();
    wDownload_->hide();

    // Least address
    char *rest;
    std::string s = boost::trim_copy(wDownloadFrom_->text().narrow());
    errno = 0;
    rose_addr_t leastVa = rose_strtoull(s.c_str(), &rest, 0);
    if (*rest || errno) {
        wDownloadMessage_->setText("Invalid starting address.");
        wDownloadMessage_->setStyleClass("text-error");
        wDownloadMessage_->show();
        return;
    }

    // Greatest address
    s = boost::trim_copy(wDownloadTo_->text().narrow());
    bool isSize = !s.empty() && s[0]=='+';
    if (isSize)
        s = boost::trim_copy(s.substr(1));
    errno = 0;
    rose_addr_t greatestVa = rose_strtoull(s.c_str(), &rest, 0);
    if (*rest || errno) {
        wDownloadMessage_->setText("Invalid " + std::string(isSize?"size":"ending") + " value.");
        wDownloadMessage_->setStyleClass("text-error");
        return;
    }
    if (isSize) {
        if (0==greatestVa)
            return;                                     // nothing to download (greatestVa is the size)
        greatestVa += leastVa - 1;                      // convert from size to greatest address
    }

    // Sanity checks
    if (leastVa > greatestVa) {
        wDownloadMessage_->setText("Invalid address range (first > last).");
        wDownloadMessage_->setStyleClass("text-error");
        return;
    }
    AddressInterval addresses = AddressInterval::hull(leastVa, greatestVa);
    if (BinaryFormat==fmt && memoryMap_->at(leastVa).limit(addresses.size()).available().size()<addresses.size()) {
        wDownloadMessage_->setText("Some addresses the the range are not mapped.");
        wDownloadMessage_->setStyleClass("text-error");
        return;
    }

    // Prepare a file to download.
    boost::filesystem::path fileName;
    if (BinaryFormat == fmt) {
        fileName = uniquePath(".bin");
        std::ofstream output(fileName.c_str());
        static uint8_t buf[8192];
        rose_addr_t nRemaining = addresses.size();
        rose_addr_t va = leastVa;
        while (size_t nRead = memoryMap_->at(va).limit(std::min((rose_addr_t)(sizeof buf), nRemaining)).read(buf).size()) {
            output.write((const char*)buf, nRead);
            va += nRead;
            nRemaining -= nRead;
        }
    } else {
        ASSERT_require(SRecordFormat == fmt);
        fileName = uniquePath(".srec");
        std::ofstream output(fileName.c_str());
        MemoryMap::Ptr tmp = memoryMap_->shallowCopy();
        if (leastVa>0)
            tmp->erase(AddressInterval::hull(0, leastVa-1));
        if (greatestVa < tmp->hull().greatest())
            tmp->erase(AddressInterval::hull(greatestVa+1, tmp->hull().greatest()));
        static const Rose::BinaryAnalysis::SRecord::Syntax syntax = Rose::BinaryAnalysis::SRecord::SREC_MOTOROLA;
        std::vector<Rose::BinaryAnalysis::SRecord> srecs = Rose::BinaryAnalysis::SRecord::create(tmp, syntax);
        BOOST_FOREACH (const Rose::BinaryAnalysis::SRecord &srec, srecs)
            output <<srec <<"\n";
    }

    wDownloadMessage_->setText(" for " + StringUtility::addrToString(leastVa) + "-" +
                               StringUtility::addrToString(greatestVa) + " is ready.");
    wDownloadMessage_->setStyleClass("");
    wDownload_->setLink(fileName.string());
    wDownload_->setTarget(Wt::TargetNewWindow);
    wDownload_->show();
}

} // namespace


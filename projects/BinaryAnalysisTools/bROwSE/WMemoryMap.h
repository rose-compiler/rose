#ifndef bROwSE_WMemoryMap_H
#define bROwSE_WMemoryMap_H

#include <bROwSE/bROwSE.h>
#include <Wt/WContainerWidget>

namespace bROwSE {

class WHexValueEdit;

/** Displays and edits a memory map. */
class WMemoryMap: public Wt::WContainerWidget {
public:
    enum ColumnNumber {
        // No editable column should be number zero
        ZeroColumn = 0,
        IdentColumn = 0,                                /**< Column containing identification info. */
        DeleteColumn,                                   /**< Button that deletes a map entry. */
        SplitColumn,                                    /**< Button that splits a segment into two segments. */
        MergeColumn,                                    /**< Merge two adjacent segments into one segment. */
        MoveColumn,                                     /**< Move a memory map to a different address. */
        LeastVaColumn,                                  /**< Column containing the starting address. */
        GreatestVaColumn,                               /**< Column containing the ending address. */
        SizeColumn,                                     /**< Column containing the segment size. */
        ReadableColumn,                                 /**< Column containing read permission bit. */
        WritableColumn,                                 /**< Column containing write permission bit. */
        ExecutableColumn,                               /**< Column containing execute permission bit. */
        NameColumn,                                     /**< Column containing segment name. */
        NColumns,                                       /**< Number of columns. */
        EditColumn = LeastVaColumn
    };

    enum RowNumber {
        DataRow,                                        /**< Row containing segment data w.r.t. segment's first row. */
        EditRow,                                        /**< Row containing edit info w.r.t. segment's first row. */
        NRowsPerSegment,                                /**< Number of table rows occupied by each segment. */
        NHeaderRows = 1                                 /**< Number of header rows. */
    };

private:
    // A group of table rows corresponding to one memory segment
    struct RowGroup {
        rose_addr_t segmentVa;                          // starting address of the segment described herein
        Wt::WText *wId;
        Wt::WImage *wDelete, *wSplit, *wMerge, *wMove;
        Wt::WText *wLeastVa, *wGreatestVa, *wSize;
        Wt::WCheckBox *wReadable, *wWritable, *wExecutable;
        Wt::WInPlaceEdit *wName;
        ColumnNumber editingColumn;                     // non-zero means we are editing (we never edit column zero)
        Wt::WStackedWidget *wEditStack;                 // various editing things, hidden/shown based on editingColumn
        WHexValueEdit *wHexValueEdit;                   // for editing limited-interval hexadecimal columns
        Wt::WContainerWidget *wDeleteConfirm;           // confirmation for deleting a segment
        Wt::WContainerWidget *wMergeConfirm;            // confirmation for merging two segments
        WHexValueEdit *wMoveSegment;                    // where to move this segment

        RowGroup()                                      // only defined for std::vector's sake
            : segmentVa(0), wId(NULL),
              wDelete(NULL), wSplit(NULL), wMerge(NULL), wMove(NULL),
              wLeastVa(NULL), wGreatestVa(NULL), wSize(NULL),
              wReadable(NULL), wWritable(NULL), wExecutable(NULL), wName(NULL),
              editingColumn(ZeroColumn), wEditStack(NULL), wHexValueEdit(NULL), wDeleteConfirm(NULL), wMergeConfirm(NULL),
              wMoveSegment(NULL) {}
        RowGroup(rose_addr_t segmentVa)
            : segmentVa(segmentVa), wId(NULL),
              wDelete(NULL), wSplit(NULL), wMerge(NULL), wMove(NULL),
              wLeastVa(NULL), wGreatestVa(NULL), wSize(NULL),
              wReadable(NULL), wWritable(NULL), wExecutable(NULL), wName(NULL),
              editingColumn(ZeroColumn), wEditStack(NULL), wHexValueEdit(NULL), wDeleteConfirm(NULL), wMergeConfirm(NULL),
              wMoveSegment(NULL) {}
    };

private:
    MemoryMap memoryMap_;                               // the memory map being manipulated
    Wt::WTable *wTable_;
    bool isEditable_;                                   // can entries be edited?
    Wt::Signal<> mapChanged_;                           // emitted when the memory map changes

    // Information about each segment in the table. We use a list rather than a vector because a list's iterators are stable
    // over insertion and we want to be able to take a reference to a RowGroup and not have it change from under us.  The row
    // groups are sorted by segment starting address, the same order they appear in the table.
    typedef std::list<RowGroup> RowGroups;
    RowGroups rowGroups_;

public:
    explicit WMemoryMap(const MemoryMap &map = MemoryMap(), Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), memoryMap_(map), wTable_(NULL), isEditable_(true) {
        init();
    }

    const MemoryMap& memoryMap() const { return memoryMap_; }
    void memoryMap(const MemoryMap&);

    bool isEditable() const { return isEditable_; }
    void isEditable(bool b);

    Wt::Signal<>& mapChanged() { return mapChanged_; }

private:
    void init();

    // Synchronize the WTable so it corresponds to the MemoryMap. This should be called every time the MemoryMap is modified.
    void synchronize();

    // Adjust the edit rows of WTable, setting the specified RowGroup to edit the specified column.
    void synchronizeEdits(RowGroup &rg, ColumnNumber toEdit);

    // Convert RowGroup index to WTable row index.
    size_t rowGroupTableIndex(size_t rowGroupIdx) const;

    // Builds a table row by instantiating all its widgets initialized to default values.
    void instantiateTableWidgets(RowGroup&, size_t tableIdx);

    // Update table row widgets with interval and segment information
    void updateRowGroupWidgets(RowGroup&, MemoryMap::NodeIterator);
    void updateRowGroupDataWidgets(RowGroup&, MemoryMap::NodeIterator);
    void updateRowGroupEditWidgets(RowGroup&, MemoryMap::NodeIterator);

    // Predicates for a RowGroup
    bool canSplit(const RowGroup&, MemoryMap::NodeIterator);
    bool canMerge(const RowGroup&, MemoryMap::NodeIterator);

    // Return memory map node for row group
    MemoryMap::NodeIterator findMapNode(const RowGroup&);

    // Return RowGroup given table ID
    RowGroup& rowGroup(Wt::WText *wId);
    RowGroup& rowGroup(rose_addr_t segmentVa);

    // Ask for confirmation, then delete a segment
    void startDeleteSegment(Wt::WText *wId);
    void finishDeleteSegment(Wt::WText *wId);

    // Start moving a segment to a new location.
    void startMoveSegment(Wt::WText *wId);
    void checkMoveSegment(Wt::WText *wId, rose_addr_t destinationVa);
    void finishMoveSegment(Wt::WText *wId);

    // Start splitting a segment in two
    void startSplitSegment(Wt::WText *wId);

    // Merge two adjacent segments
    void startMergeSegments(Wt::WText *wId);
    void finishMergeSegments(Wt::WText *wId);

    // Start editing one hex value.
    void startHexValueEdit(Wt::WText *wId, ColumnNumber column);
    void finishHexValueEdit(Wt::WText *wId);

    // Update segment accessibility based on state of check box.
    void toggleAccess(Wt::WText *wId, Wt::WCheckBox *wCheckBox, unsigned accessBits);

    // Stop editing hex value.
    void cancelEdit(Wt::WText *wId);
};

} // namespace
#endif

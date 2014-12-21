#include <bROwSE/WAddressSpace.h>
#include <bROwSE/WFunctionList.h>

#include <bROwSE/FunctionListModel.h>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>

using namespace rose;

namespace bROwSE {

void
WFunctionList::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    // Initialize the ATTR_HEAT values so they're not returned as NaN on the first update of wAddressSpace_
    BOOST_FOREACH (const P2::Function::Ptr &function, ctx_.partitioner.functions())
        function->attr<double>(ATTR_HEAT, 0.0);

    // Address space
    wAddressSpace_ = new WAddressSpace(ctx_);
    wAddressSpace_->insertSegmentsAndFunctions();
    wAddressSpace_->topGutterClicked().connect(this, &WFunctionList::selectSegment);
    wAddressSpace_->bottomGutterClicked().connect(this, &WFunctionList::selectFunction);
    vbox->addWidget(wAddressSpace_);

    // Container for the function table, with a horizontal layout so the table scrolls horizontally
    Wt::WContainerWidget *wTableContainer = new Wt::WContainerWidget;
    vbox->addWidget(wTableContainer, 1 /*stretch*/);
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout;
    wTableContainer->setLayout(hbox);

    // Function table model. The order that analyzers are registered is the order that their columns will appear in the table.
    model_ = new FunctionListModel(ctx_);
    model_->analyzers().push_back(FunctionEntryAddress::instance()); // should normally be first since it's a bit special
    model_->analyzers().push_back(FunctionName::instance());
    model_->analyzers().push_back(FunctionSizeBytes::instance());
    model_->analyzers().push_back(FunctionSizeInsns::instance());
    model_->analyzers().push_back(FunctionSizeBBlocks::instance());
    model_->analyzers().push_back(FunctionSizeDBlocks::instance());
    model_->analyzers().push_back(FunctionImported::instance());
    model_->analyzers().push_back(FunctionExported::instance());
    model_->analyzers().push_back(FunctionNCallers::instance());
    model_->analyzers().push_back(FunctionNReturns::instance());
    model_->analyzers().push_back(FunctionMayReturn::instance());
    model_->analyzers().push_back(FunctionStackDelta::instance());

    // Function table
    tableView_ = new Wt::WTableView;
    tableView_->setModel(model_);
    tableView_->setRowHeaderCount(1); // this must be first property set
    tableView_->setHeaderHeight(28);
    tableView_->setSortingEnabled(true);
    tableView_->setAlternatingRowColors(true);
    for (size_t i=0; i<model_->analyzers().size(); ++i)
        tableView_->setColumnWidth(i, model_->analyzers()[i]->naturalWidth());
    tableView_->setColumnResizeEnabled(true);
    tableView_->setSelectionMode(Wt::SingleSelection);
    tableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    tableView_->clicked().connect(this, &WFunctionList::clickRow);
    tableView_->doubleClicked().connect(this, &WFunctionList::doubleClickRow);
    tableView_->headerClicked().connect(this, &WFunctionList::updateFunctionHeatMaps);
    hbox->addWidget(tableView_);
}

// Selects a specific segment, zooming the address map in or out to select just that segment
void
WFunctionList::selectSegment(rose_addr_t va, const Wt::WMouseEvent&) {
    boost::iterator_range<MemoryMap::ConstNodeIterator> found = ctx_.partitioner.memoryMap().at(va).nodes();
    if (found.begin() != found.end()) {
        const AddressInterval &interval = found.begin()->key();
        wAddressSpace_->displayedDomain(interval);
        wAddressSpace_->redraw();
    }
}

// Select function from clicking on the address space's bottom gutter
// Implemented in terms of selectFunction by pointer
void
WFunctionList::selectFunction(rose_addr_t va, const Wt::WMouseEvent&) {
    std::vector<P2::Function::Ptr> functions = ctx_.partitioner.functionsOverlapping(va);
    if (!functions.empty())
        selectFunction(functions.front());
}

// Select function by specifying a function pointer
// Implemented in terms of selectFunction by table row
Wt::WModelIndex
WFunctionList::selectFunction(const P2::Function::Ptr &function) {
    Wt::WModelIndex idx = model_->functionIdx(function);
    selectFunction(idx);
    return idx;
}

// Select function by specifying a table row
// Lowest level way to select a function
void
WFunctionList::selectFunction(const Wt::WModelIndex &idx) {
    static const size_t functionIdx = 1;
    wAddressSpace_->highlights(functionIdx).clear();

    if (idx.isValid()) {
        // Highlight the function in the table
        tableView_->select(idx);
        tableView_->scrollTo(idx);

        // Highlight the function in the address space map
        if (P2::Function::Ptr function = model_->functionAt(idx.row())) {
            wAddressSpace_->highlights(functionIdx) = ctx_.partitioner.functionExtent(function);
            functionSelected_.emit(function);
        }
    }
    wAddressSpace_->redraw();
}

// Same as selectFunction, but also emit a tableRowClicked event
void
WFunctionList::clickRow(const Wt::WModelIndex &idx) {
    selectFunction(idx);
    if (P2::Function::Ptr function = model_->functionAt(idx.row()))
        tableRowClicked_.emit(function);
}

// Same as selectFunction, but also emit a tableRowDoubleClicked event
void
WFunctionList::doubleClickRow(const Wt::WModelIndex &idx) {
    selectFunction(idx);
    if (P2::Function::Ptr function = model_->functionAt(idx.row()))
        tableRowDoubleClicked_.emit(function);
}

// Redraw the address space map        
void
WFunctionList::updateFunctionHeatMaps() {
    static const size_t functionIdx = 1;
    double minHeat=0, maxHeat=0;
    if (model_->heatStats().count()>0) {
        minHeat = model_->heatStats().minimum();
        maxHeat = model_->heatStats().maximum();
    }

    Color::Gradient g(Color::HSV(2/6.0, 0.3, 0.3), Color::HSV(1.0, 1.0, 0.75));
    wAddressSpace_->gradient(functionIdx).clear();
    if (minHeat == maxHeat) {
        wAddressSpace_->gradient(functionIdx).insert(0, g(0.5));
    } else {
        double p5  = model_->heatStats().p5();
        double p50 = model_->heatStats().p50();
        double p95 = model_->heatStats().p95();
        ASSERT_forbid(isnan(p5) || isnan(p50) || isnan(p95));

        // Inserted in increasing order of importance in case some values overlap
        wAddressSpace_->gradient(functionIdx).insert(p5,      g(0.05));
        wAddressSpace_->gradient(functionIdx).insert(p95,     g(0.95));
        wAddressSpace_->gradient(functionIdx).insert(p50,     g(0.50));
        wAddressSpace_->gradient(functionIdx).insert(minHeat, g(0.00));
        wAddressSpace_->gradient(functionIdx).insert(maxHeat, g(1.00));
    }
    wAddressSpace_->bottomGutterGradient() = wAddressSpace_->gradient(functionIdx);
    wAddressSpace_->insert(ctx_.partitioner, ctx_.partitioner.functions());
    wAddressSpace_->redraw();
}

Wt::Signal<P2::Function::Ptr>&
WFunctionList::functionSelected() {
    return functionSelected_;
}

Wt::Signal<P2::Function::Ptr>&
WFunctionList::tableRowClicked() {
    return tableRowClicked_;
}

Wt::Signal<P2::Function::Ptr>&
WFunctionList::tableRowDoubleClicked() {
    return tableRowDoubleClicked_;
}


} // namespace

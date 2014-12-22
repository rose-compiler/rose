#include <bROwSE/WFunctionCfg.h>

#include <bROwSE/FunctionUtil.h>
#include <bROwSE/WFunctionNavigation.h>
#include <Wt/WEnvironment>
#include <Wt/WScrollArea>
#include <Wt/WVBoxLayout>

namespace bROwSE {

void
WFunctionCfg::init() {
    Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout;
    setLayout(vbox);

    wNavigation_ = new WFunctionNavigation;
    wNavigation_->functionChanged().connect(this, &WFunctionCfg::changeFunction);
    vbox->addWidget(wNavigation_);

    wMessage_ = new Wt::WText("No function.");
    vbox->addWidget(wMessage_);

    wScrollArea_ = new Wt::WScrollArea(this);
    vbox->addWidget(wScrollArea_, 1);
}

void
WFunctionCfg::clearNavigation() {
    wNavigation_->clear();
    if (function_)
        wNavigation_->push(function_);
}

void
WFunctionCfg::changeFunction(const P2::Function::Ptr &function) {
    if (function_ == function)
        return;
    changeFunctionNoSignal(function);
    if (function)
        functionChanged_.emit(function);
}

void
WFunctionCfg::changeFunctionNoSignal(const P2::Function::Ptr &function) {
    if (function_ == function)
        return;
    function_ = function;
    if (function)
        wNavigation_->push(function);

    // We need to create a new wImage each time because we're adding WRectArea.  See limitation documented for the
    // WImage::addArea method in Wt-3.3.3 [Robb P. Matzke 2014-09-10]
    areas_.clear();
#if 0 // [Robb P. Matzke 2014-12-21]: not needed since the next setWidget will do the same
    if (wImage_) {
        wScrollArea_->takeWidget();
        delete wImage_;
    }
#endif
    wImage_ = new Wt::WImage();
    wScrollArea_->setWidget(wImage_);

    // Get the CFG image
    if (NULL==function_) {
        wMessage_->setText("No function.");
        wMessage_->show();
        wImage_->hide();
        return;
    }
    wMessage_->setText("Building CFG...");
    wMessage_->show();
    boost::filesystem::path imagePath = functionCfgImage(ctx_.partitioner, function);
    if (imagePath.empty()) {
        wMessage_->setText("CFG not available.");
        wMessage_->show();
        wImage_->hide();
        return;
    }
    wImage_->setImageLink(Wt::WLink(imagePath.string()));

    // Add sensitive areas to the image.
    try {
        if (!function)
            throw std::runtime_error("No function");
        CfgVertexCoords coords = functionCfgVertexCoords(ctx_.partitioner, function);
        BOOST_FOREACH (const CfgVertexCoords::Node &node, coords.nodes()) {
            rose_addr_t va = node.key();
            Wt::WRectArea *area = new Wt::WRectArea(node.value().x, node.value().y, node.value().dx, node.value().dy);
            P2::Function::Ptr other = ctx_.partitioner.functionExists(va);
            if (other && other!=function) {
                // Clicking on a called function will show that function's CFG
                area->clicked().connect(this, &WFunctionCfg::selectFunction);

                // Tool tip for called function.  The address and name (if known) are already shown as the node label, so the
                // tool tip should contain other useful information.
                std::string toolTip = StringUtility::plural(functionNInsns(ctx_.partitioner, other), "instructions");
                int64_t stackDelta = functionStackDelta(ctx_.partitioner, other);
                if (stackDelta != SgAsmInstruction::INVALID_STACK_DELTA) {
                    toolTip += "; ";                    // semicolon separators since Gecko doesn't honor linefeeds
                    toolTip += (stackDelta>0 ? "+" : "") + boost::lexical_cast<std::string>(stackDelta) + " stack delta";
                }
                area->setToolTip(toolTip);

            } else if (P2::BasicBlock::Ptr bblock = ctx_.partitioner.basicBlockExists(va)) {
                // Clicking on a basic block selects the basic block, whatever that means.
                area->clicked().connect(this, &WFunctionCfg::selectBasicBlock);

                // Tool tips for basic blocks should show important information that isn't already available just by looking at
                // the CFG.
                static const bool nodesHaveInstructions = true;
                static const bool nodesHaveAddresses = false;
                std::string toolTip;
                if (!nodesHaveInstructions) {
                    // List a few of the instructions in the tooltip.  Gecko agents (e.g., firefox) don't like multi-line
                    // tooltips--they rewrap the tip as they see fit--show don't even bother trying to show instructions.
                    if (ctx_.application->environment().agentIsGecko()) {
                        toolTip = StringUtility::plural(bblock->nInstructions(), "instructions");
                    } else {
                        bool exitEarly = bblock->nInstructions()>10;
                        for (size_t i=0; (i<9 || (!exitEarly && i<10)) && i<bblock->nInstructions(); ++i)
                            toolTip += (i?"\n":"") + unparseInstruction(bblock->instructions()[i]);
                        if (bblock->nInstructions()>10)
                            toolTip += "\nand " + StringUtility::numberToString(bblock->nInstructions()-9) + " more...";
                    }
                } else {
                    if (!nodesHaveAddresses) {
                        toolTip = StringUtility::addrToString(bblock->address());
                    }
                }
                if (!toolTip.empty())
                    area->setToolTip(toolTip);
            }
            wImage_->addArea(area);
            areas_.push_back(std::make_pair(area, va));
        }
    } catch (const std::runtime_error &e) {
        wMessage_->setText(e.what());
        wMessage_->show();
        wImage_->hide();
        return;
    }

    wMessage_->hide();
    wImage_->show();
    return;
}
    
void
WFunctionCfg::selectBasicBlock(const Wt::WMouseEvent &event) {
    BOOST_FOREACH (const AreaAddr &pair, areas_) {
        Wt::WRectArea *area = pair.first;
        if (event.widget().x >= area->x() && event.widget().x <= area->x() + area->width() &&
            event.widget().y >= area->y() && event.widget().y <= area->y() + area->height()) {
            P2::BasicBlock::Ptr bblock = ctx_.partitioner.basicBlockExists(pair.second);
            if (bblock)
                basicBlockClicked_.emit(bblock);
            return;
        }
    }
}

void
WFunctionCfg::selectFunction(const Wt::WMouseEvent &event) {
    BOOST_FOREACH (const AreaAddr &pair, areas_) {
        Wt::WRectArea *area = pair.first;
        if (event.widget().x >= area->x() && event.widget().x <= area->x() + area->width() &&
            event.widget().y >= area->y() && event.widget().y <= area->y() + area->height()) {
            P2::Function::Ptr function = ctx_.partitioner.functionExists(pair.second);
            if (function)
                functionClicked_.emit(function);
            return;
        }
    }
}

} // namespace

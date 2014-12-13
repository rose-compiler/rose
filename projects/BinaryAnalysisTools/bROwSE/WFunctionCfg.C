#include <bROwSE/WFunctionCfg.h>

#include <bROwSE/FunctionUtil.h>
#include <Wt/WEnvironment>

namespace bROwSE {

void
WFunctionCfg::init() {
    wMessage_ = new Wt::WText("No function.", this);
}

void
WFunctionCfg::changeFunction(const P2::Function::Ptr &function) {
    if (function_ == function)
        return;
    function_ = function;

    // We need to create a new wImage each time because we're adding WRectArea.  See limitation documented for the
    // WImage::addArea method in Wt-3.3.3 [Robb P. Matzke 2014-09-10]
    areas_.clear();
    delete wImage_;
    wImage_ = new Wt::WImage(this);

    // Get the CFG image
    if (NULL==function_) {
        wMessage_->setText("No function.");
        wMessage_->show();
        wImage_->hide();
        return;
    }
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
            P2::Function::Ptr callee = ctx_.partitioner.functionExists(va);
            if (callee && callee!=function) {
                if (!callee->name().empty())
                    area->setToolTip(callee->name());
                area->clicked().connect(this, &WFunctionCfg::selectFunction);
            } else if (P2::BasicBlock::Ptr bblock = ctx_.partitioner.basicBlockExists(va)) {
                // Gecko agents (e.g., firefox) don't like multi-line tooltips--they rewrap the tip as they see fit.
                // Therefore, on such agents just indicate the number of instructions.  For other agents, create a tooltip
                // with up to 10 lines listing the instructions.
                if (ctx_.application->environment().agentIsGecko()) {
                    area->setToolTip(StringUtility::plural(bblock->nInstructions(), "instructions"));
                } else {
                    std::string toolTip;
                    bool exitEarly = bblock->nInstructions()>10;
                    for (size_t i=0; (i<9 || (!exitEarly && i<10)) && i<bblock->nInstructions(); ++i)
                        toolTip += (i?"\n":"") + unparseInstruction(bblock->instructions()[i]);
                    if (bblock->nInstructions()>10)
                        toolTip += "\nand " + StringUtility::numberToString(bblock->nInstructions()-9) + " more...";
                    area->setToolTip(toolTip);
                }
                area->clicked().connect(this, &WFunctionCfg::selectBasicBlock);
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

#include <rose.h>
#include <bROwSE/WAssemblyListing.h>

#include <BinaryControlFlow.h>                          // ROSE
#include <boost/algorithm/string/replace.hpp>
#include <bROwSE/FunctionUtil.h>
#include <Wt/WFont>
#include <Wt/WScrollArea>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

namespace bROwSE {

void
WAssemblyListing::init() {
    // The WText that holds the assembly listing
    wText_ = new Wt::WText("No function.", this);
    wText_->setTextFormat(Wt::XHTMLText);
    wText_->setWordWrap(false);
    wText_->setTextAlignment(Wt::AlignLeft);
    Wt::WCssDecorationStyle decor;
    decor.setFont(Wt::WFont::Monospace);
    wText_->setDecorationStyle(decor);

#if 0 // [Robb P. Matzke 2014-12-18]
    // A VBoxLayout containing a WScrollArea
    Wt::WVBoxLayout *layout = new Wt::WVBoxLayout();
    setLayout(layout);

    Wt::WScrollArea *sa = new Wt::WScrollArea(this);
    sa->setScrollBarPolicy(Wt::WScrollArea::ScrollBarAlwaysOn);
    sa->setWidget(wText_);
    layout->addWidget(sa);
#endif
}

void
WAssemblyListing::changeFunction(const P2::Function::Ptr &function) {
    using namespace rose;

    if (function_ == function)
        return;
    function_ = function;

    SgAsmFunction *ast = functionAst(ctx_.partitioner, function);

    BinaryAnalysis::ControlFlow::BlockGraph cfg;
    BinaryAnalysis::ControlFlow().build_block_cfg_from_ast(ast, cfg /*out*/);

    BinaryAnalysis::AsmUnparser unparser;
    unparser.set_registers(ctx_.partitioner.instructionProvider().registerDictionary());
    unparser.add_control_flow_graph(cfg);

    std::string s = StringUtility::htmlEscape(unparser.to_string(ast));
    boost::replace_all(s, " ", "&nbsp;");
    boost::replace_all(s, "\n", "<br/>");
    wText_->setText("<pre style=\"white-space: nowrap;\">" + s + "</pre>");
}

} // namespace

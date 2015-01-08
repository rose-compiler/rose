#include <bROwSE/WSplash.h>
#include <Wt/WPushButton>
#include <Wt/WText>

namespace bROwSE {

void
WSplash::init() {
    Wt::WText *wIntro = new Wt::WText(this);
    wIntro->setTextFormat(Wt::XHTMLUnsafeText);
    wIntro->setText("<h2>Welcome to the binary ROSE on-line workbench for specimen exploration (bROwSE).</h2>"

                    "<br/>"
                    "This application runs in a single web-page via Javascript and Ajax and is not RESTful&mdash;don't "
                    "use your browser's \"back\" or \"reload\" buttons unless you intend to start a new session. The "
                    "server uses the <a href=\"http://rosecompiler.org\">ROSE</a> library for all of its analysis."

                    "<h3>Alpha quality</h3>"
                    "This server is currently alpha-quality software and has a number of issues to be fixed:"
                    "<ul>"
                    "<li>It's only tested with Google's Chrome browser.  It may work with other browsers if they "
                    "    are modern enough to display SVG graphics and javascript is enabled, but Chrome is the "
                    "    only one I routinely use for testing.</li>"
                    "<li>It does not support multiple users/sessions talking to the same server.  ROSE is not fully "
                    "    thread safe and no attempt has been made to serialize the server's access to ROSE.</li>"
                    "<li>Backing out of a disassembly leaks a substantial amount of memory. ROSE was designed "
                    "    to primarily support non-interactive workflows and doesn't always clean up well after itself.</li>"
                    "<li>Many, many features are missing.  The server uses only a very small subset of the features "
                    "    available in ROSE.</li>"
                    "<li>No attempt is made to limit the time it takes to generate a control flow graph. If one tries "
                    "    to display the CFG for a large function the server could hang for many minutes.  Some other "
                    "    operations can take many seconds to complete. A red \"Loading...\" indicator will appear at the "
                    "    top right corner of the window.</li>"
                    "<li>The server occassionally crashes, and when this happens it should automatically restart. If you "
                    "    need to forcibly restart it (and it's not hung), clicking on the dark, round logo in the top "
                    "    left of the window should reboot the server (clicking your \"reload\" button will only start a "
                    "    new session in the same browser using the old instance of the ROSE library).</li>"
                    "</ul>");
    
    Wt::WPushButton *wContinue = new Wt::WPushButton("Continue", this);
    wContinue->clicked().connect(boost::bind(&WSplash::handleContinue, this));

    new Wt::WText("<br/>Webmaster/issues: <a href=\"mailto:matzke@hoosierfocus.com\">Robb Matzke</a>.", this);


}

void
WSplash::handleContinue() {
    clicked_.emit();
}

} // namespace

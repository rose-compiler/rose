#ifndef bROwSE_Application_H
#define bROwSE_Application_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/Partitioner.h>
#include <sawyer/CommandLine.h>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WGridLayout>

namespace bROwSE {

class WFunctions;

class Application: public Wt::WApplication {
    Context ctx_;
    Wt::WGridLayout *wGrid_;
    WFunctions *wFunctions_;
public:
    Application(P2::Partitioner &partitioner, const Wt::WEnvironment &env)
        : Wt::WApplication(env), ctx_(partitioner, this),  wFunctions_(NULL) {
        init();
    }

    static Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[], Settings&);

    static void main(int argc, char *argv[]);

private:
    void init();
};

} // namespace
#endif

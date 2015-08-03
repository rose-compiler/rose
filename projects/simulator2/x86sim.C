/* Emulates an executable. */
#include <rose.h>
#include <RSIM_Private.h>

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include <RSIM_Debugger.h>
#include <RSIM_Linux32.h>
#include <RSIM_Linux64.h>
#include <RSIM_ColdFire.h>
#include <RSIM_Tools.h>
#include <Diagnostics.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;

Sawyer::Message::Facility mlog;

enum GuestOs {
    GUEST_OS_NONE,                                      // No guest operating system specified
    GUEST_OS_LINUX_x86,                                 // Linux on 32-bit Intel x86 processor ("x86" is the kernel's name)
    GUEST_OS_LINUX_amd64,                               // Linux on 64-bit Intel x86 processor ("amd64" is the kernel's name)
    GUEST_OS_ColdFire,                                  // Raw FreeScale ColdFire hardware
};

struct Settings {
    GuestOs guestOs;
    bool catchingSignals;
    bool usingDebugger;
    RSIM_Simulator::Settings simSettings;
    Settings()
        : guestOs(GUEST_OS_NONE), catchingSignals(false), usingDebugger(false) {}
};

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser
        .purpose("concrete simulation of an executable")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] [--] @v{specimen} [@v{specimen_args}...]\n\n"
             "@prop{programName} [@v{switches}] [--] @v{pid}\n\n")
        .doc("Description", "a",
             "This tool simulates concrete execution of an executable specimen in an unsafe manner. Any system calls made by "
             "the specimen are passed along by the underlying operating system after possible translation by this tool. The "
             "tool emulates various services typically provided by the operating system, such as memory management, signal "
             "handling, and thread support.\n\n"

             "This tool is not intended to be a full-fledged virtual machine, but rather a dynamic analysis platform. "
             "The dynamic nature comes from ROSE's ability to \"execute\" an instruction in a concrete domain, and the "
             "tool's ability to simulate certain aspects of an operating system, including process and thread creation, "
             "signal delivery, memory management, and system call emulation. There may be other alternatives more "
             "suitable to your situation:"

             "@bullet{ROSE can analyze a natively running process.  It does so by attaching to the process as if it "
             "were a debugger and querying the process's state (memory and registers) from the operating system. This is "
             "much less complicated than trying to simulate execution by emulating substantial features of the operating "
             "system.}"

             "@bullet{ROSE has a built-in debugger. ROSE can execute a specimen natively within its own debugger, which "
             "includes the ability to set breakpoints for entire regions of memory (e.g., all addresses). This allows "
             "user-written tools to easily customize execution within the debugger. This can be combined with the previous "
             "bullet to analyze the program both dynamically and staticlly is and is significantly faster than "
             "simulating each instruction.}")
        .doc("Specimen", "x",
             "The specimen can be specified by name or process ID.  If a name is given then the simulator can either "
             "emulate the Linux \"exec\" system call to load the specimen into a simulated process; or it can use Linux "
             "directly to load the specimen into a temporary native process, initialize the simulator from that process, "
             "and then kill that process. The @s{native-load} switch causes the latter behavior.\n\n"

             "If the specimen is specified with a process ID then the simulator attaches to that process, initializes its "
             "memory and registers from that process, and then detaches.  One should note that only memory and registers "
             "are initialized and not the simulated Linux kernel state. In particular, file descriptors open in the process "
             "will not be present in the simulator; parent/child relationships will be broken since the simulator is not "
             "related to the process; the simulated process will have only one thread; any process ID stored in the "
             "simulated memory will be incorrect; pending signals and signal masks will not be initialized from the process; "
             "process real and effective uid and gid may be different; etc.  This method is best used only with a process "
             "that has been just created and then stopped.")
        .doc("Caveats", "z",
             "Speed of simulation is not a primary goal of this tool. ROSE is mostly a static analysis library "
             "whose capabilities happen to include the ability to write a concrete simulation tool.\n\n"

             "Specimen memory is managed in a container within the simulating ROSE process. Therefore operations like "
             "interprocess communication via shared memory will probably never work.  This limitation may also include "
             "intra-process communication with shared memory mapped at two different addresses, and other \"tricks\" that "
             "specimens sometimes do with their memory maps.\n\n"

             "The specimen process and threads are simulated inside this tool's process and threads. Certain operations that "
             "modify process and thread properties will end up modifying the tool's process and threads. For instance, reading "
             "from the Linux \"/proc/@v{n}\" filesystem will return information about the simulator tool rather than "
             "the specimen being simulated. Sending a signal to a process or thread will cause the tool to forward the "
             "signal to the simulated process or thread only if this behavior is enabled at runtime.\n\n"

             "Emulating the fork (clone) system call is not practically possible because the simulator is multi-threaded "
             "even when simulating a single-threaded guest, and the simulator needs to do more than call only async-signal "
             "safe functions after the fork returns in the child. A guest that forks cannot be simulated past the fork. This "
             "includes guests that call C library functions like @man{system}{3} and @man{popen}{3}.\n\n"

             "See the README file in the simulator source code directory for more caveats.");

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("arch")
              .argument("architecture", enumParser<GuestOs>(settings.guestOs)
                        ->with("linux-x86", GUEST_OS_LINUX_x86)
                        ->with("linux-amd64", GUEST_OS_LINUX_amd64)
                        ->with("coldfire", GUEST_OS_ColdFire))
              .doc("Simulated host architecture.  The supported architectures are:"
                   "@named{linux-x86}{Linux operating system running on 32-bit x86-compatible hardware.}"
                   "@named{linux-amd64}{Linux operating system running on 64-bit amd64-compatible hardware.}"
                   "@named{coldfire}{Naked FreeScale ColdFire hardware with no operating system.}"));

    sg.insert(Switch("signals")
              .intrinsicValue(true, settings.catchingSignals)
              .doc("Causes the simulator to catch signals sent by other processes and deliver them to the "
                   "specimen by emulating the operating system's entire signal delivery mechanism.  The "
                   "@s{no-signals} switch disables this feature, in which case signals sent to this tool "
                   "will cause the default action to occur (e.g., pressing Control-C will likely terminate "
                   "the tool directly, whereas when signal handling is emulated it will cause a simulated SIGINT "
                   "to be sent to the specimen possibly causing the an emulated termination). Signals raised by "
                   "the specimen to be delivered to itself are always emulated; signals raised by the specimen to "
                   "be delivered to some other process are handled by system call emulation.  The default is that "
                   "signals generated by some other process and delivered to the simulator process will " +
                   std::string(settings.catchingSignals ? "be forwarded to the specimen." :
                               "cause the default signal action within the simulator tool.")));
    sg.insert(Switch("no-signals")
              .key("signals")
              .intrinsicValue(false, settings.catchingSignals)
              .hidden(true));

    sg.insert(Switch("debugger")
              .intrinsicValue(true, settings.usingDebugger)
              .doc("Invokes a simple interactive debugger.  See RSIM_Tools.C for the commands; this isn't really "
                   "intended for end users yet."));

    return parser
        .with(CommandlineProcessing::genericSwitches())
        .with(sg)                                       // tool-specific
        .with(RSIM_Simulator::commandLineSwitches(settings.simSettings))
        .parse(argc, argv).apply().unreachedArgs();
}

template<class Simulator>
static void
simulate(const Settings &settings, const std::vector<std::string> &args, char *envp[]) {
    Simulator sim;

    if (settings.usingDebugger)
        RSIM_Debugger::attach(sim);

    sim.configure(settings.simSettings, envp);

    pid_t existingPid = -1;
    char *rest = NULL;
    errno = 0;
    if (args.size()==1 && (existingPid=strtoul(args.front().c_str(), &rest, 0))>=0 && !errno && !*rest) {
        if (sim.loadSpecimen(existingPid) < 0)
            return;
    } else {
        if (sim.loadSpecimen(args) < 0)
            return;
    }

    if (settings.catchingSignals)
        sim.activate();
    sim.main_loop();
    if (settings.catchingSignals)
        sim.deactivate();
    std::cerr <<sim.describe_termination() <<"\n";
    sim.terminate_self(); // probably doesn't return
}
    
int
main(int argc, char *argv[], char *envp[]) {
    // Initialization
    Diagnostics::initialize();
    ::mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Command-line parsing
    Settings settings;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, settings);

    // Simulate the specimen execution
    switch (settings.guestOs) {
        case GUEST_OS_LINUX_x86:
            simulate<RSIM_Linux32>(settings, specimen, envp);
            break;
        case GUEST_OS_LINUX_amd64:
            simulate<RSIM_Linux64>(settings, specimen, envp);
            break;
        case GUEST_OS_ColdFire:
            simulate<RSIM_ColdFire>(settings, specimen, envp);
            break;
        case GUEST_OS_NONE:
            ::mlog[FATAL] <<"no architecture specified (\"--arch\"); see \"--help\"\n";
            exit(1);
    }
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform\n";
    exit(1);
}

#endif /* ROSE_ENABLE_SIMULATOR */

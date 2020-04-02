#ifndef PAR_PRO_AUTOMATA_H
#define PAR_PRO_AUTOMATA_H

namespace CodeThorn {
  namespace ParProAutomata {
    // returns true if command line argument was detected and an action was performed, otherwise false.
    bool handleCommandLineArguments(CommandLineOptions& args,Sawyer::Message::Facility& logger);
    void automataDotInput(Sawyer::Message::Facility logger);
    void generateAutomata();
  }
}
#endif

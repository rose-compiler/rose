#ifndef PAR_PRO_AUTOMATA_H
#define PAR_PRO_AUTOMATA_H

namespace CodeThorn {
  namespace ParProAutomata {
    // returns true if command line argument was detected and an action was performed, otherwise false.
    bool handleCommandLineArguments(ParProOptions& parProOpt,CodeThornOptions& ctOpt, LTLOptions& ltlOpt, Sawyer::Message::Facility& logger);
    void automataDotInput(ParProOptions& parProOpt, CodeThornOptions& ctOpt, LTLOptions& ltlOpt, Sawyer::Message::Facility logger);
    void generateAutomata(ParProOptions& parProOpt);
  }
}
#endif

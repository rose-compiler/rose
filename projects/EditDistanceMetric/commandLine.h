#include <Sawyer/CommandLine.h>

// Settings that control behavior of this tool.
struct Settings {
    double insertionCost, deletionCost, substitutionCost;
    bool useOldImplementation;
    Settings()
        : insertionCost(1.0), deletionCost(1.0), substitutionCost(1.0), useOldImplementation(true) {}
};

Sawyer::CommandLine::SwitchGroup toolCommandLineSwitches(Settings&);

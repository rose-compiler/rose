#include <sawyer/CommandLine.h>

// Settings that control behavior of this tool.
struct Settings {
    double insertionCost, deletionCost, substitutionCost;
    Settings()
        : insertionCost(1.0), deletionCost(1.0), substitutionCost(1.0) {}
};

Sawyer::CommandLine::ParserResult parseCommandLine(int argc, char *argv[], Settings &settings);

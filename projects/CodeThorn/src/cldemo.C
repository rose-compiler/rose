// The purpose is short, non-capitalized, non-puntuated.
static const char *purpose = "purpose-text";
static const char *description = "description-text";

#include <rose/rose.h>
#include <rose/CommandLine.h>
#include <Sawyer/CommandLine.h>

bool opt1 = false;
bool opt2 = true;
bool boolflag1 = false;
std::string name;

std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
  using namespace Sawyer::CommandLine;
  
  SwitchGroup sg("Switches for Markus");
  sg.name("markus");                                  // optional (see --help output)
  
  // boolean switches. Descriptions can be arbitrarily long.
  // These are just wrappers around sg.insert(Switch(....)).
  Rose::CommandLine::insertBooleanSwitch(sg, "opt1", opt1, "This switch does something.");
  Rose::CommandLine::insertBooleanSwitch(sg, "opt2", opt2, "This switch does something.");
  Rose::CommandLine::insertBooleanSwitch(sg, "boolflag1", boolflag1, "This switch does something.");
  
  // A switch that takes exactly one argument. The one-char switch name is optional.
  sg.insert(Switch("name", 'n')
            .argument("some_name", anyParser(name))
            .doc("Causes @v{some_name} to be the name used for analysis."));

  // parsing
  Parser parser = Rose::CommandLine::createEmptyParserStage(purpose, description);
  parser.with(Rose::CommandLine::genericSwitches());  // optional: --help, etc.
  return parser.with(sg).parse(argc, argv).apply().unparsedArgs();
}

int
main(int argc, char *argv[]) {
    std::vector<std::string> roseArgs = parseCommandLine(argc, argv);

    std::cout <<"opt1      = " <<opt1 <<"\n"
              <<"opt2      = " <<opt2 <<"\n"
              <<"boolflag1 = " <<boolflag1 <<"\n"
              <<"name      = \"" <<Rose::StringUtility::cEscape(name) <<"\"\n";
    for (size_t i = 0; i < roseArgs.size(); ++i)
        std::cout <<"arg[" <<i <<"]    = \"" <<Rose::StringUtility::cEscape(roseArgs[i]) <<"\"\n";
}

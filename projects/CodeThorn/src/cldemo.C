// The purpose is short, non-capitalized, non-puntuated.
static const char *purpose = "purpose-text";
static const char *description = "description-text";

#include <rose.h>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>

bool opt1 = false;
bool opt2 = true;
bool opt3 = false;
std::string argswitch1name;

std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
  using namespace Sawyer::CommandLine;
  
  SwitchGroup sg("SwitchesGroup1");
  sg.name("switchesgroup1"); // optional (see --help output)
  
  sg.insert(Switch("help", 'h')
            .doc("Show this documentation.")
            .action(showHelpAndExit(0)));

  // boolean switches. Descriptions can be arbitrarily long.
  // These are just wrappers around sg.insert(Switch(....)).
  Rose::CommandLine::insertBooleanSwitch(sg, "bool1", opt1, "This optional bool switch 1 does something.");
  Rose::CommandLine::insertBooleanSwitch(sg, "bool2", opt2, "This optional bool switch 2 does something.");
  Rose::CommandLine::insertBooleanSwitch(sg, "bool3", opt3, "This mandatory bool switch 3 does something.");
  
  // switch with one argument. The one-char switch name is optional.
  sg.insert(Switch("argswitch1", 'a')
            .argument("argswitch1name", anyParser(argswitch1name))
            .doc("Causes @v{argswitch1name} to be the name used for analysis."));


  // parsing
  Parser parser = Rose::CommandLine::createEmptyParserStage(purpose, description);
  //parser.with(Rose::CommandLine::genericSwitches());  // optional: --help, etc.
  parser.version("Toolversion 0.0.1","Tool Releasedate");
  parser.chapter(1,"ToolHelpPageChapterName");
  return parser.with(sg).parse(argc, argv).apply().unparsedArgs();
}

int
main(int argc, char *argv[]) {
  for (int i = 0; i < argc; ++i) {
        std::cout <<"arg[" <<i <<"]    = \"" <<Rose::StringUtility::cEscape(argv[i]) <<"\"\n";
  }
  std::vector<std::string> nonToolArgs = parseCommandLine(argc, argv);
  std::cout <<"opt1      = " <<opt1 <<"\n"
	    <<"opt2      = " <<opt2 <<"\n"
	    <<"opt3      = " <<opt3 <<"\n"
	    <<"name      = \"" <<Rose::StringUtility::cEscape(argswitch1name) <<"\"\n";
  for (size_t i = 0; i < nonToolArgs.size(); ++i) {
    std::cout <<"nontoolarg[" <<i <<"]    = \"" <<Rose::StringUtility::cEscape(nonToolArgs[i]) <<"\"\n";
  }
}

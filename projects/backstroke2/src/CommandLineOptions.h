#ifndef COMMAND_LINED_OPTIONS_H
#define COMMAND_LINED_OPTIONS_H

namespace Backstroke {

  class CommandLineOptions {
  public:
    CommandLineOptions();
    void process(int argc, char* argv[]);
    bool optionRoseAstCheck();
    bool optionShowRoseFileNodeInfo();
    bool optionShowTransformationTrace();
    bool isFinished();
    bool isError();
  private:
    bool _optionVersion;
    bool _optionRoseHelp;
    bool _optionRoseAstCheck;
    bool _optionShowRoseFileNodeInfo;
    bool _optionShowTransformationTrace;
    bool _isFinished;
  };

};

#endif

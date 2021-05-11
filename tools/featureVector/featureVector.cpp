#include "rose.h"
#include <Sawyer/CommandLine.h>
using namespace std;
using namespace Sawyer::Message::Common; // if you want unqualified DEBUG, WARN, ERROR, FATAL, etc.
Sawyer::Message::Facility mlog; // a.k.a., Rose::Diagnostics::Facility

bool enable_debug = false;
bool enable_verbose = false;

Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup standard;
    standard.doc("The following switches are recognized by all tools in this package.");

    standard.insert(Switch("help", 'h')
                    .shortName('?')
                    .action(showHelpAndExit(0))
                    .doc("Show this documentation."));

    SwitchGroup featureVector;
    featureVector.doc("The following switches are specific to scalarizer.");

    featureVector.insert(Switch("debug")
                .intrinsicValue(true, enable_debug)
                .doc("Enable the debugging mode: print out FVDebug file"));

    featureVector.insert(Switch("verbose")
                .intrinsicValue(true, enable_verbose)
                .doc("Enable the verbose mode: print debug info to stdout"));

    Parser parser;
    parser
        .purpose("Feature Vector")
        .doc("synopsis", "@prop{programName} [@v{switches}]")
        .doc("description",
             "This program prints out the feature vector of a program "
             "to help verifying ROSE output correctness. ");

    //parser.skippingUnknownSwitches(true);
    return parser.with(standard).with(featureVector).parse(argc, argv).apply();
}

class nodeTraversal : public AstSimpleProcessing
{
  private:
      string sourcefilename;
      vector<int> outputVetor;
      vector<string> debugVector;
  public:
      virtual void visit(SgNode* n);

      void setSourceFile(string);

      const vector<int> & getVector() const;

      const vector<string> & getDebugVector() const;
};

void nodeTraversal::setSourceFile(string filename)
{
   this->sourcefilename = filename;;
}

const vector<int> & nodeTraversal::getVector() const
{
    return outputVetor;
}

const vector<string> & nodeTraversal::getDebugVector() const
{
    return debugVector;
}

void nodeTraversal::visit(SgNode* n)
{
//  cout << n->variantT() << ":" << n->class_name() << endl;
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if(locatedNode)
    if(sourcefilename.compare( locatedNode->getFilenameString()) == 0)
    {
      outputVetor.push_back(n->variantT());
      if(enable_verbose || enable_debug)
      {
        std::ostringstream o;
        o <<  n->variantT() <<":" << n->class_name();
        if(enable_verbose)
            cout << o.str() << endl; 
        //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
        debugVector.push_back(o.str());
      }
    }
}

int main( int argc, char * argv[] ){

  ROSE_INITIALIZE;
  Rose::Diagnostics::initAndRegister(&mlog, "featureVector");

  Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv);
  std::vector<std::string> positionalArgs = cmdline.unreachedArgs();

  SgProject* project = frontend(argc,argv);
  Rose_STL_Container<std::string> filenames = project->getAbsolutePathFileNames();
  for(Rose_STL_Container<std::string>::iterator it = filenames.begin(); it != filenames.end(); it++)
    cout << "filename: " << *it << endl;
  ROSE_ASSERT(filenames.size() == 1);
  string inputfilename = filenames[0];
  //for(int i=0; i < filenames.size(); i++)
  //  cout << "name:" << i << ":" << filenames[i] << endl;

  nodeTraversal travese;
  travese.setSourceFile(inputfilename);
  travese.traverseInputFiles(project,preorder);
  std::string outfilename = SageInterface::generateProjectName(project)+".FV.txt";
  const vector<int>& featureVector = travese.getVector();

//  for(int i=0; i < featureVector.size(); i++)
//    cout << featureVector[i] << endl; 

  ofstream output_file(outfilename);
  ostream_iterator<int> output_iterator(output_file, ",");
  copy(featureVector.begin(), featureVector.end(), output_iterator);
  output_file.close();
 
  if(enable_debug)
  {
    std::string debugfilename = SageInterface::generateProjectName(project)+".FVDebug.txt";
    const vector<string>& debugFeatureVector = travese.getDebugVector();
    
    ofstream debug_file(debugfilename);
    ostream_iterator<string> debug_iterator(debug_file, "\n");
    copy(debugFeatureVector.begin(), debugFeatureVector.end(), debug_iterator);
    debug_file.close();
  }
 
  return 0;
}

#include "rose.h"
using namespace std;

class nodeTraversal : public AstSimpleProcessing
{
  private:
      string sourcefilename;
      vector<int> outputVetor;
  public:
      virtual void visit(SgNode* n);

      void setSourceFile(string);

      const vector<int> & getVector() const;
};

void nodeTraversal::setSourceFile(string filename)
{
   this->sourcefilename = filename;;
}

const vector<int> & nodeTraversal::getVector() const
{
    return outputVetor;
}

void nodeTraversal::visit(SgNode* n)
{
//  cout << n->variantT() << ":" << n->class_name() << endl;
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if(locatedNode)
    if(sourcefilename.compare( locatedNode->getFilenameString()) == 0)
    {
      outputVetor.push_back(n->variantT());
      // cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
    }
}

int main( int argc, char * argv[] ){

  SgProject* project = frontend(argc,argv);
  Rose_STL_Container<std::string> filenames = project->getAbsolutePathFileNames();
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
  
  return 0;
}

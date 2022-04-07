#include "rose.h"
#include <Rose/CommandLine.h> // Commandline support in librose
#include <Sawyer/CommandLine.h>
using namespace std;
using namespace Sawyer::Message::Common; // if you want unqualified DEBUG, WARN, ERROR, FATAL, etc.
Sawyer::Message::Facility mlog; // a.k.a., Rose::Diagnostics::Facility

bool enable_debug = false;
bool enable_verbose = false;

Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup featureVector;
    featureVector.doc("The following switches are specific to scalarizer.");
  
    bool showRoseHelp = false;
    featureVector.insert(Switch("rose:help")
             .longPrefix("-")
             .intrinsicValue(true, showRoseHelp)
             .doc("Show the old-style ROSE help.")); 

    featureVector.insert(Switch("debug")
                .intrinsicValue(true, enable_debug)
                .doc("Enable the debugging mode: print out FVDebug file"));

    featureVector.insert(Switch("verbose")
                .intrinsicValue(true, enable_verbose)
                .doc("Enable the verbose mode: print debug info to stdout"));

    Parser parser;
    parser
        .purpose("Feature Vector")
        .doc("synopsis", "@prop{programName} [@v{switches}] [@v{files}] ")
        .doc("description",
             "This program prints out the feature vector of a program "
             "to help verifying ROSE output correctness. ");

    parser.skippingUnknownSwitches(true);
    return parser.with(Rose::CommandLine::genericSwitches()).with(featureVector).parse(argc, argv).apply();
}

class nodeTraversal : public AstSimpleProcessing
{
  private:
      string sourcefilename;
      vector<std::size_t> outputVetor;
      vector<string> debugVector;
  public:
      virtual void visit(SgNode* n);

      void setSourceFile(string);

      const vector<std::size_t> & getVector() const;

      const vector<string> & getDebugVector() const;

      void getNamedTypeInfo(SgNamedType* namedType);
};

void nodeTraversal::setSourceFile(string filename)
{
   this->sourcefilename = filename;;
}

const vector<std::size_t> & nodeTraversal::getVector() const
{
    return outputVetor;
}

const vector<string> & nodeTraversal::getDebugVector() const
{
    return debugVector;
}

void nodeTraversal::getNamedTypeInfo(SgNamedType* namedType)
{
   //cout << "have a named type" << endl;
   SgTemplateInstantiationDecl* templateInstDecl = isSgTemplateInstantiationDecl(namedType->get_declaration()); 
   if(templateInstDecl)
   {
     SgTemplateArgumentPtrList argPtrList = templateInstDecl->get_templateArguments();
     //cout << "have a class decl:" << argPtrList.size() << endl;
     for(SgTemplateArgumentPtrList::iterator tempArgId = argPtrList.begin(); tempArgId != argPtrList.end(); ++tempArgId)
     {
       SgTemplateArgument* tempArg = *tempArgId;
       //cout << "template argument type: " << tempArg->get_argumentType() << " Mangled name: " << tempArg->get_mangled_name() << endl;
       if(tempArg->get_argumentType() == SgTemplateArgument::type_argument)
       {
         SgType* tempArgType = tempArg->get_type();
         outputVetor.push_back(tempArgType->variantT());
         //if(enable_verbose || enable_debug)
         {
           std::ostringstream o;
           //o <<  tempArgType->variantT() <<":" << tempArgType->class_name();
           o << tempArgType->class_name();
           if(enable_verbose)
               cout << o.str() << endl; 
           //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
           debugVector.push_back(o.str());
         }
         tempArgType = (isSgTypedefType(tempArgType) != nullptr) ? isSgTypedefType(tempArgType)->get_base_type() : tempArgType;
         SgNamedType* newNamedType = isSgNamedType(tempArgType);
         if(newNamedType != nullptr)
         {
           //cout << "have another named type" << endl;
           getNamedTypeInfo(newNamedType);
         }
       }
       else
       {
         SgName mangledName = tempArg->get_mangled_name();
         string mangledNameString = mangledName.getString();
         std::size_t hashStringVal = std::hash<std::string>{}(mangledNameString);
         outputVetor.push_back(hashStringVal);
         //if(enable_verbose || enable_debug)
         {
           std::ostringstream o;
           o  <<"template argument mangled name:" << hashStringVal << ":" << mangledNameString;;
           if(enable_verbose)
               cout << o.str() << endl; 
           //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
           debugVector.push_back(o.str());
         }
       }
     }
     
   }
}

void nodeTraversal::visit(SgNode* n)
{
//  cout << n->variantT() << ":" << n->class_name() << endl;
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if(locatedNode)
    if(sourcefilename.compare( locatedNode->getFilenameString()) == 0)
    {
      outputVetor.push_back(n->variantT());
      //if(enable_verbose || enable_debug)
      {
        std::ostringstream o;
        //o <<  n->variantT() <<":" << n->class_name();
        o << n->class_name();
        if(isSgValueExp(n) != nullptr)
        {
          SgValueExp* valueExp = isSgValueExp(n);
          o << ":" << valueExp->get_constant_folded_value_as_string();
        }

        if(enable_verbose)
            cout << o.str() << endl; 
        //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
        debugVector.push_back(o.str());
      }

      // retrieve preprocessing information
      AttachedPreprocessingInfoType* info = locatedNode->getAttachedPreprocessingInfo();
      if(info)
      {
        std::ostringstream o;
        for (AttachedPreprocessingInfoType::iterator i = info->begin (); i != info->end (); i++)
        {
          PreprocessingInfo::DirectiveType  directive =  (*i)->getTypeOfDirective();
          //std::cout << PreprocessingInfo::directiveTypeName (directive) << " : " << (*i)->getString() << std::endl;
          //o << n->variantT() << ":" << n->class_name() << " (" << PreprocessingInfo::relativePositionName((*i)->getRelativePosition()) << "):" << PreprocessingInfo::directiveTypeName (directive) << ":" << (*i)->getString();
          o << n->class_name() << " (" << PreprocessingInfo::relativePositionName((*i)->getRelativePosition()) << "):" << PreprocessingInfo::directiveTypeName (directive) << ":" << (*i)->getString();
/*  Keep the following code in case we need to print only content from partial of the list
      
          switch(directive)
          {
            case PreprocessingInfo::DirectiveType::CpreprocessorUnknownDeclaration:
            case PreprocessingInfo::DirectiveType::C_StyleComment:
            case PreprocessingInfo::DirectiveType::CplusplusStyleComment:
            case PreprocessingInfo::DirectiveType::FortranStyleComment:
            case PreprocessingInfo::DirectiveType::F90StyleComment:
            case PreprocessingInfo::DirectiveType::AdaStyleComment:
            case PreprocessingInfo::DirectiveType::JovialStyleComment:
            case PreprocessingInfo::DirectiveType::CpreprocessorBlankLine:
            case PreprocessingInfo::DirectiveType::CpreprocessorIncludeDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorIncludeNextDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorDefineDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorUndefDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorIfdefDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorIfndefDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorIfDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorDeadIfDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorElseDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorElifDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorEndifDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorLineDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorErrorDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorWarningDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorEmptyDeclaration:
            case PreprocessingInfo::DirectiveType::CSkippedToken:
            case PreprocessingInfo::DirectiveType::CMacroCall:
            case PreprocessingInfo::DirectiveType::CMacroCallStatement:
            case PreprocessingInfo::DirectiveType::LineReplacement:
            case PreprocessingInfo::DirectiveType::ClinkageSpecificationStart:
            case PreprocessingInfo::DirectiveType::ClinkageSpecificationEnd:
            case PreprocessingInfo::DirectiveType::CpreprocessorIdentDeclaration:
            case PreprocessingInfo::DirectiveType::CpreprocessorCompilerGeneratedLinemarker:
            case PreprocessingInfo::DirectiveType::RawText:
            case PreprocessingInfo::DirectiveType::CpreprocessorEnd_ifDeclaration:
            case PreprocessingInfo::DirectiveType::LastDirectiveType:
              o << n->variantT() << ":" << n->class_name() << " (" << PreprocessingInfo::relativePositionName((*i)->getRelativePosition()) << "):" << PreprocessingInfo::directiveTypeName (directive) << ":" << (*i)->getString();
              break;
            default :
              ;
          }
*/
        }
        if(!o.str().empty())        
        {
          if(enable_verbose)
              cout << o.str() << endl; 
          debugVector.push_back(o.str());
        }
      }

      // get type info from SgVariableDeclaration
      SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);
      if(varDecl) {
        SgInitializedNamePtrList nameList = varDecl->get_variables();
        //cout << "have decl:" << nameList.size() << endl;
        for(SgInitializedNamePtrList::iterator nameId = nameList.begin(); nameId != nameList.end(); ++nameId)
        {
          SgInitializedName* initNameId = *nameId;
          //cout << "have Initializedname" << endl;
          SgType* varType = initNameId->get_type()->findBaseType();
          outputVetor.push_back(varType->variantT());
          //if(enable_verbose || enable_debug)
          {
            std::ostringstream o;
            //o <<  varType->variantT() <<":" << varType->class_name();
            o << varType->class_name();
            if(enable_verbose)
                cout << o.str() << endl; 
            //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
            debugVector.push_back(o.str());
          }
          // get int class type declaration
          SgNamedType* namedType = isSgNamedType(varType);
          if(namedType) {
            getNamedTypeInfo(namedType); 
          }
        }
      }
      // get type info from SgTypedefDeclaration
      SgTypedefDeclaration* typedefDecl = isSgTypedefDeclaration(n);
      if(typedefDecl) {
         //cout << "have typedecl" << endl;
         SgType* basetype = typedefDecl->get_type()->findBaseType();
         outputVetor.push_back(basetype->variantT());
         //if(enable_verbose || enable_debug)
         {
           std::ostringstream o;
           //o <<  basetype->variantT() <<":" << basetype->class_name();
           o << basetype->class_name();
           if(enable_verbose)
               cout << o.str() << endl; 
           //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
           debugVector.push_back(o.str());
         }
         // get int class type declaration
         SgNamedType* namedType = isSgNamedType(basetype);
         if(namedType) {
           getNamedTypeInfo(namedType); 
         }
      }
      SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(n);
      if(funcDecl) {
        SgInitializedNamePtrList funcArgList = funcDecl->get_args();
        for(SgInitializedNamePtrList::iterator argId = funcArgList.begin(); argId != funcArgList.end(); ++argId)
        { 
          SgInitializedName* initNameId = *argId;
          //cout << "have Initializedname" << endl;
          SgType* varType = initNameId->get_type()->findBaseType();
          outputVetor.push_back(varType->variantT());
          //if(enable_verbose || enable_debug)
          {
            std::ostringstream o;
            //o <<  varType->variantT() <<":funcArgType:" << varType->class_name();
            o <<"funcArgType:" << varType->class_name();
            if(enable_verbose)
                cout << o.str() << endl; 
            //cout << "## " << n->variantT() <<":" << n->class_name() << " " << locatedNode->getFilenameString()  << endl;
            debugVector.push_back(o.str());
          }
        }
      }
    }
}

int main( int argc, char * argv[] ){

  ROSE_INITIALIZE;
  Rose::Diagnostics::initAndRegister(&mlog, "featureVector");

  Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv);
  std::vector<std::string> positionalArgs = cmdline.unparsedArgs();
  positionalArgs.insert(positionalArgs.begin(), argv[0]);

  SgProject* project = frontend(positionalArgs);
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
  ofstream output_file(outfilename);

/* Use the debug mode as default
  const vector<std::size_t>& featureVector = travese.getVector();
//  for(int i=0; i < featureVector.size(); i++)
//    cout << featureVector[i] << endl; 
  ostream_iterator<std::size_t> output_iterator(output_file, ",");
  copy(featureVector.begin(), featureVector.end(), output_iterator);
*/

  const vector<string>& debugFeatureVector = travese.getDebugVector();
  ostream_iterator<string> debug_iterator(output_file, "\n");
  copy(debugFeatureVector.begin(), debugFeatureVector.end(), debug_iterator);
  output_file.close();
/* 
  if(enable_debug)
  {
    std::string debugfilename = SageInterface::generateProjectName(project)+".FVDebug.txt";
    const vector<string>& debugFeatureVector = travese.getDebugVector();
    
    ofstream debug_file(debugfilename);
    ostream_iterator<string> debug_iterator(debug_file, "\n");
    copy(debugFeatureVector.begin(), debugFeatureVector.end(), debug_iterator);
    debug_file.close();
  }
*/
  return 0;
}

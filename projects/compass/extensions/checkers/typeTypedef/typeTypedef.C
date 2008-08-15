// Type Typedef
// Author: Andreas Saebjoernsen
// Date: 24-July-2007

#include "compass.h"

#ifndef COMPASS_TYPE_TYPEDEF_H
#define COMPASS_TYPE_TYPEDEF_H

namespace CompassAnalyses
   { 
     namespace TypeTypedef
        { 
        /*! \brief Type Typedef: Add your description here 
         */

          extern const std::string checkerName;
           extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             {
                    SgType* toType;
                    SgInitializedName* IRnode;

                    //What the type is 
                    std::string is;
                    //What typedef the type shoud be
                    std::string shouldBe;
               public:
                    CheckerOutput(SgType* to, std::string isN, std::string shouldBeN, SgInitializedName* node);
                    virtual std::string getString() const;

             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
                    std::string ruleFile;
                    //map of a typedef to a pair. The pair is boolean and a string.
                    //The string is the type of the typedef. The boolean is true if
                    //const modifiers are allowed in front of the type of the typedef.
                    std::map<std::string, std::pair<bool,std::string> > typedefsToUse;
                    //A map from the type of a typedef to the typedef
                    std::map<std::string, std::string > typeToTypedefs;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_TYPE_TYPEDEF_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Type Typedef Analysis
// Author: Andreas Saebjoernsen
// Date: 24-July-2007

#include "compass.h"
// #include "typeTypedef.h"
#include <fstream>

using namespace std;

namespace CompassAnalyses
   {


//Will read in the file at the filename provided as an argument and
//create a map of typedef identifiers to a pair of a bool and a string.
//The bool will be true if the typedef should also be used for it's type
//with a const type modifier. The string in the pair stands for the
//type of the typedef, but that is found in the AST later.

std::map<std::string, std::pair<bool,std::string> > 
readFile( std::string filename){
     std::map<std::string, std::pair<bool,std::string> > typedefsToUse;
  // DQ (11/10/2007): Modified to use ROSE specific paths defined in rose_paths.h
  // Not clear how to handl this if Compass is spearated from ROSE.
  // std::fstream file_op(filename.c_str());
     //AS(12/04/07) I do not think making the directory of the styles static was
     //a good idea. The problem locating the style in the tests was a makefile
     //problem and should be fixed as such

  // std::string compassLocation = COMPASS_SOURCE_DIRECTORY;
//     std::fstream file_op( (compassLocation+"/typeTypedef"+filename).c_str());

     std::ifstream* file_op = Compass::openFile( filename );

     if (file_op->fail()) {
	  std::cout << "error: could not find file \"" << filename 
	            << "\" which is meant to include the styles to enforce with " 
               << "the name checker." << std::endl;

            // DQ (11/10/2007): Use assert so that this can be caught in the debugger.
               ROSE_ASSERT(false);
               exit(1);    // abort program
				      
				  }

     std::string current_word;
  //read file
     char dummyString[2000];

     while((*file_op) >> current_word){
       //First word denotes what the regular expression should operate
       //upon. Second word denotes the regular expression

	     if(  current_word.substr(0,1) == std::string("#") ){
	      //Skip rest of the line if a "#" character is found. This denotes a 
	      //comment
	      file_op->getline(dummyString,2000);
      
	     }else{
        if(current_word.substr(0,2) == "c:" ){
           typedefsToUse[current_word.substr(2)] = std::pair<bool,std::string>(true,"");
        }else{
           typedefsToUse[current_word] = std::pair<bool,std::string>(false,"");
        }
	  	 }
     }

    return typedefsToUse;
}

//Replaces all whitespaces with a char in a string
void
replaceWhitespace(std::string& inStr, char replaceWith ){
    for ( size_t i = 0; i < inStr.size(); ++i)
         if(inStr[i] == ' ')
          inStr[i] = replaceWith;

};


     namespace TypeTypedef
        { 
          const std::string checkerName      = "TypeTypedef";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Enforces that a typedef should be used instead of a type.";
          const std::string longDescription  = "To ease maintainability some projects use typedefs for some types." 
                                               "This checker reports when the type is used instead of the typedef.";
        } //End of namespace TypeTypedef.
   } //End of namespace CompassAnalyses.

CompassAnalyses::TypeTypedef::
CheckerOutput::CheckerOutput (SgType* to, std::string isN, std::string shouldBeN, SgInitializedName* node)
        : OutputViolationBase(node,checkerName,shortDescription),
          toType(to),  IRnode(node), is(isN), shouldBe(shouldBeN)
   {}

CompassAnalyses::TypeTypedef::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["TypeTypedef.YourParameter"]);
     typedefsToUse  = std::map<std::string, std::pair<bool,std::string> >();
     typeToTypedefs = std::map<std::string, std::string >();


    ruleFile = Compass::parseString(inputParameters["TypeTypedef.RulesFile"]);
    typedefsToUse = readFile(ruleFile); 
   }

std::string 
CompassAnalyses::TypeTypedef::CheckerOutput::getString() const {
        std::string loc =
           Compass::formatStandardSourcePosition(IRnode->get_file_info());

        std::string castExp = IRnode->unparseToString();

        std::string typeOfCast;
 
        return loc + ": " + castExp + ": is " +is + " should be " + shouldBe;

}

void
CompassAnalyses::TypeTypedef::Traversal::
visit(SgNode* node)
   { 
        SgVariableDeclaration* vd = isSgVariableDeclaration(node);
     // DQ (12/15/2007): This appears to be a mistake.
     // return;

     // If a typedef is found fill in the values for it in the map
     // such that a type maps to it's typedef.

     // DQ (11/15/2007): Removed declaration of typedefDecl from inside of if conditional because it cases the g++ 3.4.3 compiler to seg fault.
        SgTypedefDeclaration* typedefDecl = isSgTypedefDeclaration(node);
        if(typedefDecl != NULL){
           std::map<std::string, std::pair<bool,std::string> >::iterator iItr = typedefsToUse.find( typedefDecl->get_name().getString() );
           if( iItr != typedefsToUse.end()  ){
                iItr->second.second =typedefDecl->get_base_type()->unparseToString(); 
                replaceWhitespace(iItr->second.second,'_');
                typeToTypedefs[iItr->second.second] = iItr->first;
                if(iItr->second.first == true)
                   typeToTypedefs["const_" + iItr->second.second] = iItr->first;

           }


        }

        if(vd == NULL)
           return;

        //Find all violations of the typedef rules
        for(SgInitializedNamePtrList::iterator i = vd->get_variables().begin();
               i != vd->get_variables().end(); ++i  )
         {
           SgInitializedName* in = isSgInitializedName(*i);

           //Type casted to
           SgType* toType       = in->get_type();
           string  toTypeName   = toType->unparseToString(); //TransformationSupport::getTypeName(toType);
           replaceWhitespace(toTypeName,'_');
 
           //Create an object for the violation if there is one
           if( typeToTypedefs.find(toTypeName) != typeToTypedefs.end() ){
               output->addOutput(new CheckerOutput(toType, toTypeName, typeToTypedefs.find(toTypeName)->second , in));
           }

        }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::TypeTypedef::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::TypeTypedef::Traversal(params, output);
}

extern const Compass::Checker* const typeTypedefChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::TypeTypedef::checkerName,
        CompassAnalyses::TypeTypedef::shortDescription,
        CompassAnalyses::TypeTypedef::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);

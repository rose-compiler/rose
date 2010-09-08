// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rose_config.h"

// DQ (11/28/2009): I think this is equivalent to "USE_ROSE"
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if (CAN_NOT_COMPILE_WITH_ROSE == 0)
#ifndef USE_ROSE

#include "attributeListMap.h"

// Include files to get the current path
#include <unistd.h>

#ifndef _MSC_VER
#include <sys/param.h>
#endif

#if 1
// DQ (5/9/2007): This is used to support the work with WAVE.

////////////////////////////////////////////////////////////////////////
// Function queryFloatDoubleValExp()  finds all nodes with type 
//      SgDoubleVal, SgLongDoubleVal or SgFloatVal
////////////////////////////////////////////////////////////////////////
NodeQuerySynthesizedAttributeType
queryFloatDoubleValExp (SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  switch(astNode->variantT()){
     case V_SgDoubleVal:
     case V_SgLongDoubleVal:
     case V_SgFloatVal:
        returnNodeList.push_back(astNode);
        break;
     default:  
        break;

  }

  return returnNodeList;
}
#endif

// DQ (4/13/2007): Moved from header file to source file.
AttributeListMap::AttributeListMap(SgFile* sageFilePtr)
   {
  // Set the block status to true as standard. This can be changed when a preprocessor conditional is evaluated as negative and later 
  // as positive.
  // rescan_macro_status should be 0 as long as we are not in the process of expanding_object_like_macro or expanding_function_like_macro
     rescan_macro_status = 0;
  // if rescan_macro_status==0 macro_call_to_expand should be NULL
     macro_call_to_expand = NULL;
  // if rescan_macro_status==0 then macro_expand_filename==""
     macro_expand_filename = "";
     skippedTokenStream = NULL;

     currentMapOfAttributes = attribute_map_type();
  // valueExpList = NodeQuery::querySubTree (sageFilePtr,&queryFloatDoubleValExp);
   }


  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //   The function
  //      attach_line_to_macro_call()
  //   will extract from the source file the lines containing the unexpanded macros and put it into the AST.
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void AttributeListMap::attach_line_to_macro_call(){
      attribute_map_type::iterator begin = currentMapOfAttributes.begin();
      attribute_map_type::iterator end   = currentMapOfAttributes.end();  
  #if 0 
      //Find all lines within files with macros and create a PreprocessingInfo object containing that line from
      //from the original source code
      while(begin!=end){
         vector<PreprocessingInfo*> infoVec = begin->second->getList();
         list<int> lineAlreadyRepl;
         for(int i = 0; i<infoVec.size(); ++i){
            if( infoVec[i]->getTypeOfDirective() == PreprocessingInfo::CMacroCall ){
                 lineAlreadyRepl.push_back(infoVec[i]->getLineNumber());
                 //   cout <<" " << infoVec[i]->directiveTypeName(infoVec[i]->getTypeOfDirective());
            }
//            if(find(lineAlreadyRepl.begin(), lineAlreadyRepl.end(), infoVec[i]->getLineNumber())==lineAlreadyRepl.end())
//                      lineAlreadyRepl.push_back(infoVec[i].getLineNumber()); 

         }
         lineAlreadyRepl.unique();
         lineAlreadyRepl.sort();

         string sourceFileName = (begin->first);
         char CurrentPath[MAXPATHLEN];
         getcwd(CurrentPath, MAXPATHLEN);
         if(sourceFileName.find("/",0)==string::npos)
             sourceFileName=string(CurrentPath)+"/"+sourceFileName;

         std::ifstream instream(sourceFileName.c_str());
         
         

         if (!instream.is_open()) {
           std::cerr << "Could not open input file: " << std::endl;
           ROSE_ASSERT(false==true);
         }

         //Extract from the original source-code the line containing the
         //unexpanded macro calls.
         int currentStreamLineNumber = 0;
         list<int>::iterator it_begin = lineAlreadyRepl.begin();
         list<int>::iterator it_end   = lineAlreadyRepl.end();
         while(it_begin != it_end){
            int macroCallLine = *it_begin;
            string lineUnexpandedMacros;
              
            while(currentStreamLineNumber != macroCallLine)
               if(currentStreamLineNumber < macroCallLine){
                   currentStreamLineNumber+=1;
                   ROSE_ASSERT(getline(instream,lineUnexpandedMacros).eof() == false);
                   if(currentStreamLineNumber==macroCallLine){
                      cout << std::endl<< "The unexpanded line " << macroCallLine << lineUnexpandedMacros << std::endl;
                      const char* pLine = lineUnexpandedMacros.c_str();
                      ROSE_ASSERT(pLine!=0);
                      ROSE_ASSERT(*pLine!='\0');

                      //AS(050106) Deactivated the macro replacement functionality until it has been tested more
                      //begin->second->addElement(PreprocessingInfo::LineReplacement,lineUnexpandedMacros,sourceFileName , macroCallLine,1, 1);
                      break; 
                    }
               } 
             
            ++it_begin;
         }
      
         ++begin;
      }
  #endif
}

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  The function
  //          addElement(boost::wave::token_id,...)
  //  will register a preprocessor directive when it is found (e.g #define, #if, #endif etc) and will translate it from Wave form into ROSE form. It needs
  //  the helper function addElement(boost::wave::token_category,...) to connect it to the body. E.g in #define NAME VALUE #define is registered by this function
  //  but the NAME and VALUE is found by addElement(boost::wave::token_id,...).
  //  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void AttributeListMap::addElement(boost::wave::token_id wave_typeid, const string pLine, const string filename, int lineNumber,
                                    int columnNumber, int numberOfLines){
 //Map between the wave preprocessorid to the ROSE preprocessorid
 PreprocessingInfo::DirectiveType rose_typeid;
 using namespace boost::wave;

 //Translate from wave to rose type-id notation
 switch(wave_typeid){
    case T_PP_DEFINE:    //#define
       rose_typeid = PreprocessingInfo::CpreprocessorDefineDeclaration;
       break;
    case T_PP_IFDEF:     //#ifdef
       rose_typeid = PreprocessingInfo::CpreprocessorIfdefDeclaration;
       break;
    case T_PP_IFNDEF:    //#ifndef
       rose_typeid = PreprocessingInfo::CpreprocessorIfndefDeclaration;
       break;
    case T_PP_IF:        //#if
       rose_typeid = PreprocessingInfo::CpreprocessorIfdefDeclaration;
       break;
    case T_PP_ELIF:      //#elif
       rose_typeid = PreprocessingInfo::CpreprocessorElifDeclaration;
       break;
    case T_PP_ELSE:      //#else
       rose_typeid = PreprocessingInfo::CpreprocessorElseDeclaration;
       break;
    case T_PP_ENDIF:      //#endif
        rose_typeid = PreprocessingInfo::CpreprocessorEndifDeclaration;
        break;
    case T_CPPCOMMENT:   //  // ... \n
       rose_typeid = PreprocessingInfo::CplusplusStyleComment;
       break;
    case T_CCOMMENT:     ///* ... */
       rose_typeid = PreprocessingInfo::C_StyleComment;
       break;
    case T_PP_ERROR:     //#error
       rose_typeid = PreprocessingInfo::CpreprocessorErrorDeclaration;
       break;
    case T_PP_LINE:      //#line
       rose_typeid = PreprocessingInfo::CpreprocessorLineDeclaration;
       break;
    case T_PP_UNDEF:     //#undef
       rose_typeid = PreprocessingInfo::CpreprocessorUndefDeclaration;
       break;
    case T_PP_WARNING:   //#warning
       rose_typeid = PreprocessingInfo::CpreprocessorWarningDeclaration;
       break;
    case T_PP_QHEADER:   //#include "..."
    case T_PP_HHEADER:   //#include <...>
    case T_PP_INCLUDE:   //#include "..."
       rose_typeid = PreprocessingInfo::CpreprocessorIncludeDeclaration;
       break;
    case T_PP_PRAGMA:    //#pragma
      // Part of the AST in ROSE. Do not handle it this way 
      return;
      break; 

    default:
      // This case should not be reached 
      ROSE_ASSERT(false==true);
      rose_typeid  = PreprocessingInfo::CpreprocessorUnknownDeclaration;
      break;
 
 }

 // 
 switch(wave_typeid){
    case T_PP_IF:
    case T_PP_IFDEF:
    case T_PP_IFNDEF:
         current_directive.wave_id = wave_typeid;
         current_directive.rose_id = rose_typeid;
         current_directive.accumulate = pLine.c_str();
         current_directive.lineNumber = lineNumber;
         current_directive.columnNumber = columnNumber;
         current_directive.numberOfLines = numberOfLines;
         break;
    default:
         string outputstring(pLine.c_str());
         //If the if block status is false this will be the first directive after an skipped part due to
         //an evaluation of a preprocessor conditional. Then we would like to include skipped directives into
         //the current string
         if(currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
              currentMapOfAttributes[filename] = new ROSEAttributesList();
         currentMapOfAttributes.find(filename)->second->addElement(rose_typeid,pLine.c_str() , filename ,lineNumber, columnNumber, numberOfLines);  

         break;
 }

   
}
#endif

#endif

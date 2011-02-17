#ifndef  XYZ_ATTRIBUTE_LIST_MAP
#define XYZ_ATTRIBUTE_LIST_MAP

#define DEBUG_WAVE_ROSE_CONNECTION 1
#define DEBUG_USE_ROSE_BOOST_WAVE_SUPPORT 1

// DQ (11/30/2008): This will be defined within the header file system for ROSE.
// #include <map>
// Header files needed for ROSE

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#ifndef _MSC_VER
#include <boost/wave.hpp>
#endif

///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

// DQ: I don't think this forward declaration is required...
// class AttributeListMap;

////////////////////////////////////////////////////////////////////////
// Function queryFloatDoubleValExp()  finds all nodes with type 
//      SgDoubleVal, SgLongDoubleVal or SgFloatVal
////////////////////////////////////////////////////////////////////////
NodeQuerySynthesizedAttributeType
queryFloatDoubleValExp (SgNode * astNode);


//class advanced_preprocessing_hooks;
//#include "advanced_preprocessing_hooks.hpp"

class AttributeListMap {

     private:
       //rescan_macro_status is a helper for extracting the corresponding expanded macro to a macro call from Wave. 
       //This variable makes it so that the complexity of the macro expansion process can be ignoredd while still
       //safely extracting the expanded string.
          int  rescan_macro_status;
          std::string macro_expand_filename;
       //If rescan_macro_status!=0 the variable macro_call_to_expand should point to the macro call which is expanded. If other macros are 
       //called as an argument or as part of the macro definition corresponding to the macro call this data is *not* inside this variable.
          PreprocessingInfo::rose_macro_call* macro_call_to_expand;
          token_container currentTokSeq;

       //For optimization and practical purposes a list of preprocessor attributes is created
       //for each file. Since the preprocessor does an auxiliary pass over the AST a map
       //is created mapping the filename to the ROSEAttributesList.

     public:
       //A map of a filename to an ROSEAttributesList. All preprocessingdirectives and macros found in a file will be put into this list.
          typedef  std::map<std::string,ROSEAttributesList*> attribute_map_type;
          attribute_map_type currentMapOfAttributes;

          token_container* skippedTokenStream;

       //A map over all macro definitions found and their corresponding {filename, line number} 
          std::map<std::pair<std::string,int>, PreprocessingInfo*> defMap;

  // DQ (4/13/2007): These were required to support holding literals as strings (not required as a result of Rama's work).
  // std::list<SgNode*> valueExpList;
  // DQ (4/13/2007): This is due to a bug in ROSE, or a misunderstanding about constant folded values and their source position information.
  // token_type lastOperator;

          AttributeListMap(SgFile* sageFilePtr);

          template <typename TokenT> bool found_include_directive(TokenT directive, std::string relname, std::string absname );
      


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //  The function
    //           found_directive(TokenT const&, ...)
    //  will register a preprocessor directive with an expression (e.g #if, #ifdef etc) and will attach it to the AST.
    //  
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       template <typename TokenT, typename ContainerT>
               bool
               found_directive(TokenT const& directive, ContainerT const& expression, bool expression_value)
                  {
                 //Map between the wave preprocessorid to the ROSE preprocessorid
                    PreprocessingInfo::DirectiveType rose_typeid;
                    using namespace boost::wave;
                    token_id wave_typeid = token_id(directive);

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
                            rose_typeid = PreprocessingInfo::CpreprocessorIfDeclaration;
                            break;
                         case T_PP_ELIF:      //#elif
             // DQ (4/17/2008): Commented out to avoid fly-make interpreting as an error!
          // std::cout << "ELIF\n";
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
                            if(SgProject::get_verbose() >= 1)
                                    std::cout << "Token to include directive: " << directive.get_value() << std::endl;        
                            rose_typeid = PreprocessingInfo::CpreprocessorIncludeDeclaration;
                            break;
                         case T_PP_PRAGMA:    //#pragma
                            // Part of the AST in ROSE. Do not handle it this way 
                            return false;
          // DQ (8/29/2009): Unreachable statement after "return" 
          // break; 

                         default:
                            // This case should not be reached 
                            ROSE_ASSERT(false==true);
                            rose_typeid  = PreprocessingInfo::CpreprocessorUnknownDeclaration;
                            break;

                    }
                    if(SgProject::get_verbose() >= 1){

                            std::cout << "THE FOUND DIRECTIVE IS: " << directive.get_value().c_str() << std::endl;
                            std::cout << "THE FOUND DEF IS: " << boost::wave::util::impl::as_string(expression) << std::endl;
                    }
                    // 
                    std::string filename(directive.get_position().get_file().c_str());
                    token_list_container tokListCont;

                    copy (expression.begin(), expression.end(),
                                    inserter(tokListCont, tokListCont.end()));



                    if(currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
                            currentMapOfAttributes[filename] = new ROSEAttributesList();
                    currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(directive,tokListCont,expression_value,rose_typeid,PreprocessingInfo::before)));

                 return false;
                  }

        // King84 (2010.09.09): Added to support getting arguments for conditionals which had been skipped
        template <typename TokenT, typename ContainerT>
        void
        update_token(TokenT const& token, ContainerT const& stream, bool expression_value)
        {
//              typename TokenT::position_type::string_type filename(token.get_position().get_file().c_str());
                std::string filename(token.get_position().get_file().c_str());
                // ensure that the file exists
                ROSE_ASSERT(currentMapOfAttributes.find(filename) != currentMapOfAttributes.end());
                // get the list of attributes
                std::vector<PreprocessingInfo*>& infos = currentMapOfAttributes.find(filename)->second->getList();
                for(std::vector<PreprocessingInfo*>::reverse_iterator i = infos.rbegin(); i != infos.rend(); ++i)
                {
                        // match this one
                        if ((*(*i)->get_token_stream())[0] == token && (*(*i)->get_token_stream())[0].get_position() == token.get_position())
                        {
                                // TODO: something with expression_value once the PreprocessingInfo actually decides to start storing it
                                for (typename ContainerT::const_iterator item = stream.begin(); item != stream.end(); ++item)
                                {
                                        (*i)->push_back_token_stream(*item);
                                }
                                return;
                        }
                }
                ROSE_ASSERT(!"Token to update not found!");
        }


       ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       //  The function
       //           found_directive(TokenT const&, ...)
       //  will register a preprocessor directive when it is found (e.g #define, #if, #endif etc) and will attach it to the AST.
       //  
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       template <typename TokenT>
               void
               found_directive(TokenT const& directive)
               {
                       //Map between the wave preprocessorid to the ROSE preprocessorid
                       PreprocessingInfo::DirectiveType rose_typeid;
                       using namespace boost::wave;
                       token_id wave_typeid = token_id(directive);


                       //Translate from wave to rose type-id notation
                       switch(wave_typeid){
                               case T_PP_IFDEF:     //#ifdef
                                                rose_typeid = PreprocessingInfo::CpreprocessorIfdefDeclaration;
                                                break;
                               case T_PP_IFNDEF:    //#ifndef
                                                rose_typeid = PreprocessingInfo::CpreprocessorIfndefDeclaration;
                                                break;
                               case T_PP_IF:        //#if
                                                rose_typeid = PreprocessingInfo::CpreprocessorIfDeclaration;
                                                break;
                                       //These directive have expression attached to them and should be handled
                                       //by  found_directive(TokenT const& directive, ContainerT const& expression)
                                       ROSE_ASSERT(false); 
                                       break;
                               case T_PP_DEFINE:    //#define
                                       rose_typeid = PreprocessingInfo::CpreprocessorDefineDeclaration;
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
                               case T_PP_INCLUDE:   //#include ...
                                       if(SgProject::get_verbose() >= 1)
                                               std::cout << "Token to include directive: " << directive.get_value() << std::endl;        
                                       rose_typeid = PreprocessingInfo::CpreprocessorIncludeDeclaration;
                                       break;
                               case boost::wave::T_PP_HHEADER_NEXT:
                                       if(SgProject::get_verbose() >= 1)
                                               std::cout << "Token to include next directive: " << directive.get_value() << std::endl;        
                                       rose_typeid = PreprocessingInfo::CpreprocessorIncludeNextDeclaration;

                                       break;
                               case T_PP_PRAGMA:    //#pragma
                                       // Part of the AST in ROSE. Do not handle it this way 
                                       return;

                // DQ (8/29/2009): Unreachable statement after "return" 
                // break; 

                               default:
                   {
                                      // This case should not be reached 

                  // DQ (8/29/2009): It is a horrible idea to only fail when verbose mode is turned on!
                                         if (SgProject::get_verbose() >= 1)
                        {
                          std::cout << "Error: Unknown preprocessor declaration found : " << directive.get_value().c_str() << std::endl;
                          std::cout << boost::wave::get_token_name(wave_typeid) << " " << directive.get_position().get_file().c_str() << " " << directive.get_position().get_line() 
                                    << " " << directive.get_position().get_column() << std::endl;
                          ROSE_ASSERT(false);
                        }

                     rose_typeid  = PreprocessingInfo::CpreprocessorUnknownDeclaration;
                     break;
                                       }

                       }

                       //std::cout << "THE FOUND DIRECTIVE IS: " << directive.get_value().c_str() << std::endl;

                       token_container currentTokSeq2;
                       currentTokSeq2.push_back(directive);
                       std::string filename(directive.get_position().get_file().c_str());
                       if(currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
                               currentMapOfAttributes[filename] = new ROSEAttributesList();
                       currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(currentTokSeq2,rose_typeid,PreprocessingInfo::before)));


               }



       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       //   The function
       //      PreprocessingInfo* build_preprocessingInfo_macro_def(TokenT&
       //                          macro_name, bool is_functionlike,..)
       //   will create a PreprocessingInfo object for a preprocessor
       //   defined macro definition. E.g
       //     #ifdef NOT_DEFINED_MACRO
       //     #endif
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

       template<typename TokenT, typename ParametersT, typename DefinitionT> 
               PreprocessingInfo* build_preprocessingInfo_macro_def(TokenT& macro_name, bool is_functionlike,
                               ParametersT& parameters, DefinitionT &definition, bool is_predefined){

                       PreprocessingInfo::rose_macro_definition* macro_def = new PreprocessingInfo::rose_macro_definition();

                       //Make copies of all the variables for ROSE 
                       macro_def->is_functionlike = is_functionlike;
                       macro_def->is_predefined   = is_predefined; 

                       macro_def->macro_name = macro_name;
                       macro_def->paramaters = parameters;

                       copy (definition.begin(), definition.end(),
                                       inserter(macro_def->definition, macro_def->definition.end()));
                       PreprocessingInfo* preprocMacroDef = new PreprocessingInfo(macro_def,PreprocessingInfo::before);

                       return preprocMacroDef;
               }

       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       //   The function 
       //               defined_macro<TokentT,ParametersT, DefinitionT>
       //   takes a macro definition as paramaters and convert it into a PreprocessingInfo object for the AST
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       template<typename TokenT, typename ParametersT, typename DefinitionT> 
               void defined_macro(TokenT& macro_name, bool is_functionlike,
                               ParametersT& parameters, DefinitionT &definition, bool is_predefined){
                       if(skippedTokenStream != NULL){

                               if(macro_name.get_position().get_file().size()!=0){  
                                       if(macro_name.get_position().get_file()!="<built-in>") 
                                               skipped_token(macro_name,true);
                               }else if(SgProject::get_verbose() >= 1)
                                       std::cout << "SKIPPED BECAUSE FILE IS NULL: " << macro_name.get_value().c_str() << std::endl;

                       }
                       //All the information in the paramaters here comes from an  macro_definition object declared in boost/wave/utils/macro_defintion.hpp
                       //The real values of the typename which is found in this class is
                       //          typedef std::vector<TokenT> parameter_container_t;
                       //          typedef ContainerT          definition_container_t;

                       //From cpp_context
                       //DefinitionT is a ContextT::token_sequence_type which is the type of a token sequence defined in cpp_context
                       //    typedef std::list<token_type, boost::fast_pool_allocator<token_type> > token_sequence_type;

                       PreprocessingInfo* preprocMacroDef = build_preprocessingInfo_macro_def(macro_name, is_functionlike, parameters, definition, is_predefined);
                       ROSE_ASSERT(preprocMacroDef != NULL);

                       Sg_File_Info* file_info = preprocMacroDef->get_file_info();
                       ROSE_ASSERT(file_info != NULL);

                       std::string filename = file_info->get_filenameString();

                       //Make sure that the macro definition has been declared within a file
                       //and not on the commandline
                       ROSE_ASSERT(filename != "");

                       if(currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
                               currentMapOfAttributes[filename] = new ROSEAttributesList();
                       if(SgProject::get_verbose() >= 1)
                               std::cout << "DONE adding to map" << std::endl;
                       currentMapOfAttributes.find(filename)->second->addElement(*preprocMacroDef);
                       if(SgProject::get_verbose() >= 1)
                               std::cout << "Before mapKey" << std::endl;
                       //Insert the macro definition into the map of macro definitions (used for macro calls)
                       std::pair<std::string,int> mapKey(filename,macro_name.get_position().get_line());
                       if(SgProject::get_verbose() >= 1){
                               std::cout << "After mapKey" << std::endl;

                               std::cout << "ASXXX Defining macro: " << macro_name.get_value().c_str() << std::endl;
                               std::cout << "at " << filename << " l" << mapKey.second << std::endl;
                       }

                       //A macro can not be defined twice
                       //ROSE_ASSERT( defMap.find(mapKey) == defMap.end() );
                       //Although a macro can not be defined twice it is possible that a macro
                       //can be undefined and then defined again. Since we do not yet use the
                       //#undef information we should just consolidate those macro definitions
                       //into one macro
                       if( defMap.find(mapKey) == defMap.end() )
                               defMap[mapKey]=preprocMacroDef;

               }

       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       //   The function 
       //               addElement<TokentT,ContainerT>
       //   takes an expansion of a function like macro as paramater and convert it into a PreprocessingInfo::rose_macro_call object for the AST
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       template<typename TokenT, typename ContainerT> 
               bool expanding_function_like_macro(TokenT const &macrodef, std::vector<TokenT> const &formal_args,
                               ContainerT const &definition, TokenT const &macrocall, std::vector<ContainerT> const &arguments){
                       if(skippedTokenStream != NULL)
                               skipped_token(macrodef,true);

                       ////////////////////////////////////////////////////////////////////
                       //
                       //Note from AS:
                       //The typename ContainerT is often equivalent to list<TokenT>.
                       //TokentT is often equivalent to  boost::wave::cpplexer::lex_token<>
                       //found in boost_1_33_0/boost/wave/cpplexer/cpp_lex_token.hpp.
                       //
                       ///////////////////////////////////////////////////////////////////
                       //All the information in the paramaters here comes from an  macro_definition object declared in boost/wave/utils/macro_defintion.hpp
                       //The real values of the typename which is found in this class is
                       //          typedef std::vector<TokenT> parameter_container_t;
                       //          typedef ContainerT          definition_container_t;

                       //From cpp_context
                       //DefinitionT is a ContextT::token_sequence_type which is the type of a token sequence defined in cpp_context
                       //    typedef std::list<token_type, boost::fast_pool_allocator<token_type> > token_sequence_type;

                       if(rescan_macro_status==0){

                               if(SgProject::get_verbose() >= 1){

                                       std::cout << "DEFINITION: " << boost::wave::util::impl::as_string(definition);

                                       std::cout << "\n MACRO CALL: " << macrocall.get_value() << std::endl;
                               }
                               //have to implement mechanism to find macro definition here 
                               ROSE_ASSERT(macro_call_to_expand == NULL);
                               macro_call_to_expand = new PreprocessingInfo::rose_macro_call();
                               macro_call_to_expand->is_functionlike = true; 
                               macro_call_to_expand->macro_call      = macrocall;

                               //           std::pair<std::string,int> mapKey(string(definition.begin()->get_position().get_file().c_str()),
                               //                            definition.begin()->get_position().get_line());

                               std::pair<std::string,int> mapKey;

                               if(macrodef.get_position().get_file()!="<built-in>"){
                                       mapKey.first  =  string(macrodef.get_position().get_file().c_str());
                                       mapKey.second = macrodef.get_position().get_line();
                                       ROSE_ASSERT( defMap.find(mapKey) != defMap.end() );
                                       macro_call_to_expand->macro_def = defMap[mapKey];
                                       //     std::cout << definition.begin()->get_position().get_line() << std::endl;

                               }else{
                                       bool is_function_like = true;
                                       bool is_predefined    = false;
                                       macro_call_to_expand->macro_def = build_preprocessingInfo_macro_def(macrodef, is_function_like, formal_args, definition, is_predefined);
                               }
                               
                               if( (defMap.find(mapKey) == defMap.end())&&(SgProject::get_verbose() >= 1) ){
                                       std::cout << "Did not find: " <<  macrodef.get_value().c_str() <<  " " << boost::wave::util::impl::as_string(definition) << std::endl;
                                       std::cout << "in " << mapKey.first << " l " << mapKey.second << std::endl;
                               };


                               //       typedef typename std::vector<std::list<token_type,boost::fast_pool_allocator<token_type> > > testType;
                               typedef typename std::vector<ContainerT>::const_iterator vec_call_iterator_t;

                               //BEGIN: Make a copy of the arguments

                               vec_call_iterator_t it = arguments.begin();
                               vec_call_iterator_t it_end = arguments.end();

                               //extract the arguments in the macro call from wave    

                               //ARGUMENTS
                               if(SgProject::get_verbose() >= 1)
                                       std::cout << "ARGUMENTS:\n";

                               while (it != it_end ){
                                       if(SgProject::get_verbose() >= 1){
                                               std::cout << boost::wave::util::impl::as_string(*it);
                                       }
                                       std::list<token_type> tk;

                                       copy (it->begin(), it->end(),
                                                       inserter(tk, tk.end()));

                                       macro_call_to_expand->arguments.push_back(tk);
                                       ++it;
                               }

                               //END: Make a copy of the arguments
                       }

                       ++rescan_macro_status;


#if 0
                       typename testType::iterator it2 = test.begin();
                       typename testType::iterator it_end2 = test.end();

                       while (it2 != it_end2 ){
                               std::cout<<  boost::wave::util::impl::as_string(*it2);
                               ++it2;
                       }
#endif
               return false;        

               }

       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       //   The function 
       //               expanding_object_like_macro<TokentT,ContainerT>
       //   takes an expansion of a object like macro as paramater and convert it into a PreprocessingInfo::macr_call object for the AST
       //  The macroname parameter marks the position, where the macro to expand 
       //  is defined.
       //  The definition parameter holds the macro definition for the macro to 
       //  trace.
       //
       //  The macro call parameter marks the position, where this macro invoked.
       //
       ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       template<typename TokenT, typename ContainerT> 
               bool expanding_object_like_macro(TokenT const &macro, ContainerT const &definition, TokenT const &macrocall){
                       if(skippedTokenStream != NULL)
                               skipped_token(macro,true);

                       //All the information in the paramaters here comes from an  macro_definition object declared in boost/wave/utils/macro_defintion.hpp
                       //The real values of the typename which is found in this class is
                       //          typedef std::vector<TokenT> parameter_container_t;
                       //          typedef ContainerT          definition_container_t;

                       //From cpp_context
                       //DefinitionT is a ContextT::token_sequence_type which is the type of a token sequence defined in cpp_context
                       //    typedef std::list<token_type, boost::fast_pool_allocator<token_type> > token_sequence_type;

                       //Put the macro_definition into the AST

                       //have to implement mechanism to find macro definition here 

                       if(rescan_macro_status==0){
                               ROSE_ASSERT(macro_call_to_expand == NULL);
                               if(SgProject::get_verbose() >= 1)
                                       std::cout << "DEFINITION: " << boost::wave::util::impl::as_string(definition);

                               macro_call_to_expand = new PreprocessingInfo::rose_macro_call();

                               macro_call_to_expand->is_functionlike = false;
                               macro_call_to_expand->macro_call      = macrocall;

                               std::pair<std::string,int> mapKey;

                               if( (macro.get_position().get_file().size() != 0 ) && (macro.get_position().get_file()!="<built-in>") 
                                   && (macro.get_position().get_file()!="<command line>") ){
                                       mapKey.first  = string(macro.get_position().get_file().c_str());
                                       mapKey.second = macro.get_position().get_line();

                                       if( defMap.find(mapKey) == defMap.end() ){
                                               std::cout << "Did not find: " <<  macro.get_value().c_str() << "  " << 
                                                       macrocall.get_value().c_str() <<  " " << boost::wave::util::impl::as_string(definition) << std::endl;
                                               std::cout << "in " << mapKey.first << " l " << mapKey.second << std::endl;
                                       };

                                       ROSE_ASSERT( defMap.find(mapKey) != defMap.end() );
                                       macro_call_to_expand->macro_def = defMap[mapKey];
                                       //                std::cout << definition.begin()->get_position().get_line() << std::endl;

                               }else{

                                       bool is_function_like = false;
                                       bool is_predefined    = false;

                                       token_container parameters;
                                       macro_call_to_expand->macro_def = build_preprocessingInfo_macro_def(macro, is_function_like, parameters, definition, is_predefined);
                               }


                               if(SgProject::get_verbose() >= 1)
                                       std::cout << "\n MACRO CALL: " << macrocall.get_value() << std::endl;
                       }

                       ++rescan_macro_status;
                return false;
               }

       ///////////////////////////////////////////////////////////////////////////
       //  
       //  The function 'expanded_macro' is called, whenever the expansion of a 
       //  macro is finished but before the rescanning process starts.
       //
       //  The parameter 'result' contains the token sequence generated as the 
       //  result of the macro expansion.
       //
       ///////////////////////////////////////////////////////////////////////////
       template <typename ContainerT>
               void expanded_macro(ContainerT const &result)
               {

               }


       ///////////////////////////////////////////////////////////////////////////
       //  
       //  The function 'rescanned_macro' is called, whenever the rescanning of a 
       //  macro is finished. It connects the macro_call and expanded macro call
       //  from Wave, and inserts it into the AST.
       //
       //  The parameter 'result' contains the token sequence generated as the 
       //  result of the rescanning.
       //
       ///////////////////////////////////////////////////////////////////////////
       template <typename ContainerT>
               void rescanned_macro(ContainerT const &result)
               {

                       //Attach the expanded macro to the macro call
                       --rescan_macro_status;
                       ROSE_ASSERT( rescan_macro_status >= 0 );
#if 0
                       std::cout << "Rescanned macro: " << boost::wave::util::impl::as_string(result) << std::endl;
#endif
                       if(rescan_macro_status==0){
                               ROSE_ASSERT(macro_call_to_expand != NULL);

                               copy (result.begin(), result.end(),
                                               inserter(macro_call_to_expand->expanded_macro, macro_call_to_expand->expanded_macro.end()));


                               //Attach the define to ROSE
                               std::string filename(macro_call_to_expand->macro_call.get_position().get_file().c_str());
                               if(currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
                                       currentMapOfAttributes[filename] = new ROSEAttributesList();
                               currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(macro_call_to_expand,PreprocessingInfo::before)));
                               macro_call_to_expand = NULL;

                       }
               }

       //////////////////////////////////////////////////////////////////////////////////////
       // The functor
       //     struct findDirective: public std::binary_function<TokenIterator,DirectiveType,bool>
       // helps to see if TokenIterator is of type directive.
       ///////////////////////////////////////////////////////////////////////////////////// 

       template<typename TokenIterator, typename DirectiveType>
               struct findDirective: public std::binary_function<TokenIterator,DirectiveType,bool>
               {
                       bool operator()(TokenIterator node, DirectiveType directive) const{
                               bool returnValue = false;

                               using namespace boost::wave;

                               token_id wave_typeid = token_id(node);

                               if(wave_typeid == directive)
                                       returnValue = true;

                               return returnValue;
                       };

               };

       //////////////////////////////////////////////////////////////////////////////////////
       // The functor
       //     struct findDirectiveInList: public
       //     std::binary_function<TokenIterator,std::list<DirectiveType>,bool>
       // helps to see if the token is of one of the types in directiveList.
       ///////////////////////////////////////////////////////////////////////////////////// 
       template<typename TokenIterator, typename DirectiveType>
               struct findDirectiveInList: public std::binary_function<TokenIterator,std::list<DirectiveType>,bool>
               {
                       bool operator()(TokenIterator node, std::list<DirectiveType> directiveList) const{
                               bool returnValue = false;

                               using namespace boost::wave;

                               token_id wave_typeid = token_id(node);
#if 0
             // DQ (4/17/2008): Commented out to avoid error in fly-make!
                               if( T_PP_ELIF  == wave_typeid)
                                       std::cout << "Found an #elif\n";
#endif

                               if(std::find(directiveList.begin(),directiveList.end(), wave_typeid) != directiveList.end())
                                       returnValue = true;

                               return returnValue;
                       };

               };

                template <typename TokenT>
                void
                skipped_token(TokenT const& token, bool last_skipped = false)
                {
                        //The skipped_token(...) function is called whenever a 
                        //part of a preprocessing #if #else/#elif #endif conditional
                        //is evaluated as false. The false part is handled here.
                        //King84 (2010.09.09):
                        //This does not handle the test conditions of #elif or #if
                        //Those are handled via update_token(...).  For now, other
                        //directives that are within the false branch are just skipped
                        //and don't have directives built from them.

                        if(skippedTokenStream == NULL)
                                skippedTokenStream = new token_container();

                        if(last_skipped == false)
                        {
                                if(SgProject::get_verbose() >= 1)
                                        std::cout << "Pushed Skipped Token: " << token.get_value().c_str() << std::endl;
                                skippedTokenStream->push_back(token);
                        }
                        else
                        {
                                skippedTokenStream->push_back(token);

                                if(SgProject::get_verbose() >= 1)
                                {
                                        std::cout << "Pushed Skipped Token: " << token.get_value().c_str() << std::endl;
                                        std::cout << "Popping Skipped Tokens: " << boost::wave::util::impl::as_string(*skippedTokenStream).c_str() << std::endl;
                                }
                                std::string filename(skippedTokenStream->begin()->get_position().get_file().c_str());
                                if (currentMapOfAttributes.find(filename) == currentMapOfAttributes.end())
                                        currentMapOfAttributes[filename] = new ROSEAttributesList();
                                currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(*skippedTokenStream, PreprocessingInfo::CSkippedToken, PreprocessingInfo::before)));

                                ROSE_ASSERT(skippedTokenStream != NULL);
                                delete skippedTokenStream;
                                skippedTokenStream = NULL;
                        }
                        if (SgProject::get_verbose() >= 1)
                                std::cout << "SKIPPED TOKEN: " << token.get_value().c_str() << std::endl;
                }

                inline void flush_token_stream()
                {
                        if (skippedTokenStream == NULL || skippedTokenStream->begin() == skippedTokenStream->end())
                                return;

                        std::string filename(skippedTokenStream->begin()->get_position().get_file().c_str());
                        if (currentMapOfAttributes.find(filename) == currentMapOfAttributes.end())
                                currentMapOfAttributes[filename] = new ROSEAttributesList();
                        currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(*skippedTokenStream, PreprocessingInfo::CSkippedToken, PreprocessingInfo::before)));
                        delete skippedTokenStream;
                        skippedTokenStream = NULL;
                }

                template <typename ContextT, typename TokenT>
            bool
            may_skip_whitespace(ContextT const& ctx, TokenT& token, bool& skipped_newline)
            {
                        using namespace boost::wave;
                        //After the last skipped token has been processed the first non-skipped
                        //token will trigger this call to attach the skipped tokens to the AST
                        token_id id = token_id(token);
                        bool skip = id == T_PP_ELSE || id == T_PP_ELIF || id == T_PP_ENDIF;
                        using namespace boost::wave;
                        if(SgProject::get_verbose() >= 1)
                        {
                                switch(token_id(token))
                                {
                                        case T_PP_DEFINE:    std::cout << "Skip White: #define\n"; break;
                                        case T_PP_IF:        std::cout << "Skip White: #if\n"; break;
                                        case T_PP_IFDEF:     std::cout << "Skip White: #ifdef\n"; break;
                                        case T_PP_IFNDEF:    std::cout << "Skip White: #ifndef\n"; break;
                                        case T_PP_ELSE:      std::cout << "Skip White: #else\n"; break;
                                        case T_PP_ELIF:      std::cout << "Skip White: #elif\n"; break;
                                        case T_PP_ENDIF:     std::cout << "Skip White: #endif\n"; break;
                                        case T_PP_ERROR:     std::cout << "Skip White: #error\n"; break;
                                        case T_PP_LINE:      std::cout << "Skip White: #line\n"; break;
                                        case T_PP_PRAGMA:    std::cout << "Skip White: #pragma\n"; break;
                                        case T_PP_UNDEF:     std::cout << "Skip White: #undef\n"; break;
                                        case T_PP_WARNING:   std::cout << "Skip White: #warning\n"; break;
                                        case T_PP_INCLUDE:   std::cout << "Skip White: #include \"...\"\n"; break;
                                        case T_PP_QHEADER:   std::cout << "Skip White: #include <...>\n"; break;
                                        case T_PP_HHEADER:   std::cout << "Skip White: #include ...\n"; break;
                                        default:             std::cout << "Skip White: <something else (" << token.get_value() << ")>\n"; break;
                                }
                        }
                        if (skip)
                        {
                                skipped_token(token, true);
                        }
                        else
                        {
                                if (skippedTokenStream != NULL) // flush the tokens and don't add this one (instead of calling skipped_token)
                                {
                                        if(SgProject::get_verbose() >= 1)
                                        {
                                                std::cout << "Whitespace makes us pop skipped tokens: " << boost::wave::util::impl::as_string(*skippedTokenStream).c_str() << std::endl;
                                        }
                                        flush_token_stream();
                                }
                                else if(SgProject::get_verbose() >= 1)
                                        std::cout << "Token stream is null?" << std::endl;
                        }

               if(SgProject::get_verbose() >= 1)
                       std::cout << "MAX_SKIP_WHITESPACE: " << token.get_value().c_str() << std::endl;
               return false; 
       }


       ////////////////////////////////////////////////////////////////////////////////////  
       //The function 
       //     attach_line_to_macro_call()
       //will replace the line where a macro is called with the line from
       //the sourcecode.
       //WARNING! Currently disabled
       ///////////////////////////////////////////////////////////////////////////////////
       void attach_line_to_macro_call();


       template<typename IteratorT>
         void attach_line_to_macro_call(std::vector<IteratorT*> vec){
               }

       template <typename StringT, typename IteratorT>
               inline StringT
               as_string(IteratorT it, IteratorT end)
               {
                       StringT result;
                       for (/**/; it != end; ++it)
                       {
                               result += (*it).get_value();
                       }
                       return result;
               }
};


template <typename TokenT>
bool AttributeListMap::found_include_directive(TokenT directive, std::string relname, std::string absname )
   {
     boost::wave::token_id wave_typeid = boost::wave::token_id(directive);

     switch(wave_typeid)
        {
          case boost::wave::T_PP_INCLUDE:
               break;
          case boost::wave::T_EOI: //end of input reached
               break;
          default:

               if( directive.get_position().get_file().size() == 0 ) break;

               PreprocessingInfo::r_include_directive* inclDir = new PreprocessingInfo::r_include_directive;

               inclDir->directive = directive;
               inclDir->absname   = absname;
               inclDir->relname   = relname;

               std::string filename(directive.get_position().get_file().c_str());
               if (currentMapOfAttributes.find(filename)==currentMapOfAttributes.end())
                    currentMapOfAttributes[filename] = new ROSEAttributesList();
               currentMapOfAttributes.find(filename)->second->addElement(*(new PreprocessingInfo(inclDir, PreprocessingInfo::before ) ));
               break;                                     
        }
        return false;
   }

#endif



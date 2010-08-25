/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2001-2005 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_WAVE_ADVANCED_PREPROCESSING_HOOKS_INCLUDED)
#define BOOST_WAVE_ADVANCED_PREPROCESSING_HOOKS_INCLUDED

// ROSE standard include files

#include <cstdio>
#include <ostream>
#include <string>

#include <boost/assert.hpp>
#include <boost/config.hpp>

#include <boost/wave/token_ids.hpp>
#include <boost/wave/util/macro_helpers.hpp>
#include <boost/wave/preprocessing_hooks.hpp>

#include "attributeListMap.h"

#if 0
namespace {

    char const *get_directivename(boost::wave::token_id id)
    {
        using namespace boost::wave;
        switch (static_cast<unsigned int>(id)) {
        case T_PP_IFDEF:          return "#ifdef";
        case T_PP_IFNDEF:         return "#ifndef";
        case T_PP_IF:             return "#if";
        case T_PP_ELSE:           return "#else";
        case T_PP_ELIF:           return "#elif";
        case T_PP_ENDIF:          return "#endif";
        default:
            return "#unknown directive";
        }
    }
}

#endif

///////////////////////////////////////////////////////////////////////////////
//  
//  The advanced_preprocessing_hooks policy class is used to register some
//  of the more advanced (and probably more rarely used hooks with the Wave
//  library.
//
//  This policy type is used as a template parameter to the boost::wave::context<>
//  object.
//
///////////////////////////////////////////////////////////////////////////////
//template<typename ContextT>
class advanced_preprocessing_hooks
:   public boost::wave::context_policies::default_preprocessing_hooks
{

public:
    AttributeListMap* attributeListMap;
    token_type lastPreprocDirective;
    int numberOfIfs;

    token_type last_elif;

    std::string includeDirective;
    token_type includeDirectiveToken;    
    advanced_preprocessing_hooks() /*: need_comment(true)*/ {
       numberOfIfs = 0;
       using namespace boost::wave;
       last_elif = token_type(boost::wave::T_PP_ELIF,"#elif",boost::wave::util::file_position_type(BOOST_WAVE_STRINGTYPE(),0,0));

    }

    
    
       ////////////////////////////////////////////////////////////////////
       //
       //Note from AS:
       //The typename ContainerT is often equivalent to list<TokenT>.
       //TokentT is often equivalent to  boost::wave::cpplexer::lex_token<>
       //found in boost_1_33_0/boost/wave/cpplexer/cpp_lex_token.hpp.
       //
       ///////////////////////////////////////////////////////////////////


 

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_function_like_macro' is called, whenever a 
    //  function-like macro is to be expanded.
    //
    //  The macroname parameter marks the position, where the macro to expand 
    //  is defined.
    //  The formal_args parameter holds the formal arguments used during the
    //  definition of the macro.
    //  The definition parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The macro call parameter marks the position, where this macro invoked.
    //  The arguments parameter holds the macro arguments used during the 
    //  invocation of the macro
    //
    ///////////////////////////////////////////////////////////////////////////

    template <typename ContextT, typename TokenT, typename ContainerT, typename IteratorT>
    bool expanding_function_like_macro(ContextT const& ctx,
        TokenT const& macrodef, std::vector<TokenT> const& formal_args, 
        ContainerT const& definition,
        TokenT const& macrocall, std::vector<ContainerT> const& arguments,
        IteratorT const& seqstart, IteratorT const& seqend) 
    {
       ////////////////////////////////////////////////////////////////////
       //
       //Note from AS:
       //The typename ContainerT is often equivalent to list<TokenT>.
       //TokentT is often equivalent to  boost::wave::cpplexer::lex_token<>
       //found in boost_1_33_0/boost/wave/cpplexer/cpp_lex_token.hpp.
       //
       ///////////////////////////////////////////////////////////////////

       attributeListMap->expanding_function_like_macro(macrodef, formal_args, definition, macrocall, arguments);

      return false; 
    }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_object_like_macro' is called, whenever a 
    //  object-like macro is to be expanded .
    //
    //  The macroname parameter marks the position, where the macro to expand 
    //  is defined.
    //  The definition parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The macro call parameter marks the position, where this macro invoked.
    //
    ///////////////////////////////////////////////////////////////////////////

    template <typename ContextT, typename TokenT, typename ContainerT>
    bool expanding_object_like_macro(ContextT const& ctx, TokenT const& macro, 
        ContainerT const& definition, TokenT const& macrocall)
    {
        attributeListMap->expanding_object_like_macro(macro, definition, macrocall);
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
    template <typename ContextT, typename ContainerT>
    void expanded_macro(ContextT const& ctx, ContainerT const& result)
    {
       attributeListMap->expanded_macro(result);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'rescanned_macro' is called, whenever the rescanning of a 
    //  macro is finished.
    //
    //  The parameter 'result' contains the token sequence generated as the 
    //  result of the rescanning.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename ContainerT>
    void rescanned_macro(ContextT const& ctx, ContainerT const& result)
    {
        attributeListMap->rescanned_macro(result);

    }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'found_include_directive' is called, whenever a #include
    //  directive was located.
    //
    //  The parameter 'filename' contains the (expanded) file name found after 
    //  the #include directive. This has the format '<file>', '"file"' or 
    //  'file'.
    //  The formats '<file>' or '"file"' are used for #include directives found 
    //  in the preprocessed token stream, the format 'file' is used for files
    //  specified through the --force_include command line argument.
    //
    //  The parameter 'include_next' is set to true if the found directive was
    //  a #include_next directive and the BOOST_WAVE_SUPPORT_INCLUDE_NEXT
    //  preprocessing constant was defined to something != 0.
    //
    ///////////////////////////////////////////////////////////////////////////

    template <typename ContextT>
    bool
    found_include_directive(ContextT const& ctx, std::string const& filename, 
        bool include_next) 
    {
       if(SgProject::get_verbose() >= 1)
           std::cout << "Found include directive: " << filename << std::endl;

       includeDirective = filename;
       return false;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'opened_include_file' is called, whenever a file referred 
    //  by an #include directive was successfully located and opened.
    //
    //  The parameter 'filename' contains the file system path of the 
    //  opened file (this is relative to the directory of the currently 
    //  processed file or a absolute path depending on the paths given as the
    //  include search paths).
    //
    //  The include_depth parameter contains the current include file depth.
    //
    //  The is_system_include parameter denotes, whether the given file was 
    //  found as a result of a #include <...> directive.
    //  
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT>
    void 
    opened_include_file(ContextT const& ctx, std::string const& relname, 
        std::string const& absname, bool is_system_include) 
    {
                                   
       if(SgProject::get_verbose() >= 1){
           std::cout << "openend include file relname: " << relname << " absname: " << absname << std::endl;
           std::cout << "it is connected to : " << includeDirective << std::endl;
       }
       attributeListMap->found_include_directive(includeDirectiveToken, relname, absname);
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'returning_from_include_file' is called, whenever an
    //  included file is about to be closed after it's processing is complete.
    //
    ///////////////////////////////////////////////////////////////////////////
    // new signature
    template <typename ContextT>
    void
    returning_from_include_file(ContextT const& ctx) 
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'interpret_pragma' is called, whenever a #pragma wave 
    //  directive is found, which isn't known to the core Wave library. 
    //
    //  The parameter 'ctx' is a reference to the context object used for 
    //  instantiating the preprocessing iterators by the user.
    //
    //  The parameter 'pending' may be used to push tokens back into the input 
    //  stream, which are to be used as the replacement text for the whole 
    //  #pragma wave() directive.
    //
    //  The parameter 'option' contains the name of the interpreted pragma.
    //
    //  The parameter 'values' holds the values of the parameter provided to 
    //  the pragma operator.
    //
    //  The parameter 'act_token' contains the actual #pragma token, which may 
    //  be used for error output.
    //
    //  If the return value is 'false', the whole #pragma directive is 
    //  interpreted as unknown and a corresponding error message is issued. A
    //  return value of 'true' signs a successful interpretation of the given 
    //  #pragma.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename ContainerT>
    bool 
    interpret_pragma(ContextT const &ctx, ContainerT &pending, 
        typename ContextT::token_type const &option, ContainerT const &values, 
        typename ContextT::token_type const &act_token)
    {
        return false;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //
    //  The function 'defined_macro' is called, whenever a macro was defined
    //  successfully.
    //
    //  The parameter 'name' is a reference to the token holding the macro name.
    //
    //  The parameter 'is_functionlike' is set to true, whenever the newly 
    //  defined macro is defined as a function like macro.
    //
    //  The parameter 'parameters' holds the parameter tokens for the macro
    //  definition. If the macro has no parameters or if it is a object like
//  macro, then this container is empty.
    //
    //  The parameter 'definition' contains the token sequence given as the
    //  replacement sequence (definition part) of the newly defined macro.
    //
    //  The parameter 'is_predefined' is set to true for all macros predefined 
    //  during the initialisation phase of the library.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename TokenT, typename ParametersT, 
        typename DefinitionT>
    void
    defined_macro(ContextT const& ctx, TokenT const& macro_name, 
        bool is_functionlike, ParametersT const& parameters, 
        DefinitionT const& definition, bool is_predefined)
    {
#if 0
       string name(macro_name.get_value().c_str());
       //add all macros which is not a builtin macro to ROSE attribute
       if(! (name.substr(0,2)=="__")
            //&&(name.substr(name.length()-2,name.length())=="__"))
        ){
       //AS(041906) Filter out macros defined on the commandline as they are not
       //part of a file and is therefore not interesting for macro-rewrapping.
#endif
       //if(is_predefined!=true)
       if( macro_name.get_position().get_file().size()!=0 ){
       if( (macro_name.get_position().get_file()!="<built-in>") )  {
          attributeListMap->defined_macro(macro_name, is_functionlike, parameters, definition, is_predefined);

       }
       }else  attributeListMap->defined_macro(macro_name, is_functionlike, parameters, definition, is_predefined);

    }
   

    ///////////////////////////////////////////////////////////////////////////
    //
    //  The function 'found_directive' is called, whenever a preprocessor 
    //  directive was encountered, but before the corresponding action is 
    //  executed.
    //
    //  The parameter 'directive' is a reference to the token holding the 
    //  preprocessing directive.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename TokenT>
    bool 
    found_directive(ContextT const& ctx, TokenT const& directive)
    {

        lastPreprocDirective = directive;
        // print the commented conditional directives
        using namespace boost::wave;
        token_id id = token_id(directive);

        //A define statement is handled differently from all other preprocessor
        //directives. Therefore it should not be handled by found_directive;
        switch(id){
             case T_PP_QHEADER:   //#include "..."
             case T_PP_HHEADER:   //#include <...>
                // std::cout << "Token to include directive: " << directive.get_value() << std::endl;        
                includeDirectiveToken = directive;
                // attributeListMap->found_directive(directive);
                break;       
             case T_PP_INCLUDE:   //#include
             case T_PP_IF:        //#if
             case T_PP_IFDEF:     //#ifdef
             case T_PP_IFNDEF:    //#ifndef
             case T_PP_DEFINE:    //#define
             case T_PP_WARNING:   //#warning
             case T_PP_LINE:      //#line
             case T_PP_UNDEF:     //#undef
                break;
             case T_PP_ELIF:      //#elif
                last_elif = directive;
                // std::cout << "Found #elif\n";
                break; 
             default:
                if(id!=T_PP_DEFINE)
                     attributeListMap->found_directive(directive);
                break;
        }
      return false;  
    }

 ///////////////////////////////////////////////////////////////////////////
 //
 //  The function 'evaluated_conditional_expression' is called, whenever a 
 //  conditional preprocessing expression was evaluated (the expression
 //  given to a #if, #ifdef or #ifndef directive)
 //
 //  The parameter 'expression' holds the non-expanded token sequence
 //  comprising the evaluated expression.
 //
 //  The parameter expression_value contains the result of the evaluation of
 //  the expression in the current preprocessing context.
 //
 ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename TokenT, typename ContainerT>
            bool
            evaluated_conditional_expression(ContextT const& ctx, 
                            TokenT const& directive, ContainerT const& expression, 
                            bool expression_value)
               {

                 using namespace boost::wave;
                 token_id wave_typeid = token_id(directive);

                 if(SgProject::get_verbose() >= 1){
                                                     
                      if( T_PP_ELIF  == wave_typeid)
                             std::cout << "Found an #elif\n";
                 }
                      
 

                 attributeListMap->found_directive(lastPreprocDirective,expression, expression_value);
                 return false;  // ok to continue, do not re-evaluate expression


               }

 ///////////////////////////////////////////////////////////////////////////
 //
 //  The function 'skipped_token' is called, whenever a token is about to be
 //  skipped due to a false preprocessor condition (code fragments to be
 //  skipped inside the not evaluated conditional #if/#else/#endif branches).
 //
 //  The parameter 'token' refers to the token to be skipped.
 //  
 //
 ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename TokenT>
            void
            skipped_token(ContextT const& ctx, TokenT const& token)
               {
              //Process all tokens to be skipped except the ones without a filename,
              //e.g macro definitions from the commandline
              //       if(tokenmacro_name.get_position().get_file().size()!=0)  

                 int lineNo = token.get_position().get_line(); 
                 //int colNo  = token.get_position().get_column(); 
                 string filename = std::string(token.get_position().get_file().c_str());

                 int lineNoElif = last_elif.get_position().get_line(); 
                 //int colNoElif  = last_elif.get_position().get_column(); 


                 //AS(01/16/07) If the false-block of a preprocessor #if #elif #endif is an #elif the #elif token is not
                 //registered as a skipped token. This is a temporary fix for this problem. FIXME               
                 if(lineNo==lineNoElif)
                   if(filename == std::string(last_elif.get_position().get_file().c_str()) ){
                         attributeListMap->skipped_token(last_elif);
                         last_elif = token_type(boost::wave::T_SPACE," ",boost::wave::util::file_position_type(BOOST_WAVE_STRINGTYPE(),lineNo,0));
                         attributeListMap->skipped_token(last_elif);

                         last_elif = token_type(boost::wave::T_PP_ELIF,"#elif",boost::wave::util::file_position_type(BOOST_WAVE_STRINGTYPE(),0,0));

                   }
 

                 attributeListMap->skipped_token(token);
                 using namespace boost::wave;

                 token_id wave_typeid = token_id(token);



                 if(SgProject::get_verbose() >= 1)
                      if( T_PP_ELIF  == wave_typeid)
                             std::cout << "Found an #elif\n";
 


              //     else
              //      std::cout << "SKIPPED BECAUSE FILE IS NULL: " << token.get_value().c_str();

               }

 ///////////////////////////////////////////////////////////////////////////
 //
 //  The function 'undefined_macro' is called, whenever a macro definition
 //  was removed successfully.
 //
 //  The parameter 'name' holds the name of the macro, which definition was
 //  removed.
 //
 ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename TokenT>
            void
            undefined_macro(ContextT const& ctx, TokenT const& macro_name)
               {
                 token_list_container tokListCont;
                 tokListCont.push_back(macro_name);
                 attributeListMap->found_directive(lastPreprocDirective,tokListCont, false);
               }

#if 0
    template <typename ContainerT>
            void
            on_warning(ContainerT const& tokenStream)
               {
                 attributeListMap->found_directive(lastPreprocDirective,tokenStream, false);

                 std::cout << "ON TOKEN WARNING: " << boost::wave::util::impl::as_string(tokenStream) << std::endl;
               }
#endif

 ///////////////////////////////////////////////////////////////////////////
 //
 //  The function 'found_warning_directive' is called, will be called by the
 //  library, whenever a #warning directive is found.
 //
 //  The parameter 'ctx' is a reference to the context object used for
 //  instantiating the preprocessing iterators by the user.
 //
 //  The parameter 'message' references the argument token sequence of the
 //  encountered #warning directive.
 //
 ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename ContainerT>
            bool
            found_warning_directive(ContextT const& ctx, ContainerT const& message)
               { 
                 attributeListMap->found_directive(lastPreprocDirective,message, false);

                 if(SgProject::get_verbose() >= 1)
                     std::cout << "ON TOKEN WARNING: " << boost::wave::util::impl::as_string(message) << std::endl;

              //Do not throw warning message
                 return true; 
               }


 ///////////////////////////////////////////////////////////////////////////
 //
 //  The function 'found_line_directive' will be called by the library
 //  whenever a #line directive is found.
 //
 //  The parameter 'ctx' is a reference to the context object used for 
 //  instantiating the preprocessing iterators by the user.
 //
 //  The parameter 'arguments' references the argument token sequence of the
 //  encountered #line directive.
 //
 //  The parameter 'line' contains the recognized line number from the #line
 //  directive.
 //
 //  The parameter 'filename' references the recognized file name from the 
 //  #line directive (if there was one given).
 //
 ///////////////////////////////////////////////////////////////////////////
    template <typename ContextT, typename ContainerT>
            void
            found_line_directive(ContextT const& ctx, ContainerT const& arguments,
                            unsigned int line, std::string const& filename)
               {
                 std::string filenameString(filename.c_str());
                                             
                 if(SgProject::get_verbose() >= 1)
                      std::cout << "On line found" << std::endl;
              /*
                 token_list_container toexpand;
                 std::copy(first, make_ref_transform_iterator(end, boost::wave::util::get_value),
                 std::inserter(toexpand, toexpand.end()));
               */

                 attributeListMap->found_directive(lastPreprocDirective,arguments, false);

               }


    template <typename ContextT, typename TokenT>
            bool
            may_skip_whitespace(ContextT const& ctx, TokenT& token, bool& skipped_newline)
               { 
                 if(SgProject::get_verbose() >= 1)
                    std::cout << "MAX_SKIP_WHITESPACE: " << token.get_value().c_str() << std::endl;

                 attributeListMap->may_skip_whitespace(ctx,token,skipped_newline);
                 return false; }



#if 0
                 bool need_comment;
#endif
};

#endif // !defined(BOOST_WAVE_ADVANCED_PREPROCESSING_HOOKS_INCLUDED)


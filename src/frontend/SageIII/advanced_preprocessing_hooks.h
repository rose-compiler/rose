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

    std::string includeDirective;
    token_type includeDirectiveToken;    
	bool skipping;
	bool updatingLastToken;
	token_type last_token;
    advanced_preprocessing_hooks() /*: need_comment(true)*/ {
       numberOfIfs = 0;
	   skipping = false;
	   updatingLastToken = false;
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
// The "#if 0" was already here, but I also commented these lines out because '{' and '}' do not balance otherwise. The
// unbalanced braces cause some problems for some tools (indenting, automatic enum detection, etc) [RPM 2010-10-11]
//       string name(macro_name.get_value().c_str());
//       //add all macros which is not a builtin macro to ROSE attribute
//       if(! (name.substr(0,2)=="__")
//            //&&(name.substr(name.length()-2,name.length())=="__"))
//        ){
//       //AS(041906) Filter out macros defined on the commandline as they are not
//       //part of a file and is therefore not interesting for macro-rewrapping.
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
		if(SgProject::get_verbose() >= 1)
			std::cout << "found_directive!" << std::endl;

		skipping = false;
        lastPreprocDirective = directive;
        // print the commented conditional directives
        using namespace boost::wave;
        token_id id = token_id(directive);

		bool record = true;
		switch(id)
		{
			case T_PP_LINE:    //#line
			case T_PP_UNDEF:   //#undef
			case T_PP_WARNING: //#warning
			case T_PP_DEFINE:  //#define
				record = false;
				break; // handled via lastPreprocDirective in their own methods
			case T_PP_IF:      //#if
			case T_PP_IFDEF:   //#ifdef
			case T_PP_IFNDEF:  //#ifndef
			case T_PP_ELSE:	   //#else
			case T_PP_ELIF:	   //#elif
				break; // appended too as necessary in skipped_token and evaluated_conditional_expression
			case T_PP_ENDIF:   //#endif
			case T_PP_ERROR:   //#error
			case T_PP_PRAGMA:  //#pragma
			case T_PP_INCLUDE: //#include \"...\"
				break;
			case T_PP_QHEADER: //#include <...>
			case T_PP_HHEADER: //#include ...
				break;
			default:           //<something else (" << directive.get_value() << ")>
				break;
		}
		if (record)
			attributeListMap->found_directive(directive);

		attributeListMap->flush_token_stream();
		updatingLastToken = true;

		if(SgProject::get_verbose() >= 1)
		{
			switch(id)
			{
				case T_PP_DEFINE:    std::cout << "Directive is: #define\n"; break;
				case T_PP_IF:        std::cout << "Directive is: #if\n"; break;
				case T_PP_IFDEF:     std::cout << "Directive is: #ifdef\n"; break;
				case T_PP_IFNDEF:    std::cout << "Directive is: #ifndef\n"; break;
				case T_PP_ELSE:      std::cout << "Directive is: #else\n"; break;
				case T_PP_ELIF:      std::cout << "Directive is: #elif\n"; break;
				case T_PP_ENDIF:     std::cout << "Directive is: #endif\n"; break;
				case T_PP_ERROR:     std::cout << "Directive is: #error\n"; break;
				case T_PP_LINE:      std::cout << "Directive is: #line\n"; break;
				case T_PP_PRAGMA:    std::cout << "Directive is: #pragma\n"; break;
				case T_PP_UNDEF:     std::cout << "Directive is: #undef\n"; break;
				case T_PP_WARNING:   std::cout << "Directive is: #warning\n"; break;
				case T_PP_INCLUDE:   std::cout << "Directive is: #include \"...\"\n"; break;
				case T_PP_QHEADER:   std::cout << "Directive is: #include <...>\n"; break;
				case T_PP_HHEADER:   std::cout << "Directive is: #include ...\n"; break;
				default:             std::cout << "Directive is: <something else>\n"; break;
			}
		}

		last_token = directive;
		return false;
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  The function 'generated_token' is called, whenever a token is about
	//  to be returned from the library
	//
	///////////////////////////////////////////////////////////////////////////
	template <typename ContextT, typename TokenT>
	TokenT const& generated_token(ContextT const& ctx, TokenT const& token)
	{
		if(SgProject::get_verbose() >= 1)
			std::cout << "Generating token: ";

		using namespace boost::wave;
		token_id id = token_id(token);

		if(SgProject::get_verbose() >= 1)
		{
			switch(id)
			{
				case T_PP_DEFINE:    std::cout << "#define: "; break;
				case T_PP_IF:        std::cout << "#if: "; break;
				case T_PP_IFDEF:     std::cout << "#ifdef: "; break;
				case T_PP_IFNDEF:    std::cout << "#ifndef: "; break;
				case T_PP_ELSE:      std::cout << "#else: "; break;
				case T_PP_ELIF:      std::cout << "#elif: "; break;
				case T_PP_ENDIF:     std::cout << "#endif: "; break;
				case T_PP_ERROR:     std::cout << "#error: "; break;
				case T_PP_LINE:      std::cout << "#line: "; break;
				case T_PP_PRAGMA:    std::cout << "#pragma: "; break;
				case T_PP_UNDEF:     std::cout << "#undef: "; break;
				case T_PP_WARNING:   std::cout << "#warning: "; break;
				case T_PP_INCLUDE:   std::cout << "#include \"...\": "; break;
				case T_PP_QHEADER:   std::cout << "#include <...>: "; break;
				case T_PP_HHEADER:   std::cout << "#include ...: "; break;
				default:             std::cout << "<something else (" << id << ")>: "; break;
			}
//		   std::cout << boost::wave::util::impl::as_string(token).c_str() << std::endl;
			if (token != token_type())
				std::cout << token.get_value().c_str() << std::endl;
		}
		return token;
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
	evaluated_conditional_expression(ContextT const& ctx, TokenT const& directive, ContainerT const& expression, bool expression_value)
	{
		using namespace boost::wave;
		token_id id = token_id(directive);

		ROSE_ASSERT(directive == lastPreprocDirective);

		if(SgProject::get_verbose() >= 1)
		{
			std::cout << "Conditional: ";
			switch(id)
			{
				case T_PP_DEFINE:    std::cout << "#define: "; break;
				case T_PP_IF:        std::cout << "#if: "; break;
				case T_PP_IFDEF:     std::cout << "#ifdef: "; break;
				case T_PP_IFNDEF:    std::cout << "#ifndef: "; break;
				case T_PP_ELSE:      std::cout << "#else: "; break;
				case T_PP_ELIF:      std::cout << "#elif: "; break;
				case T_PP_ENDIF:     std::cout << "#endif: "; break;
				case T_PP_ERROR:     std::cout << "#error: "; break;
				case T_PP_LINE:      std::cout << "#line: "; break;
				case T_PP_PRAGMA:    std::cout << "#pragma: "; break;
				case T_PP_UNDEF:     std::cout << "#undef: "; break;
				case T_PP_WARNING:   std::cout << "#warning: "; break;
				case T_PP_INCLUDE:   std::cout << "#include \"...\": "; break;
				case T_PP_QHEADER:   std::cout << "#include <...>: "; break;
				case T_PP_HHEADER:   std::cout << "#include ...: "; break;
				default:             std::cout << "<something else (" << id << ")>: "; break;
			}
//		   std::cout << boost::wave::util::impl::as_string(token).c_str() << std::endl;
			std::cout << directive.get_value().c_str() << std::endl;
		}
		token_container whitespace;
		whitespace.push_back(token_type(boost::wave::T_SPACE, " ", boost::wave::util::file_position_type(BOOST_WAVE_STRINGTYPE(), 0, 0)));
		// directive was already given up in found_directive.  here we add to the expression
		attributeListMap->update_token(lastPreprocDirective, whitespace, expression_value);
		attributeListMap->update_token(lastPreprocDirective, expression, expression_value);

		// King84 (2010.09.09):
		// Tell later guys to update the previous token as soon as we start skipping
		// This is important to associate conditions with skipped #elif directives
		updatingLastToken = true; 

		last_token = directive;
		return false; // ok to continue, do not re-evaluate expression
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
		using namespace boost::wave;
		//Process all tokens to be skipped except the ones without a filename,
		//e.g macro definitions from the commandline
		if (token.get_position().get_file().size() == 0)
			return;
		if (token == last_token && token.get_position() == last_token.get_position())
			return;

		token_id id = token_id(token);
		token_id lastid = token_id(lastPreprocDirective);
		if (lastPreprocDirective == token_type())
		{
			attributeListMap->skipped_token(token);
		}
		else if (token != lastPreprocDirective)
		{
			if (id == T_NEWLINE) // if it's a newline, it's the end of the previous token not that trailing-slash linesare already taken into account by wave
				updatingLastToken = false;

			// This is if we have to associated skipped tokens with a previous token.  We do this for the last directive until we get a newline
			if (updatingLastToken)
			{
				token_container tc;
				tc.push_back(token);
				if(SgProject::get_verbose() >= 1)
					std::cout << "Updating previous token (" << lastPreprocDirective.get_value().c_str() << ") with token " << token.get_value().c_str() << std::endl;
				attributeListMap->update_token(lastPreprocDirective, tc, false);
			}
			else if (skipping || (lastid != T_PP_IF && lastid != T_PP_IFDEF && lastid != T_PP_IFNDEF && lastid != T_PP_ELIF)) // if we skipped the first newline
			{
				attributeListMap->skipped_token(token);
			}
		}

		switch(id)
		{
			case T_PP_DEFINE:
			case T_PP_IF:
			case T_PP_IFDEF:
			case T_PP_IFNDEF:
			case T_PP_ELSE:
			case T_PP_ELIF:
			case T_PP_ENDIF:
			case T_PP_ERROR:
			case T_PP_LINE:
			case T_PP_PRAGMA:
			case T_PP_UNDEF:
			case T_PP_WARNING:
			case T_PP_INCLUDE:
			case T_PP_QHEADER:
			case T_PP_HHEADER:
				lastPreprocDirective = token; // since found_directive doesn't get called
				break;
			default:
				break;
		}

		if(SgProject::get_verbose() >= 1)
		{
			switch(id)
			{
				case T_PP_DEFINE:    std::cout << "Skipped: #define\n"; break;
				case T_PP_IF:        std::cout << "Skipped: #if\n"; break;
				case T_PP_IFDEF:     std::cout << "Skipped: #ifdef\n"; break;
				case T_PP_IFNDEF:    std::cout << "Skipped: #ifndef\n"; break;
				case T_PP_ELSE:      std::cout << "Skipped: #else\n"; break;
				case T_PP_ELIF:      std::cout << "Skipped: #elif\n"; break;
				case T_PP_ENDIF:     std::cout << "Skipped: #endif\n"; break;
				case T_PP_ERROR:     std::cout << "Skipped: #error\n"; break;
				case T_PP_LINE:      std::cout << "Skipped: #line\n"; break;
				case T_PP_PRAGMA:    std::cout << "Skipped: #pragma\n"; break;
				case T_PP_UNDEF:     std::cout << "Skipped: #undef\n"; break;
				case T_PP_WARNING:   std::cout << "Skipped: #warning\n"; break;
				case T_PP_INCLUDE:   std::cout << "Skipped: #include \"...\"\n"; break;
				case T_PP_QHEADER:   std::cout << "Skipped: #include <...>\n"; break;
				case T_PP_HHEADER:   std::cout << "Skipped: #include ...\n"; break;
				default:             std::cout << "Skipped: <something else (" << token.get_value().c_str() << ")>\n"; break;
			}
			if (lastPreprocDirective != token_type())
				std::cout << "\tskipping is " << skipping << "\tupdatingLastToken is " << updatingLastToken << "\tlastPreprocDirective is " << lastPreprocDirective.get_value().c_str() << std::endl;
		}
		skipping = true; // skipping lets us skip the extra newline that shows up after #if statements
		last_token = token;
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
		if (token == lastPreprocDirective && token.get_position() == lastPreprocDirective.get_position())
			return false;
		if (token == last_token && token.get_position() == last_token.get_position())
			return false;
		if (SgProject::get_verbose() >= 1)
			if (token != TokenT())
				std::cout << "MAX_SKIP_WHITESPACE: " << token.get_value().c_str() << std::endl;

		using namespace boost::wave;
		token_id id = token_id(token);
		if (id != T_EOF && id != T_EOI && id != T_NEWLINE)
		{
			if (SgProject::get_verbose() >= 1)
				std::cout << "Normal reported" << std::endl;
			attributeListMap->may_skip_whitespace(ctx, token, skipped_newline);
		}
		else if (skipping) // then we need to flush tokens
		{
			if (SgProject::get_verbose() >= 1)
				std::cout << "Skip leads to flushing" << std::endl;
			attributeListMap->flush_token_stream();
		}
		else
		{
			if (SgProject::get_verbose() >= 1)
				std::cout << "EOF or newline leads to flushing" << std::endl;
			attributeListMap->flush_token_stream();
		}
		return false;
	}

	template <typename ContextT, typename ExceptionT>
	void throw_exception(ContextT const &ctx, ExceptionT const& e)
	{
		if (SgProject::get_verbose() >= 1)
			std::cout << "THROW_EXCEPTION" << std::endl;
	}

	template <typename ContextT>
	void detected_include_guard(ContextT const &ctx, std::string const& filename, std::string const& include_guard)
	{
		if (SgProject::get_verbose() >= 1)
			std::cout << "DETECTED_INCLUDE_GUARD" << include_guard << " in file " << filename << std::endl;
	}

	template <typename ContextT, typename TokenT>
	void detected_pragma_once(ContextT const &ctx, TokenT const& pragma_token, std::string const& filename)
	{
		if (SgProject::get_verbose() >= 1)
			std::cout << "DETECTED_PRAGMA_ONCE " << pragma_token.get_value() << " in file " << filename << std::endl;
	}

	template <typename ContextT, typename ContainerT>
	bool found_error_directive(ContextT const &ctx, ContainerT const &message)
	{
		if (SgProject::get_verbose() >= 1)
			std::cout << "FOUND_ERROR_DIRECTIVE" << std::endl;
		return false;
	}


#if 0
                 bool need_comment;
#endif
};

#endif // !defined(BOOST_WAVE_ADVANCED_PREPROCESSING_HOOKS_INCLUDED)


/**
 * \file compass_main.cpp
 * \author Justin Too <too1@llnl.gov>
 */

#include "rose.h"

/*-----------------------------------------------------------------------------
 * C/C++ system includes
 **--------------------------------------------------------------------------*/
#include <iostream>
#include <map>

/*-----------------------------------------------------------------------------
 * Library includes
 **--------------------------------------------------------------------------*/
// Boost C++ libraries
#include <boost/lexical_cast.hpp>

/*-----------------------------------------------------------------------------
 * Project includes
 **--------------------------------------------------------------------------*/
#include "compass2/compass.h"

extern const Compass::Checker* const deadFunctionChecker;
extern const Compass::Checker* const defaultArgumentChecker;
extern const Compass::Checker* const functionPointerChecker;
extern const Compass::Checker* const functionPrototypeChecker;
extern const Compass::Checker* const functionWithMultipleReturnsChecker;
extern const Compass::Checker* const globalVariablesChecker;
extern const Compass::Checker* const keywordMacroChecker;
extern const Compass::Checker* const nonGlobalCppDirectiveChecker;
extern const Compass::Checker* const nonStaticArraySizeChecker;
extern const Compass::Checker* const variableNameSimilarityChecker;

/*-----------------------------------------------------------------------------
 * Main program
 **--------------------------------------------------------------------------*/

/**
  * \todo document
  */
void build_checkers (std::vector<const Compass::Checker*> & checkers, Compass::Parameters & params,
    Compass::OutputObject & output, SgProject* pr)
  {}

/**
  * \todo document
  */
int main (int argc, char** argv)
  {
    // use only C++ I/O for faster performance.
    std::ios::sync_with_stdio (false);

    if (SgProject::get_verbose () > 0)
    {
        std::cout << "[Compass] [Main] In compass_main.cpp::main ()" << std::endl;
    }

    // -------------------------------------------------------------------------
    //  Command line processing
    // -------------------------------------------------------------------------

    Rose_STL_Container<std::string> cli_args =
        CommandlineProcessing::generateArgListFromArgcArgv (argc, argv);
    Compass::commandLineProcessing (cli_args);

    // -------------------------------------------------------------------------
    //  Compass parameters
    // -------------------------------------------------------------------------

    // Read the Compass parameter file (contains input data for all checkers)
    // This has been moved ahead of the parsing of the AST so that it is more
    // obvious when it is a problem.
    Compass::Parameters params;
    Compass::Xml::ParametersParser parser ("compass_parameters.xml");
    params = parser.parse_parameters ();

    // Enable which checkers?
    typedef std::map<std::string, bool> EnabledCheckersMapType;
    EnabledCheckersMapType enabled_checkers;
    static const bool CHECKER_ENABLED = true;

    Compass::ParametersMap enabled_checkers_parameters =
        params[boost::regex("general::enabled_checker")];
    // Save checker name in map for faster lookups.
    BOOST_FOREACH(const Compass::ParametersMap::value_type& pair,
                  enabled_checkers_parameters)
    {
        Compass::ParameterValues values = pair.second;
        BOOST_FOREACH(std::string checker_name, values)
        {
            enabled_checkers[checker_name] = CHECKER_ENABLED;
        }
    }

    // -------------------------------------------------------------------------
    //  Call ROSE frontend
    // -------------------------------------------------------------------------

    // Use a modified commandline that inserts specific additional options
    // to the ROSE frontend to make use with Compass more appropriate.
    // SgProject* project = frontend(argc,argv);
    SgProject* project = frontend (cli_args);

    // -------------------------------------------------------------------------
    //  Build Compass Checkers
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    //  Checkers
    // -------------------------------------------------------------------------
    std::vector<const Compass::Checker*> traversals;

    // Only enable the user-specified checkers
    BOOST_FOREACH(const EnabledCheckersMapType::value_type& pair,
                  enabled_checkers)
    {
        const bool checker_is_enabled = pair.second;
        if (CHECKER_ENABLED == checker_is_enabled)
        {
            std::string checker_name = pair.first;
            if ("functionPointer" == checker_name)
                traversals.push_back (functionPointerChecker);
            else if ("deadFunction" == checker_name)
                traversals.push_back (deadFunctionChecker);
            else if ("defaultArgument" == checker_name)
                traversals.push_back (defaultArgumentChecker);
            else if ("functionPrototype" == checker_name)
                traversals.push_back (functionPrototypeChecker);
            else if ("functionWithMultipleReturns" == checker_name)
                traversals.push_back (functionWithMultipleReturnsChecker);
            else if ("globalVariables" == checker_name)
                traversals.push_back (globalVariablesChecker);
            else if ("keywordMacros" == checker_name)
                traversals.push_back (keywordMacroChecker);
            else if ("nonGlobalCppDirective" == checker_name)
                traversals.push_back (nonGlobalCppDirectiveChecker);
            //else if ("nonStaticArraySize" == checker_name)
            //    traversals.push_back (nonStaticArraySizeChecker);
            else if ("variableNameSimilarity" == checker_name)
                traversals.push_back (variableNameSimilarityChecker);
            else
            {
                if (SgProject::get_verbose () >= 0)
                {
                    std::cout
                      << "[Compass] [Parameters] "
                      << "Uknown enabled_checker configuration detected: "
                      << checker_name
                      << std::endl;
                }
            }
        }// CHECKER_ENABLED == checker_is_enabled
    }


    Compass::PrintingOutputObject output(std::cerr);

    build_checkers (traversals, params, output, project);
    for (std::vector<const Compass::Checker*>::iterator itr = traversals.begin();
         itr != traversals.end();
         ++itr)
    {
        ROSE_ASSERT (*itr);
        Compass::runPrereqs (*itr, project);
    }

    // -------------------------------------------------------------------------
    //  Run Compass Analyses
    // -------------------------------------------------------------------------

    std::vector<std::pair<std::string, std::string> > errors;
    for (std::vector<const Compass::Checker*>::iterator itr = traversals.begin();
         itr != traversals.end();
         ++itr)
    {
        if (*itr == NULL)
        {
            std::cerr
              << "[Compass] [Main] "
              << "Error: Traversal failed to initialize"
              << std::endl;
            return 1;
        }
        else
        {

            if (SgProject::get_verbose () >= 0)
            {
              std::cout
                << "[Compass] [Main] "
                << "Running checker "
                << (*itr)->checkerName.c_str ()
                << std::endl;
            }

            try
            {
                int spaceAvailable = 40;
                std::string name = (*itr)->checkerName + ":";
                int n = spaceAvailable - name.length();
                //Liao, 4/3/2008, bug 82, negative value
                if (n<0) n=0;
                std::string spaces(n,' ');

                // -------------------------------------------------------------
                //  !! PERFORM TRAVERSAL !!
                // -------------------------------------------------------------
                (*itr)->run (params, &output);
            }
            catch (const std::exception& e)
            {
                std::cerr
                  << "[Compass] [Main] "
                  << "error running checker : "
                  << (*itr)->checkerName
                  << " - reason: "
                  << e.what()
                  << std::endl;

                errors.push_back(
                  std::make_pair((*itr)->checkerName,
                  e.what()));
            }
        }
    }//for each checker traversal

    // Output errors specific to any checkers that didn't initialize properly
    if (!errors.empty ())
    {
        std::cerr << "The following checkers failed due to internal errors:" << std::endl;
        std::vector<std::pair<std::string, std::string> >::iterator e_itr;
        for (e_itr = errors.begin(); e_itr != errors.end(); ++e_itr)
        {
            std::cerr << e_itr->first << ": " << e_itr->second << std::endl;
        }
    }

    return backend(project);
  }

/**
 * \file compass_main.cpp
 * \author Justin Too <too1@llnl.gov>
 */

#include "rose.h"

/*-----------------------------------------------------------------------------
 * C/C++ system includes
 **--------------------------------------------------------------------------*/
#include <iostream>

/*-----------------------------------------------------------------------------
 * Library includes
 **--------------------------------------------------------------------------*/
// Boost C++ libraries
#include <boost/lexical_cast.hpp>

/*-----------------------------------------------------------------------------
 * Project includes
 **--------------------------------------------------------------------------*/
#include "compass2/compass.h"

extern const Compass::Checker* const functionPointerChecker;
extern const Compass::Checker* const keywordMacroChecker;
extern const Compass::Checker* const nonGlobalCppDirectiveChecker;

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
        std::cout << "In compass_main.cpp::main ()" << std::endl;
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

    std::vector<const Compass::Checker*> traversals;

    traversals.push_back (functionPointerChecker);
    traversals.push_back (keywordMacroChecker);
    traversals.push_back (nonGlobalCppDirectiveChecker);

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
            std::cerr << "Error: Traversal failed to initialize" << std::endl;
            return 1;
        }
        else
        {
            // TODO: enable toggling of checkers
            // Compass::ParametersMap enabled_checkers =
            //     params[boost::regex("general::.*")];
            // // Save keywords in map for faster lookups.
            // BOOST_FOREACH(const Compass::ParametersMap::value_type& pair, enabled_checkers)
            // {
            //     Compass::ParameterValues values = pair.second;
            //     BOOST_FOREACH(std::string keyword, values)
            //     {
            //     }
            // }

            if (SgProject::get_verbose () >= 0)
            {
              std::cout
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
                std::cerr << "error running checker : " << (*itr)->checkerName << " - reason: " << e.what() << std::endl;
                errors.push_back(std::make_pair((*itr)->checkerName, e.what()));
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

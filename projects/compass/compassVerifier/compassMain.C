#include "compass.h"
#include <rose.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "checkers.h"

void buildCheckers( std::vector<Compass::TraversalBase*> & checkers, Compass::Parameters & params, Compass::OutputObject & output );


void outputTgui( std::string & tguiXML,
                 std::vector<Compass::TraversalBase*> & checkers,
                 Compass::PrintingOutputObject & output )
{
  std::fstream xml( tguiXML.c_str(), std::ios::out|std::ios::app );

  if( xml.good() == false )
  {
    std::cerr << "Error: outputTgui()\n";
    exit(1);
  }

  long pos = xml.tellp();

  if( pos == 0 )
  {
    xml << "<tool_gear>\n"
        << "<format>1</format>\n"
        << "  <version>2.00</version>\n"
        << "  <tool_title>Compass Analysis Static View</tool_title>\n";

    for( std::vector<Compass::TraversalBase*>::const_iterator itr = 
         checkers.begin(); itr != checkers.end(); itr++ )
    {
      std::string checkerName( (*itr)->getName() );

      xml << "  <message_folder>\n"
          << "    <tag>" << checkerName << "</tag>\n"
          << "    <title>" << checkerName << " Checker</title>\n"
          << "    <if_empty>hide</if_empty>\n"
          << "  </message_folder>\n";
    } //for, itr
  } //if( pos == 0 )

  const std::vector<Compass::OutputViolationBase*>& outputList = 
    output.getOutputList();

  for( std::vector<Compass::OutputViolationBase*>::const_iterator itr =
         outputList.begin(); itr != outputList.end(); itr++ )
  {
    const Sg_File_Info *info = (*itr)->getNode()->get_file_info();

    xml << "  <message>\n"
        << "    <folder>" << (*itr)->getCheckerName() << "</folder>\n"
        << "    <heading>" << (*itr)->getCheckerName() << ": " << info->get_filenameString() << " : " << info->get_line() << "</heading>\n"
        << "    <body><![CDATA[" << (*itr)->getString() << "]]></body>\n"
        << "    <annot>\n"
        << "      <site>\n"
        << "        <file>" << info->get_filenameString() << "</file>\n"
        << "        <line>" << info->get_line() << "</line>\n"
        << "        <desc><![CDATA[" << (*itr)->getShortDescription() << "]]></desc>\n" 
        << "      </site>\n"
        << "    </annot>\n"
        << "  </message>\n";
  } //for, itr

  xml.close();

  return;
} //outputTgui()



int main(int argc, char** argv)
{
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("Compass performance (main): time (sec) = ",true);

     std::ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!

     if (SgProject::get_verbose() > 0)
          printf ("In compassMain.C: main() \n");

  const bool remove = true;
  std::string tguiXML;

  SgProject* sageProject = frontend(argc,argv);
  std::vector<Compass::TraversalBase*> traversals;

  Compass::Parameters params(Compass::findParameterFile());
  Compass::PrintingOutputObject output(std::cerr);

  buildCheckers(traversals,params,output);

  std::vector<std::pair<std::string, std::string> > errors;

  for( std::vector<Compass::TraversalBase*>::iterator itr = traversals.begin();
       itr != traversals.end(); itr++ )
  {
    if( (*itr) != NULL )
    {
      printf ("Running checker %s \n",(*itr)->getName().c_str());
      try {
        (*itr)->run( sageProject );
      } catch (const std::exception& e) {
        std::cerr << "error running checker " << (*itr)->getName()
            << ": " << e.what() << std::endl;
        errors.push_back(std::make_pair((*itr)->getName(), e.what()));
      }
    } //if( (*itr) != NULL )
    else
    {
      std::cerr << "Error: Traversal failed to initialize" << std::endl;
      return 1;
    } //else
  } //for()

// This is the ToolGear Option
// if( CommandlineProcessing::isOptionWithParameter( argc, argv, "--tgui", "*", tguiXML, remove ) )
// if( CommandlineProcessing::isOptionWithParameter( CommandlineProcessing::generateArgListFromArgcArgv(argc, argv), std::string("--tgui"), std::string("*"), tguiXML, remove ) )
  std::vector<std::string> argvList = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  if( CommandlineProcessing::isOptionWithParameter( argvList, std::string("--tgui"), std::string("*"), tguiXML, remove ) )
  {
    outputTgui( tguiXML, traversals, output );
  } //if

  if (!errors.empty()) {
    std::cerr << "The following checkers failed due to internal errors:"
        << std::endl;
    std::vector<std::pair<std::string, std::string> >::iterator e_itr;
    for (e_itr = errors.begin(); e_itr != errors.end(); ++e_itr) {
      std::cerr << e_itr->first << ": " << e_itr->second << std::endl;
    }
  }


  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(sageProject);


  return 0;
} //main()

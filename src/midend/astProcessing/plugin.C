#include <string>
#include <iostream>
#include <stdlib.h>
//#include "sage3basic.h"
//#include "cmdline.h"
#include "commandline_processing.h"
#include "plugin.h"
using namespace std;

namespace Rose {

  std::vector<std::string> PluginLibs; // one or more plugin shared libraries  
  std::vector<std::string> PluginActions; // one or more actions defined in the shared libs
  std::map <std::string, std::vector <std::string> > PluginArgs; // plugin arguments

  template class Registry<PluginAction>;

  // CommandlineProcessing::isOptionWithParameter(argv,"-rose:","plugin_lib", lib_name, true) does not work.
  // it will extract the last hit and remove all matched pairs!!
  //
  // A helper function to extract -rose:xx value pair from command lines
  // each call will extract the first match and return.
  bool extractOptionWithParameter(vector<string>& argv, const string & option,  string& value)
  {
    int pos1=-1, pos2=-1; // the matched option offsets for option and value
#ifndef NDEBUG
    int sz=argv.size();
#endif
    for (size_t i=0; i< argv.size(); i++)
    {
      if (argv[i]== option)
      {
        pos1=i; 
        pos2=pos1+1; 
        assert (pos2<sz);
        value = argv[pos2]; 
        break; 
      }
    }

    if (pos1==-1) return false;

    argv.erase(argv.begin()+pos2);
    argv.erase(argv.begin()+pos1);

    return true;
  }

  //! Hooked up with SgProject::frontend(..) to process command line options related to plugins
  /*
  -rose:plugin_lib filename //  multiple .so, each is loaded with -load file.so
  -rose:plugin_action xxx // one or more actions to be called, each is specified with -plugin action_name
  -rose:plugin_arg_xxx // arguments to each action, each is specified with -plugin-arg-name … 
   */
  void processPluginCommandLine(vector<string>& argv) 
  {
     bool found=false; 
     // parse shared lib file names
     do   
     {
       string lib_name;
       //if (CommandlineProcessing::isOptionWithParameter(argv,"-rose:","plugin_lib", lib_name, true))
       if (extractOptionWithParameter(argv,"-rose:plugin_lib", lib_name))
       {
         PluginLibs.push_back(lib_name);
         found = true; 
       }
       else
         found = false;
     } while (found); 
   
     // parse action names
     do   
     {
       string act_name;
       //if (CommandlineProcessing::isOptionWithParameter(argv,"-rose:","plugin_action", act_name, true))
       if (extractOptionWithParameter(argv,"-rose:plugin_action", act_name))
       {
         PluginActions.push_back(act_name);
         found = true; 
       }
       else
         found = false;
     } while (found); 

     // parse action arguments
     // For each action named X,  we search for -rose:pulgin_arg_X 
     for (size_t i=0; i< PluginActions.size(); i++)
     {
       string act_name = PluginActions[i];
       vector <string> args; 
       do   
       {
         string arg;
         // if (CommandlineProcessing::isOptionWithParameter(argv,"-rose:", "plugin_arg_"+act_name, arg, true))
         if (extractOptionWithParameter(argv,"-rose:plugin_arg_"+act_name, arg))
         {
           args.push_back(arg);
           found = true;  
         }
         else
           found = false;
       } while (found); 

       PluginArgs[act_name]=args;
     }  
     
     //debugging
#if 0     //
     cout<<PluginLibs.size()<< " plugin libs:"<<endl;
     for (size_t i=0; i< PluginLibs.size(); i++)
     {
       cout<<PluginLibs[i]<<endl;
     }

     cout<<PluginActions.size()<<" plugin actions:"<<endl;
     for (size_t i=0; i< PluginActions.size(); i++)
     {
       cout<<PluginActions[i]<<endl;

       vector<string> args = PluginArgs[PluginActions[i]]; 
       cout<<args.size()<<" arguments for this plugin:"<<endl;
       for (size_t j=0; j< args.size(); j++ )
          cout<<args[j]<<endl;
     }
#endif     

  }  // end processPluginCommandLine ();


  //int obtainAndExecuteActions(const char* lib_file_name, string action_name, SgNode* n)
  int obtainAndExecuteActions(SgProject* n)
  {
    int res =0; 
    //load the shared libraries 
    for (size_t i=0; i< PluginLibs.size(); i++)
    {
      const char* lib_file_name= PluginLibs[i].c_str();
      void * handle = dlopen(lib_file_name, RTLD_LAZY|RTLD_GLOBAL);

      if (!handle)
      {
        cout<<"Error in dlopen: error code: "<<dlerror()<<" when loading "<<lib_file_name <<endl;
        exit(1);
      }
    }

    //4. Iterate through the registered plugins
    for (size_t i=0; i<PluginActions.size(); i++ )
    {
      string action_name = PluginActions[i];
      PluginAction* p_action=NULL;
      for (PluginRegistry::iterator it = PluginRegistry::begin();
          it != PluginRegistry::end(); ++it)
      {
        // find an action matching the action name 
        if (it->getName() == action_name)
        {
          p_action= it->instantiate();

          //call command line parsing for the plugin
          if(!p_action->ParseArgs(PluginArgs[action_name]))
            return 0;
          p_action->process(n);
          res++; 
        }
      } // end for registry
    } // end for actions

    return res; 
  }
} // end namespace



// A program to read in a compilation database json file and 
// generate an equivalent makefile to build all the targets.
// Liao, 2018/10
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <argp.h>

using json = nlohmann::json;
using namespace std; 

map<string, bool> visited; // check if a directory+ file is visited before.
vector<string> targets; // all unique targets to build

// input and outputfile names, with default values
string j_file_name="compile_commands.json";
string mkfile_name="makefile-default";
string compiler_name="identityTranslator"; // replace the compiler with another one, identityTranslator as the default new compiler
string add_options=""; // additional options to add
int timeout_length=10; // timeout length 10 minutes by default

bool replaceCompiler= false;

const char* argp_program_bug_address = "liao6@llnl.gov";
const char* argp_program_bug_version= "version 0.1";
// callback function to parse options
static int parse_opt (int key, char* arg, struct argp_state * state)
{
  switch (key)
  {
    case 'i':
      {
        // arg stores the parsed value followed after the option 
        if (arg!=NULL)
        {
          j_file_name =string(arg);
//          cout<<"arg not null:" << arg <<endl;
        }
        break;
      }
    case 'o':
      {
        // arg stores the parsed value followed after the option
        if (arg!=NULL)
        {
          mkfile_name =string(arg);
        }
        break;
      }
   case 'c':
      {
        // arg stores the parsed value followed after the option
        if (arg!=NULL)
        {
          compiler_name =string(arg);
        }
        replaceCompiler = true; 
        break;
      }
     case 'a':
      {
        // arg stores the parsed value followed after the option
        if (arg!=NULL)
        {
          add_options=string(arg);
        }
        else
        {
          cerr<<"error: you must provide an option string if -a is used."<<endl;
          return 1;
        }
        break;
      }
     case 't':
      {
        // arg stores the parsed value followed after the option
        if (arg!=NULL)
        {
          timeout_length=std::stoi(string(arg));
        }
        break;
      }
 
 }

  return 0;
}

int main(int argc, char** argv)
{
  //---------command line processing -----------
  // all opitions in one data structure
  struct argp_option options [] =
  {
    // first field: long name for the option
    // 2nd: short name for the option
    // third field: the name for the mandatory value for -d
    // mandatory option
    {"input", 'i', "String", OPTION_ARG_OPTIONAL, "input file name of compilation database json file, default name if not provided: compile_commands.json"},
    {"output", 'o', "String", OPTION_ARG_OPTIONAL, "output file name for the generated makefile, default name if not provided: makefile-default"},
    {"compiler", 'c', "String", OPTION_ARG_OPTIONAL, "replace the compiler with a new compiler command, default replacement compiler name if not provided: identityTranslator"},
    {"timeout", 't', "String", OPTION_ARG_OPTIONAL, "specify a timeout in minutes for running the translator/compiler, default 10 minutes if not provided."},
    // must provide values for this option
    {"add_options", 'a', "String", 0, "add additional compiler options to command lines"},
    {0}
  };

  // combine options and parser call back function, register with argp_parse
  struct argp argp = {options, parse_opt, 0, 0};
  int ret =  argp_parse (& argp, argc, argv, 0, 0, 0);
  if (ret !=0)  
  { 
    cerr<<"argp_parse() fails, stopped with error code:" << ret<<endl;
    return ret;
  }

  //--------------------------------------------
  std::ifstream my_ifs (j_file_name.c_str());
  if (!(my_ifs.is_open())) 
  {
    cout<<"Input file:"<<j_file_name<<" may not exists! Aborting ..."<<endl; 
    //    my_ifs.close();
    exit(1);
  } 

  // output makefile
  std::ifstream testfile(mkfile_name);
  if (testfile.is_open()) 
  {
    cout<<"Output file already exists! Please backup and remove it first."<<endl; 
    testfile.close();
    exit(1);
  } 

  cout<<"Input file is:"<< j_file_name <<". Creating the makefile named: "<< mkfile_name <<" ... "<<endl; 
  ofstream outfile(mkfile_name,ios::out); // ios::out mode has side effect!! if file does not exist, a new empty file will be created !!!

  outfile <<"# this is an automatically generated makefile"<<endl;
  outfile<<"default: all"<<endl;
  json j;  
  // read the json file
  my_ifs >> j; 

  //-----------------------------------
  // iterator to retrieve information
  // top level should be an array, elements should be objects within the array.
  // Each object is a collection of key-value pairs
  //   for ( json::iterator it = j.begin(); it!=j.end(); ++it)
  for (auto& element : j)
  {
    //json element = (*it);
    json dir = element["directory"];
    json filename = element["file"];
    string dir_str,filename_str; 
    // we can directly assign the object to a string

    // Extract dir path + file name
    if (dir.type()==json::value_t::string)
      dir_str  = dir; 
    if (filename.type()==json::value_t::string)
      filename_str  = filename; 
    // it is possible filename_str already contains the dir_str
    string fullfilename;
    //    cout<<"file name:"<<filename_str<<endl;
    //    cout<<"dir_name:"<<dir_str<<endl;
    string::size_type pos1 = filename_str.find(dir_str);
    // dir_str is not the leading substring of filename_str
    //    cout<<"pos of dir within filename is:"<< pos1<<endl;
    if (pos1==0)
      fullfilename = filename_str;
    else 
      fullfilename = dir_str+"/"+filename_str;

    // Somehow Bear generates a database file with duplicated entries
    // we have to skip them. 
    if (visited[fullfilename]!=true)
    {
      //cout<< "--------"+ dir_str+"/"+filename_str <<endl;

      // Now retrieve key-value from each object: std::map
      json args = element["arguments"]; // arguments are stored in an array
      //cout<< args <<endl; 
      // arrays are vector
      string args_str; // compose all arguments into a string, representing the full command line
      string target_str; 
      bool found_o= false;
      int counter =0; 
      for (json::iterator it2 = args.begin(); it2!=args.end(); it2++)
      {
        json opt = (*it2);
        // type_name() return string format of the type

        if (opt.type()==json::value_t::string)
        {
          // we can directly assign the object to a string
          string opt_str  = opt; 

          // detect -o and later extract the target of the makefile rule
          if (opt_str=="-o")
          {
            //            cout<< "find -o"<<endl;
            found_o = true;
          }
          else
          {
            if ((target_str.size()==0) && found_o)
              target_str = opt_str; 
          }
          
          // replace the first word with a new compiler name, if requested.
          string time_out_str= "timeout "+std::to_string(timeout_length)+"m";
          if ( (counter == 0) && replaceCompiler)
            opt_str = time_out_str + " "+ compiler_name;  // add timeout for each tool's invoking
          args_str += " " + opt_str;

          // add additional compiler options if provided
          if ( (counter == 0) && add_options.size() !=0 ) 
            args_str += " " + add_options;

          counter ++; 
        }
      }

      string fullTargetName;

      // target_str may already contain dir_path
      // it is possible a makefile does not specify -o explicitly. 
      // we patch it up with the known info.
      // replace the full input file name (with path)'s suffix (.c, .cxx, .f ,etc.) to .o
      // fullfilename : 
      if (target_str.size()==0)
      {
        size_t last_pos = fullfilename.find_last_of('.'); 
        if (last_pos == string::npos)
        {
          cerr<<"Fatal error: cannot find valid file suffix within the input file:"<< fullfilename <<endl;
          cerr<<"Please check your compilation database has valid entries."<< fullfilename <<endl;
           assert (last_pos != string::npos); 
        }
        fullTargetName = fullfilename.substr(0, last_pos)+".o";
      }
      else // this is a "-o target_str" in the command line
      {
        string::size_type pos2 = target_str.find(dir_str);
        if (pos2==0)
          fullTargetName = target_str;
        else 
          fullTargetName= dir_str+"/"+target_str;
      }

      targets.push_back(fullTargetName);
      //-----------------------------------
      // create a rule
      // *.o: file.cxx
      //     cd dir; compile_line

      outfile<< fullTargetName << ":"<< fullfilename << endl;
      outfile<<"\t" << "cd " << dir_str<<";" << args_str<<endl;

      visited[fullfilename]=true;
    }  // end if not visited
  } // end all elements

  cout<<"Total processed json entry count="<< targets.size()<<endl;
  //-----------------------------------
  // create a list of all: target1 target 2 ...
  // all : *.o 
  outfile <<"all : "; //<<endl;
  for (auto i: targets)
  {
    outfile <<" " <<i ; 
  }
  outfile<< endl;

  //-----------------------------------
  // clean:
  //       rm -rf *.o
  outfile <<"clean:"<<endl;
  outfile <<"\trm "; // dont use rm -rf here!! Removing directories are very risky!! 
  for (auto i: targets)
  {
    outfile <<" " <<i ; 
  }
  outfile<< endl;


  outfile.close();
  return 0; 
}

// ROSE translator example: identity translator.
//
// No AST manipulations, just a simple translation:
//
//    input_code > ROSE AST > output_code

#include <rose.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
using namespace std;
using namespace rose::StringUtility;
using namespace SageInterface;
using namespace SageBuilder;

enum exe_mode_enum {
  e_loop_counting, //counting the number of loops
  e_loop_instrumenting, // instrumenting loops for loop iteration count etc.
  e_last
  };

exe_mode_enum exe_mode; 
string array_counter_name("__lc_counters");
string array_counter_loop_index_name("__lc_init_i");
//Default path 
//string file_w_loop_count("/home/liao6/codeInstrumentation.log");
//Be more portable
string file_w_loop_count("/tmp/codeInstrumentation_file_with_loop_count.log");
string file_w_loop_iter_count;
size_t loop_counter = 0; 
int max_line_len=1024*4;

// Does the log file exist?
bool hasLogFile(string log_file_name)
{
  bool rt = false; 
  ifstream logfile (log_file_name.c_str());
  if (logfile.is_open())
    rt = true;
  else 
    rt = false;
  return rt;   
}


// retrieve the latest counter value from the log file
size_t getLatestLoopCounterValue ()
{
  size_t retVal = 0; 
  size_t line_count =0;
  char buffer[max_line_len];
  string prev_buffer;
  ifstream logfile (file_w_loop_count.c_str(), ios::in); // read only mode to avoid create empty file!!
  if (logfile.is_open())
  {
    while (! logfile.eof()) //TODO: read last line only
    {
      prev_buffer = buffer;
      logfile.getline(buffer,max_line_len, '\n');
      line_count ++; 
      // cout<<"Debugging .. getLatestLoopCounterValue() get a line "<< buffer << "line count " << line_count<<endl;
    }

    // somehow the last line extract is empty line, we need to backoff one line
    //  cout<<"Debugging .. getLatestLoopCounterValue() finds last line "<< prev_buffer <<endl;
    istringstream istr(prev_buffer);
    istr >> retVal; 

    // Sanity check for file content 
    // the line count = line_id +1
    assert (line_count = retVal +1);
  }

//  cout<<"Debugging .. getLatestLoopCounterValue() returns "<< retVal <<endl;
  return retVal;  
}

// if Log file does not exist, return 0
// if it exists, return the latest counter value +1
size_t getCurrentCounterValue (string file_w_loop_count_p)
{
  size_t rt =0; 
  if (hasLogFile(file_w_loop_count_p))
  {
    rt = getLatestLoopCounterValue () +1;
  }
  cout<<"Debugging .. getCurrentCounterValue() returns "<< rt <<endl;
  return rt; 
}
// Avoid undesired side effect of file creation when ofs.open() is called with a write mode
// We dont want empty log file. This will cause trouble for obtain last loop counter value from the file.
void writeToFile (size_t loop_counter, string loop_id)
{
    ofstream ofs;
    ofs.open(file_w_loop_count.c_str(), ios::app);
    ofs<<loop_counter <<"\t"<<loop_id<<endl;
    ofs.close();
}
// Counting the number of loops , save results into a file
// This function will be called multiple times during a compilation of an application.
// So the results will be appended to the file.
void countingLoops(SgProject* project)
{
  // previous compilation command's status
  // set current loop counter value, based on the log file
  loop_counter = getCurrentCounterValue (file_w_loop_count);

  // Insert your own manipulations of the AST here...
  ofstream ofs;
  // 
  // This open() has side effect, an empty file will be created.
  // We don't want this. We only want to create the file when trying to write a line
  //  ofs.open(file_w_loop_count.c_str(), ios::app);

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(project, V_SgStatement);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgStatement* stmt= isSgStatement(*i);
    // skip system headers
    if (insideSystemHeader(stmt))
      continue; 
    switch (stmt->variantT()) 
    {
      case V_SgForStatement:
        {
          Sg_File_Info * fi = stmt->get_file_info(); 
          ROSE_ASSERT(fi != NULL);
          string s1(":");
          string loop_id = fi->get_filename() + s1+ numberToString(fi->get_line()) + s1 + numberToString(fi->get_col());
          //          ofs<<loop_counter <<"\t"<<loop_id<<endl;
          writeToFile(loop_counter, loop_id);
          loop_counter ++; 
          break;
        }
      default:
        break;
    }
  }

  ofs.close();
}


//Now in the instrumenting mode, we first read in the program info. file
// store the file content info. into a map loop_id_map[loop_path]=loop_id
std::map<string, size_t> loop_id_map; 
int readProgramInfoFile()
{
  char buffer[max_line_len];
  ifstream logfile (file_w_loop_count.c_str(), ios::in); // read only mode to avoid create empty file!!
  int while_count =0; 
  if (logfile.is_open())
  {
    while (! logfile.eof()) //this will enter the last empty line 
    {
      int loop_id;
      string loop_path; 

      logfile.getline(buffer,max_line_len, '\n'); // here EOF bit will be set
      istringstream istr(buffer);
      istr >> loop_id;
      istr >> loop_path; 
#if 0      
      if (while_count!=0 && loop_id_map[loop_path]!=0)
      {
        cerr<<"Warning. Possible redundant loop path is found:"<< loop_path<<endl;
        cerr<<"\t Previous ID is:"<< loop_id_map[loop_path]<<endl;
      }
#endif      
     // skip the last empty line after \n 
     // empty file path, and empty loop integer id
     // Only save if either of the two fields exists.
     if (!logfile.fail() ) //  special handling for last line: it is empty , it is after \n of previous line, it is before \eof.
     {
      loop_id_map[loop_path]=loop_id;  
      while_count++; 
     }
    }
  }
  else
  {
    cerr<<"Warning: readProgramInfoFile() cannot open file:"<<file_w_loop_count<<endl;
    //while_count = 0; 
    // We cannot assert here. Some app's build system has a test command for C++ compiler with -g -c option, which triggers the assertion
    //assert (false);
    return -1; 
  }

  cout<<"readProgramInfoFile() read line # "<< while_count<<endl;
  return while_count; 
}

/*
 *
int i; 
for (i=0; i< max; i++)
  _lc_counters[i]=0;  //array_counter_name
*/
void generateInitializeLoop(SgFunctionDeclaration* main_func)
{
   ROSE_ASSERT (main_func != NULL);
   SgScopeStatement* func_body_scope = main_func->get_definition()->get_body();
   
   ROSE_ASSERT (func_body_scope != NULL);
   // int i; 
   SgVariableDeclaration * var_decl = buildVariableDeclaration ("__lc_init_i", buildIntType(), NULL, func_body_scope);
   SgStatement* ldel = findLastDeclarationStatement(func_body_scope);
   if (ldel != NULL)
   {
     insertStatementAfter(ldel, var_decl);
   }
   else
   {
     prependStatement(var_decl, func_body_scope);
   }

   SgBasicBlock* loop_body = buildBasicBlock();

   SgForStatement* for_stmt = buildForStatement ( buildAssignStatement(buildVarRefExp("__lc_init_i", func_body_scope), buildIntVal(0)),
       buildExprStatement(buildLessThanOp( buildVarRefExp("__lc_init_i", func_body_scope), buildIntVal(loop_id_map.size()))), 
       buildPlusPlusOp ( buildVarRefExp("__lc_init_i", func_body_scope), SgUnaryOp::postfix),
       loop_body 
     );

   insertStatementAfter (var_decl, for_stmt);  

   // build   _lc_counters[i]=0;
   SgStatement* assign_stmt = buildAssignStatement (buildPntrArrRefExp(buildVarRefExp(array_counter_name, func_body_scope),  buildVarRefExp("__lc_init_i", func_body_scope)), 
       buildIntVal(0));
   appendStatement(assign_stmt, loop_body);

}

// each for loop, normalize it, add __lc_counters [loop_id_map[path]] ++; 
void instrumentForLoop (SgSourceFile * sfile)
{
  ROSE_ASSERT (sfile !=NULL);
 Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(sfile, V_SgStatement);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgStatement* stmt= isSgStatement(*i);
    if (insideSystemHeader(stmt))
      continue; 
    switch (stmt->variantT()) 
    {
      case V_SgForStatement:
        {
          SgInitializedName * loop_variable = getLoopIndexVariable (stmt);
          string loop_index_var_name = (loop_variable->get_name()).getString();
          // we must skip the translator introduced loop which initialize the counter array
          if (loop_index_var_name!= array_counter_loop_index_name)
          {
            Sg_File_Info * fi = stmt->get_file_info(); 
            ROSE_ASSERT(fi != NULL);
            string s1(":");
            string loop_id = fi->get_filename() + s1+ numberToString(fi->get_line()) + s1 + numberToString(fi->get_col());
            int lc_index = loop_id_map[loop_id];
            // __lc_counters[index] ++;
            SgExpression* array_ref = buildPntrArrRefExp(buildVarRefExp(array_counter_name, stmt->get_scope()), buildIntVal (lc_index));
            SgExpression* exp = buildPlusPlusOp (array_ref, SgUnaryOp::postfix);

            SgBasicBlock* lbody = ensureBasicBlockAsBodyOfFor(isSgForStatement(stmt));
            appendStatement(buildExprStatement(exp), lbody);
          }
          break;
        }
        default:
        break;
    }
  }
  
}

void generateResultsWritingCall (SgFunctionDeclaration* mfunc, string input_file_name, string file_w_loop_iter_count, string counter_name, int loop_count)
{
  ROSE_ASSERT (mfunc != NULL);
  SgBasicBlock* bb = mfunc->get_definition()->get_body();
  SgExprListExp* parameters = buildExprListExp (buildStringVal(input_file_name),buildStringVal(file_w_loop_iter_count), buildVarRefExp(counter_name, bb), buildIntVal(loop_count));
  SgExprStatement* fc = buildFunctionCallStmt ("__ci_writeResultsToFile", buildVoidType(),parameters , bb);
  instrumentEndOfFunction (mfunc, fc); 
}

void instrumentSourceFile (SgSourceFile * sfile, string file_w_loop_count, string file_w_loop_iter_count)
{
  assert (sfile != NULL);
  string filename = sfile->getFileName();
  SgGlobal * gscope = sfile->get_globalScope();
  ROSE_ASSERT (gscope != NULL);

  // declare unsigned int  _lc_counters[10000];
  SgVariableDeclaration* array_decl = 
    buildVariableDeclaration( array_counter_name , 
        buildArrayType(buildIntType(), buildIntVal(loop_id_map.size())), 
        NULL, gscope);

  // this is the full path
  //cout<<filename<<endl; 
  SgFunctionDeclaration* mfunc = findMain(sfile);
  if (mfunc)
  {
    insertStatementBefore(mfunc, array_decl); 
    generateInitializeLoop (mfunc);

    // for all return points, generate writeResultsToFile ()
    generateResultsWritingCall(mfunc, file_w_loop_count, file_w_loop_iter_count, array_counter_name, loop_id_map.size());
  }
  else
  {
    setExtern(array_decl);
    prependStatement(array_decl, gscope);
  } 

  // instrument all loops, add   _lc_counters[id]++; 
  instrumentForLoop (sfile);
}

int main (int argc, char** argv)
{
  // Some command line processing must happen before frontend() is called. 
  vector<string> argvList (argv, argv + argc); 

  if (CommandlineProcessing::isOption(argvList, "-exe_mode=", "loop_counting", true) )
  {
    cout<<"Running in the loop counting mode ... "<<endl;
    exe_mode = e_loop_counting;
  }
  else
  {
    exe_mode= e_loop_instrumenting; 
  }
 
  string s0("-file-for-loop-count");
  string s1("");
  CommandlineProcessing::isOptionWithParameter(argvList, s0, s1, file_w_loop_count, true);
  cout<<"Using a file for loop numbering and counting:"<< file_w_loop_count<<endl;

  string s2("-file-for-loop-iteration-count");
  string s3("");

  // default is to expand loop-count file with a suffix, unless specified with -file-for-loop-iteration-count filename
  if (!CommandlineProcessing::isOptionWithParameter(argvList, s2, s3, file_w_loop_iter_count, true))
  {
    file_w_loop_iter_count = file_w_loop_count+"-w-iter-count";
  }
  // Only the instrumentation mode needs the file name to save iteration count
  if (exe_mode == e_loop_instrumenting)
    cout<<"Using a file to save iter counts info into:"<< file_w_loop_iter_count<<endl;

 // debugging command line options now
#if 0 // Note: don't use rose translator as the linker, with an option to link with .a
      // This will cause error message since the translator will try to compile the binary .a file!!
  for (int ii=0; ii<argvList.size(); ii++)
  {
    cout<<"Debugging command line:"<<argvList[ii]<<endl;
  }
#endif  
  // ---------------end of command line processing -----------

  // Build the AST used by ROSE
  SgProject* project = frontend(argvList);

//option:  -exe_mode:loop_counting
  int read_count = 0; 
  if (exe_mode==e_loop_counting)
  {
    countingLoops (project);
  }
  else
  {
    SgFilePtrList file_list = project->get_files();
    // if the translator is used as a linker, then no need to read program info.
    // No source files are involved. 
    if (file_list.size()>0)
    {
      cout<<"Running in the loop instrumenting mode ... "<<endl;
      read_count = readProgramInfoFile();
    }

    // Do nothing if the loop count file is not available.
    if (read_count != -1 )
    {
      SgFilePtrList::iterator iter;
      for (iter= file_list.begin(); iter!=file_list.end(); iter++)
      {
        SgFile* cur_file = *iter;
        SgSourceFile * sfile = isSgSourceFile(cur_file);
        if (sfile!=NULL)
          instrumentSourceFile(sfile, file_w_loop_count, file_w_loop_iter_count);
      } // end for SgFile
    }
  }

  // Generate source code from AST and invoke your
  // desired backend compiler
  return backend(project);
}


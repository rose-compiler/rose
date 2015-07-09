// This forms the dot file for a call graph generated incrementally.
// Things to consider:
//   1) Generate subgraphs to show the seperate translation units (for each file).
//   2) Build a single data structure to represent the call graph and associated data:
//         Source position of function declaration, function definition, and function calls

// #include "rose.h"
#include "sage3basic.h"

#include "newCallGraph.h"

#include <fstream>

#if 1
// file locking support
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef _MSC_VER
#include <sys/resource.h>
#else
#include <windows.h>            // getpagesize()
#include "timing.h"             // gettimeofday()
#endif
#endif

#include <boost/thread.hpp>     // sleep()

#define DEBUG_CALLGRAPH 0

using namespace std;
using namespace NewCallGraph;


CallGraphNodeInfo::CallGraphNodeInfo(SgFunctionDeclaration* x)
   : functionDeclaration(x), skip_writing_out_to_file(false)
   {
     ROSE_ASSERT(functionDeclaration != NULL);
   }


CallGraphEdgeInfo::CallGraphEdgeInfo(CallGraphNodeInfo* from, CallGraphNodeInfo* to)
   : from(from), to(to), skip_writing_out_to_file(false)
   {
     ROSE_ASSERT(from != NULL);
     ROSE_ASSERT(to   != NULL);
   }


CallGraph::CallGraph()
   {
   }


CallGraphFileStructure::CallGraphFileStructure()
// : functionNameLength(256), fileNameLength(256)
// : isNode(true), function_name({0}), function_call_name({0}), file_name({0})
   : isNode(false), isDefiningDeclaration(false)
   {
     fill(function_name,function_name+functionNameLength,0);
     fill(function_call_name,function_call_name+functionNameLength,0);
     fill(file_name,file_name+fileNameLength,0);

     fill(directoryToUseInCompilingSourceFile,directoryToUseInCompilingSourceFile+directoryLength,0);
     fill(commandlineToCompileSourceFile,commandlineToCompileSourceFile+commandlineLength,0);
   }


void
CallGraph::visit ( SgNode* astNode )
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL)
        {
#if 0
          printf ("Found a function declaration: functionDeclaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
#if 0
          string unique_name = SageInterface::get_name(functionDeclaration);
          printf ("unique_name = %s \n",unique_name.c_str());
#endif
#if 1
       // string name = functionDeclaration->get_mangled_name();
          string name = SageInterface::get_name(functionDeclaration);

          if (functionDeclaration->get_definingDeclaration() != NULL)
             {
#if 0
               printf ("Defining function declaration DOES exist in this translation unit \n");
#endif
               if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                  {
                 // Check for if this function is in the nodeMap and if so update the pointer to the function declaration with the defining function declaration.
#if 1
                    printf ("This IS the defining function declaration: mangled_name = %s \n",name.c_str());
#endif
                    if (nodeMap.find(name) != nodeMap.end())
                       {
                         printf ("Update function declaration in CallGraphNodeInfo to use the defining function declaration \n");
                         nodeMap[name]->functionDeclaration = functionDeclaration;
                       }
                      else
                       {
#if 1
                         printf ("Insert a new node into the nodeMap for function = %s \n",functionDeclaration->get_name().str());
#endif
                         nodeMap.insert(pair<string,CallGraphNodeInfo*>(name,new CallGraphNodeInfo(functionDeclaration)));
                       }
                  }
                 else
                  {
#if 0
                    printf ("This is NOT the defining function declaration, but insert this function declaration into the nodeMap \n");
#endif
                 // Insert the node into the node map so that we can build edges ("from" and "to" nodes are required to define edges).
                    nodeMap.insert(pair<string,CallGraphNodeInfo*>(name,new CallGraphNodeInfo(functionDeclaration)));
                  }
             }
            else
             {
#if 0
               printf ("Defining function declaration does NOT exist in this translation unit, but insert this function declaration into the nodeMap \n");
#endif
               nodeMap.insert(pair<string,CallGraphNodeInfo*>(name,new CallGraphNodeInfo(functionDeclaration)));
             }
#endif

       // form dot file string for function definition
        }

     SgFunctionRefExp* functionRefExpression = isSgFunctionRefExp(astNode);
     if (functionRefExpression != NULL)
        {
          SgFunctionSymbol* functionSymbol = functionRefExpression->get_symbol();
          ROSE_ASSERT(functionSymbol != NULL);

          printf ("Found a function ref expression: functionRefExpression = %p = %s = %s \n",functionRefExpression,functionRefExpression->class_name().c_str(),functionSymbol->get_name().str());

       // How do we handle a function call from global scope (e.g. constructor initialization)?
          SgFunctionDeclaration* from_functionDeclaration = SageInterface::getEnclosingFunctionDeclaration(functionRefExpression);
          ROSE_ASSERT(from_functionDeclaration != NULL);

          SgFunctionDeclaration* to_functionDeclaration   = functionSymbol->get_declaration();
          ROSE_ASSERT(to_functionDeclaration != NULL);

       // string from_function_name = from_functionDeclaration->get_mangled_name();
       // string to_function_name   = to_functionDeclaration->get_mangled_name();
          string from_function_name = SageInterface::get_name(from_functionDeclaration);
          string to_function_name   = SageInterface::get_name(to_functionDeclaration);

          string edge_name = from_function_name + "--to--" + to_function_name;

          printf ("   edge_name = %s \n",edge_name.c_str());

       // CallGraphNodeInfo* from = new CallGraphNodeInfo(from_functionDeclaration);
          CallGraphNodeInfo* from = NULL;
          if (nodeMap.find(from_function_name) != nodeMap.end())
             {
               from = nodeMap[from_function_name];
             }
            else
             {
            // This might be a case of a C language implicit function call (function without a prtotype).
               from = new CallGraphNodeInfo(from_functionDeclaration);

            // We might want to insert a call graph node to stand for an implicit function.
            // edgeMap.insert(pair<string,CallGraphEdgeInfo*>(name,new CallGraphEdgeInfo(from,to)));

               printf ("ERROR: Call to function not in node map! \n");
               ROSE_ASSERT(false);
             }

          CallGraphNodeInfo* to = NULL;
          if (nodeMap.find(to_function_name) != nodeMap.end())
             {
               to = nodeMap[to_function_name];
             }
            else
             {
            // This might be a case of a C language implicit function call (function without a prototype).
            // Or in C++ when a function call appears before it's prototype or definition in a class or structure.
               to = new CallGraphNodeInfo(to_functionDeclaration);

               printf ("Adding node to graph for function call to unseen function \n");

               nodeMap.insert(pair<string,CallGraphNodeInfo*>(to_function_name,to));
#if 0
               printf ("ERROR: Call to function not in node map! \n");
               ROSE_ASSERT(false);
#endif
             }

          ROSE_ASSERT(from != NULL);
          ROSE_ASSERT(to != NULL);

          edgeMap.insert(pair<string,CallGraphEdgeInfo*>(edge_name,new CallGraphEdgeInfo(from,to)));


       // form dot file string for function call
        }
   }


int
NewCallGraph::getLock()
   {
     int fd;

  // printf ("Build the lock file \n");
  // generate a lock 
     if ( SgProject::get_verbose() >= 0 )
          printf ("Acquiring a lock: rose_callgraph_lockfile.lock \n");

  // DQ (8/24/2008): Setup counters to detect when file locks are in place (this was a problem this morning)
     unsigned long counter             = 0;
     const unsigned long userTolerance = 10;

     while ( (fd = open("rose_callgraph_lockfile.lock", O_WRONLY | O_CREAT | O_EXCL)) == -1 )
        {
       // Skip the message output if this is the first try!
          if ( counter > 0 )
               printf ("Waiting for lock! counter = %lu userTolerance = %lu \n",counter,userTolerance);

#ifdef _MSC_VER
          Sleep(1000);
#else
          sleep(1);
#endif
          counter++;

       // DQ (8/24/2008): If after waiting a short while and the lock is still there, then report the issue.
          if ( counter > userTolerance )
             {
               printf ("Waiting for file lock (run \"make clean\" to remove lock files, if problem persists)... \n");

            // Reset the counter to prevent it from over flowing on nightly tests, though that might take a long time :-).
               counter = 1;
             }
        }

     if (fd == -1)
        {
          perror("error in opening lock file: rose_callgraph_lockfile.lock");
       // exit(1);
        }

     return fd;
   }


void
NewCallGraph::releaseLock (int fd )
   {
     close(fd);

     if ( SgProject::get_verbose() >= 0 )
          printf ("Removing (releasing) the lock file \n");

     remove("rose_callgraph_lockfile.lock");
   }

vector<CallGraphFileStructure>*
NewCallGraph::readCallGraphFile (string binaryFilename)
   {
  // Open the callgraph file and fill in the nodeMap and edgeMap in the CallGraph.

  // Open the file and put the file cursor at the end of the file so that the file size can be computed.
     ifstream binaryStream(binaryFilename.c_str(), ifstream::ate | ifstream::binary);

#if 0
  // I don't think this means what I thought it meant.
     if ( binaryStream.bad() == true )
        {
          printf ("Error, some sort of problem with file (not just that it does not exist) \n");
          ROSE_ASSERT(false);
        }
#endif

     if ( binaryStream.good() == false )
        {
#if 1
          printf ("Binary file failed to open (does not exist) \n");
#endif
          return NULL;
        }

  // Must have used ifstream::ate or this will be zero.
     size_t fileSize = binaryStream.tellg();
     size_t sizeOfEntry = sizeof(CallGraphFileStructure);
     size_t numberOfEntries = fileSize / sizeOfEntry;

#if 1
     printf ("fileSize = %zu \n",fileSize);
     printf ("sizeOfEntry = %zu numberOfEntries = %zu \n",sizeOfEntry,numberOfEntries);
     printf ("numberOfEntries * sizeOfEntry = %zu \n",numberOfEntries * sizeOfEntry);
#endif

  // This should make sure that we have a file with a size that is a whole multiple of the CallGraphFileStructure data members.
     ROSE_ASSERT(numberOfEntries * sizeOfEntry == fileSize);

  // allocate space for data read from file
     vector<CallGraphFileStructure>* callGraphVector = new vector<CallGraphFileStructure>(numberOfEntries);

#if 0
     printf ("callGraphVector = %p \n",callGraphVector);
#endif

  // Build a pointer to the first element of the STL vector of entries allocated above.
     CallGraphFileStructure* data = &((*callGraphVector)[0]);

  // Set the file cursor to the begining so that we can read the whole file.
     binaryStream.seekg(0, ios::beg);

  // Read the file from the current file position.
     binaryStream.read(reinterpret_cast<char*>(data), fileSize);

  // Close the file (for reading), we will re-open it for writing (overwriting with new data)
     binaryStream.close();

     return callGraphVector;
   }


void
NewCallGraph::generateCallGraphFile(SgProject* project, CallGraph & cg)
   {
  // output data on call graph from analysis of each translation unit.

     ROSE_ASSERT(project != NULL);
     ROSE_ASSERT(project->numberOfFiles() > 0);

     string output_filename = "textCallgraph";

  // printf ("Get the lock ... \n");

  // generate a lock 
     int fd = NewCallGraph::getLock();
     ROSE_ASSERT(fd > 0);
  // printf ("Got the lock ... \n");

  // This file is to be used for debugging (a simple text form of the binary file's data.
  // This file in regenerated from scratch the same as the binary file.
  // ofstream datafile ( output_filename.c_str() , ios::out | ios::app );
     ofstream datafile ( output_filename.c_str() , ios::out );

     printf ("WARNING: part 1: text file is not yet implemented to be consistatn with the binary file! \n");

     if ( datafile.good() == false )
        {
          printf ("Text file failed to open \n");
          ROSE_ASSERT(false);
        }

     string binaryFilename = "binaryCallGraph";

     printf ("Read the binary file (if it exists) \n");

  // Read the existing file (if it exists) of previously processed files and their parts of the call graph.
     vector<CallGraphFileStructure>* callGraphVector = readCallGraphFile(binaryFilename);

  // Reopen the file for writing (we still have the lock), and position the file cursor at 
  // the start of the file so that it can be overwritten with updated data (e.g. where we 
  // have located defining declaration in the current translation unit that were referenced 
  // using non-defining declaration in the entries from the binary file).
  // ofstream binaryStream(binaryFilename.c_str(), std::ios::binary);
  // ofstream binaryStream(binaryFilename.c_str(), ofstream::ate | ios::binary);
  // ofstream binaryStream(binaryFilename.c_str(), ofstream::app | ios::binary);
     ofstream binaryStream(binaryFilename.c_str(), ios::binary);

     if ( binaryStream.good() == false )
        {
          printf ("Binary file failed to open \n");
          ROSE_ASSERT(false);
        }

  // datafile << "This is a test!" << std::endl;

  // This assumes that there is only one source file per invokation of ROSE on the command line.
     string source_file = project->get_file(0).get_sourceFileNameWithPath();

  // Enforce a static limit until we can support a dynamic length string.
     ROSE_ASSERT(source_file.length() < fileNameLength);

  // Put the data for each ProcessingPhase out to a CSV formatted file
  // output the data
  // datafile << "filename," << source_file << ", number of AST nodes, " << numberOfNodes() << ", memory, " << memoryUsage() << " ";

     map<string,CallGraphNodeInfo*> & nodeMap = cg.nodeMap;
     map<string,CallGraphEdgeInfo*> & edgeMap = cg.edgeMap;

     printf ("After reading existing call graph: callGraphVector = %p \n",callGraphVector);

  // Check if we have entries from an existing binary file of call graph data.
     if (callGraphVector != NULL)
        {
          vector<CallGraphFileStructure>::iterator i = callGraphVector->begin();

       // Iterate over the data from the file of previously build call graph data (from the saved file).
          while (i != callGraphVector->end())
             {
               if ( i->isNode == true)
                  {
#if 0
                    printf ("process call graph entry read from binary file as NODE \n");
                    printf ("   --- function name = %s \n",i->function_name);
#endif
                    string function_name = i->function_name;
                    string filename = i->file_name;

                    if (nodeMap.find(function_name) != nodeMap.end())
                       {
#if 0
                         printf ("   --- Found a node from the binary file that matches an entry in this translation unit \n");
#endif
                         CallGraphNodeInfo* functionNode = nodeMap[function_name];
                         ROSE_ASSERT(functionNode != NULL);

                         functionNode->skip_writing_out_to_file = true;

                         SgFunctionDeclaration* functionDeclaration = functionNode->functionDeclaration;
                         ROSE_ASSERT(functionDeclaration != NULL);

                      // If this translation unit has the defining declaration then update the source file in the date we write (update) in the saved binary file.
                         if (functionDeclaration == functionDeclaration->get_definingDeclaration())
                            {
                              if (i->isDefiningDeclaration == false)
                                 {
                                // Change the file name associated with this function to the current translation unit.
                                   filename = source_file;
                                 }
                                else
                                 {
                                // Use the file name recorded from the first translation using to see the function definition.
                                   printf ("Note: This node (function_name = %s) was previously defined in filename = %s but is also defined in source_file = %s \n",function_name.c_str(),filename.c_str(),source_file.c_str());
                                 }
                            }
                       }
                      else
                       {
#if 0
                         printf ("Found a function not in the current translation unit: function_name = %s \n",function_name.c_str());
#endif
                       }

                 // This function from the binary file is not in the current translation unit 
                 // (and we can't add it to the CallGraphNodeInfo because the function declaration 
                 // is not in this translation unit).  So we can skip processing it here.
#if 0
                    printf ("Add this a node entry to the data that will be written to the binary file (overwriting the existing binary file) \n");
                    printf ("   --- node name = %s \n",i->function_name);
#endif
                    CallGraphFileStructure function;

                    function.isNode = true;
                    function.isDefiningDeclaration = i->isDefiningDeclaration;

                    memcpy (function.function_name, i->function_name, strlen(i->function_name)+1);
                 // memcpy (function.file_name, i->file_name, strlen(i->file_name)+1);
                    memcpy (function.file_name, filename.c_str(), filename.length());

                    binaryStream.write(reinterpret_cast<const char*>(&function), sizeof(CallGraphFileStructure));
#if 0
                    printf ("WARNING: part 2: text file is not yet implemented to be consistatn with the binary file! \n");
#endif
                    SgName mangledName = i->function_name;

                    ROSE_ASSERT(mangledName.get_length() < functionNameLength);

                    string s = "ORIGINAL NODE: filename," + source_file + ", function, " + mangledName.str() + "\n";

                    datafile << s;
                  }
                 else
                  {
#if 0
                    printf ("process call graph entry read from binary file as EDGE \n");
                    printf ("   --- from function name    = %s \n",i->function_name);
                    printf ("   --- to function call name = %s \n",i->function_call_name);
#endif
                    string from_function_name = i->function_name;
                    string to_function_name   = i->function_call_name;

                    string edge_name = from_function_name + "--to--" + to_function_name;

                    if (edgeMap.find(edge_name) != edgeMap.end())
                       {
                      // This edge can be marked to not be updated from the current translation unit.
#if 0
                         printf ("   --- Found an edge from the binary file that matches an entry in this translation unit (mark to avoid redundnat edge) \n");
#endif
                         CallGraphEdgeInfo* functionEdge = edgeMap[edge_name];
                         ROSE_ASSERT(functionEdge != NULL);

                         functionEdge->skip_writing_out_to_file = true;
                       }

                 // This function call (edge) from the binary file is not in the current translation unit 
                 // (and we can't add it to the CallGraphNodeInfo because the edge is not is not in this 
                 // translation unit).  So we need to write it into the binary file.
#if 0
                    printf ("Add this as an edge entry to the data that will be written to the binary file (overwriting the existing binary file) \n");
                    printf ("   --- edge name = %s \n",edge_name.c_str());
#endif
                    CallGraphFileStructure functionCall;

                    functionCall.isNode = false;
                 // This is false since it does not apply to edges.
                    functionCall.isDefiningDeclaration = false;

                 // memcpy (functionCall.function_name,      i->function_name, strlen(i->function_name)+1);
                 // memcpy (functionCall.function_call_name, to_function_name, strlen(to_function_name)+1);
                    memcpy (functionCall.function_name,      from_function_name.c_str(), from_function_name.length());
                    memcpy (functionCall.function_call_name, to_function_name.c_str(),   to_function_name.length());

                    binaryStream.write(reinterpret_cast<const char*>(&functionCall), sizeof(CallGraphFileStructure));
#if 0
                    printf ("WARNING: part 3: text file is not yet implemented to be consistatn with the binary file! \n");
#endif
#if 0
                    string s = "ORIGINAL NODE: filename," + source_file + ", function, " + mangledName.str() + "\n";
                    datafile << s;
#endif
                 // string edge_name = (*i_edge).first;
                 // SgName from_mangledName = (*i_edge).second->from->functionDeclaration->get_mangled_name();
                 // SgName to_mangledName   = (*i_edge).second->to->functionDeclaration->get_mangled_name();

                 // string s = "ORIGINAL EDGE: filename," + source_file + 
                 //            ", from function, " + from_function_name.c_str() + 
                 //            ", to function, " + to_function_name.c_str() + "\n";
#if 1
                    string s = "ORIGINAL EDGE: filename," + source_file + ", edge, " + edge_name + "\n";

                    datafile << s;
#endif
                  }

               i++;
             }
        }

     printf ("Output nodes on call graph: \n");

     printf ("sizeof(CallGraphFileStructure) = %zu \n",sizeof(CallGraphFileStructure));

     printf ("Writing %zu nodes to the file \n",nodeMap.size());

     map<string,CallGraphNodeInfo*>::iterator i_node = nodeMap.begin();
     while (i_node != nodeMap.end())
        {
       // SgName mangledName = (*i_node).second->functionDeclaration->get_mangled_name();
          SgName mangledName = SageInterface::get_name((*i_node).second->functionDeclaration);

          ROSE_ASSERT(mangledName.get_length() < functionNameLength);

       // string s = "filename," + source_file + ", function, " + (*i_node).second->functionDeclaration->get_name().str() + "\n";
       // datafile << s;
#if 0
          printf ("   s = %s",s.c_str());
          printf ("   --- mangledName = %s length = %d \n",mangledName.str(),mangledName.get_length());
          printf ("   --- source_file = %s length = %zu \n",source_file.c_str(),source_file.length());
#endif
       // The data structure is initialized to zero in the constuctor.
       // CallGraphFileStructure function = { true, {0}, {0}, {0} };
          CallGraphFileStructure function;

       // This test avoid recording nodes in the call graph redundantly in the binary file.
          if ((*i_node).second->skip_writing_out_to_file == false)
             {
               function.isNode = true;
               if ((*i_node).second->functionDeclaration == (*i_node).second->functionDeclaration->get_definingDeclaration())
                  {
                    function.isDefiningDeclaration = true;
                  }
                 else
                  {
                    function.isDefiningDeclaration = false;
                  }
               memcpy (function.function_name, mangledName.str(), mangledName.get_length());
               memcpy (function.file_name, source_file.c_str(), source_file.length());

               binaryStream.write(reinterpret_cast<const char*>(&function), sizeof(CallGraphFileStructure));
#if 0
               printf ("WARNING: part 4: text file is not yet implemented to be consistatn with the binary file! \n");
#endif
            // SgName mangledName = i->function_name;

            // ROSE_ASSERT(mangledName.get_length() < functionNameLength);

            // string s = "original entry: filename," + source_file + ", function, " + mangledName.str() + "\n";
            // string s = "NEW NODE: in this tranalation unit: filename," + source_file + ", function, " + (*i_node).second->functionDeclaration->get_name().str() + "\n";
               string s = "NEW NODE: in this tranalation unit: filename," + source_file + ", function, " + mangledName.str() + "\n";
#if 0
               printf ("   s = %s",s.c_str());
               printf ("   --- mangledName = %s length = %d \n",mangledName.str(),mangledName.get_length());
               printf ("   --- source_file = %s length = %zu \n",source_file.c_str(),source_file.length());
#endif
               datafile << s;
             }
            else
             {
#if 0
               printf ("Skip output of this node for a function in this translation unit) to the binary file (redundantly present in existing file) \n");
               printf ("   --- mangledName = %s length = %d \n",mangledName.str(),mangledName.get_length());
               printf ("   --- source_file = %s length = %zu \n",source_file.c_str(),source_file.length());
#endif
             }

          i_node++;
        }

     printf ("Writing %zu edges to the file \n",edgeMap.size());

     map<string,CallGraphEdgeInfo*>::iterator i_edge = edgeMap.begin();
     while (i_edge != edgeMap.end())
        {
          ROSE_ASSERT((*i_edge).second != NULL);
          ROSE_ASSERT((*i_edge).second->from != NULL);
          ROSE_ASSERT((*i_edge).second->from->functionDeclaration != NULL);
          ROSE_ASSERT((*i_edge).second->to != NULL);
          ROSE_ASSERT((*i_edge).second->to->functionDeclaration != NULL);

          string edge_name = (*i_edge).first;

       // SgName from_mangledName = (*i_edge).second->from->functionDeclaration->get_mangled_name();
       // SgName to_mangledName   = (*i_edge).second->to->functionDeclaration->get_mangled_name();
          SgName from_mangledName = SageInterface::get_name((*i_edge).second->from->functionDeclaration);
          SgName to_mangledName   = SageInterface::get_name((*i_edge).second->to->functionDeclaration);
#if 0
          string s = "filename," + source_file + 
                     ", from function, " + (*i_edge).second->from->functionDeclaration->get_name().str() + 
                     ", to function, "   + (*i_edge).second->to->functionDeclaration->get_name().str() + "\n";

          datafile << s;
#endif
#if 0
          printf ("   s = %s \n",s.c_str());
#endif
          CallGraphFileStructure functionCall;

          if ((*i_edge).second->skip_writing_out_to_file == false)
             {
               functionCall.isNode = false;

            // Always false for a call graph edge.
               functionCall.isDefiningDeclaration = false;

               memcpy (functionCall.function_name,      from_mangledName.str(), from_mangledName.get_length());
               memcpy (functionCall.function_call_name, to_mangledName.str(),   to_mangledName.get_length());

               binaryStream.write(reinterpret_cast<const char*>(&functionCall), sizeof(CallGraphFileStructure));
#if 0
               printf ("WARNING: part 5: text file is not yet implemented to be consistatn with the binary file! \n");
#endif
            // string s = "NEW EDGE: filename," + source_file + 
            //       ", from function, " + (*i_edge).second->from->functionDeclaration->get_name().str() + 
            //       ", to function, "   + (*i_edge).second->to->functionDeclaration->get_name().str() + "\n";
               string s = "NEW EDGE: filename," + source_file + ", edge, " + edge_name + "\n";

               datafile << s;
             }
            else
             {
#if 1
               printf ("Skip output of this edge for a function call in this translation unit) to the binary file (redundantly present in existing file) \n");
               printf ("   --- edge_namee = %s \n",edge_name.c_str());
               printf ("   --- source_file = %s length = %zu \n",source_file.c_str(),source_file.length());
#endif
             }

          i_edge++;
        }

     datafile << endl;

  // printf ("Done: Output the data to the file ... (calling flush) \n");

     datafile.flush();

     binaryStream.flush();

  // printf ("Done with file flush() ... \n");

  // release the lock
  // printf ("Releasing the file lock \n");
     NewCallGraph::releaseLock(fd);

  // printf ("File output complete \n");
     datafile.close();
     binaryStream.close();
   }



void 
NewCallGraph::generateDotFile (string binaryFilename)
   {
     vector<CallGraphFileStructure>* callGraphVector = NewCallGraph::readCallGraphFile (binaryFilename);

     if (callGraphVector == NULL)
        {
          return;
        }

     ROSE_ASSERT(callGraphVector != NULL);

     vector<CallGraphFileStructure>::iterator i = callGraphVector->begin();

  // Iterate over the data from the file of previously build call graph data (from the saved file).
     while (i != callGraphVector->end())
        {
          if ( i->isNode == true)
             {
#if 0
               printf ("process call graph entry read from binary file as NODE \n");
               printf ("   --- function name = %s \n",i->function_name);
#endif
               string function_name = i->function_name;
               string filename = i->file_name;

             }
            else
             {
#if 0
               printf ("process call graph entry read from binary file as EDGE \n");
               printf ("   --- from function name    = %s \n",i->function_name);
               printf ("   --- to function call name = %s \n",i->function_call_name);
#endif
               string from_function_name = i->function_name;
               string to_function_name   = i->function_call_name;

               string edge_name = from_function_name + "--to--" + to_function_name;
             }

          i++;
        }
   }



int
NewCallGraph::buildCallGraph (SgProject* project)
   {
     int status = 0;

     ROSE_ASSERT (project != NULL);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // Put analysis traversal here!
     CallGraph treeTraversal;
#if 0
     treeTraversal.traverseInputFiles ( project, preorder );
#else
     treeTraversal.traverse ( project, preorder );
#endif

  // Write the file using the collected data from this translation unit.
     generateCallGraphFile(project,treeTraversal);

     return status;
   }


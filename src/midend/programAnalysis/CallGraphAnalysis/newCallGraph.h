
// DQ (6/30/2015): This is a new call graph analysis designed to support intremental
// generation of the a dot file representing the call graph of an application as it
// is compiled file by file.  It is the start of a different sort fo ultifile support 
// in ROSE.

namespace NewCallGraph
   {

     class CallGraphNodeInfo
        {
       // Express nodes in terms of function declarations for now.
       // Later this will have to be mangled names.
          public:
               CallGraphNodeInfo(SgFunctionDeclaration* x);
               SgFunctionDeclaration* functionDeclaration;

            // Avoid redundant nodes (e.g. inlined functions that would be seen in each translation unit).
               bool skip_writing_out_to_file;
        };

     class CallGraphEdgeInfo
        {
       // Express edges in terms of links between nodes.
          public:
            // SgFunctionDeclaration* from_functionDeclaration;
            // SgFunctionDeclaration* to_functionDeclaration;
               CallGraphEdgeInfo(CallGraphNodeInfo* from, CallGraphNodeInfo* to);
               CallGraphNodeInfo* from;
               CallGraphNodeInfo* to;

            // Avoid redundant edges (e.g. function call in inlined functions that would be seen in each translation unit).
               bool skip_writing_out_to_file;
        };

     class CallGraph: public SgSimpleProcessing
        {
          public:
               std::map<std::string,CallGraphNodeInfo*> nodeMap;
               std::map<std::string,CallGraphEdgeInfo*> edgeMap;

               CallGraph();
               void visit ( SgNode* astNode );
        };

     class CallGraphFileStructure
        {
       // This data type is used to write the call graph nodes and edges to a file 
       // to support accumulation of multiple file data.

       // Note: 
       //   1) If we want to write and read variable length strings then we would have 
       // to write the size of the string into the binary file, read that value, then 
       // read that many bytes and put the data into a string.  
       //   2) We could also write out a table into the top of the binary file to hold
       // all of the file names, directory names, and command lines used to compile the
       // source files to generate each function.  This would be a space optimization, 
       // to be done later.

#define functionNameLength 256
#define fileNameLength     256
#define directoryLength   1000
#define commandlineLength 2000

          public:
            // const size_t functionNameLength;
            // const size_t fileNameLength;

               bool isNode; // if not a node then this is a call graph edge.
               bool isDefiningDeclaration;
               char function_name[functionNameLength]; // If this is an edge this is the "from" function.
#if 0
            // Use of a union is just a space optimization.
               union name_data
                  {
                    char function_call_name[functionNameLength]; // if this is an edge then this is the "to" function.
                    char file_name[fileNameLength]; // if this is a node then this is the absolute path with the filename for the function.
                  };
#else
               char function_call_name[functionNameLength]; // if this is an edge then this is the "to" function.
               char file_name[fileNameLength]; // if this is a node then this is the absolute path with the filename for the function.
#endif
            // If this is an node then this is the location from which to compile the source code (using the saved command line).
               char directoryToUseInCompilingSourceFile[directoryLength];

            // This is a potentially very long string.
            // If this is an node then this is the command line required to compile the source file to generate the AST for the defining function declaration.
               char commandlineToCompileSourceFile[commandlineLength];

               CallGraphFileStructure();
        };

  // File support
     int getLock();
     void releaseLock (int fd );
     void generateCallGraphFile(SgProject* project, CallGraph & cg);
  // void readCallGraphFile(SgProject* project, CallGraph & cg);
     std::vector<CallGraphFileStructure>* readCallGraphFile(std::string binaryFilename);

  // Write out the dot file for the call graph (uses data from whatever is written in the binary file).
     void generateDotFile (std::string binaryFilename);

  // API function
     int buildCallGraph (SgProject* project);
   }

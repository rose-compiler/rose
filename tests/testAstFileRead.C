#include "rose.h"

// Note that this is required to define the Sg_File_Info_getPointerFromGlobalIndex( unsigned long ) function.
// This function is used to reset the Sg_File_Info object in the statically defined filename/integer maps.
#include "Cxx_GrammarMemoryPoolSupport.h"

using namespace std;

class AstFileSpecificInfo
   {
     public:
       // This is the pointer to the global function type table for each AST file
          SgFunctionTypeTable* functionTable;

       // These are the base and bound in the Sg_File_Info object memory pool for each AST file.
       // Sg_File_Info objects in this range must have their file name ID's be changed to a global mapping.
          int baseOfASTFileInfo;
          int boundOfASTFileInfo;

       // These are the static maps defined to hold file number codes to file name string mappings for each AST file.
          map<int, std::string> fileidtoname_map;
          map<std::string, int> nametofileid_map;

     public:
         AstFileSpecificInfo( int base, int bound );

         void fixupAstFileSpecificInfo(SgFunctionTypeTable* input_functionTable, const map<int, std::string> & input_fileidtoname_map, const map<std::string, int> & input_nametofileid_map);

         void display( const string & label);
   };

AstFileSpecificInfo::AstFileSpecificInfo(int base, int bound)
   {
  // This is the only data set by the construction (because this data must be generated 
  // as each AST file is read from disk, so that e get the base and bound for each AST 
  // file's set of Sg_File_Info in the memory pool).
     baseOfASTFileInfo  = base;
     boundOfASTFileInfo = bound;

  // This data needs to be reset after we have called AST_FILE_IO::setStaticDataOfAst() for each AST.
     functionTable      = NULL;

  // display("Inside of AstFileSpecificInfo constructor");
   }

void
AstFileSpecificInfo::fixupAstFileSpecificInfo(SgFunctionTypeTable* input_functionTable, const map<int, std::string> & input_fileidtoname_map, const map<std::string, int> & input_nametofileid_map)
   {
     functionTable      = input_functionTable;
     fileidtoname_map   = input_fileidtoname_map;
     nametofileid_map   = input_nametofileid_map;

  // display("Inside of AstFileSpecificInfo::fixupAstFileSpecificInfo()");
   }

void
AstFileSpecificInfo::display( const string & label)
   {
     printf ("Inside of AstFileSpecificInfo::display(%s) \n",label.c_str());
     printf ("     functionTable          = %p \n",functionTable);
     printf ("     base bound of fileInfo = (%d,%d) \n",baseOfASTFileInfo,boundOfASTFileInfo);
     printf ("     fileidtoname_map size  = %zu \n",fileidtoname_map.size());
     printf ("     nametofileid_map size  = %zu \n",nametofileid_map.size());
  // printf ("\n");
   }

SgFunctionTypeTable* mergeFunctionTypeSymbolTables ( vector<SgFunctionTypeTable*> functionTableArray );
void mergeStaticASTFileInformation(vector<AstFileSpecificInfo*> & AstFileInfoArray);


int
main ( int argc, char * argv[] )
   {
     ROSE_ASSERT(argc >= 0);
     if (argc == 0)
        {
          printf ("Error: This AST file reader requires the name of a binary AST file. \n");
          ROSE_ASSERT(false);
        }

#if 0
  // DQ (2/23/2010): This can't be called since it will generate a SgFunctionTypeTable and the memory pools must be empty.
     printf ("Before reading AST files: SgNode::get_globalFunctionTypeTable() = %p \n",SgNode::get_globalFunctionTypeTable());
  // ROSE_ASSERT(isSgFunctionTypeTable(SgNode::get_globalFunctionTypeTable()) != NULL);
#endif

     int numFiles = argc - 1;
     std::vector<std::string> fileNames;
     for (int i= 0; i < numFiles; ++i)
        {
          fileNames.push_back(argv[i+1]) ;
        }

#if 0
     cout << "################ In astFileRead.C ############## " << endl;
     cout << "#  Going to read " << numFiles << " files  " << endl;
     cout << "################################################ " << endl;
#endif

  // We should be built into a data structure to hold this sepeaate file by file AST information.
  // Static data structures in ROSE that require special handling when reading more than one AST from a file.
     vector<SgFunctionTypeTable*> functionTableArray;
  // vector<map<int, std::string> > fileidtoname_mapArray;
  // vector<map<std::string, int> > nametofileid_mapArray;

  // This uses the new data structure to hold all the required information for each 
  // AST file so that we can do a merge of the static information of the AST.
     vector<AstFileSpecificInfo*> AstFileInfoArray;

     int currentNumberOfNodes  = 0;
     int previousNumberOfNodes = 0;

  // cout  << endl << "Here we call the AST_FILE_IO :: readASTFromFile ..." << endl;
     for (int i= 0; i < numFiles; ++i)
        {
       // cout  << "Here we read .... " << fileNames[i] << endl;
          AST_FILE_IO :: readASTFromFile ( fileNames[i] + ".binary" );

          currentNumberOfNodes = Sg_File_Info::numberOfNodes();

          printf ("file #%d = %s AST size = %d memory usage = %d Sg_File_Info::numberOfNodes() = %d \n",i,fileNames[i].c_str(),numberOfNodes(),memoryUsage(),currentNumberOfNodes);
#if 0
          printf ("In loop reading AST files: SgNode::get_globalFunctionTypeTable() = %p \n",SgNode::get_globalFunctionTypeTable());
          printf ("file #%d AST Sg_File_Info::numberOfNodes() = %d \n",i,currentNumberOfNodes);
#endif
       // Some of this information has to be setup after we call AST_FILE_IO::setStaticDataOfAst() for each AST.
       // AstFileSpecificInfo* astFileSpecificInformation = new AstFileSpecificInfo(SgNode::get_globalFunctionTypeTable(),previousNumberOfNodes,currentNumberOfNodes,Sg_File_Info::get_fileidtoname_map(),Sg_File_Info::get_nametofileid_map());
          AstFileSpecificInfo* astFileSpecificInformation = new AstFileSpecificInfo(previousNumberOfNodes,currentNumberOfNodes-1);
          ROSE_ASSERT(astFileSpecificInformation != NULL);
          AstFileInfoArray.push_back(astFileSpecificInformation);

       // Track the positions of the base and bound of the Sg_File_Info list in the memory pool.
          previousNumberOfNodes = currentNumberOfNodes;
        }

  // cout  << endl << "AST_FILE_IO :: readASTFromFile done ... " << endl;

  // SgFunctionTypeTable* globalFunctionTypeTable = NULL;
     SgProject*           globalProject                 = NULL;

     ROSE_ASSERT(AstFileInfoArray.size() == (size_t)numFiles);

     AstData* ast = NULL;
     for (int i= 0; i < numFiles; ++i)
        {
          ast = AST_FILE_IO::getAst(i);
          AST_FILE_IO::setStaticDataOfAst(ast);

#if 0
          printf ("In loop reading AST files: SgNode::get_globalFunctionTypeTable() = %p \n",SgNode::get_globalFunctionTypeTable());
          printf ("In loop reading AST files: Sg_File_Info::get_fileidtoname_map() = %p size() = %zu \n",&Sg_File_Info::get_fileidtoname_map(),Sg_File_Info::get_fileidtoname_map().size());
          printf ("In loop reading AST files: Sg_File_Info::get_nametofileid_map() = %p size() = %zu \n",&Sg_File_Info::get_nametofileid_map(),Sg_File_Info::get_nametofileid_map().size());
          printf ("Sg_File_Info::numberOfNodes() = %d \n",Sg_File_Info::numberOfNodes());
#endif
          ROSE_ASSERT(Sg_File_Info::get_fileidtoname_map().size() == Sg_File_Info::get_nametofileid_map().size());

       // We need to make a second call to fixup the information in the AstFileSpecificInfo objects once we have called AST_FILE_IO::setStaticDataOfAst() for each AST.
          AstFileInfoArray[i]->fixupAstFileSpecificInfo(SgNode::get_globalFunctionTypeTable(),Sg_File_Info::get_fileidtoname_map(),Sg_File_Info::get_nametofileid_map());

       // Save references to all the function type tables.
          functionTableArray.push_back(SgNode::get_globalFunctionTypeTable());

       // Save copies of all the filename maps (copies because they are not implemented via a static pointer).
       // fileidtoname_mapArray.push_back(Sg_File_Info::get_fileidtoname_map());
       // nametofileid_mapArray.push_back(Sg_File_Info::get_nametofileid_map());

#if 0
       // DQ (2/24/2010): This is a significant bottleneck to the performance on large codes since it is n^2 in the size of the AST.
          AstTests::runAllTests(ast->getRootOfAst());
#endif
#if 0
       // To read and merge the separate AST files, we don't have to call the backend.
          backend(ast->getRootOfAst());
#endif
          if (globalProject == NULL)
             {
               globalProject = ast->getRootOfAst();
             }
            else
             {
               SgProject* localProject = ast->getRootOfAst();
               SgFile*    localFile    = (*localProject)[0];
               ROSE_ASSERT(localProject->numberOfFiles() == 1);

            // Add the file to the global project. This also sets the parent of the input file. Is this a side-effect that we want?
               globalProject->set_file(*localFile);
#if 1
               printf ("globalProject = %p numberOfFiles() = %d \n",globalProject,globalProject->numberOfFiles());
#endif
//             delete localProject;
             }
        }

#if 0
     for (int i= 0; i < numFiles; ++i)
        {
          AstFileInfoArray[i]->display("static AST file information");
        }
#endif

#if 0
  // DQ (2/24/2010): Better to run this once at the end to avoid a significant bottleneck to the 
  // performance on large codes (it is n^2 in the size of the AST if run for each file separately).
     AstTests::runAllTests(ast->getRootOfAst());
#endif

  // Merge the function tables from each AST.
  // SgFunctionTypeTable* globalFunctionTypeTable = mergeFunctionTypeSymbolTables (functionTableArray);
  // ROSE_ASSERT(globalFunctionTypeTable != NULL);

     mergeStaticASTFileInformation(AstFileInfoArray);

  // printf ("Before processing via DOT: globalProject = %p numberOfFiles() = %d \n",globalProject,globalProject->numberOfFiles());

#if 0
  // Output an optional graph of the AST (just the tree, when active). Note that we need to multiple file version 
  // of this with includes so that we can present a single SgProject rooted AST with multiple SgFile objects.
  // generateDOT ( *globalProject );
     generateDOT_withIncludes ( *globalProject, "aggregatedAST.dot" );
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(globalProject,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

     return 0;
   }


void
mergeStaticASTFileInformation(vector<AstFileSpecificInfo*> & AstFileInfoArray)
   {
  // Take care of merging the function type table first.
     vector<SgFunctionTypeTable*> functionTableArray;

     for (size_t i = 0; i < AstFileInfoArray.size(); ++i)
        {
          functionTableArray.push_back(AstFileInfoArray[i]->functionTable);
        }

  // Merge the function tables from each AST.
     SgFunctionTypeTable* globalFunctionTypeTable = mergeFunctionTypeSymbolTables (functionTableArray);
     ROSE_ASSERT(globalFunctionTypeTable != NULL);

     map<int,std::string> mergedFileidtoname_map;
     map<std::string,int> mergedNametofileid_map;
     map<int,int> mergedFileidtoid_map;

  // Now merge the file name maps (static information in Sg_File_Info).
  // int maxFilenameIndex = 0;
  // int minFilenameIndex = 100000000;
     for (size_t index = 0; index < AstFileInfoArray.size(); index++)
        {
          map<int,std::string> & fileidtoname_map = AstFileInfoArray[index]->fileidtoname_map;

       // Build a new map of all the possible file names
          for (std::map<int,std::string>::iterator i = fileidtoname_map.begin(); i != fileidtoname_map.end(); i++)
             {
            // printf ("id = %d name = %s \n",i->first,i->second.c_str());

               if (mergedNametofileid_map.count(i->second) == 0)
                  {
                    int returnValue = (int)mergedNametofileid_map.size();
#if 0
                    printf ("Adding new name to merged maps id = %d name = %s will use new id = %d \n",i->first,i->second.c_str(),returnValue);
#endif
                    mergedNametofileid_map[i->second]   = returnValue;
                    mergedFileidtoname_map[returnValue] = i->second;

                 // We still need the mapping of old ids to new ids for each AST file.
                    mergedFileidtoid_map [i->first] = returnValue;
                  }
             }

#if 0
          printf ("Resetting memory pool entries (%d,%d) \n",AstFileInfoArray[index]->baseOfASTFileInfo,AstFileInfoArray[index]->boundOfASTFileInfo);
#endif
       // Now iterate over the memory pool of Sg_File _Info objects so that we can reset the file ids.
          for (int i = AstFileInfoArray[index]->baseOfASTFileInfo; i < AstFileInfoArray[index]->boundOfASTFileInfo; i++)
             {
            // Set each id to the new id as specified in mergedFileidtoid_map

            // printf ("Accessing the %d = %lu entry of the Sg_File_Info memory pool. \n",i,(unsigned long) i);
            // printf ("Memory pool size = %lu \n",AST_FILE_IO::getAccumulatedPoolSizeOfNewAst(V_Sg_File_Info));

            // Compute the postion of the indexed Sg_File_Info object in the memory pool.
               unsigned long localIndex = i;
               unsigned long positionInPool = localIndex % Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE ;
               unsigned long memoryBlock    = (localIndex - positionInPool) / Sg_File_Info_CLASS_ALLOCATION_POOL_SIZE;

               Sg_File_Info* fileInfo = &(((Sg_File_Info*)(Sg_File_Info_Memory_Block_List[memoryBlock]))[positionInPool]);

            // $CLASSNAME* $CLASSNAME_getPointerFromGlobalIndex ( unsigned long globalIndex )
            // Sg_File_Info* fileInfo = Sg_File_Info_getPointerFromGlobalIndex((unsigned long) i + 1);
               ROSE_ASSERT(fileInfo != NULL);

               int oldFileId = fileInfo->get_file_id();
               int newFileId = mergedFileidtoid_map[oldFileId];

            // Only reset those file ids that are associated with valid file names. 
            // Values less than zero indicate file name classifications.
               if (oldFileId >= 0 && oldFileId != newFileId)
                  {
#if 0
                    printf ("Accessing the %d Exchanging old file id = %d for new file id = %d \n",i,oldFileId,newFileId);
#endif
                    fileInfo->set_file_id ( newFileId );
                  }
             }
       }

  // Now reset the static maps to use the new mapping.
     Sg_File_Info::get_nametofileid_map() = mergedNametofileid_map;
     Sg_File_Info::get_fileidtoname_map() = mergedFileidtoname_map;

  // Note that GNU g++ 4.2.x allows us to reference "index" out of scope!
  // printf ("maxFilenameIndex = %d minFilenameIndex = %d \n",maxFilenameIndex,minFilenameIndex);
   }


SgFunctionTypeTable* mergeFunctionTypeSymbolTables ( vector<SgFunctionTypeTable*> functionTableArray )
   {
  // This is the function type table from the last AST read (since AST_FILE_IO::setStaticDataOfAst()
  // resets the static variable used to store the global function type table for ROSE).
     SgFunctionTypeTable* globalFunctionTypeTable = SgNode::get_globalFunctionTypeTable();
     ROSE_ASSERT(globalFunctionTypeTable != NULL);

  // printf ("Using globalFunctionTypeTable = %p as a table to merge all symbols into. \n",globalFunctionTypeTable);
  // printf ("BEFORE removing the global function type table: functionTableArray size = %zu \n",functionTableArray.size());

     ROSE_ASSERT(functionTableArray.empty() == false);
     vector<SgFunctionTypeTable*>::iterator globalTable = find(functionTableArray.begin(),functionTableArray.end(),globalFunctionTypeTable);
     ROSE_ASSERT(globalTable != functionTableArray.end());
     functionTableArray.erase(globalTable);

  // printf ("AFTER removing the global function type table: functionTableArray size = %zu \n",functionTableArray.size());

     for (size_t index = 0; index < functionTableArray.size(); index++)
        {
          ROSE_ASSERT(functionTableArray[index] != NULL);
          SgSymbolTable::BaseHashType* internalTable = functionTableArray[index]->get_function_type_table()->get_table();
          ROSE_ASSERT(internalTable != NULL);

          SgSymbolTable::hash_iterator i = internalTable->begin();
          while (i != internalTable->end())
             {
#if 0
               printf ("In symbol table = %p symbol name = i->first = %s i->second = %p = %s \n",localFunctionTypeTable,i->first.str(),i->second,i->second->class_name().c_str());
#endif
               ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

            // printf ("Symbol number: %d (pair.first (SgName) = %s) pair.second (SgSymbol) sage_class_name() = %s \n",
            //      idx,(*i).first.str(),(*i).second->sage_class_name());

               SgSymbol* symbol = isSgSymbol((*i).second);
               ROSE_ASSERT ( symbol != NULL );

               if (globalFunctionTypeTable->lookup_function_type(i->first) == NULL)
                  {
                 // This function in the local function type table is not in the global function type table, so add it.
#if 0
                    printf ("Symbol should be added to the global table: i->first = %s \n",i->first.str());
#endif
                 // globalFunctionTypeTable->insert_function_type(i->first,i->second);
                    globalFunctionTypeTable->get_function_type_table()->insert(i->first,i->second);
                  }
                 else
                  {
                 // These are redundant symbols, but likely something in the AST points to them so be careful.

                 // This function type is already in the global function type table, so there is nothing to do (later we can delete it to save space)
                 // printf ("Symbol already in global table: i->first = %s \n",i->first.str());
                  }

               i++;
             }
        }

     return globalFunctionTypeTable;
   }


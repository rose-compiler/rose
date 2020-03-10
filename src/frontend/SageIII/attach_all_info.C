// #include "attach_all_info.h"
#include "sage3.h"

using namespace std;
using namespace Rose;

// extern ROSEAttributesList *getPreprocessorDirectives(const char *fileName);

#if 0
// wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t::wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node, ROSEAttributesList * preprocInfo,int len)
wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t::
wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node)
  : previousLocNodePtr(node)
#if 0
 // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
  , currentListOfAttributes(preprocInfo), 
    sizeOfCurrentListOfAttributes(len)
#endif
{
}

#else
// DQ (12/3/2008): This is the data that we have to save (even though the currentListOfAttributes has been moved to the inherited attribute)
wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t::wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t(SgLocatedNode* node, ROSEAttributesList * preprocInfo,int len)
   : previousLocNodePtr(node),
     currentListOfAttributes(preprocInfo), 
     sizeOfCurrentListOfAttributes(len)
   {
   }
#endif

static const char* OPTION_PREFIX_ROSE = "-rose:";
static const char* OPTION_VALUE_SEPARATOR = "$^";


static const char* OPTION_EXCLUDEDIRECTIVES  = "*excludeCommentsAndDirectives";
static const char* OPTION_INCLUDEDIRECTIVES  = "*includeCommentsAndDirectives";
static const char* OPTION_EXCLUDEDIRECTIVESFILENAME  = "*excludeCommentsAndDirectivesFrom";
static const char* OPTION_INCLUDEDIRECTIVESFILENAME  = "*includeCommentsAndDirectivesFrom";


//Read all the words in a file into an vector of strings
std::vector<std::string> 
readWordsInFile( std::string filename){
#if 1
// DQ (2/3/2009): Moved functionality to StringUtility namespace.
   StringUtility::readWordsInFile(filename);
#else
     std::vector<std::string> variantsToUse;
     std::fstream file_op(filename.c_str());
     if (file_op.fail()) {
          std::cout << "error: could not find file \"" << filename 
                   << "\" which is meant to include the styles to enforce with " 
                   << "the name checker." << std::endl;
                                  exit(1);    // abort program
                                      
                                  }

     std::string current_word;

     while(file_op >> current_word){
       //First word denotes what the regular expression should operate
       //upon. Second word denotes the regular expression
       variantsToUse.push_back(current_word);
     }

    return variantsToUse;
#endif
}

//! Wrapper around the SLA string option processing routine.
int
getRoseOptionValuesFromCommandline (vector<string>& argv, const string& opt_name,
                std::vector<std::string>& values)
   {
     int num_matches = sla_str (argv,
                     OPTION_PREFIX_ROSE,
                     OPTION_VALUE_SEPARATOR,
                     opt_name,
                     (std::string *)NULL);
     if (num_matches > 0)
        {
          vector<string> raw_values(num_matches);
          sla_str (argv,
OPTION_PREFIX_ROSE, OPTION_VALUE_SEPARATOR, opt_name,
                          &raw_values[0]);
          values.insert(values.end(), raw_values.begin(), raw_values.end());
        }
     return num_matches;
   }




AttachAllPreprocessingInfoTreeTrav::
AttachAllPreprocessingInfoTreeTrav(SgFile * sagep)
  : AttachPreprocessingInfoTreeTrav(), 
    sage_file(sagep), 
    map_of_all_attributes(), 
    nFiles(0), 
    map_of_file_order(), 
    array_of_first_nodes(16, (SgNode*)NULL),
    dependancies(cin)
   {
     lookForExcludePaths = false;
     lookForIncludePaths = false;
     start_index = 0;
     src_file_name = sage_file->getFileName();

  // AS(093007) Find the commandline options to include and/or exclude paths

     vector<string> argv = sage_file->get_originalCommandLineArgumentList();

     std::vector<std::string> raw_conf_filename;

     printf ("AttachAllPreprocessingInfoTreeTrav constructor: This code is now redundant with the include/exclude support for paths and filename in SgProject \n");

  // Read option to exclude directives
     getRoseOptionValuesFromCommandline (argv, OPTION_EXCLUDEDIRECTIVES, raw_conf_filename);

     if ( raw_conf_filename.size() >= 1  )
        {
          ROSE_ASSERT( raw_conf_filename.size() >= 1);
               lookForExcludePaths = true;

          for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   )
             {
                 pathsToExclude.push_back(raw_conf_filename[i]);
             }
        }

  // Read option to include directives
     raw_conf_filename.clear();
     getRoseOptionValuesFromCommandline (argv, OPTION_INCLUDEDIRECTIVES, raw_conf_filename);

     if ( raw_conf_filename.size() >= 1  )
        {
          ROSE_ASSERT( raw_conf_filename.size() >= 1);
               lookForIncludePaths = true;

          for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   )
             {
                 pathsToInclude.push_back(raw_conf_filename[i]);
             }
        }

  // Read option to include directives from filename
     raw_conf_filename.clear();
     getRoseOptionValuesFromCommandline (argv, OPTION_INCLUDEDIRECTIVESFILENAME, raw_conf_filename);

     if ( raw_conf_filename.size() >= 1  )
        {
          ROSE_ASSERT( raw_conf_filename.size() >= 1);
          lookForIncludePaths = true;

          std::vector<std::string> wordsInFile;
          for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   )
             {
                 wordsInFile = readWordsInFile(raw_conf_filename[i]);
               for( unsigned int j = 0 ; j < wordsInFile.size() ; j++   )
                    pathsToInclude.push_back(wordsInFile[j]);
             }
        }

  // Read option to exclude directives from filename
     raw_conf_filename.clear();
     getRoseOptionValuesFromCommandline (argv, OPTION_EXCLUDEDIRECTIVESFILENAME, raw_conf_filename);

     if ( raw_conf_filename.size() >= 1  )
        {
          ROSE_ASSERT( raw_conf_filename.size() == 1);

          lookForExcludePaths = true;
          std::vector<std::string> wordsInFile;
          for( unsigned int i = 0 ; i < raw_conf_filename.size() ; i++   )
             {
               wordsInFile = readWordsInFile(raw_conf_filename[i]);
               for( unsigned int j = 0 ; j < wordsInFile.size() ; j++   )
                    pathsToExclude.push_back(wordsInFile[j]);
             }
        }
   }


AttachPreprocessingInfoTreeTraversalInheritedAttrribute
AttachAllPreprocessingInfoTreeTrav::evaluateInheritedAttribute ( SgNode *n, AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh)
   {
     wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t save_data;

     SgFile *currentFilePtr =  dynamic_cast<SgFile*>(n);
     SgLocatedNode *currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
     int file_name_id = -1;
     Sg_File_Info * file_info_ptr = NULL;
     bool isCompilerGenerated = false;
     bool isTransformation = false;

     if ( ( currentFilePtr == NULL ) && ( currentLocNodePtr == NULL ) )
          return inh;

  // get a valid file name, and check if it is a new file; if yes, get
  // all the comments and store the list into map. Then set save_data
  // to prepare for the parent function.

  // DQ (12/1/2008): I think this code can be simplified now, but I am not clear how it should be done!

  // 1. find the file name for the current node:
     if ( currentFilePtr!=NULL )
        {
          file_name_id = currentFilePtr->get_file_info()->get_file_id();
        }
       else
        {
          if ( currentLocNodePtr!=NULL )
             {
               file_info_ptr = currentLocNodePtr->get_file_info();
               assert ( file_info_ptr!=NULL );

            // Pi: compiler generated nodes have no real file names with them,
            // so use the currentFileName as their names:
               isCompilerGenerated = file_info_ptr->isCompilerGenerated();
            // isCompilerGenerated = file_info_ptr->isCompilerGeneratedNodeToBeUnparsed();
               isTransformation = file_info_ptr->isTransformation();
               if ( isCompilerGenerated == true || isTransformation == true )
                  {
                    file_name_id = currentFileNameId;
                  }
                 else
                  {
                    file_name_id = file_info_ptr->get_file_id();
                  }
             }
        }

  // 2. check whether the file name is new:
  // AS(09/21/07) Reset start_index since we are dealing with a new file
     start_index = 0;

     map<int, wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t>::iterator attr_list_itr = map_of_all_attributes.find(file_name_id);

  // If the name is not in the map then ... it is a new file and we have to process it
     if ( attr_list_itr == map_of_all_attributes.end() )
        {
#ifdef outputallfilenamefound
          cerr << "-->>> NEW file : " << file_name_str << endl;
#endif
          save_data.previousLocNodePtr = previousLocNodePtr;

       // AS(092907) Only collect the filenames we are interested in
       // AS(093007) Logic to include or exclude finding comments in paths
          bool includePath = true;

          std::string file_name_str = Sg_File_Info::getFilenameFromID(file_name_id);

       // If the command line directive to look for include paths has been
       // specified. See if the current filename is in the list of
       // included paths.
          if( lookForIncludePaths == true )
             {
               includePath = false;
               for (std::vector<std::string>::iterator iItr = pathsToInclude.begin(); iItr != pathsToInclude.end(); ++iItr)
                  {
                    if ( file_name_str.find(*iItr) != string::npos )
                       {
                         includePath  = true; 
                         break;                 
                       }
                  }
             }

          bool excludePath = false;

       // If the command line directive to look for exclude paths has been
       // specified. See if the current filename is in the list of
       // excluded paths.
          if ( lookForExcludePaths == true )
             {
               for (std::vector<std::string>::iterator iItr = pathsToExclude.begin(); iItr != pathsToExclude.end(); ++iItr)
                  {
                    if ( file_name_str.find(*iItr) != string::npos )
                       {
                         excludePath  = true; 
                         break;
                       }
                  }
             }

          ROSE_ASSERT(inh.currentListOfAttributes != NULL);

          if ( (excludePath == false) && (includePath == true) )
             {
#ifdef ROSE_BUILD_CPP_LANGUAGE_SUPPORT
            // This scans the file and collects the token stream...
               printf ("AttachAllPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): Calling getPreprocessorDirectives for file = %s \n",file_name_str.c_str());

            // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
            // save_data.currentListOfAttributes = getPreprocessorDirectives(file_name_str);
            // save_data.currentListOfAttributes = inh.currentListOfAttributes;

            // This will work for C/C++ but not for Fortran.
            // For Fortran use: returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran9x_language);
            // or: returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran77_language);
               save_data.currentListOfAttributes = getPreprocessorDirectives(file_name_str);
#endif
             }
            else
             {
            // This builds an empty list; does not traverse the file
               printf ("AttachAllPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): Building an empty list to represent the data from file = %s \n",file_name_str.c_str());

            // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
            // save_data.currentListOfAttributes = new ROSEAttributesList();
            // save_data.currentListOfAttributes = inh.currentListOfAttributes;
               save_data.currentListOfAttributes = new ROSEAttributesList();
             }

       // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
       // save_data.sizeOfCurrentListOfAttributes = save_data.currentListOfAttributes->getLength();
          map_of_all_attributes[file_name_id] = save_data;

        }
       else
        {
          save_data = (*attr_list_itr).second;
        }

  // 3. set data used by the parent before calling it:
     currentFileNameId             = file_name_id;
     previousLocNodePtr            = save_data.previousLocNodePtr;

  // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
  // currentListOfAttributes       = save_data.currentListOfAttributes;
  // sizeOfCurrentListOfAttributes = save_data.sizeOfCurrentListOfAttributes;

  // 4. call the parent and record changes to the data used:
     AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh_rsl = AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(n, inh);
     save_data.previousLocNodePtr = previousLocNodePtr;

  // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
  // save_data.currentListOfAttributes = currentListOfAttributes;
  // save_data.sizeOfCurrentListOfAttributes = sizeOfCurrentListOfAttributes;
     map_of_all_attributes[currentFileNameId] = save_data;

  // 5. set the first attachable node for each file to make it simple
  // 5. to attach rest preproc info (the special case when a file
  // 5. contains no IR nodes): such info is attached to the first node
  // 5. of the next file.
     if ( dynamic_cast<SgStatement*>(n) != NULL )
        {
       // Nodes that should not have comments attached (since they are not unparsed)
       // Conditions obtained from src/ROSETTA/Grammar/LocatedNode.code:251
          switch ( n->variantT() )
             {
               case V_SgForInitStatement:
               case V_SgTypedefSeq:
               case V_SgCatchStatementSeq:
               case V_SgFunctionParameterList:
               case V_SgCtorInitializerList:
                    goto return_inh;

               default:
                  {
                 // 5. if the first node for a file doesn't exist, then add it; otherwise do nothing:
                    map<int, int>::iterator first_node_itr = map_of_file_order.find(currentFileNameId);
                    if ( first_node_itr == map_of_file_order.end() )
                       {
                         map_of_file_order[currentFileNameId] = nFiles;
                         add_first_node_for_file(currentFileNameId, n, nFiles);
                         nFiles++;
                       }
                    break;
                  }
             }
        }

  // The inh/inh_rsl is no use for now.
     return_inh:

     return inh_rsl;
   }

AttachPreprocessingInfoTreeTraversalSynthesizedAttribute
AttachAllPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute ( SgNode *n, AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh, SubTreeSynthesizedAttributes st)
   {
     wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t save_data;

     SgFile *currentFilePtr =  dynamic_cast<SgFile*>(n);
     SgLocatedNode *currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
     int file_name_id = -1;
     Sg_File_Info * file_info_ptr = NULL;
     bool isCompilerGenerated = false;
     bool isTransformation = false;

     if( ( currentFilePtr != NULL ) || ( currentLocNodePtr != NULL ) )
        {
       // get a valid file name, and check if it is a new file; if yes, get
       // all the comments and store the list into map. Then set save_data
       // to prepare for the parent function.

       // 1. find the file name for the current node:
          if ( currentFilePtr!=NULL )
             {
               file_name_id = currentFilePtr->get_file_info()->get_file_id();
             }
            else
             {
               if ( currentLocNodePtr!=NULL )
                  {
                    file_info_ptr = currentLocNodePtr->get_file_info();
                    assert ( file_info_ptr!=NULL );

                 // Pi: compiler generated nodes have no real file names with them,
                 // so use the currentFileName as their names:
                    isCompilerGenerated = file_info_ptr->isCompilerGenerated();
                 // isCompilerGenerated = file_info_ptr->isCompilerGeneratedNodeToBeUnparsed();
                    isTransformation = file_info_ptr->isTransformation();
                    if ( isCompilerGenerated==true || isTransformation==true )
                       {
                         file_name_id = currentFileNameId;
                       }
                      else
                       {
                         file_name_id = file_info_ptr->get_file_id();
                       }
                  }
             }

       // 2. check whether the file name is new:
       // AS(09/21/07) Reset start_index since we are dealing with a new file
          start_index = 0;

          map<int, wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t>::iterator attr_list_itr = map_of_all_attributes.find(file_name_id);
          if ( attr_list_itr==map_of_all_attributes.end() )
             {
#ifdef outputallfilenamefound
               cerr << "-->>> NEW file : " << file_name_str << endl;
#endif
               save_data.previousLocNodePtr = previousLocNodePtr;

            // AS(093007) Logic to include or exclude finding comments in paths
               bool includePath = true;
               std::string file_name_str = Sg_File_Info::getFilenameFromID(file_name_id);

            // If the command line directive to look for include paths has been
            // specified. See if the current filename is in the list of
            // included paths.
               if( lookForIncludePaths == true )
                  {
                    includePath = false;
                    for(std::vector<std::string>::iterator iItr = pathsToInclude.begin(); iItr != pathsToInclude.end(); ++iItr)
                       {
                         if( file_name_str.find(*iItr) != string::npos )
                            {
                              includePath  = true; 
                              break;
                            }
                       }
                  }

               bool excludePath = false;

            // If the command line directive to look for exclude paths has been
            // specified. See if the current filename is in the list of
            // excluded paths.
               if( lookForExcludePaths == true )
                  {
                    for(std::vector<std::string>::iterator iItr = pathsToExclude.begin(); iItr != pathsToExclude.end(); ++iItr)
                       {
                         if( file_name_str.find(*iItr) != string::npos )
                            {
                              excludePath  = true; 
                              break;
                            }
                       }
                  }

#if 0
            // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
               if( (excludePath == false) && (includePath == true) )
                    save_data.currentListOfAttributes = getPreprocessorDirectives(file_name_str);
                 else
                    save_data.currentListOfAttributes = new ROSEAttributesList;

               save_data.sizeOfCurrentListOfAttributes = save_data.currentListOfAttributes->getLength();
#endif
               map_of_all_attributes[file_name_id] = save_data;
             }
            else
             {
               save_data = (*attr_list_itr).second;
             }

       // 3. set data used by the parent before calling it:
          currentFileNameId = file_name_id;
          previousLocNodePtr = save_data.previousLocNodePtr;
#if 0
       // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
          currentListOfAttributes = save_data.currentListOfAttributes;
          sizeOfCurrentListOfAttributes = save_data.sizeOfCurrentListOfAttributes;
#endif
        }

     AttachPreprocessingInfoTreeTraversalSynthesizedAttribute syn;

     if( ( currentFilePtr != NULL ) || ( currentLocNodePtr != NULL ) )
        {
          syn = AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(n, inh, st);
          save_data.previousLocNodePtr = previousLocNodePtr;

#if 0
       // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
          save_data.currentListOfAttributes = currentListOfAttributes;
          save_data.sizeOfCurrentListOfAttributes = sizeOfCurrentListOfAttributes;
#endif

          map_of_all_attributes[currentFileNameId] = save_data;
        }
 
  // The inh/st/syn is no use for now.
  // return_syn:
     return syn;
   }

bool
AttachAllPreprocessingInfoTreeTrav::add_first_node_for_file(const int fn, SgNode* n, int pos)
   {
     bool flag = false;

     int totalfiles = array_of_first_nodes.size();
     if ( pos >= totalfiles )
        {
          array_of_first_nodes.resize(2*totalfiles, NULL);
        }

     if ( array_of_first_nodes[pos]==NULL )
        {
          flag = true;
          array_of_first_nodes[pos] = n;
        }

     return flag;
   }

pair<SgNode*, PreprocessingInfo::RelativePositionType> 
AttachAllPreprocessingInfoTreeTrav::get_first_node_for_file(const int fn, int hintfororder)
   {
     map<int, int>::iterator first_node_itr = map_of_file_order.find(fn);
     int pos_itr = hintfororder;
     PreprocessingInfo::RelativePositionType loc = PreprocessingInfo::before;
     if ( first_node_itr!=map_of_file_order.end() )
          pos_itr = (*first_node_itr).second;

     for (int i=pos_itr; i<nFiles; i++ )
        {
          if ( array_of_first_nodes[i]!=NULL )
               return pair<SgNode*, PreprocessingInfo::RelativePositionType>(array_of_first_nodes[i], loc);
        }

     loc = PreprocessingInfo::after;
     for (int i=pos_itr-1; i>=0; i-- )
        {
          if ( array_of_first_nodes[i]!=NULL )
               return pair<SgNode*, PreprocessingInfo::RelativePositionType>(array_of_first_nodes[i], loc);
        }

     loc = PreprocessingInfo::defaultValue;
     return pair<SgNode*, PreprocessingInfo::RelativePositionType>(NULL, loc);
   }


// DQ (10/27/2007): Added display function to output information gather durring the collection of 
// comments and CPP directives across all files.
void
AttachAllPreprocessingInfoTreeTrav::display(const std::string & label) const
   {
  // Output internal information

     printf ("Inside of AttachAllPreprocessingInfoTreeTrav::display(%s) \n",label.c_str());

#if 0
     std::string src_file_name;
     SgFile * sage_file;
  /* map: key = filename, value = a wrapper for the data used in AttachPreprocessingInfoTreeTrav. */
     std::map<int, wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t> map_of_all_attributes;
  /* map: key = filename, value = the first node in AST from the file (could be NULL) */
  /* std::map<std::string, SgNode*> map_of_first_node; */
  /* I need to keep the order for each file when it is discovered from AST */
     int nFiles;                        /* total number of files involved */
     std::map<int, int> map_of_file_order;
     std::vector<SgNode*> array_of_first_nodes;

  // Holds paths to exclude when getting all commments and directives
     std::vector<std::string> pathsToExclude;
     bool lookForExcludePaths;

  // Holds paths to include when getting all comments and directives
     std::vector<std::string> pathsToInclude;
     bool lookForIncludePaths;
#endif

     printf ("src_file_name            = %s \n",src_file_name.c_str());
     printf ("sage_file                = %p = %s \n",sage_file,sage_file->getFileName().c_str());
     printf ("Number of files (nFiles) = %d \n",nFiles);
     printf ("lookForExcludePaths      = %s \n",lookForExcludePaths ? "true" : "false");
     printf ("lookForIncludePaths      = %s \n",lookForIncludePaths ? "true" : "false");

     std::map<int, wrap_data_used_by_AttachPreprocessingInfoTreeTrav_t>::const_iterator i = map_of_all_attributes.begin();
     while (i != map_of_all_attributes.end())
        {
          int file_id = i->first;
          SgLocatedNode *previousLocNodePtr           = i->second.previousLocNodePtr;

       // DQ (11/30/2008): Moved currentListOfAttributes to inherited attribute
       // ROSEAttributesList *currentListOfAttributes = i->second.currentListOfAttributes;
       // int sizeOfCurrentListOfAttributes           = i->second.sizeOfCurrentListOfAttributes;

          printf (" file_id                       = %d \n",file_id);
          printf (" previousLocNodePtr            = %p \n",previousLocNodePtr);
       // printf (" currentListOfAttributes       = %p \n",currentListOfAttributes);
       // printf (" sizeOfCurrentListOfAttributes = %d \n",sizeOfCurrentListOfAttributes);

          i++;
        }
#if 0
     AttachPreprocessingInfoTreeTrav::display("Called from processing ALL comments and directives");
#endif
   }


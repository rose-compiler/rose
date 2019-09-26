// Support for building include graphs (graphs of include files)

#include "sage3basic.h"

#include "FileHelper.h"

#include "includeGraph.h"

using namespace std;
using namespace Rose;

std::ofstream IncludeFileGraph::file;

// DQ (11/11/2018): We need a list to make sure that we don't add nodes twice (once when traversing the source sequence lists and a second time when traversing the AST).
// We need to show the AST plus the edges that are specific to the source sequence lists (and which node's source sequence list they are associated with).
std::set<void*> IncludeFileGraph::graphNodeSet;

#define DEBUG_HEADER_GRAPH_SUPPPORT 0

// DQ(11/14/2018): Build a simpler graph while debugging include tree.
#define TRANSLATION_UNIT_POINTER 0

void
IncludeFileGraph::insertGraphNode (void* node)
   {
#if 0
     printf ("In insertGraphNode(): node = %p \n",node);
#endif

     ROSE_ASSERT(node != NULL);

     graphNodeSet.insert(node);
   }


void
IncludeFileGraph::graph_include_files( SgSourceFile* sourceFile )
   {
     ROSE_ASSERT(sourceFile != NULL);

#if DEBUG_HEADER_GRAPH_SUPPPORT
     printf ("graph_include_files(SgSourceFile*): sourceFile = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif

     if (graphNodeSet.find(sourceFile) == graphNodeSet.end())
        {
       // string name = string("file : ") + sourceFile->getFileName();
          string filename = FileHelper::getFileName(sourceFile->getFileName());
          string name = string("file : ") + filename;

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(SgSourceFile*): name = %s \n",name.c_str());
#endif

          file << "\"" << StringUtility::numberToString(sourceFile) << "\"[" << "label=\"" << name << "\\n" 
               << StringUtility::numberToString(sourceFile) << "\" color=\"green\" ];" << endl;

          insertGraphNode(sourceFile);

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): sourceFile (node = %p): graphNodeSet.size() = %" PRIuPTR " \n",sourceFile,graphNodeSet.size());
#endif
        }
       else
        {
#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): graph_headers previously processed, sourceFile in graphNodeSet: sourceFile = %p \n",sourceFile);
#endif
          return;
        }

     SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
     if (includeFile != NULL)
        {
       // ROSE_ASSERT(sourceFile->get_isHeaderFile() == true);

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("sourceFile->get_associated_include_file() = %p filename = %s \n",includeFile,includeFile->get_filename().str());
#endif
          file << "\"" << StringUtility::numberToString(sourceFile) << "\" -> \"" << StringUtility::numberToString(includeFile)
            // << "\"[label=\"" << "including : " + StringUtility::numberToString(42) << ":"
            // << "\"[label=\"" << "assoc-include"
               << "\"[label=\"" << "assoc-include"
            // << StringUtility::numberToString(includeFile) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(8) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

          graph_include_files(includeFile);
        }

#if 0
  // SgIncludeFilePtrList", "include_file_list
     for (size_t i = 0; i < sourceFile->get_include_file_list().size(); i++)
        {
          SgIncludeFile* nested_includeFile = sourceFile->get_include_file_list()[i];
          ROSE_ASSERT(nested_includeFile != NULL);

          file << "\"" << StringUtility::numberToString(sourceFile) << "\" -> \"" << StringUtility::numberToString(nested_includeFile)
            // << "\"[label=\"" << "including : " + StringUtility::numberToString(42) << ":"
               << "\"[label=\"" << "include"
            // << StringUtility::numberToString(nested_includeFile) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(7) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

          graph_include_files(nested_includeFile);
        }
#endif
   }


void
IncludeFileGraph::graph_include_files( SgIncludeFile* includeFile )
   {
     ROSE_ASSERT(includeFile != NULL);

#if DEBUG_HEADER_GRAPH_SUPPPORT
     printf ("graph_include_files(SgIncludeFile*): includeFile = %p = %s \n",includeFile,includeFile->get_filename().str());
#endif

     if (graphNodeSet.find(includeFile) == graphNodeSet.end())
        {
#if 1
       // Use the full filename to make multiple included files more clear.
          string filename = includeFile->get_filename();
#else
          string filename = FileHelper::getFileName(includeFile->get_filename());
#endif
          string name = string("include-directive : ") + filename;

       // name = name + "\\n name_as_written = " << includeFile->get_name_used_in_include_directive() << "\"";
       // name = name + "\\n name_as_written = " << includeFile->get_name_used_in_include_directive().str() << " ";
       // name = name + "\\n name_as_written = "; // << includeFile->get_name_used_in_include_directive().str() << " ";
          name = name + "\\n name_as_written = " + includeFile->get_name_used_in_include_directive().str() + " ";
          name = name + "\\n directory_prefix = " + includeFile->get_directory_prefix().str() + " ";

       // DQ (11/16/2018): Added reference to the application root (only output for the root of the include tree).
       // SgSourceFile* source_file = includeFile->get_source_file();
       // SgSourceFile* source_file = includeFile->get_including_source_file();
       // SgSourceFile* parent_source_file = includeFile->get_including_source_file();
          SgSourceFile* source_file = includeFile->get_source_file();
       // ROSE_ASSERT(source_file != NULL);
          if (source_file != NULL && source_file->get_isHeaderFile() == false)
             {
               name = name + "\\n applicationRootDirectory = " + includeFile->get_applicationRootDirectory().str() + "      ";
             }

#if 0
          if (includeFile->get_requires_explict_path_for_unparsed_headers() == true)
             {
            // name = name + "\\n requires_explict_path_for_unparsed_headers = " + includeFile->get_applicationRootDirectory().str() + " ";
               name = name + "\\n requires_explict_path_for_unparsed_headers = " + includeFile->get_applicationRootDirectory().str() + " ";
             }
#endif

          name = name + "\\n requires_explict_path_for_unparsed_headers = " + (includeFile->get_requires_explict_path_for_unparsed_headers() ? "true" : "false") + "      ";

       // DQ (11/17/2018): Is this an include file specified using "<>".
          name = name + "\\n isSystemInclude = " + (includeFile->get_isSystemInclude() ? "true" : "false") + "      ";

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(SgIncludeFile*): name = %s \n",name.c_str());
#endif

          string fill_color = "";
       // fill_color = "fillcolor=\"lightgreen\" ";
          fill_color = ",fillcolor=\"lightblue\" ";

          bool node_color_set = false;
          if (includeFile->get_can_be_supported_using_token_based_unparsing() == false)
             {
               fill_color = ",fillcolor=\"red\" ";
               node_color_set = true;
             }

          // if (includeFile->get_source_file() == NULL)
          if (node_color_set == false && includeFile->get_isRoseSystemInclude() == true)
             {
               fill_color = ",fillcolor=\"orange\" ";
               node_color_set = true;
             }

       // DQ (11/21/2018): Mark those header files included from system paths (internal default paths searched by the compiler).
          if (node_color_set == false && includeFile->get_from_system_include_dir() == true)
             {
               fill_color = ",fillcolor=\"yellow\" ";
               node_color_set = true;
             }

       // DQ (11/21/2018): Mark those header files included from system paths (internal default paths searched by the compiler).
          if (node_color_set == false && includeFile->get_preinclude_macros_only() == true)
             {
               fill_color = ",fillcolor=\"brown\" ";
               node_color_set = true;
             }

       // DQ (11/21/2018): Mark those header files included from system paths (internal default paths searched by the compiler).
          if (includeFile->get_isApplicationFile() == true)
             {
               fill_color = ",fillcolor=\"lightgreen\" ";
               node_color_set = true;
             }

       // DQ (11/15/2018): Mark this as a header file that will be unparsed.
          if (includeFile->get_will_be_unparsed() == true)
             {
            // Mark this as a header file that will be unparsed.

            // fill_color = ",fillcolor=\"lightgreen\" ";
            // fill_color = ",fillcolor=\"greenyellow\" ";
            // fill_color = ",fillcolor=\"lightgreen\" ";
               fill_color = ",fillcolor=\"green\" ";
               node_color_set = true;
             }

          fill_color += ",style=filled";

       // file << "\"" << StringUtility::numberToString(includeFile) << "\"[" << "label=\"" << name << "\\n" 
       //      << StringUtility::numberToString(includeFile) << "\" color=\"purple\" ];" << endl;
          file << "\"" << StringUtility::numberToString(includeFile) << "\"[" << "label=\"" << name << "\\n" 
               << StringUtility::numberToString(includeFile) << "\" color=\"purple\"" << fill_color << " ];" << endl;

          insertGraphNode(includeFile);

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): includeFile (node = %p): graphNodeSet.size() = %" PRIuPTR " \n",includeFile,graphNodeSet.size());
#endif
        }
       else
        {
#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): graph_headers previously processed, SgIncludeFile in graphNodeSet: includeFile = %p \n",includeFile);
#endif
          return;
        }

     SgSourceFile* sourceFile = includeFile->get_source_file();
     if (sourceFile != NULL)
        {
       // DQ (11/14/2018): This is no longer true for the input file which now has a 
       // SgIncludeFile associated with it so that we can build a SgIncludeFile tree 
       // that we can operate upon with ROSE traversals.
       // ROSE_ASSERT(sourceFile->get_isHeaderFile() == true);

          file << "\"" << StringUtility::numberToString(includeFile) << "\" -> \"" << StringUtility::numberToString(sourceFile)
            // << "\"[label=\"" << "file : " + StringUtility::numberToString(42) << ":"
            // << "\"[label=\"" << "assoc-file" 
               << "\"[label=\"" << "file   " 
            // << StringUtility::numberToString(sourceFile) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(9) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

       // We should have already seen this file.
          graph_include_files(sourceFile);
        }

     SgIncludeFile* parent_include_file = includeFile->get_parent_include_file();
     if (parent_include_file != NULL)
        {
          file << "\"" << StringUtility::numberToString(includeFile) << "\" -> \"" << StringUtility::numberToString(parent_include_file)
            // << "\"[label=\"" << "parent : " + StringUtility::numberToString(42) << ":"
               << "\"[label=\"" << "parent   "
            // << StringUtility::numberToString(parent_include_file) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(10) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

       // We should have already seen this file.
          graph_include_files(parent_include_file);
        }

#if 0
  // IncludeFile.setDataPrototype ( "SgSourceFile*", "including_source_file", " = NULL",
     SgSourceFile* parent_source_file = includeFile->get_including_source_file();
     if (parent_source_file != NULL)
        {
          file << "\"" << StringUtility::numberToString(includeFile) << "\" -> \"" << StringUtility::numberToString(parent_source_file)
            // << "\"[label=\"" << "parent : " + StringUtility::numberToString(42) << ":"
               << "\"[label=\"" << "including source file   "
            // << StringUtility::numberToString(parent_include_file) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(10) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

       // We should have already seen this file.
          graph_include_files(parent_source_file);
        }
#endif

// DQ(11/14/2018): Build a simpler graph while debugging include tree.
#if TRANSLATION_UNIT_POINTER
  // IncludeFile.setDataPrototype ( "SgSourceFile*", "source_file_of_translation_unit", " = NULL",
     SgSourceFile* translation_unit_source_file = includeFile->get_source_file_of_translation_unit();
     if (translation_unit_source_file != NULL)
        {
          file << "\"" << StringUtility::numberToString(includeFile) << "\" -> \"" << StringUtility::numberToString(translation_unit_source_file)
            // << "\"[label=\"" << "parent : " + StringUtility::numberToString(42) << ":"
               << "\"[label=\"" << "translation unit"
            // << StringUtility::numberToString(parent_include_file) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(10) << "\" color=\"black\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"black\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

       // We should have already seen this file.
          graph_include_files(translation_unit_source_file);
        }
#endif

  // Process the SgIncludeFilePtrList
  // SgIncludeFilePtrList", "include_file_list
     for (size_t i = 0; i < includeFile->get_include_file_list().size(); i++)
        {
          SgIncludeFile* nested_includeFile = includeFile->get_include_file_list()[i];
          ROSE_ASSERT(nested_includeFile != NULL);

          file << "\"" << StringUtility::numberToString(includeFile) << "\" -> \"" << StringUtility::numberToString(nested_includeFile)
            // << "\"[label=\"" << "include : " + StringUtility::numberToString(42) << ":"
               << "\"[label=\"" << "include"
            // << StringUtility::numberToString(nested_includeFile) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
            // << StringUtility::numberToString(11) << "\" color=\"red\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

          graph_include_files(nested_includeFile);
        }
   }


void
IncludeFileGraph::graph_include_files( SgProject* project )
   {
#if DEBUG_HEADER_GRAPH_SUPPPORT
     printf ("In graph_include_files(): project = %p: graphNodeSet.size() = %" PRIuPTR " \n",project,graphNodeSet.size());
#endif

     if (graphNodeSet.find(project) == graphNodeSet.end())
        {
          string name = string("project : (generate name)") ;

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): name = %s \n",name.c_str());
#endif

     file << "\"" << StringUtility::numberToString(project) << "\"[" << "label=\"" << name << "\\n" 
          << StringUtility::numberToString(project) << "\" color=\"blue\" ];" << endl;

          insertGraphNode(project);

#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): project (node = %p): graphNodeSet.size() = %" PRIuPTR " \n",project,graphNodeSet.size());
#endif
        }
       else
        {
#if DEBUG_HEADER_GRAPH_SUPPPORT
          printf ("In graph_include_files(): graph_headers previously processed, project in graphNodeSet: project = %p \n",project);
#endif
          return;
        }

     SgFileList* fileList = project->get_fileList_ptr();

     for (size_t i = 0; i < fileList->get_listOfFiles().size(); ++i)
        {
          SgFile* tmp_file = fileList->get_listOfFiles()[i];
          ROSE_ASSERT(tmp_file != NULL);
          SgSourceFile* sourceFile = isSgSourceFile(tmp_file);
          ROSE_ASSERT(sourceFile != NULL);

          file << "\"" << StringUtility::numberToString(project) << "\" -> \"" << StringUtility::numberToString(sourceFile)
               << "\"[label=\"" << "source_file : " + StringUtility::numberToString(i)
            // << StringUtility::numberToString(sourceFile) << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;
               << "\" color=\"blue\" labelfontcolor=\"blue4\" weight=1 len=2];" << endl;

          graph_include_files(sourceFile);
        }
   }


void
generateGraphOfIncludeFiles( SgProject* project, std::string filename )
   {
  // DQ (11/10/2018): Build a dot graph of the header files used and where they are included 
  // from (should be a tree, except where headers might be included multiple times).

     ROSE_ASSERT(project != NULL);

  // Build filename...
     if (filename.empty() == true)
        {
          filename = "include_file_graph";
        }

     string dot_header = filename;
     filename += ".dot";

  // DQ (6/15/2019): Output a message so that we can know the DOT file is being output.
     printf ("In generateGraphOfIncludeFiles(SgProject*): filename = %s \n",filename.c_str());

#if DEBUG_HEADER_GRAPH_SUPPPORT
     printf ("In generateGraphOfIncludeFiles(SgProject*): filename = %s \n",filename.c_str());
#endif

  // Open file...(file is declared in the IncludeFileGraph namespace).
     IncludeFileGraph::file.open(filename.c_str());

  // Output the opening header for a DOT file.
     IncludeFileGraph::file << "digraph \"" << dot_header << "\" {" << endl;

  // Calling recursive function to build dot file for scope, starting at the top level scope.
     IncludeFileGraph::graph_include_files (project);

  // Close off the DOT file.
     IncludeFileGraph::file << endl;
     IncludeFileGraph::file << "} " << endl;
     IncludeFileGraph::file.close();

     IncludeFileGraph::graphNodeSet.clear();
   }


void
generateGraphOfIncludeFiles( SgSourceFile* sourceFile, std::string filename )
   {
  // DQ (11/10/2018): Build a dot graph of the header files used and where they are included 
  // from (should be a tree, except where headers might be included multiple times).

     ROSE_ASSERT(sourceFile != NULL);

  // Build filename...
     if (filename.empty() == true)
        {
          filename = "include_file_graph";
        }

     string dot_header = filename;
     filename += ".dot";

#if DEBUG_HEADER_GRAPH_SUPPPORT
     printf ("In generateGraphOfIncludeFiles(SgSourceFile*): filename = %s \n",filename.c_str());
     printf ("   --- sourceFile = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif

  // Open file...(file is declared in the IncludeFileGraph namespace).
     IncludeFileGraph::file.open(filename.c_str());

  // Output the opening header for a DOT file.
     IncludeFileGraph::file << "digraph \"" << dot_header << "\" {" << endl;

  // Calling recursive function to build dot file for scope, starting at the top level scope.
     IncludeFileGraph::graph_include_files (sourceFile);

  // Close off the DOT file.
     IncludeFileGraph::file << endl;
     IncludeFileGraph::file << "} " << endl;
     IncludeFileGraph::file.close();

     IncludeFileGraph::graphNodeSet.clear();
   }

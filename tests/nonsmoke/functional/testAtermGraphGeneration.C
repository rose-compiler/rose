// DQ (9/27/2013): This is required to be defined for the 64bit ATerm support.
#if (__x86_64__ == 1)
// 64-bit machines are required to set this before including the ATerm header files.
   #define SIZEOF_LONG 8
   #define SIZEOF_VOID_P 8
#else
// 32-bit machines need not have the values defined (but it is required for this program).
   #define SIZEOF_LONG 4
   #define SIZEOF_VOID_P 4
#endif

#include <libgen.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iomanip>

#if 0
// These tests are here to make sure that the ATerm library is setup for use by 64 vs. 32 bit machines.
#ifdef SIZEOF_LONG
   #if SIZEOF_LONG > 4
      #warning "SIZEOF_LONG > 4"
   #else
      #warning "SIZEOF_LONG <= 4"
   #endif
#else
   #error "SIZEOF_LONG is NOT defined"
#endif

#ifdef SIZEOF_VOID_P
   #if SIZEOF_VOID_P > 4
      #warning "SIZEOF_VOID_P > 4"
   #else
      #warning "SIZEOF_VOID_P <= 4"
   #endif
#else
   #error "SIZEOF_VOID_P is NOT defined"
#endif
#endif


#include <aterm1.h>
#include <aterm2.h>
#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include <string.h>

#define ROSE_ASSERT(x) assert(x)

// This code is to support building DOT graphs of the EDG AST ans associated nodes
// so that we can better understand issues in the translation of the EDG representation
// into ROSE.  The modivating problem has been the handling of GNU statement expressions
// where types are declared and extra source sequence points are added to the AST and
// we need to understnad this better so that we can skip over them in the translation.

namespace ATerm_Graph
   {
  // File for output for generated graph.
     std::ofstream file;

     void graph ( ATerm term, std::string color );

     std::vector<ATerm> getAtermList(ATerm ls);

     std::string aterm_type_name( ATerm term );

  // Main function to call to build the DOT file for the EDG AST.
     void graph_aterm_ast(ATerm term, std::string filename);
   }



using namespace std;

// DQ (9/7/2013): We need a list to make sure that we don't add nodes twice (once when traversing 
// the source sequence lists and a second time when traversing the AST). We need to show the AST 
// plus the edges that are specific to the source sequence lists (and which node's source sequence 
// list they are associated with).
std::set<ATerm> graphNodeSet;

#define DEBUG_DOT_GRAPH_SUPPPORT 0

#if DEBUG_DOT_GRAPH_SUPPPORT
static void printGraphNodeSet()
   {
     int counter = 0;
     std::set<void*>::iterator i = graphNodeSet.begin();

     printf ("In printGraphNodeSet(): size() = %zu \n",graphNodeSet.size());
     while (i != graphNodeSet.end())
        {
          printf ("   --- graphNodeSet[%d] = %p \n",counter,*i);
          i++;
          counter++;
        }
   }
#endif


// This function is copied from the ROSE ATerm termToStratego.C file (or it's support files).
// It is copied here to allow this program to be independent of ROSE in the future.
vector<ATerm> 
ATerm_Graph::getAtermList(ATerm ls) 
   {
     ATerm a, b;
     vector<ATerm> result;
     while (true) 
        {
          if (ATmatch(ls, "[]"))
             {
               return result;
             }
            else 
             {
               if (ATmatch(ls, "[<term>, <list>]", &a, &b)) 
                  {
                    result.push_back(a);
                    ls = b;
                  }
                 else
                  {
                    ROSE_ASSERT (!"getAtermList");
                  }
             }
        }
   }


// This function is from the ROSE src/util/stringSupport/escape.C file. It is 
// copied here to allow this program to be independent of ROSE in the future.
std::string 
escapeString(const std::string & s) 
   {
     std::string result;
     for (size_t i = 0; i < s.length(); ++i) 
        {
          switch (s[i]) 
             {
               case '\\': result += "\\\\"; break;
               case '"':  result += "\\\""; break;
               case '\a': result += "\\a"; break;
               case '\f': result += "\\f"; break;
            // This permits the "\n" sequences to be seen where they are used as labels in the dot files.
               case '\n': result += "(CR)\\n"; break;
               case '\r': result += "\\r"; break;
               case '\t': result += "\\t"; break;
               case '\v': result += "\\v"; break;
               default:
                  {
                    if (isprint(s[i])) 
                       {
                         result.push_back(s[i]);
                       }
                      else
                       {
                         std::ostringstream stream;
                         stream << '\\';
                         stream << std::setw(3) << std::setfill('0') <<std::oct << (unsigned)(unsigned char)(s[i]);
                         result += stream.str();
                       }
                  }
               break;
             }
        }
     return result;
   }


void
ATerm_Graph::graph ( ATerm term, string color )
   {
     ROSE_ASSERT(term != NULL);

     if (graphNodeSet.find(term) == graphNodeSet.end())
        {
          graphNodeSet.insert(term);
        }
       else
        {
          return;
        }

  // atermType will be one of: AT_APPL, AT_INT, AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
     int atermType = ATgetType(term);

     int arity = 0;
     string escaped_label;

     switch (atermType)
        {
          case AT_APPL:
             {
               AFun appl_symbol = ATgetAFun(term);

               ROSE_ASSERT(at_lookup_table != NULL);
               ROSE_ASSERT(at_lookup_table[(appl_symbol)] != NULL);

               string label  = ATgetName(appl_symbol);
               arity         = ATgetArity(appl_symbol);
               escaped_label = escapeString(label);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_APPL: arity = %d escaped_label = %s \n",term,arity,escaped_label.c_str());
#endif
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

               for (int i = 0; i < arity; i++)
                  {
                    ATerm aterm_arg = ATgetArgument(term,i);

                    graph (aterm_arg,color);

                    file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(aterm_arg) << "\"[label=\"" << "arg" << "\"];" << endl;
                  }

            // ROSE_ASSERT(!"case AT_APPL: not implemented!");
               break;
             }

          case AT_INT:
             {
               int value = ATgetInt(term);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_INT: atermType = %d value = %d \n",term,atermType,value);
#endif
               escaped_label = boost::lexical_cast<std::string>(value);
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

            // ROSE_ASSERT(!"case AT_INT: not implemented!");
               break;
             }

          case AT_REAL: 
             {
               double value = ATgetReal(term);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_REAL: atermType = %d value = %f \n",term,atermType,value);
#endif
               escaped_label = boost::lexical_cast<std::string>(value);
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

            // ROSE_ASSERT(!"case AT_REAL: not implemented!");
               break;
             }

          case AT_LIST:
             {
               escaped_label = "AT_LIST";
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

               vector<ATerm> aterm_list = ATerm_Graph::getAtermList(term);
               for (vector<ATerm>::iterator i = aterm_list.begin(); i != aterm_list.end(); i++)
                  {
                    graph (*i,color);

                    file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(*i) << "\"[label=\"" << "arg" << "\"];" << endl;
                  }

            // ROSE_ASSERT(!"case AT_LIST: not implemented!");
               break;
             }

          case AT_PLACEHOLDER:
             {
               printf ("In graph(ATerm): term = %p: case AT_PLACEHOLDER: atermType = %d \n",term,atermType);

               ROSE_ASSERT(!"case AT_PLACEHOLDER: not implemented!");
               break;
             }

          case AT_BLOB:
             {
            // We can likely output this blob thing as an escaped string (using ROSE function for this).
               printf ("In graph(ATerm): term = %p: case AT_BLOB: atermType = %d \n",term,atermType);

            // Generate the size and a pointer for the blob.
               int   blob_size = ATgetBlobSize(term);
               void* blob_ptr  = ATgetBlobData(term);

               printf ("In graph(ATerm): term = %p: case AT_BLOB: atermType = %d: blob_ptr = %p blob_size = %d \n",term,atermType,blob_ptr,blob_size);

               ROSE_ASSERT(!"case AT_BLOB: not implemented!");
               break;
             }

          default:
             {
               printf ("In graph(ATerm): term = %p: default reached atermType = %d \n",term,atermType);
               ROSE_ASSERT(!"error to reach default in switch!");
             }
        }

  // Check for an ATerm annotations.
     ATerm annotation = AT_getAnnotations(term);

#if 0
     printf ("In graph(ATerm): term = %p: case %s: arity = %d escaped_label = %s \n",term,aterm_type_name(term).c_str(),arity,escaped_label.c_str());
#endif

     if (annotation != NULL) 
        {
#if 0
          printf ("   --- annotation = %p \n",annotation);
#endif
          string color = "\"lightgreen\"";
          graph (annotation,color);

          file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(annotation) << "\"[label=\"" << "arg" << "\"];" << endl;
        }

#if 0
     printf ("Leaving graph(ATerm): term = %p \n",term);
#endif
   }

string
ATerm_Graph::aterm_type_name( ATerm term )
   {
     string s;
     int atermType = ATgetType(term);

     switch (atermType)
        {
          case AT_APPL:        s = "AT_APPL";        break;
          case AT_INT:         s = "AT_INT";         break;
          case AT_REAL:        s = "AT_REAL";        break;
          case AT_LIST:        s = "AT_LIST";        break;
          case AT_PLACEHOLDER: s = "AT_PLACEHOLDER"; break;
          case AT_BLOB:        s = "AT_BLOB";        break;

          default:
             {
               printf ("In graph(ATerm): term = %p: default reached atermType = %d \n",term,atermType);
               ROSE_ASSERT(!"error to reach default in switch!");
             }
        }

     return s;
   }

#if 0
bool
ATerm_Graph::outputInGraph( ATerm term )
   {
     bool match = true;
     return match;
   }
#endif

// ************************************************************************
// ************************************************************************
// ************************************************************************

void
ATerm_Graph::graph_aterm_ast(ATerm term, string filename)
   {
  // DQ (9/6/2013): Build a dot graph of the ATerm AST.

     string dot_header = filename;
     filename += ".dot";

#if DEBUG_DOT_GRAPH_SUPPPORT
     printf ("In graph_aterm_ast(): filename = %s \n",filename.c_str());
#endif

  // Open file...(file is declared in the EDG_ROSE_Graph namespace).
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

  // Calling recursive function to build dot file for scope, starting at the top level scope.
     string color = "\"lightblue\"";
     graph (term,color);

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();

#if 0
  // Communicate the name of the output file (in the current directory).
     printf ("Generated dot file: filename = %s \n",filename.c_str());
#endif
   }

int
main(int argc, char* argv[])
   {
  // This is the initialization procedure for the Aterm library.
     ATerm bottom;
     ATinit(argc, argv, &bottom);

  // printf ("At top of main(): argc = %d \n",argc);

     string aterm_filename;
     if (argc == 2)
        {
          aterm_filename = argv[1];
       // printf ("Using aterm file: %s \n",aterm_filename.c_str());
        }
       else
        {
          printf ("Usage: %s <aterm file> (requires one filename as an option) \n",argv[0]);
          return 1;
        }

  // Open a file for writing...
  // FILE* file = fopen("atermFile.aterm","r");
     FILE* file = fopen(aterm_filename.c_str(),"r");
     ROSE_ASSERT(file != NULL);

  // printf ("Read aterm file \n");

  // Write the aterm to the file.
     ATerm term = ATreadFromTextFile(file);
  // ATerm term = ATreadFromNamendFile("atermFile.aterm");
  // ATerm term = ATreadFromFile(file);
     ROSE_ASSERT(term != NULL);

  // Close the file.
     fclose(file);

     char* s = strdup(aterm_filename.c_str());
     string file_basename = basename(s);

     ATerm_Graph::graph_aterm_ast(term,file_basename);

#if 0
     printf ("Program Terminated Normally \n");
#endif
     return 0;
   }

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

#include "rose.h"

// #include "rose_aterm_api.h"
#include "aterm1.h"
#include "aterm2.h"

#include <libgen.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iomanip>

#include <fstream>
#include <vector>
#include <set>
#include <assert.h>

#include "atermTranslation.h"

// #define ROSE_ASSERT(x) assert(x)

// Use this file ahead of any source code where we want to intercept the ATerm API
// and forward functions to the ROSE implementation of the ATerm API.  This implements
// function forwarding which is supported in the Java langauge ATerm API and Java 
// implementation of Stratego but which is a hack in the C language API we are using.
// #include "intercept_ATerm_API.h"

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

  // This will allow the output of a NULL valued node in the graph (but we only want to do so once).
     bool null_node_output = false;
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


SgNode* convertStringToPointer(string label)
   {
  // This function converts the string representation saved in and retrieved from an ATerm and converts it to a proper SgNode pointer.

     SgNode* returnNode = NULL;

#if 0
     printf ("In convertStringToPointer(): label = %s \n",label.c_str());
#endif

     const char* c_style_string = label.c_str();

#if 0
     printf ("In convertStringToPointer(): c_style_string = %s \n",c_style_string);
     printf ("In convertStringToPointer(): c_style_string = %p label.length() = %zu c_style_string+label.length() = %p \n",c_style_string,label.length(),c_style_string+label.length());
#endif

  // Convert the string starting at a starting pointer, to an ending pointer, using base 16.
  // unsigned long valueFromHex = strtoul(c_style_string,c_style_string+label.length()),16);
     unsigned long valueFromHex = strtoul(c_style_string,NULL,16);

#if 0
     printf ("In convertStringToPointer(): valueFromHex = %zu \n",valueFromHex);
#endif

     returnNode = (SgNode*) valueFromHex;

#if 0
     printf ("In convertStringToPointer(): returnNode = %p \n",returnNode);
#endif

#if 0
  // Test the pointer by calling a ROSE member function from it.
     printf ("In convertStringToPointer(): returnNode = %p = %s \n",returnNode,returnNode->class_name().c_str());
#endif

     return returnNode;
   }


SgNode* getNodeFromAterm(ATerm term);

// Map used to store the connection of AFun values to ATerm (pointers).
std::map<size_t,ATerm> afunToAtermMap;

AFun rose_ATgetAFun(ATerm term)
   {
     AFun appl_symbol = ATgetAFun(term);

#if 0
     printf ("In rose_ATgetAFun(): term = %p appl_symbol = %zu \n",term,appl_symbol);
#endif

  // Save the term so that we can look it up using the appl_symbol, so that when we query arity 
  // (for ATgetArity() in rose_ATgetArity()), we can get back to the associated AST node to 
  // generate the correct arity.
     if (afunToAtermMap.find(appl_symbol) != afunToAtermMap.end())
        {
          printf ("In rose_ATgetAFun(): We have this already in the map: afunToAtermMap[(size_t)appl_symbol = %zu] = %p \n",appl_symbol,afunToAtermMap[(size_t)appl_symbol]);

       // I'm not clear if this is allowed, so output a warning for now.
          if (afunToAtermMap[(size_t)appl_symbol] != term)
             {
               printf ("***** WARNING: entry in map does not match what was expected! term = %p afunToAtermMap[(size_t)appl_symbol] = %p appl_symbol = %zu \n",term,afunToAtermMap[(size_t)appl_symbol],appl_symbol);
#if 0
            // DQ (10/27/2013): It is not robust to call: "getNodeFromAterm(afunToAtermMap[(size_t)appl_symbol]);" see test2004_77.C.
               SgNode* termNode_1 = getNodeFromAterm(term);
               SgNode* termNode_2 = getNodeFromAterm(afunToAtermMap[(size_t)appl_symbol]);
               if (termNode_1 != NULL && termNode_2 != NULL)
                  {
                    printf ("   --- term = %p = %p = %s afunToAtermMap[(size_t)appl_symbol] = %p = %p = %s \n",term,termNode_1,termNode_1->class_name().c_str(),afunToAtermMap[(size_t)appl_symbol],termNode_2,termNode_2->class_name().c_str());
                  }
                 else
                  {
                    printf ("termNode_1 = %p termNode_2 = %p \n",termNode_1,termNode_2);
                  }
#endif
             }
       // ROSE_ASSERT(afunToAtermMap[(size_t)appl_symbol] == term);
        }
       else
        {
       // Save this reference to the aterm so we can look it up later.
#if 0
          printf ("In rose_ATgetAFun(): Adding this aterm to the map: term = %p appl_symbol = %zu \n",term,appl_symbol);
#endif
          afunToAtermMap[(size_t)appl_symbol] = term;
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     return appl_symbol;
   }


int rose_ATgetArity(AFun appl_symbol)
   {
     int arity = 0;

#if 0
     printf ("In rose_ATgetArity(): appl_symbol = %zu \n",appl_symbol);
#endif

     if (afunToAtermMap.find(appl_symbol) != afunToAtermMap.end())
        {
          ATerm term = afunToAtermMap[(size_t)appl_symbol];

       // FIXME: refactor to a function that returns the SgNode from the Aterm.
          ATerm ptr = ATgetAnnotation(term, ATmake("ptr"));
       // ROSE_ASSERT(ptr != NULL);
          if (ptr != NULL)
             {
#if 0
               printf ("In rose_ATgetArity(): Found the ptr annotation in the term: ptr = %p \n",ptr);
#endif
               AFun local_appl_symbol = ATgetAFun(ptr);
               string label           = ATgetName(local_appl_symbol);
               string escaped_label   = escapeString(label);
#if 0
               printf ("In rose_ATgetArity(): term = %p: case AT_APPL: escaped_label = %s \n",term,escaped_label.c_str());
#endif
               SgNode* associatedNode = convertStringToPointer(label);
               ROSE_ASSERT(associatedNode != NULL);
#if 1
               printf ("In rose_ATgetArity(): associatedNode = %p = %s \n",associatedNode,associatedNode->class_name().c_str());
#endif
               arity = associatedNode->get_traversalSuccessorNamesContainer().size();

               SgFunctionParameterList* parameterList = isSgFunctionParameterList(associatedNode);
               if (parameterList != NULL)
                  {
                    printf ("In rose_ATgetArity(): arity = %d parameterList->size() = %zu \n",arity,parameterList->get_args().size());
                    ROSE_ASSERT((size_t)arity == parameterList->get_args().size());
                    for (int i = 0; i < arity; i++)
                       {
                         printf ("   --- name = %s \n",associatedNode->get_traversalSuccessorNamesContainer()[i].c_str());
                       }
                  }
             }
            else
             {
#if 0
               printf ("Warning: In rose_ATgetArity(): Found the ptr annotation in the term: ptr == NULL \n");
#endif
             }
        }
       else
        {
       // Error: can't find the aterm associated with this symbol.
#if 0
          printf ("Error: In rose_ATgetArity(): can't find the aterm associated with this symbol \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("In rose_ATgetArity(): returning arity = %d \n",arity);
#endif

     return arity;
   }


int rose_ATgetArity(ATerm term)
   {
  // This is NOT an ATerm API function, but we need it to provide a more direct way to get 
  // the arity (the use of the afunToAtermMap is not working except on little example codes).
     int arity = 0;

#if 0
     printf ("In rose_ATgetArity(ATerm): term = %p \n",term);
#endif

     SgNode* associatedNode = getNodeFromAterm(term);

     if (associatedNode != NULL)
        {
#if 0
          printf ("In rose_ATgetArity(ATerm): associatedNode = %p = %s \n",associatedNode,associatedNode->class_name().c_str());
#endif
          arity = associatedNode->get_traversalSuccessorNamesContainer().size();
        }

#if 0
     printf ("In rose_ATgetArity(ATerm): returning arity = %d \n",arity);
#endif

     return arity;
   }


SgNode* getNodeFromAterm(ATerm term)
   {
     ROSE_ASSERT(term != NULL);

  // Get the pointer to the AST.
     ATerm ptr = ATgetAnnotation(term, ATmake("ptr"));

  // ROSE_ASSERT(ptr != NULL);

#if 0
     printf ("In getNodeFromAterm(ATerm term): Found the ptr annotation in the term: ptr = %p \n",ptr);
#endif

     SgNode* parentNode = NULL;
     if (ptr != NULL)
        {
       // Make sure this the the correct kind of aterm.
          ROSE_ASSERT(ATgetType(ptr) == AT_APPL);

          AFun appl_symbol     = ATgetAFun(ptr);
          string label         = ATgetName(appl_symbol);
       // int arity            = ATgetArity(appl_symbol);
          string escaped_label = escapeString(label);
#if 0
          printf ("In getNodeFromAterm(ATerm term): term = %p: escaped_label = %s \n",term,escaped_label.c_str());
#endif
#if 0
          string color = "\"red\"";
          ATerm_Graph::graph(ptr,color);
#endif

       // Then find it's associated pointer to the ROSE AST IR node.
       // SgNode* parentNode = convertAtermToNode(term);
          parentNode = convertStringToPointer(label);
        }

     return parentNode;
   }


ATerm rose_ATgetArgument(ATerm term, int i)
   {
  // This is the required ROSE specific ATerm implementation for the "ATerm ATgetArgument(ATerm term,int i);" function.

#if 0
     printf ("In rose_ATgetArgument(ATerm term, int i): TOP \n");
#endif

  // Check if this is a ROSE specific ATerm (check associated pointer to ROSE AST node).

#if 1
     SgNode* parentNode = getNodeFromAterm(term);
#else
  // Get the pointer to the AST.
     ATerm ptr = ATgetAnnotation(term, ATmake("ptr"));
     ROSE_ASSERT(ptr != NULL);

#error "DEAD CODE!"

     printf ("Found the ptr annotation in the term: ptr = %p \n",ptr);

  // Make sure this the the correct kind of aterm.
     ROSE_ASSERT(ATgetType(ptr) == AT_APPL);

     AFun appl_symbol     = ATgetAFun(ptr);
     string label         = ATgetName(appl_symbol);

  // int arity            = ATgetArity(appl_symbol);
     int arity            = ATgetArity(term);

     string escaped_label = escapeString(label);

  // Then find it's associated pointer to the ROSE AST IR node.
  // SgNode* parentNode = convertAtermToNode(term);
     SgNode* parentNode = convertStringToPointer(label);
#endif

     ROSE_ASSERT(parentNode != NULL);

#if 0
     printf ("In rose_ATgetArgument(ATerm term, int i = %d): parentNode = %p = %s \n",i,parentNode,parentNode->class_name().c_str());
#endif
     SgFunctionParameterList* parameterList = isSgFunctionParameterList(parentNode);
     if (parameterList != NULL)
        {
#if 0
          string name = parentNode->get_traversalSuccessorNamesContainer()[i];
          printf ("In rose_ATgetArgument(ATerm term, int i = %d): parameterList->size() = %zu name = %s \n",i,parameterList->get_args().size(),name.c_str());
#endif
          ROSE_ASSERT((size_t) i < parameterList->get_args().size());
        }
#if 0
     parentNode->get_file_info()->display("In rose_ATgetArgument(ATerm term, int i): debug");
#endif

  // This find the AST IR node representing the i'th entry in the AST.
     SgNode* childNode = parentNode->get_traversalSuccessorByIndex(i);

     if (childNode == NULL)
        {
#if 0
          printf ("Found a null childNode: i = %d \n",i);
#endif
          string name = parentNode->get_traversalSuccessorNamesContainer()[i];
#if 0
          printf ("Found a null childNode: i = %d name = %s \n",i,name.c_str());
#endif
       // We have to return a null pointer.
          return NULL;
        }
     ROSE_ASSERT(childNode != NULL);

     string name = parentNode->get_traversalSuccessorNamesContainer()[i];

#if 0
     printf ("In rose_ATgetArgument(ATerm term, int i = %d): parentNode = %p = %s childNode = %p = %s (edge name = %s) \n",i,parentNode,parentNode->class_name().c_str(),childNode,childNode->class_name().c_str(),name.c_str());
#endif

  // Then call convertNodeToAterm() to generate the child ATerm and return it.
     ATerm childTerm = convertNodeToAterm(childNode);
     ROSE_ASSERT(childTerm != NULL);

#if 0
     printf ("In rose_ATgetArgument(ATerm term, int i): childTerm = %p \n",childTerm);
#endif

     return childTerm;
   }


string getEdgeString(ATerm term, int i)
   {
     string edge_string = "argx";

     SgNode* parentNode = getNodeFromAterm(term);

     if (parentNode != NULL)
          edge_string = parentNode->get_traversalSuccessorNamesContainer()[i];

     return edge_string;
   }



void
ATerm_Graph::graph ( ATerm term, string color )
   {
  // Since an ATerm is implemented as a pointer to an internal type, we can only check for non-NULL as a test.
  // When the term is using lazy evaluation to control the unfolding of the AST into ATerms then we need to 
  // hide that in the ATerm library.

     ROSE_ASSERT(term != NULL);

#if 0
     printf ("In graph(ATerm): term = %p: TOP of function \n",term);
#endif

     if (graphNodeSet.find(term) == graphNodeSet.end())
        {
          graphNodeSet.insert(term);
        }
       else
        {
          return;
        }

#if 0
     printf ("In graph(ATerm): term = %p: calling ATgetType() \n",term);
#endif

  // Even when we have generated children as uninterpreded aterms we can still call some functions
  // that only operate directly on the term without accessin the children.  So the ATgetType()
  // is an example of such a function.

  // atermType will be one of: AT_APPL, AT_INT, AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
     int atermType = ATgetType(term);

     int arity = 0;
     string escaped_label;

#if 0
     printf ("In graph(ATerm): term = %p: before switch: atermType = %d \n",term,atermType);
#endif

     switch (atermType)
        {
          case AT_APPL:
             {
#if 0
               printf ("In graph(ATerm): term = %p: case AT_APPL: calling ATgetAFun() \n",term);
#endif
            // AFun appl_symbol = ATgetAFun(term);
            // AFun appl_symbol = rose_ATgetAFun(term);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_APPL: appl_symbol = %zu \n",term,appl_symbol);
#endif
#if 0
               printf ("In graph(ATerm): term = %p: case AT_APPL: at_lookup_table \n",term);
#endif
               ROSE_ASSERT(at_lookup_table != NULL);
            // ROSE_ASSERT(at_lookup_table[(appl_symbol)] != NULL);
#if 0
                printf ("In graph(ATerm): term = %p: case AT_APPL: calling ATgetName() \n",term);
#endif
            // Trying to eliminate the call to rose_ATgetAFun() since it is a problem (can't use appl_symbol as a key to a map to locate IR nodes).
            // string label  = ATgetName(appl_symbol);
                SgNode* node = getNodeFromAterm(term);
                string label  = (node != NULL) ? node->class_name() : "unknown";

            // DQ (10/26/2013): It is a problem to associate the ROSE IR nodes with the AFun
            // objects (which loose there association with the original ATerm objects.
            // For the moment let's simplify the mapping so that we can get the arity more directly.
#if 0
               printf ("Using ROSE specific function to get the arity directly from the ATerm instead of via the AFun \n");
#endif
            // arity         = ATgetArity(appl_symbol);
            // arity         = rose_ATgetArity(appl_symbol);
               arity         = rose_ATgetArity(term);

               escaped_label = escapeString(label);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_APPL: arity = %d escaped_label = %s \n",term,arity,escaped_label.c_str());
#endif
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

               for (int i = 0; i < arity; i++)
                  {
                 // I think that ATgetArgument() is a function where we have to now force the interpretation of child aterm nodes to be proper ATerms.
                 // ATerm aterm_arg = ATgetArgument(term,i);
                    ATerm aterm_arg = rose_ATgetArgument(term,i);
#if 1
                 // This change is required because the AST has some NULL pointers which have to be 
                 // skipped (null pointers as children don't translate well to aterms, I think).
                    if (aterm_arg == NULL)
                       {
#if 0
                         printf ("In graph(ATerm): term = %p: case AT_APPL: from rose_ATgetArgument(): Detected aterm_arg == NULL \n",term);
#endif
                         if (null_node_output == false)
                            {
                              file << "\"" << boost::lexical_cast<std::string>(aterm_arg) << "\"[" << "label=\"" << "NULL" << "\\n" 
                                   << "\" color=" << color << ",fillcolor=" << color 
                                   << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;
                              null_node_output = true;
                            }
                       }
                      else
                       {
                         graph (aterm_arg,color);
                       }
#else
                 // This is the original case of reading the aterms directly.
                    graph (aterm_arg,color);
#endif
                    string edge_string = getEdgeString(term,i);

                    SgNode* parent_node = getNodeFromAterm(term);
                    SgNode* child_node  = aterm_arg != NULL ? getNodeFromAterm(aterm_arg) : NULL;

                    string parent_string = (parent_node != NULL) ? parent_node->class_name() : "null";
                    string child_string  = (child_node  != NULL) ? child_node->class_name()  : "null";

                    string node_name = boost::lexical_cast<std::string>(aterm_arg);
#if 0
                    printf ("-------> building an edge: term = %p = %p = %s ---> name = %s ---> aterm_arg = %p = %p = %s \n",term,parent_node,parent_string.c_str(),edge_string.c_str(),aterm_arg,child_node,child_string.c_str());
#endif
                 // file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(aterm_arg) << "\"[label=\"" << "arg" << "\"];" << endl;
                 // file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(aterm_arg) << "\"[label=\"" << edge_string << "\"];" << endl;
                    file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << node_name << "\"[label=\"" << edge_string << "\"];" << endl;
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
               ROSE_ASSERT(term != NULL);
               ROSE_ASSERT(value != 0);
            // string node_name = boost::lexical_cast<std::string>(term);

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
#if 0
               printf ("In graph(ATerm): term = %p: case AT_LIST: atermType = %d \n",term,atermType);
#endif
               escaped_label = "AT_LIST";
               file << "\"" << boost::lexical_cast<std::string>(term) << "\"[" << "label=\"" << escaped_label << "\\n" 
                    << boost::lexical_cast<std::string>(term) << "\" color=" << color << ",fillcolor=" << color 
                    << ",fontname=\"7x13bold\",fontcolor=black,style=filled];" << endl;

               vector<ATerm> aterm_list = ATerm_Graph::getAtermList(term);
#if 0
               printf ("In graph(ATerm): term = %p: case AT_LIST: atermType = %d aterm_list.size() = %zu \n",term,atermType,aterm_list.size());
#endif
               for (vector<ATerm>::iterator i = aterm_list.begin(); i != aterm_list.end(); i++)
                  {
                    graph (*i,color);

                    file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(*i) << "\"[label=\"" << "list_arg" << "\"];" << endl;
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

#if 0
     printf ("In graph(ATerm): term = %p: case %s: arity = %d escaped_label = %s \n",term,aterm_type_name(term).c_str(),arity,escaped_label.c_str());
#endif

#if 0
  // Check for an ATerm annotations.
     ATerm annotation = AT_getAnnotations(term);

     if (annotation != NULL) 
        {
#if 0
          printf ("   --- annotation = %p \n",annotation);
#endif
          string color = "\"lightgreen\"";
          graph (annotation,color);

          file << "\"" << boost::lexical_cast<std::string>(term) << "\" -> \"" << boost::lexical_cast<std::string>(annotation) << "\"[label=\"" << "annotation_arg" << "\"];" << endl;
        }
#else
  // We can skip the annotations to the ATerm as a way of building a simpler graph that represents only the ROSE AST and not the additional decorations.
#if 0
     printf ("In graph(ATerm): Skip graphing annotations for term = %p \n",term);
#endif
#endif

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
  // filename += ".dot";
     filename += ".aterm.dot";

#if DEBUG_DOT_GRAPH_SUPPPORT
     printf ("In graph_aterm_ast(): filename = %s \n",filename.c_str());
#endif

  // Open file...(file is declared in the EDG_ROSE_Graph namespace).
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;

  // Calling recursive function to build dot file for scope, starting at the top level scope.
     string color = "\"lightblue\"";
#if 1
     graph (term,color);
#endif

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();

#if 0
  // Communicate the name of the output file (in the current directory).
     printf ("Generated dot file: filename = %s \n",filename.c_str());
#endif
   }


ATerm convertAstNodeToRoseAterm(SgNode* node)
   {
  // This function likely does not make sense because we will not be converting ROSE AST's to Aterms via a serialization.

  // I think that we need an envelope data structure to hold the SgNode* and represent the AST.

  // ROSE_ASSERT(!"convertNodeToAterm(): not implemented!");
#if 0
     printf ("Inside of convertAstNodeToRoseAterm(node = %p = %s) \n",node,node->class_name().c_str());
#endif

  // Note that this is a C data structure, so we have some likitations on what we can do.
  // We have specifically added a SgNode* data member to this data structure so that we
  // can build them as ATerm objects and permit the C ATerm API to pass them around 
  // between ATerm API functions (and macros).  We might have to represent all of the 
  // macros as proper functions at some point, but this is more complex since as macros
  // the the ATerm API exploits some liberties that would not be available to functions
  // (because of the type system).  At least this is my current understanding.

  // Allocating these on the heap is not enough, we have to properly initialize them as ATerms.
  // In the case of a SgProject IR node this should be a node with a short list of children.
  // The children should not be interpreted, only the subtree root SgProject* should be setup 
  // as a ATerm. Children would be interpreted when they are referenced (as seperate ATerm objects).

#if 0
  // ATerm tmp_aterm;
  // ATerm tmp_aterm = new _ATerm;
  // convertNodeToAterm(isSgSourceFile(n)->get_globalScope()));
     ROSE_ASSERT(isSgSourceFile(node) != NULL);
     SgGlobal* globalScope = isSgSourceFile(node)->get_globalScope();

     printf ("Building a the child Aterm for the global scope = %p \n",globalScope);

  // Let this be a signature of an unevaluated (unwrapped) child node.
     ATerm wrappedChild = ATmake("wrappedChild(<int>)",99);

     printf ("Constructed the wrappedChild = %p \n",wrappedChild);

  // wrappedChild->aterm.rose_IR_node = globalScope;

     printf ("Assgined the global scope into the wrappedChild = %p \n",wrappedChild);

     ATerm tmp_aterm = ATmake("SourceFile(<term>)",wrappedChild);

  // We have to make a proper term from this ROSE AST IR node.  To this extent we are likely to 
  // serialize and de-serialize the AST (but only on a node by node basis).  In some cases this 
  // will not be required, but when we return an ATerm we have to build something.
  // So likely we need to make a ATerm here that will at least hold the SgNode* and be able to
  // be queried using the ATerm API (including macros that reference specific parts of the ATerm 
  // data structure.

  // I think this should be a AT_APPL == 1, type of applications of function names to terms



     printf ("Assign node pointer to tmp_aterm %p \n",tmp_aterm);
  // tmp_aterm->aterm.rose_IR_node = node;

  // atermType will be one of: AT_APPL, AT_INT, AT_REAL, AT_LIST, AT_PLACEHOLDER, or AT_BLOB.
     int atermType = ATgetType(tmp_aterm);

     printf ("ATgetType(tmp_term) = %d \n",atermType);

  // The value of 7 is an error state, I think.
     ROSE_ASSERT(atermType != 7);

     printf ("building the annotation to add the ROSE IR node \n");

  // ATerm label = ATmake("wrappedChild(<str>)","ROSE_AST_node");
     ATerm label = ATmake("<str>","ROSE_AST_node");
  // ATerm anno  = ATmake("wrappedChild(<int>)",42);
     ATerm anno  = (ATerm) ATmakeInt(42);

     printf ("Building the annotationed aterm to add the ROSE IR node \n");

     ATerm annotated_aterm = ATsetAnnotation(tmp_aterm,label,anno);

#else
  // Actually this implementation in convertNodeToAterm already adds the pointer value to the aterm, so we can just return this Aterm directly.
     ATerm annotated_aterm = convertNodeToAterm(node);
#endif
#if 0
     printf ("Returning annotated_aterm = %p \n",annotated_aterm);
#endif

     return annotated_aterm;
   }




int
main(int argc, char* argv[])
   {
  // This program is only building the aterm DOT file, and using the ROSE AST as input.
  // It demonstrates the use of the ATerm API to operate on the ROSE AST API.
  // The program demonstrates that a few ATerm API details had to made ROSE specific
  // and that one ATerm API function (generating an AFun type) is problematic since
  // in this low level representation it maps to an integer, and that integer cannot be
  // used as a map back to the ROSE AST nodes.  This aspect will have to be rethought
  // or perhaps there is a way around this problems that I have not yet identified.
  // This test runs on about 800 of the C++ regression tests, but fails on 6 specific 
  // test codes (see the Makefile.am for the specific failing tests). All failing tests
  // in the fail in the same way with a memory corruption detected by GNU, Valgrind has 
  // yet to be run).

#if 0
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
#endif

  // Generate a ROSE AST as input.
  // printf ("Building the ROSE AST \n");
     SgProject* project = frontend(argc,argv);
  // printf ("Done: Building the ROSE AST \n");

  // Output an optional graph of the AST (just the tree, when active)
     generateDOT(*project);

     SgFile* roseFile = project->operator[](0);
     ROSE_ASSERT(roseFile != NULL);
     SgSourceFile* sourceFile = isSgSourceFile(roseFile);
     ROSE_ASSERT(sourceFile != NULL);

  // printf ("Calling ATinit \n");
     ATerm bottom;
     ATinit(argc, argv, &bottom);

  // printf ("Calling convertAstNodeToRoseAterm \n");
  // ATerm term = convertNodeToAterm(project);
  // ATerm term = convertNodeToAterm(sourceFile);
     ATerm term = convertAstNodeToRoseAterm(sourceFile);
  // printf ("DONE: Calling convertAstNodeToRoseAterm term = %p \n",term);

     ROSE_ASSERT (term != NULL);

     string roseAST_filename = project->get_file(0).getFileName();
     char* s = strdup(roseAST_filename.c_str());
     string file_basename = basename(s);

     ATerm_Graph::graph_aterm_ast(term,file_basename);

#if 0
  // DQ (9/17/2014): Adding test for conversion of Aterm back to AST.
     printf ("Testing the reverse process to generate the ROSE AST from the Aterm \n");
     SgNode* rootOfAST = convertAtermToNode(term);
     printf ("rootOfAST = %p = %s \n",rootOfAST,rootOfAST->class_name().c_str());
#endif

#if 0
     printf ("Program Terminated Normally \n");
#endif

     return 0;
   }

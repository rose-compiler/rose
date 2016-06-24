// This example shows the generation of unique names from declarations.

// Mangled name demo
//
// This translator queries the AST for all SgInitializedNames and
// SgFunctionDeclarations, and for each one prints (a) the source
// location, (b) the source name of the object, and (c) the mangled
// name.

#include <rose.h>

using namespace std;

// Returns a Sg_File_Info object as a display-friendly string, "[source:line]".
static string toString (const Sg_File_Info* info)
   {
     ostringstream info_str;
     if (info)
     info_str << '['
              << info->get_raw_filename ()
              << ":" << info->get_raw_line ()
              << ']';
     return info_str.str ();
   }

// Displays location and mangled name of an SgInitializedName object.
static void printInitializedName (const SgNode* node)
   {
     const SgInitializedName* name = isSgInitializedName (node);
     ROSE_ASSERT (name != NULL);

     if (name->get_file_info()->isCompilerGenerated() == false)
          cout // << toString (name->get_file_info ())
               // << " " 
               << name->get_name ().str ()
               << " --> " << name->get_mangled_name ().str ()
               << endl;
   }

// Displays location and mangled name of an SgFunctionDeclaration object.
static void printFunctionDeclaration (const SgNode* node)
   {
     const SgFunctionDeclaration* decl = isSgFunctionDeclaration (node);
     ROSE_ASSERT (decl != NULL);

     if (decl->get_file_info()->isCompilerGenerated() == false)
          cout // << toString (decl->get_startOfConstruct ())
               // << " " 
               << decl->get_qualified_name ().str ()
               << " --> " << decl->get_mangled_name ().str ()
               << endl;
   }

int main (int argc, char** argv)
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* proj = frontend (argc, argv);

     cout << endl << "***** BEGIN initialized names *****" << endl;
     Rose_STL_Container<SgNode *> init_names = NodeQuery::querySubTree (proj, V_SgInitializedName);
     for_each (init_names.begin (), init_names.end (), printInitializedName);
     cout << "***** END initialized names *****" << endl;

     cout << endl << "***** BEGIN function declarations *****" << endl;
     Rose_STL_Container<SgNode *> func_decls = NodeQuery::querySubTree (proj, V_SgFunctionDeclaration);
     for_each (func_decls.begin (), func_decls.end (), printFunctionDeclaration);
     cout << "***** END function declarations *****" << endl;

     return backend (proj);
   }


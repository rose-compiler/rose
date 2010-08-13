/*!
 *  \file testMPIAbstraction.C
 *  \brief Unit test for the MPIAbstraction module.
 */
  
#include "rose.h"
#include <iostream>
#include <string>
#include <sstream>

#ifndef CXX_IS_ROSE_CODE_GENERATION

#include <MPIAbstraction/Recognizer.h>
#include <commandline_processing.h>

using namespace std;

//=========================================================================

//! Traversal to recognize some API.
class RecognitionTraversal : public AstSimpleProcessing
{
public:
  RecognitionTraversal (const string& mpi_api_file)
    : recognizer_ (mpi_api_file), found_count_ (0)
  {
  }

  virtual void visit (SgNode* n)
  {
    visitCall (isSgFunctionCallExp (n));
    visitConst (isSgVarRefExp (n));
  }

protected:
  //! Converts a located node to a user-friendly string position.
  static string toLoc (const SgLocatedNode* n)
  {
    ostringstream o;
    if (n)
      {
	const Sg_File_Info* pos_start = n->get_startOfConstruct ();
	ROSE_ASSERT (pos_start);
	const Sg_File_Info* pos_end = n->get_endOfConstruct ();
	o << '[';
	o << pos_start->get_raw_filename () << ':' << pos_start->get_raw_line ();
	if (pos_end
	    && pos_end->get_raw_filename () == pos_start->get_raw_filename ()
	    && pos_end->get_raw_line () > pos_start->get_raw_line ())
	  o << "--" << pos_end->get_raw_line ();
	o << ']';
      }
    return o.str ();
  }

  //! Visit function for function calls.
  bool visitCall (const SgFunctionCallExp* call)
  {
    if (recognizer_.isCall (call))
      {
	const C_API::FuncSig* sig = recognizer_.lookup (call);
	ROSE_ASSERT (sig);

	const Sg_File_Info* pos = call->get_startOfConstruct ();
	ROSE_ASSERT (pos);

	cout << "  [" << ++found_count_ << ']'
             << " function:"
	     << sig->getName () << " (" << sig->getNumArgs () << " args)"
	     << " at " << toLoc (call)
	     << endl;
        return true;
      }
    return false; // Not recognized
  }

  //! Visit functions for 'constants' represented as SgVarRefExp objects.
  bool visitConst (const SgVarRefExp* var_ref)
  {
    if (recognizer_.isConst (var_ref))
      {
	const C_API::ConstSig* sig = recognizer_.lookup (var_ref);
	ROSE_ASSERT (sig);

	cout << "  [" << ++found_count_ << ']'
             << " constant:" << sig->getName ()
	     << " at " << toLoc (var_ref)
	     << endl;
        return true;
      }
    return false; // Not recognized
  }

private:
  MPIAbstraction::Recognizer recognizer_; //!< Object that will recognize calls.
  size_t found_count_; //!< Count the number of calls recognized.
}; // class RecognitionTraversal


//=========================================================================

int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv + argc);

  using namespace CommandlineProcessing;
  bool make_pdfs = isOption (argvList, "--", "make-pdfs", true);

  string mpi_api_file;
  if (!isOptionWithParameter (argvList, "--", "mpi-api",
			      mpi_api_file, true))
    mpi_api_file = "./MPI.api";

  // Parse project.
  cerr << "[Parsing ...]" << endl;
  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

#ifndef CXX_IS_ROSE_CODE_GENERATION
  // Try to recognize constructs.
  cerr << "[Recognizing ...]" << endl;
  RecognitionTraversal traversal (mpi_api_file);
  traversal.traverse (proj, preorder);
#endif

  if (make_pdfs)
    {
      cerr << "[Generating a PDF...]" << endl;
      generatePDF (*proj);
    }

  return 0;
}

#endif

//=========================================================================
// eof

/*!
 *  \file testC-API.C
 *  \brief Unit test for C-API module.
 */
#include <rose.h>
#include <iostream>
#include <string>

#include <C-API/C-API.h>
#include <C-API/Recognizer.h>


#include <commandline_processing.h>

using namespace std;
using namespace CommandlineProcessing;

//=========================================================================

//! Traversal to recognize some API.
class RecognitionTraversal : public AstSimpleProcessing
{
public:
  RecognitionTraversal (const C_API::SigMap_t& sigs)
    : recognizer_ (&sigs), found_count_ (0)
  {
  }

  virtual void visit (SgNode* n)
  {
    const SgFunctionCallExp* call = recognizer_.isCall (n);
    if (call)
      {
	const C_API::FuncSig* sig = recognizer_.lookup (call);
	ROSE_ASSERT (sig);

	const Sg_File_Info* pos = call->get_startOfConstruct ();
	ROSE_ASSERT (pos);

	cout << "  [" << ++found_count_ << ']'
	     << ' ' << sig->getName () << " (" << sig->getNumArgs () << " args)"
	     << " at ["
	     << pos->get_raw_filename () << ':' << pos->get_raw_line ()
	     << ']'
	     << endl;
      }
  }

protected:
  RecognitionTraversal (void);

private:
  C_API::Recognizer recognizer_; //!< Object that will recognize calls.
  size_t found_count_; //!< Count the number of calls recognized.
}; // class RecognitionTraversal

//=========================================================================

int
main (int argc, char* argv[])
{
  vector<string> argvList(argv, argv + argc);

  string api_file;
  if (!isOptionWithParameter (argvList, "--", "api", api_file, true)
      || api_file.empty ())
    {
      cerr << "usage: " << argv[0] << " --api <api-file> [cc-args]" << endl;
      return 1;
    }

  cerr << "[Loading API specification...]" << endl;
  C_API::SigMap_t api_sigs;  // Defines the API.

  // Read API spec from a file.
  ifstream infile (api_file.c_str ());
  if (!infile.is_open () || !C_API::readSigs (infile, &api_sigs))
    {
      cerr << "*** Can't open API file, '" << api_file << "' ***" << endl;
      return 2;
    }
  cerr << "  Found " << api_sigs.size () << " signatures." << endl;

  // Parse project.
  cerr << "[Parsing ...]" << endl;
  SgProject* proj = frontend (argvList);
  ROSE_ASSERT (proj);

  // Try to recognize constructs.
  cerr << "[Recognizing ...]" << endl;
  RecognitionTraversal traversal (api_sigs);
  traversal.traverse (proj, preorder);

  return 0;
}

//=========================================================================
// eof

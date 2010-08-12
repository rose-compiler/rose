#include "rose.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
//#include <map>
//#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/regex.hpp>
//#include <boost/wave.hpp>
//#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
//#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class
#include <boost/program_options.hpp>

using namespace boost;
using namespace boost::filesystem;
using namespace std;

#define foreach BOOST_FOREACH

// tell if p1 is the parent path of p2
bool isParent(const path& p1, const path& p2)
{
    path p = p2.parent_path();
    while (1)
    {
	if (boost::filesystem::equivalent(p, p1)) return true;
	if (!p.has_parent_path())
	    break;
	p = p.parent_path();
    }
    return false;
}

// find all headers in a directory
void getAllHeaderFiles(const path& p, set<path>& files)
{
    for (directory_iterator i(p); i != directory_iterator(); ++i)
    {
	if (is_directory(i->status()))
	    getAllHeaderFiles(i->path(), files);
	else 
	{
	    path real_path = realpath(i->path().string().c_str(), NULL);
	    if (isParent(p, real_path))
		files.insert(real_path);
	}
    }
}

#if 0
void getAllHeadersIncluded(const path& input_file,
	const path& target_path, 
	set<path>& headers_included,
	const vector<path>& include_paths,
	const vector<string>& macros_defined)
{

    // for each header, collect all headers it includes recursively
    boost::wave::util::file_position_type current_position;
    try
    {
	// The following preprocesses a given input file.
	// Open the file and read it into a string variable
	const char* filename = input_file.string().c_str();
	std::ifstream instream(filename);
	instream.unsetf(std::ios::skipws);
	std::string input(
		std::istreambuf_iterator<char>(instream.rdbuf()),
		std::istreambuf_iterator<char>());

	// The template boost::wave::cpplexer::lex_token<> is the  
	// token type to be used by the Wave library.
	// This token type is one of the central types throughout 
	// the library, because it is a template parameter to some 
	// of the public classes and templates and it is returned 
	// from the iterators.
	// The template boost::wave::cpplexer::lex_iterator<> is
	// the lexer iterator to use as the token source for the
	// preprocessing engine. In this case this is parametrized
	// with the token type.
	typedef boost::wave::cpplexer::lex_iterator<
	    boost::wave::cpplexer::lex_token<> >
	    lex_iterator_type;
	typedef boost::wave::context<
	    std::string::iterator, lex_iterator_type>
	    context_type;

	context_type ctx(input.begin(), input.end(), filename);

	// At this point you may want to set the parameters of the
	// preprocessing as include paths and/or predefined macros.

	foreach (const path& p, include_paths)
	    ctx.add_sysinclude_path(p.string().c_str());
	foreach (const string& mcr, macros_defined)
	    ctx.add_macro_definition(mcr.c_str());

	// let wave support variadics (like #define FOO(...))
	ctx.set_language(boost::wave::enable_variadics(ctx.get_language()));
	ctx.set_language(boost::wave::enable_long_long(ctx.get_language()));


	// Get the preprocessor iterators and use them to generate 
	// the token sequence.
	context_type::iterator_type first = ctx.begin();
	context_type::iterator_type last = ctx.end();

	// The input stream is preprocessed for you during iteration
	// over [first, last)

	while (first != last) {
	    // get the file name which may be a header file
	    current_position = first->get_position();
	    string current_file = first->get_position().get_file().c_str();
	    if (isParent(target_path, current_file))
		headers_included.insert(current_file);
	    ++first;
	}

    }
    catch (boost::wave::cpp_exception const& e) {
	// some preprocessing error
	std::cerr 
	    << e.file_name() << "(" << e.line_no() << "): "
		<< e.description() << std::endl;
    }
    catch (std::exception const& e) {
	// use last recognized token to retrieve the error position
	std::cerr 
	    << current_position.get_file() 
	    << "(" << current_position.get_line() << "): "
		<< "exception caught: " << e.what()
		    << std::endl;
    }
    catch (...) {
	// use last recognized token to retrieve the error position
	std::cerr 
	    << current_position.get_file() 
	    << "(" << current_position.get_line() << "): "
		<< "unexpected exception caught." << std::endl;
    }

}
#endif

struct visitorTraversal : public AstSimpleProcessing
{
    std::set<path> filenames;
    path target_path;
    visitorTraversal(const path& target) : target_path(target) {}
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* n)
{
    SgStatement* statement = isSgStatement(n);
    if (statement != NULL)
    {
	string filename = statement->get_file_info()->get_filename();
	if(boost::filesystem::exists(filename))
	{
	    filename = realpath(filename.c_str(), NULL);
	    if (isParent(target_path, filename))
		filenames.insert(filename);
	}
    }
}

// given a set of headers, and a Cxx file, then find all headers that Cxx file includes,
// then find the differece of these two set of headers
vector<path> getAllHeadersNotIncluded(
	const set<path>& all_headers, 
	const set<path>& headers_included)
{
    vector<path> diff;
    set_difference(all_headers.begin(), all_headers.end(),
	    headers_included.begin(), headers_included.end(),
	    back_inserter(diff));
    return diff;
}

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    vector<path> include_paths;
    vector<string> macro_defined;
    path input_file;
    path target_path;

    po::options_description desc("Generic options");
    desc.add_options()
	("help", "produce help message")    
	("input-file", po::value<string>(), "file to be analyzed")
	("target-path,P", po::value<string>(), "target path including headers")
	("include-path,I", po::value< vector<string> >()->composing(), 
         "include path")
        ("macro-defined,D", po::value< vector<string> >()->composing(), 
         "macro defined")
        ;
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).
	    options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")) 
    {
	cout << desc;
	return 1;
    }

    if (vm.count("include-path"))
	foreach (string s, vm["include-path"].as< vector<string> >())
	    include_paths.push_back(s);
    if (vm.count("macro-defined"))
	foreach (string s, vm["macro-defined"].as< vector<string> >())
	    include_paths.push_back(s);

    if (vm.count("input-file"))
    {
	input_file = vm["input-file"].as<string>();
	if (!exists(input_file))
	{
	    cout << "The file \"" << input_file << "\" does not exist!" << endl;
	    return 1;
	}
    }
    else
    {
	cout << "Please input an input file." << endl;
	return 1;
    }

    if (vm.count("target-path"))
    {
	target_path = vm["target-path"].as<string>();
	if (!exists(target_path))
	{
	    cout << "The directory \"" << target_path << "\" does not exist!" << endl;
	    return 1;
	}
	// It is better not to include the target path 
	//include_paths.insert(include_paths.begin(), target_path);
    }
    else
    {
	cout << "Please input a target path." << endl;
	return 1;
    }

    // Collect all headers in the target path.
    set<path> headers;
    getAllHeaderFiles(target_path, headers);

    // Get all headers included in the input file.
    // first, generate the argument list for rose
    vector<string> args;
    args.push_back("");
    args.push_back(input_file.string());
    foreach (const path& p, include_paths)
	args.push_back("-I" + p.string());
    foreach (const string& s, macro_defined)
	args.push_back("-D" + s);

    // start ROSE project
    SgProject* project = frontend(args);
    // Build the traversal object
    visitorTraversal traversal(target_path);
    // Call the traversal starting at the project node of the AST
    traversal.traverse(project,preorder);

    set<path> headers_included = traversal.filenames;
    //getAllHeadersIncluded(input_file, target_path, headers_included, include_paths, macro_defined);
    //copy(headers_included.begin(), headers_included.end(), ostream_iterator<path>(cout, "\n"));

    vector<path> headers_not_included = getAllHeadersNotIncluded(headers, headers_included);

    // output the list of headers which are not included
    cout << "The headers which are not included in the input file are:\n";
    copy(headers_not_included.begin(), headers_not_included.end(), ostream_iterator<path>(cout, "\n"));
}


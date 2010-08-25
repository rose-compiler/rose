#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <map>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
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
    path p = p2;
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
void getAllHeaderFiles(const path& p, vector<path>& files)
{
    for (directory_iterator i(p); i != directory_iterator(); ++i)
    {
	if (is_directory(i->status()))
	    getAllHeaderFiles(i->path(), files);
	else 
	{
	    path real_path = realpath(i->path().string().c_str(), NULL);
	    if (isParent(p, real_path))
		files.push_back(real_path);
	}
    }
}


// compare two paths, not according to their string, but the depth
struct PathCompare 
: public std::binary_function<const path&, const path&, bool>
{
    bool operator()(const path& p1, const path& p2)
    {
	string s1 = p1.string(), s2 = p2.string();
	int n1 = count(s1.begin(), s1.end(), '/');
	int n2 = count(s2.begin(), s2.end(), '/');
	if (n1 < n2) return true;
	if (n1 > n2) return false;
	return p1 < p2;
    }
};

/* 
   void foo(const path& target_path, 
   map<path, set<path> >& header_map,
   const vector<path>& include_paths,
   const vector<string>& macros_defined)
   {
// first, collect all headers in the target path
vector<path> headers;
getAllHeaderFiles(target_path, headers);
// sort the headers to put deeper files back.
// this may improve the performance
sort(headers.begin(), headers.end(), PathCompare());

// for each header, collect all headers it includes recursively
foreach (const path& header, headers)
{
bool flag = false;
typedef map<path, set<path> >::value_type ValueType;
foreach (const ValueType& val, header_map)
{
if (val.second.count(header) > 0)
{
flag = true;
break;
}
}
if (flag) continue;

boost::wave::util::file_position_type current_position;
try
{
// The following preprocesses a given input file.
// Open the file and read it into a string variable
const char* filename = header.string().c_str();
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
std::set<string> files;

while (first != last) {
    // get the file name which may be a header file
    current_position = first->get_position();
    string current_file = first->get_position().get_file().c_str();
    files.insert(current_file);
    ++first;
}

// if the header file is in the target path, put it into header map
    foreach (const string& fl, files)
if (isParent(target_path, fl) && header != path(fl))
{
    header_map[header].insert(fl);
    // remove the header from the map if it is a key in the map,
    // since now we have another header which includes this one already
    if (header_map.count(fl) > 0)
	header_map.erase(fl);
}

//copy(header_map[header].begin(), header_map[header].end(), ostream_iterator<path>(cout, "\n"));
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
}
*/

void getLibMapByRegex(
	const path& target_path, 
	map<path, set<path> >& header_map,
	set<path>& non_roots,
	const vector<path>& include_paths
	//const vector<string>& macros_defined
	)
{
    // first, collect all headers in the target path
    vector<path> headers;
    getAllHeaderFiles(target_path, headers);

    // for each header, collect all headers it includes recursively
    foreach (const path& header, headers)
    {
	// use regex to detect all headers included in a header file
	static boost::regex re("^\\s*#\\s*include\\s*[<\"]\\s*([^>\"]+)\\s*([>\"])\\s*");
	boost::smatch what;

	string line;
	ifstream is(header.string().c_str());
	while(std::getline(is, line))
	{
	    bool result = boost::regex_search(line, what, re); 
	    if (result)
	    {
		path header_path;
		// now we just use target_path to find all headers under this directory
		if (what[2] == ">")
		{
		    foreach (const path& p, include_paths)
		    {
			header_path = p / path(what[1]);
			if (exists(header_path) && isParent(target_path, header_path))
			{
			    header_map[header].insert(header_path);
			    break;
			}
		    }
		}
		else if (what[2] == "\"")
		{
		    header_path = header.parent_path() / path(what[1]);
		    if (exists(header_path))
			header_map[header].insert(header_path);
		    else
		    foreach (const path& p, include_paths)
		    {
			header_path = p / path(what[1]);
			if (exists(header_path) && isParent(target_path, header_path))
			{
			    header_map[header].insert(header_path);
			    break;
			}
		    }

		}
	    }
	}
    }

    //cout << "Done\n" << header_map.size() << endl;

    typedef map<path, set<path> >::value_type ValueType;

    // connect the edge of the small graph to form a bigger graph
    //set<path> to_delete;
    foreach (const ValueType& val1, header_map)
    {
	foreach (const path& p, val1.second)
	{
	    if (header_map.count(p) > 0 && p != val1.first)
		non_roots.insert(p);
	}
    }

    /*
       foreach (const ValueType& val, header_map)
       {
       cout << val.first << endl; 
       }
       cout << header_map.size() << endl;
       */
}


// build a Cxx file which includes all header files provided
void buildCxxFile(const string& filename, const set<path>& headers)
{
    ofstream fstr(filename.c_str());
    BOOST_FOREACH(const path& header, headers)
	fstr << "#include<" << header << ">\n";
}

// build a Cxx file which includes the header indicated
void buildCxxFile(const string& filename, const path& header)
{
    ofstream fstr(filename.c_str());
    fstr << "#include<" << header << ">\n";
}

string getRelativePath(const path& p, const path& target_path)
{
    string s = p.string();
    string target = target_path.string();
    if (*(target.rbegin()) != '/')
	target.push_back('/');
    s.replace(s.begin(), s.begin() + target.size(), "");
    return s;
}
void generateDotFile(const map<path, set<path> > header_map, const set<path>& non_roots, const path& target)
{
    map<path, string> names;
    int counter = 0;

    string dot_file = "graph.dot";
    ofstream f(dot_file.c_str());
    f << "digraph G {" << endl;
    for (map<path, set<path> >::const_iterator i = header_map.begin();
	    i != header_map.end(); ++i)
    {
	if (names.count(i->first) == 0)
	    names[i->first] = "node" + lexical_cast<string>(counter++);
	f << names[i->first] << " [ label = \"" << getRelativePath(i->first, target) << "\"";
	if (non_roots.count(i->first) == 0)
	    f << ", style = filled";
	f << " ];\n";
	BOOST_FOREACH(const path& p, i->second)
	{
	    if (names.count(p) == 0)
		names[p] = "node" + lexical_cast<string>(counter++);
	    f << names[i->first] << " -> " << names[p] << ";\n";
	    f << names[p] << " [ label = \"" << getRelativePath(p, target) << "\" ];\n";
	}
    }
    f << "}";
    f.close();
    cout << "The DOT file \"" << dot_file << "\" has been generated successfully!" << endl;
}

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    vector<path> include_paths;
    path target;

    po::options_description desc("Generic options");
    desc.add_options()
	("help", "produce help message")    
	("include-path,I", po::value< vector<string> >()->composing(), 
         "include path")
        ("target-path", po::value<string>(), "target path including headers")
	;
    po::positional_options_description p;
    p.add("target-path", -1);

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

    if (vm.count("target-path"))
    {
	target = vm["target-path"].as<string>();
	if (!exists(target))
	{
	    cout << "The directory \"" << target << "\" does not exist!" << endl;
	    return 1;
	}
    }
    else
    {
	cout << "Please input a target path." << endl;
	return 1;
    }

    map<path, set<path> > header_map;
    set<path> non_roots;
    getLibMapByRegex(target, header_map, non_roots, include_paths);     
    generateDotFile(header_map, non_roots, argv[1]);
    return 0;
}


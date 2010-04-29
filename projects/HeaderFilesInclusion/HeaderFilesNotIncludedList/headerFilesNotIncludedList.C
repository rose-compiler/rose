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
#include <boost/wave.hpp>
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class

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

void getLibMapByRegex(
	const path& target_path, 
	map<path, set<path> >& header_map,
	const vector<path>& include_paths,
	const vector<string>& macros_defined)
{
    // first, collect all headers in the target path
    vector<path> headers;
    getAllHeaderFiles(target_path, headers);

    // for each header, collect all headers it includes recursively
    foreach (const path& header, headers)
    {
	// use regex to detect all headers included in a header file
	static boost::regex re("^\\s*#\\s*include\\s*[<\"]\\s*([^>\"]+)\\s*[>\"]\\s*");
	boost::smatch what;

	string line;
	ifstream is(header.string().c_str());
	while(std::getline(is, line))
	{
	    bool result = boost::regex_search(line, what, re); 
	    if (result)
	    {
		path header_path;
		foreach (const path& p, include_paths)
		{
		    header_path = p / path(what[1]);
		    if (exists(header_path))
			break;
		}
		if (!exists(header_path))
		{
		    //
		}
		if (isParent(target_path, header_path))
		    header_map[header].insert(header_path);
	    }
	}
    }

    cout << "Done\n" << header_map.size() << endl;

    typedef map<path, set<path> >::value_type ValueType;

    // connect the edge of the small graph to form a bigger graph
    set<path> to_delete;
    foreach (const ValueType& val1, header_map)
    {
	foreach (const path& p, val1.second)
	{
	    if (header_map.count(p) > 0 && p != val1.first)
		to_delete.insert(p);
	    //header_map.erase(p);
	}

	/* 
	   foreach (ValueType& val2, header_map)
	   {
	   if (val1.first == val2.first)
	   continue;
	//if (val2.second.count(val1.first) > 0)
	{

	//foreach (const path& p, val1.second)
	//  val2.second.insert(p);
	//to_delete.insert(val1.first);
	header_map.erase(val1.first);
	}
	}
	*/
    }
    //foreach (const path& p, to_delete)
    //	header_map.erase(p);


    foreach (const ValueType& val, header_map)
    {
	cout << val.first << endl; 
    }
    cout << header_map.size() << endl;
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
void generateDotFile(const map<path, set<path> > header_map, const path& target)
{
    map<path, string> names;
    int counter = 0;

    ofstream f("graph.dot");
    f << "digraph G {" << endl;
    for (map<path, set<path> >::const_iterator i = header_map.begin();
	    i != header_map.end(); ++i)
    {
	if (names.count(i->first) == 0)
	    names[i->first] = "node" + lexical_cast<string>(counter++);
	f << names[i->first] << " [ label = \"" << getRelativePath(i->first, target) << "\" ];\n";
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
}

int main(int argn, char** argv)
{
    path pp = "/home/hou1";
    for(int i = 0; i < 5; ++i)
    {
	pp = pp.parent_path();
	cout << pp << endl;
	if (!pp.has_parent_path()) break;
    }
    if (argn < 3) return 1;

    // read all include paths and macros from the compiler
    vector<path> include_paths;
    vector<string> macros_defined;

    ifstream is(argv[2]);
    string line;
    while(std::getline(is, line))
	include_paths.push_back(line);
    is.close();
    is.open(argv[3]);
    while(std::getline(is, line))
	macros_defined.push_back(line);
    is.close();

    map<path, set<path> > header_map;
    //foo(argv[1], header_map, argv[2], argv[3]);
    getLibMapByRegex(argv[1], header_map, include_paths, macros_defined);
    generateDotFile(header_map, argv[1]);
    return 0;

#if 0
    path target_path = "/usr/include/c++/4.1.1";
    string filename = "/usr/include/c++/4.1.1/vector";

    ifstream is(filename.c_str());

    boost::regex re("^\\s*#\\s*include\\s*[<\"]\\s*([^>\"]+)\\s*[>\"]\\s*$");
    boost::smatch what;

    vector<string> headers;
    string line;
    while(std::getline(is, line))
    {
	bool result = boost::regex_search(line, what, re); 
	if (result)
	    headers.push_back(what[1]);
    }
    copy(headers.begin(), headers.end(), ostream_iterator<string>(cout, "\n"));
#endif
}


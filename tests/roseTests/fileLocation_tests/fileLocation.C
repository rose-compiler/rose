// This code test the classification of files.
// Files are classified at either unknown, user, 
// or system (system includes system libaries).

// The support for this work is in ROSE/src/util/stringSupport/FileNameClassifier.C


#include "rose.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <set>

#include <libgen.h>             /* basename(), dirame()               */

// CH (1/29/2010): Needed for boost::filesystem::exists(...)
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace StringUtility;

    bool
isLink( const string & name )
{
    // First, check if this file exists
    if(!boost::filesystem::exists(name))
    {
	printf("The file \"%s\" does not exist!\n", name.c_str());
	return false;
    }


    // In oorder to evaluate if this is a link we can't just check the file directly, 
    // since the file might be part of a directory that is linked.  So we have to
    // have to check each part of the whole absolute path to see if a link was used.
    // This detail make this function more complicated.

    struct stat info;

    string fileNameWithPath = name;
    char c_version[PATH_MAX];
    ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);

    // DQ (1/29/2010): Since this is a problem, escape the loop when we iterate too long!
    // A better fix is required, but this tests if this is the essential problem.
    int count = 0;
    // while (fileNameWithPath != "/")
    while (fileNameWithPath != "/" && fileNameWithPath != ".")
    {
	strcpy(c_version, fileNameWithPath.c_str());

	// string directoryName = dirname(name.c_str());
	string directoryName = dirname(c_version);

	//printf ("directoryName = %s \n",directoryName.c_str());

	fileNameWithPath = directoryName;

	// DQ (1/29/2010): Since this is a problem, escape the loop when we iterate too long!
	// A better fix is required, but this tests if this is the essential propblem.
	count++;
	if (count > 1000)
	{
	    printf ("ERROR: loop count in isLink exceeds %d \n",count);
	    break;
	}
    }


#if 0
    char fn[]="temp.file";
    char ln[]="temp.link";
    int file_descriptor;

    if ((file_descriptor = creat(fn, S_IWUSR)) < 0)
	perror("creat() error");
    else
    {
	close(file_descriptor);
	if (link(fn, ln) != 0)
	    perror("link() error");
	else
	{
	    if (lstat(ln, &info) != 0)
		perror("lstat() error");
	    else
	    {
		puts("lstat() returned:");
		printf("  inode:   %d\n",   (int) info.st_ino);
		printf(" dev id:   %d\n",   (int) info.st_dev);
		printf("   mode:   %08x\n",       info.st_mode);
		printf("  links:   %d\n",         info.st_nlink);
		printf("    uid:   %d\n",   (int) info.st_uid);
		printf("    gid:   %d\n",   (int) info.st_gid);
	    }
	    unlink(ln);
	}
	unlink(fn);
    }
#else

#if 0
    // DQ (1/30/2010): Skip the display of output (too much for testing).
    printf ("testing for link: name = %s \n",name.c_str());
#endif

    if (lstat(name.c_str(), &info) != 0)
    {
	// perror("lstat() error");
	printf("lstat() error for name = %s \n",name.c_str());
    }
    else
    {
#if 0
	// DQ (1/30/2010): Skip the display of output (too much for testing).
	puts("lstat() returned:");
	printf("  inode:   %d\n",   (int) info.st_ino);
	printf(" dev id:   %d\n",   (int) info.st_dev);
	printf("   mode:   %08x\n",       info.st_mode);
	printf("  links:   %d\n",         info.st_nlink);
	printf("    uid:   %d\n",   (int) info.st_uid);
	printf("    gid:   %d\n",   (int) info.st_gid);

	printf ("S_ISLNK(info.mode) = %s \n",S_ISLNK(info.st_mode) ? "true" : "false");
#endif
    }
#endif

    // The minimum is to have a single link for a normal file.
    // bool isALink = info.st_nlink > 1;

    // DQ (1/30/2010): Ignore info.st_nlink, the only reliable way to check this is to use the PPOSIX macro (S_ISLNK).
    bool isALink = S_ISLNK(info.st_mode);

    return isALink;
}


    bool
islinkOrPartOfLinkedDirectory( const string & fileName )
{
    // In oorder to evaluate if this is a link we can't just check the file directly, 
    // since the file might be part of a directory that is linked.  So we have to
    // have to check each part of the whole absolute path to see if a link was used.
    // This detail make this function more complicated.

    // struct stat info;

    string fileNameWithPath = fileName;
    char c_version[PATH_MAX];
    ROSE_ASSERT (fileNameWithPath.size() + 1 < PATH_MAX);

    bool directoryIsLink = false;
    bool fileIsLink = false;

    while (fileNameWithPath != "/" && directoryIsLink == false)
    {
	strcpy(c_version, fileNameWithPath.c_str());

	// I think that the interface to dirname() requires a char array.
	// string directoryName = dirname(name.c_str());
	string directoryName = dirname(c_version);

#if 0
	// DQ (1/30/2010): Skip the display of output (too much for testing).
	printf ("directoryName = %s \n",directoryName.c_str());
#endif

	fileNameWithPath = directoryName;

	directoryIsLink = isLink(directoryName);
    }

#if 0
    if (directoryIsLink == true)
    {
	printf ("This file is classified as a link because it is in a linked directory \n");
    }
#endif

    if (directoryIsLink == false)
    {
	fileIsLink = isLink(fileName);
    }
    else
    {
	// If the directory is a link then the file is indirectly a linked file (for our purposes in filename classification)
	fileIsLink = true;
    }

#if 0
    if (fileIsLink == true)
    {
	printf ("This file is classified as a link \n");
    }
#endif

    return fileIsLink;
}



class visitorTraversal : public AstSimpleProcessing
{
    map<string, string> libs;
    string app_path;
    string previousFilename;

    // CH (2/1/2010): The better way to avoid repeated filenames is to build a filanames set
    std::set<string> previousFilenames;

    public:
    void setLibs(const map<string, string> & lb) { libs = lb; }
    void setAppPath(const string& app) { app_path = app; }
    virtual void visit(SgNode* n);
};

#if 0
enum FileNameLocation { FILENAME_LOCATION_UNKNOWN, 
    FILENAME_LOCATION_USER,    
    FILENAME_LOCATION_LIBRARY };

/* Files can be classified as being part of one of these
 * libraries: Unknown, it isn't a library - it's part of
 * the user application, or any of the libraries that the
 * enum values imply, this list will likely be added to
 * over time */
enum FileNameLibrary { FILENAME_LIBRARY_UNKNOWN,
    FILENAME_LIBRARY_USER,
    FILENAME_LIBRARY_C,
    FILENAME_LIBRARY_STDCXX,
    FILENAME_LIBRARY_LINUX,
    FILENAME_LIBRARY_GCC,
    FILENAME_LIBRARY_BOOST,
    FILENAME_LIBRARY_ROSE };
#endif

void
display ( const StringUtility::FileNameLocation & X, const string & label = "" )
{
    printf ("In display(FileNameLocation): label = %s \n",label.c_str());
    string classification = "";
    switch (X)
    {
	case FILENAME_LOCATION_UNKNOWN: classification = "unknown"; break;
	case FILENAME_LOCATION_USER: classification    = "user";    break;
	case FILENAME_LOCATION_LIBRARY: classification = "library"; break;
	case FILENAME_LOCATION_NOT_EXIST: classification = "not exist"; break;

	default:
					  {
					      printf ("Error: undefined classification X = %d \n",X);
					      ROSE_ASSERT(false);
					  }
    }

    printf ("file type classification = %s \n",classification.c_str());

    // return classification;
}

void
display ( const StringUtility::FileNameLibrary & X, const string & label = "" )
{
    // Since FileNameLibrary is changed to string type, just print it out.
    /* 
       printf ("In display(FileNameLibrary): label = %s \n",label.c_str());
       string classification = "";
       switch (X)
       {
       case FILENAME_LIBRARY_UNKNOWN: classification = "unknown";        break;
       case FILENAME_LIBRARY_USER:    classification = "user";           break;
       case FILENAME_LIBRARY_C:       classification = "library C";      break;
       case FILENAME_LIBRARY_STDCXX:  classification = "library STDCXX"; break;
       case FILENAME_LIBRARY_STL:     classification = "library STL";    break;
       case FILENAME_LIBRARY_LINUX:   classification = "library LINUX";  break;
       case FILENAME_LIBRARY_GCC:     classification = "library GCC";    break;
       case FILENAME_LIBRARY_BOOST:   classification = "library BOOST";  break;
       case FILENAME_LIBRARY_ROSE:    classification = "library ROSE";   break;

       default:
       {
       printf ("Error: undefined library classification X = %d \n",X);
       ROSE_ASSERT(false);
       }
       }

       printf ("library classification = %s \n",classification.c_str());
       */
    printf("library classification = %s \n", X.c_str());

    // return classification;
}



    void 
visitorTraversal::visit(SgNode* n)
{
    SgStatement* statement = isSgStatement(n);
    if (statement != NULL)
    {
	string filename = statement->get_file_info()->get_filename();

	// CH (2/1/2010): Get the real filename (not a symlink)
	if(boost::filesystem::exists(filename))
	    filename = canonicalize_file_name(filename.c_str());

	// Skip the case of compiler generated Sg_File_Info objects.
	//if (previousFilename != filename && filename != "compilerGenerated")
	if (previousFilenames.count(filename) == 0 && filename != "compilerGenerated")
	{
#if 0
	    // DQ (1/30/2010): Skip the display of output (too much for testing).
	    printf ("\n\nfilename = %s statement = %s \n",filename.c_str(),n->class_name().c_str());
#endif

	    FileNameClassification classification;
#if 1
	    // string sourceDir = "/home/dquinlan/ROSE/roseCompileTree-g++4.2.2/developersScratchSpace/Dan/fileLocation_tests";

	    // This causes the path edit distance to be: 4
	    //string sourceDir = "/home/dquinlan/ROSE/svn-rose";
	    string sourceDir = "/home/hou1/opt/rose";
	    //map<string, string> libs;
	    //libs["/home/hou1/opt/rose"] = "MyRose";
	    //libs["/home/hou1/opt/boost"] = "MyBoost";
	    // This causes the path edit distance to be: 0
	    //string sourceDir = "/home/dquinlan/ROSE";

	    classification = classifyFileName(filename, app_path, libs);
#else
	    string home = "/home/dquinlan/";
	    string sourceDir = home + "ROSE/svn-rose/";
	    classification = classifyFileName("/usr/include/stdio.h",sourceDir);
#endif

	    FileNameLocation fileTypeClassification = classification.getLocation();
	    FileNameLibrary  libraryClassification  = classification.getLibrary();
	    int pathEditDistance = classification.getDistanceFromSourceDirectory();

#if 1
	    // DQ (1/30/2010): Skip the display of output (too much for testing).
	    printf ("\n\nfilename: %s\n", filename.c_str());
	    printf ("fileTypeClassification = %d \n",fileTypeClassification);
	    display(fileTypeClassification,"Display fileTypeClassification");
	    printf ("libraryClassification  = %s \n",libraryClassification.c_str());
	    //display(libraryClassification,"Display libraryClassification");
	    printf ("pathEditDistance       = %d \n",pathEditDistance);
#endif

#if 1
	    // DQ (1/30/2010): Skip the display of output (too much for testing).

	    // Some of our tests explicitly build a link and this tests that it is correctly identified as a link.
	    // printf ("isLink(StringUtility::stripPathFromFileName(filename)) = %s \n",isLink(StringUtility::stripPathFromFileName(filename)) ? "true" : "false");

	    // ROSE_ASSERT(isLink(filename) == false);
	    bool lk = isLink(filename);
	    printf ("isLink(filename) = %s \n",lk ? "true" : "false");

	    // DQ (1/30/2010): Added this test.
	    // ROSE_ASSERT(islinkOrPartOfLinkedDirectory(filename) == false);
	    //  printf ("islinkOrPartOfLinkedDirectory(filename) = %s \n",islinkOrPartOfLinkedDirectory(filename) ? "true" : "false");
#endif
	    previousFilenames.insert(filename);
	}

	//previousFilename = filename;
    }
}

void DeleteArg(vector<string>& args, const string& arg)
{
    vector<string>::iterator it = find(args.begin(), args.end(), arg);
    args.erase(it+1);
    args.erase(it);
}

int main(int argc, char * argv[])
{
    vector<string> args(argv, argv+argc);

    using namespace boost::program_options;
    // Declare the supported options.
    options_description desc("fileLocation options");
    desc.add_options()
	("help", "produce help message")
	("app-path", value<string>(), "set user's application path")
	("lib-path-name", value< vector<string> >()->composing(),
	     "set library path and name. The arg format is [libPath]=[libName], for example: --lib-path-name /usr=User")
	("lib-file", value<string>(), "set filename which contains library paths and names")
	;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);    

    map<string, string> libs;
    string app_path;

    if (vm.count("help")) {
	cout << desc << "\n";
    }

    if (vm.count("app-path")) {
	app_path = vm["app-path"].as<string>();
	cout << "application path: " << app_path << '\n';

	DeleteArg(args, "--app-path");
    }

    if (vm.count("lib-file")) {
	string filename = vm["lib-file"].as<string>();
	if (!boost::filesystem::exists(filename)) {
	    cout << "the file \"" << filename << "\" does not exist!\n";
	    return 1;
	}
	ifstream ifs(filename.c_str());
	string path, name;
	while (ifs >> path >> name) {
	    libs[path] = name;
	}

	DeleteArg(args, "--lib-file");
    }

    if (vm.count("lib-path-name")) {
	vector<string> paths = vm["lib-path-name"].as< vector<string> >();
	for (int i = 0; i < 1; ++i) {
	    vector<string> path_name;
	    boost::split(path_name, paths[i], boost::is_any_of("="));
	    ROSE_ASSERT(path_name.size() == 2);
	    libs[path_name[0]] = path_name[1];
	}
	
	DeleteArg(args, "--lib-path-name");
    }

    pair<string, string> sp;
    BOOST_FOREACH(sp, libs)
    {
	cout << sp.first << ' ' << sp.second << endl;
    }

    SgProject *project = frontend (args);

    visitorTraversal myvisitor;
    myvisitor.setLibs(libs);
    myvisitor.setAppPath(app_path);
    myvisitor.traverse(project,preorder);

    return backend(project);
}



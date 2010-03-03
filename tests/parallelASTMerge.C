#include <vector>
#include <string>
#include <iostream>
#include <boost/thread.hpp>
#include <fstream>
#include <algorithm>
#include <climits>
#include <queue>
#include <boost/lexical_cast.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
//#include <rose.h>

using namespace std;
using namespace boost;


class XTimer {
    public:
	XTimer() {
	    xtime_get(&_start_time, boost::TIME_UTC);
	}

	XTimer(const XTimer& other)
	    : _start_time(other._start_time)
	{
	}

	~XTimer() {
	}

	double elapsed() const {
	    boost::xtime now;
	    xtime_get(&now, boost::TIME_UTC);
	    return boost::lexical_cast<double>(now.sec - _start_time.sec) + 
		boost::lexical_cast<double>(now.nsec - _start_time.nsec) / 1000000000;
	}

    private:
	boost::xtime _start_time;
};

struct FileSizeCompare
{
    bool operator()(const string& lf, const string& rf) const
    {
	using namespace boost::filesystem;
	string f1 = lf + ".binary";
	string f2 = rf + ".binary";
	if (!exists(f1) || !exists(f2))
	    return false;
	return file_size(f1) > file_size(f2);
    }
};

class AstStorage
{
    //vector<string> astFiles_;
    priority_queue<string, vector<string>, FileSizeCompare> astFiles_;
    int counter_;
    string dir_;

    boost::mutex mutex_;

    public:
    AstStorage(const vector<string>& filenames)
	: counter_(0) 
    {
	for (int i = 0; i < filenames.size(); ++i)
	    astFiles_.push(filenames[i]);
    }

    vector<string> GetFiles(int n)
    {
	boost::mutex::scoped_lock scoped_lock(mutex_);

	if (astFiles_.size() < 2) return vector<string>();

	vector<string> files;
	for (int i = 0; i < n; ++i)
	{
	    if (!astFiles_.empty())
	    {
		string filename = astFiles_.top();
		astFiles_.pop();
		files.push_back(filename);
	    }
	}
	files.push_back(GetOutputFileName());
	return files;
    }

    void PushFile(string filename)
    {
	boost::mutex::scoped_lock scoped_lock(mutex_);
	astFiles_.push(filename);
    }

    void GenerateOutput(const string& output) 
    {
	using namespace boost::filesystem;
	if (exists(output))
	    remove(output);
	copy_file(astFiles_.top() + ".binary", output);
	Clear();
    }

    void Clear()
    {
	boost::filesystem::remove_all(dir_);
    }

    private:    
    string GetOutputFileName()
    {
	if (counter_ == 0)
	{
	    dir_ = "tmp";
	    if (boost::filesystem::exists(dir_))
	    {
		for (int i = 0; i < INT_MAX; ++i)
		{
		    dir_ = "tmp" + lexical_cast<string>(i);
		    if (!boost::filesystem::exists(dir_))
			break;
		}
	    }
	    boost::filesystem::create_directory(dir_);
	}

	return dir_ + "/" + lexical_cast<string>(counter_++) + ".C";
    }
};

void MergeAst(AstStorage* storage, int n = 2)
{
    // note that the last file is the output
    vector<string> filenames = storage->GetFiles(n);
    while (!filenames.empty())
    {

	int nfile = filenames.size();

	// string arg = "./mergeAST ";
	string arg = "../../../tests/testAstFileRead ";

	for (int i = 0; i < filenames.size(); ++i)
	{
	    arg += filenames[i] + " ";
	}

	printf (">>>>>>>>>>>>>>>>>>>>commandline to run process = %s \n",arg.c_str());
	if (::system(arg.c_str()) != 0)
	{
	    //storage->Clear();
	    throw std::exception(); //string("Calling AST Merge Application Failed at\n") + arg);
	}

	string output = filenames.back();
	storage->PushFile(output);
	filenames = storage->GetFiles(n);
    }
}

void MergeAst1(AstStorage* storage, int n = 2)
{
    // note that the last file is the output
    vector<string> filenames = storage->GetFiles(n);
    if (!filenames.empty())
    {
	string arg = "../../../tests/testAstFileRead ";

	for (int i = 0; i < filenames.size(); ++i)
	{
	    arg += filenames[i] + " ";
	}

	printf (">>>>>>>>>>>>>>>>>>>>commandline to run process = %s \n",arg.c_str());
	if (::system(arg.c_str()) != 0)
	{
	    //storage->Clear();
	    //throw std::exception(); //string("Calling AST Merge Application Failed at\n") + arg);
	}

	storage->PushFile(filenames.back());
    }
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
}

void MergeAstFiles(const vector<string>& astFiles, const string& output, int nthread)
{
    AstStorage storage(astFiles);

    int n = astFiles.size() / nthread;
    if (n < 2) n = 2; 
    vector<int> nfile(nthread, n);
//  for (int i = 0; i < astFiles.size() - n * nthread; ++i)
//       nfile[i]++;

    boost::thread_group thrds;

	for (int i = 0; i < nthread; ++i)
	    thrds.create_thread(boost::bind(MergeAst, &storage, 2/*nfile[i]*/));

    thrds.join_all();
    
    MergeAst(&storage, nthread);

    cout << "Generating output ...\n";
    storage.GenerateOutput(output);
    cout << "Done!\n";
}

int main ( int argc, char * argv[] )
{
    assert(argc >= 0);
    if (argc == 0)
    {
	printf ("Error: This AST file reader requires the name of a binary AST file. \n");
	assert(false);
    }

    int numFiles = argc - 1;
    std::vector<std::string> fileNames;
    for (int i= 0; i < numFiles; ++i)
    {
	fileNames.push_back(argv[i+1]) ;
    }

    string output = "output.txt";

    ofstream ifs("timer.txt");
    for (int i = 5; i < 6; i += 1)
    {
	XTimer t;
	string output = "output" + lexical_cast<string>(i);
	output = "output" + lexical_cast<string>(i) + ".txt";
	MergeAstFiles(fileNames, output, i);
	ifs << i << " : " << t.elapsed() << endl;
    }
}



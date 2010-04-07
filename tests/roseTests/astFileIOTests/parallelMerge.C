#include <vector>
#include <string>
#include <iostream>
#include <boost/thread.hpp>
#include <fstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
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


typedef vector<int> AstType;

AstType* ReadAstFromFile(const string& filename)
{
    AstType* ast = new AstType;
    ifstream ifs(filename.c_str());
    int n;
    while (ifs >> n)
    { 
	ast->push_back(n);
    } 
    return ast;
}

void WriteAstToFile(AstType* ast, const string& filename)
{
    ofstream of(filename.c_str());
    for (int i = 0; i < ast->size(); ++i)
    {
	of << (*ast)[i] << " ";
    }
}

AstType* Merge(AstType* ast1, AstType* ast2)
{
    AstType* ast = new AstType(ast1->size());

    sort(ast1->begin(), ast1->end());
    sort(ast2->begin(), ast2->end());
    transform(ast1->begin(), ast1->end(), ast2->begin(), ast->begin(), plus<int>());
    random_shuffle(ast->begin(), ast->end());

    delete ast1;
    delete ast2;
    return ast;
}


class AstStorage
{
    vector<string> astFiles_;
    AstType* astDone_;

    boost::mutex mutex_;
public:
    AstStorage(const vector<string>& filenames)
	: astFiles_(filenames), astDone_(NULL) {}

    AstType* GetAst(AstType* ast = NULL) 
    {
	boost::mutex::scoped_lock scoped_lock(mutex_);

	if (!astFiles_.empty())
	{
	    string filename = astFiles_.back();
	    astFiles_.pop_back();
	    //	    scoped_lock.unlock();
	    AstType* ast = ReadAstFromFile(filename);
	    return ast;
	}
	else if (ast != NULL)
	{
	    static int counter = 0;
	    string filename = "temp/" + lexical_cast<string>(counter++);
	    WriteAstToFile(ast, filename);
	    astFiles_.push_back(filename);
	    return NULL;
	}

	return NULL;
    }

    AstType* ReturnAst(AstType* ast)
    {
	boost::mutex::scoped_lock scoped_lock(mutex_);
	if (astDone_ == NULL)
	{
	    astDone_ = ast;
	    return NULL;
	}
	else
	{
	    ast = astDone_;
	    astDone_ = NULL;
	    return ast;
	}
    }
};

void ProcessAst(AstType* ast)
{
    for ( int i = 0; i < ast->size(); ++i)
    {
	float p = ast->at(i);
	for (int j = 0; j < 200; ++j)
	{
	    p = p * p / (p+1);
	}
	(*ast)[i] = p;
    }
}
void MergeAst(AstStorage* storage)
{
    AstType* ast1 = storage->GetAst();
    if (ast1 == NULL) return;
    AstType* ast2;
    while (ast2 = storage->GetAst(ast1))
    {
	ast1 = Merge(ast1, ast2);
    }
    /*  
    while (ast2 = storage->ReturnAst(ast1))
    {
	ast1 = Merge(ast1, ast2);
    }
    */
}

int num_threads = 1;

void MergeAstFiles(const vector<string>& astFiles, const string& output)
{
    AstStorage storage(astFiles);

    boost::thread_group thrds;
    for (int i = 0; i < num_threads; ++i)
	thrds.create_thread(boost::bind(MergeAst, &storage));

    thrds.join_all();

    WriteAstToFile(storage.GetAst(), output);
}

const int DATA_SIZE = 3000;
vector<string> GenerateFiles()
{
    vector<string> filenames;
    string prefix = "data/";
    for (int i = 0; i < 1000; ++i)
    {
	string filename = prefix + lexical_cast<string>(i) + ".txt";
#if 1
// This code generated the data files used in the parallel merge tests
	ofstream of(filename.c_str());
	for (int j = 0; j < DATA_SIZE; ++j)
	{
	    of << rand() << " ";
	}
	of.close();
#endif
	filenames.push_back(filename);
    }
    return filenames;
}

int main()
{
    vector<string> filenames = GenerateFiles();
    string output = "output.txt";

    for (int i = 0; i < 10; ++i)
    {
XTimer t;
	num_threads = i + 1;
	output = "output" + lexical_cast<string>(i) + ".txt";
	MergeAstFiles(filenames, output);
	cout << t.elapsed() << endl;
    }
}


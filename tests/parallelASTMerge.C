// DQ (3/4/2010): Include this so that we can use the CXX_IS_ROSE_AST_FILE_IO macro
#include "rose_config.h"

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
// #include <rose.h>

using namespace std;

using namespace boost;


class XTimer {
    public:
        XTimer() 
        {
            xtime_get(&_start_time, boost::TIME_UTC);
        }

        XTimer(const XTimer& other)
            : _start_time(other._start_time)
        {
        }

        double elapsed() const 
        {
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

    // Fetch several files to merge. The number of files is passed in.
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

    // Push a new file into file pool.
    void PushFile(string filename)
    {
        boost::mutex::scoped_lock scoped_lock(mutex_);
        astFiles_.push(filename);
    }

    void GenerateOutput(const string& output) 
    {
        // DQ (3/4/2010): This source file does not appear to compile with the AST File I/O (which is kind of ironic).
#ifndef CXX_IS_ROSE_AST_FILE_IO
        using namespace boost::filesystem;
        if (exists(output))
            remove(output);
        copy_file(astFiles_.top() + ".binary", output);
        Clear();
#endif
    }

    // Remove the temporary directory.
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
    // Note that the last file is the output.
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

        // The last file is output.
        string output = filenames.back();
        storage->PushFile(output);
        // Retrieve files for the next mission.
        filenames = storage->GetFiles(n);
    }
}

#if 0
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
#endif

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
    std::vector<std::string> fileNames(argv + 1, argv + argc - 1);
#if 0
    for (int i= 0; i < numFiles; ++i)
    {
        fileNames.push_back(argv[i+1]);
    }
#endif

    //string output = "output.txt";
    string output = argv[argc - 1];

    //ofstream ifs("timer.txt");
    for (int i = 1; i < 6; i += 1)
    {
        XTimer t;
        string output = "output" + lexical_cast<string>(i);
        output = "output" + lexical_cast<string>(i) + ".txt";
        MergeAstFiles(fileNames, output, i);
        //ifs << i << " : " << t.elapsed() << endl;
        cout << i << " : " << t.elapsed() << endl;
    }

    return 0;
}


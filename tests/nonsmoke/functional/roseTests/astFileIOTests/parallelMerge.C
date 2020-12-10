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

#ifndef TEST_AST_FILE_READ
# error "absolute-path-for $ROSE_BLD/tests/nonsmoke/functional/testAstFileRead must be set on the g++ command-line"
#endif

using namespace std;
using namespace boost;

// A timer class which can time a thread correctly.
class XTimer {
    public:
        XTimer() 
        {
#if BOOST_VERSION >= 10500
            xtime_get(&_start_time, boost::TIME_UTC_);
#else
            xtime_get(&_start_time, boost::TIME_UTC);
#endif
        }

        XTimer(const XTimer& other)
            : _start_time(other._start_time)
        {
        }

        double elapsed() const 
        {
            boost::xtime now;
#if BOOST_VERSION >= 10500
            xtime_get(&now, boost::TIME_UTC_);
#else
            xtime_get(&now, boost::TIME_UTC);
#endif
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
        string f1 = lf + ".binary";
        string f2 = rf + ".binary";
        if (!filesystem::exists(f1) || !filesystem::exists(f2))
            return false;
        return filesystem::file_size(f1) > filesystem::file_size(f2);
    }
};

class AstStorage
{
    vector<string> astFiles_;
    //priority_queue<string, vector<string>, FileSizeCompare> astFiles_;
    int counter_;
    string dir_;

    boost::mutex mutex_;

    public:
    AstStorage(const vector<string>& filenames)
        : counter_(0) 
    {
        for (size_t i = 0; i < filenames.size(); ++i)
            //astFiles_.push(filenames[i]);
            astFiles_.push_back(filenames[i]);
    }

    // Fetch several files to merge. The number of files is passed in.
    vector<string> GetFiles(size_t n)
    {
        boost::mutex::scoped_lock scoped_lock(mutex_);

        sort(astFiles_.begin(), astFiles_.end(), FileSizeCompare());

        if (astFiles_.size() < 2) return vector<string>();
        if (n > astFiles_.size()) n = astFiles_.size();

        vector<string> files;
        int step = astFiles_.size() / n;
        for (size_t i = 0; i < n; ++i)
        {
            if (!astFiles_.empty())
            {
#if 0
                string filename = astFiles_.top();
                astFiles_.pop();
                files.push_back(filename);
#endif
                size_t idx = i * step - i;
                assert(idx < astFiles_.size());
                files.push_back(astFiles_[idx]);
                astFiles_.erase(astFiles_.begin() + idx);
            }
        }
        files.push_back(GetOutputFileName());
        return files;
    }

    // Push a new file into file pool.
    void PushFile(string filename)
    {
        boost::mutex::scoped_lock scoped_lock(mutex_);
        //astFiles_.push(filename);
        astFiles_.push_back(filename);
    }

    void GenerateOutput(const string& output) 
    {
        // DQ (3/4/2010): This source file does not appear to compile with the AST File I/O (which is kind of ironic).
#ifndef CXX_IS_ROSE_AST_FILE_IO
        if (filesystem::exists(output))
            filesystem::remove(output);
        //copy_file(astFiles_.top() + ".binary", output);
        //filesystem::rename(astFiles_.top() + ".binary", output);
        filesystem::rename(astFiles_.back() + ".binary", output);
        Clear();
#endif
    }

    // Remove the temporary directory.
    void Clear()
    {
        if (filesystem::exists(dir_))
            filesystem::remove_all(dir_);
    }

    private:    
    // Generate a unique file name.
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
        //int nfile = filenames.size();

        // string arg = "./mergeAST ";
        string args = TEST_AST_FILE_READ " ";

        for (size_t i = 0; i < filenames.size(); ++i)
        {
            args += filenames[i] + " ";
        }

        //printf (">>>>>>>>>>>>>>>>>>>>commandline to run process = %s \n",args.c_str());
        if (::system(args.c_str()) != 0)
        {
            //storage->Clear();
            throw std::exception(); //string("Calling AST Merge Application Failed at\n") + args);
        }

        // The last file is output.
        string output = filenames.back();
        storage->PushFile(output);
        // Retrieve files for the next mission.
        filenames = storage->GetFiles(n);
    }
}

void MergeAstFiles(const vector<string>& astFiles, const string& output, int nthread)
{
    AstStorage storage(astFiles);

    int file_num = astFiles.size();
    // n is the number of files each thread will process.
    int n = file_num / nthread;
    if (n < 2)
    { 
        n = 2; 
        nthread = file_num / 2;
    }
    vector<int> nfile(nthread, n);
    for (int i = 0; i < file_num - n * nthread; ++i)
        nfile[i]++;

    boost::thread_group thrds;

    for (int i = 0; i < nthread; ++i)
        thrds.create_thread(boost::bind(MergeAst, &storage, nfile[i]));

    thrds.join_all();

    MergeAst(&storage, nthread);

    //cout << "Generating output ...\n";
    storage.GenerateOutput(output);
    //cout << "Done!\n";
}

int main ( int argc, char * argv[] )
{
    assert(argc >= 0);
    if (argc == 0)
    {
        printf ("Error: This AST file reader requires the name of a binary AST file. \n");
        assert(false);
    }

    //int numFiles = argc - 1;
    std::vector<std::string> fileNames(argv + 1, argv + argc - 1);

    //string output = "output.txt";
    string output = argv[argc - 1];

    vector<pair<int, double> > time_used;
    //ofstream ifs("timer.txt");
    int min_thread_num = 2;
    int max_thread_num = 10;
    for (int i = min_thread_num; i <= max_thread_num; i += 1)
    {
        XTimer t;
        //string output = "output" + lexical_cast<string>(i);
        //output = "output" + lexical_cast<string>(i) + ".txt";
        MergeAstFiles(fileNames, output, i);
        //ifs << i << " : " << t.elapsed() << endl;
        time_used.push_back(make_pair(i, t.elapsed()));
        //cout << i << " : " << t.elapsed() << endl;
    }

    cout << "Thread Num\tTime Used" << endl; 
    for (size_t i = 0; i < time_used.size(); ++i)
        cout << time_used[i].first << "\t" << time_used[i].second << endl;

    return 0;
}


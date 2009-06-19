/**
 * TestCase generator for CallGraphAnalysis - resolution of virtual function calls
 */

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cassert>


#include <boost/filesystem.hpp>


using namespace std;
using namespace boost::filesystem;

/**
 * Table which describes which class implements which function
 * Example:
 *      f1   f2   f3
 * c1   x    x    x
 * c2        x
 * c3   x
 * c4   x          x
 *
 * This table means: c1 implements functions f1,f2,f3
 *                   c2 implements f2 (virtual overwrite) ...
 * First row is always completely filled
 */
vector<vector<bool> > table;

/// maps a class id to a class-name
vector<string> classNames;

/// maps a function id to a function-name
vector<string> functionNames;



/// used for #ifndef argument - convertes everything to uppercase and replaces '.' by '_'
string stringToUpper(const string  & strToConvert)
{
   string res= strToConvert;
   for(unsigned int i=0;i<strToConvert.length(); i++)
   {
      res[i] = toupper(strToConvert[i]);

      if(strToConvert[i]=='.')
          res[i]='_';
   }

   return res;
}

void fillFunctionNames(size_t funcCount)
{
    assert(funcCount > 0);
    functionNames.reserve(funcCount);

    for(size_t i=0; i<funcCount; i++)
    {
        stringstream s;
        s << "func" << i;
        functionNames.push_back(s.str());
    }
}


void fillClassNames(size_t classCount)
{
    assert(classCount > 0);
    classNames.reserve(classCount);

    for(size_t i=0; i<classCount; i++)
    {
        stringstream s;
        s << "class" << i;
        classNames.push_back(s.str());
    }
}


void fillTable(size_t funcCount, size_t classCount, double rate)
{
    assert(funcCount  > 0);
    assert(classCount > 0);
    assert(rate>=0 && rate<=1);

    // initialize table (all values to false)
    table.assign(classCount,vector<bool>(funcCount,false));

    // set first row to true
    table[0] = vector<bool>(funcCount,true);

    // fill table with random values
    for(size_t classId=1; classId < classCount; classId++ )
        for(size_t funcId=0; funcId<funcCount; funcId++)
            table[classId][funcId] = (rand() < rate * RAND_MAX);

}

void printTable(ostream & os)
{
    for(size_t c=0; c<table.size(); c++ )
    {
        for(size_t f=0; f<table[c].size(); f++)
            os << table[c][f] << " ";

        os << endl;
    }
}

void printClass(ostream & os, int classId )
{
    string filename = classNames[classId];
    filename.append(".h");
    filename = stringToUpper(filename);

    os << "#ifndef " << filename << endl;
    os << "#define " << filename << endl << endl;

    // include superclass
    if(classId !=0)
        os << "#include \"" << classNames[classId-1] << ".h"  << "\"" << endl << endl;

    os << "class " << classNames[classId];

    if(classId !=0)
        os << " : public " << classNames[classId-1];
    os <<endl << "{" << endl;
    os << "\tpublic:" << endl;
    for(size_t func=0; func< table[classId].size(); func++  )
    {
        if(! table[classId][func])
            continue;

        os << "\t\t" << "virtual void " << functionNames[func] << "() {}" << endl;
    }
    os << "};" << endl;

    os << endl << "#endif" << endl;
}


/**
 * Creates the following code
 * void test_class<classId>_func<funcId> () {
 *    class0 * p = new class<classId>();
 *    p->func<funcId>();
 *    delete p;
 * }
 * this codes relies on the fact, that class0 implements all functions
 */
void printTestFunc(ostream & os,int classId, int funcId)
{
    os << "void test_" << classNames[classId] << "_" << functionNames[funcId] << "()" << endl << "{" << endl;
    os << "\t" << classNames[0] << " * p = new " << classNames[classId]<< "();" << endl;
    os << "\t" << "p->" << functionNames[funcId] << "();" << endl;
    os << "\t" << "delete p;" << endl;
    os << "}" << endl << endl;
}


/** Determines which virtual function get called in the following situation
 *   - class0 * p = new class<classId>
 *   - p->func<funcId>
 *
 *  Return value: if correct is true:
 *                the vector has exactly one entry: the id of the class, which function gets called
 *                if correct is false:
 *                (mode for testing the CG-Generator, it cannot determine the right function instead creates superset)
 *                all classes that implement the function between 0 and <funcId>
 */
vector<int> resolve(int classId, int funcId, bool correct)
{
    vector<int> result;
    for(size_t i =0 ; i <= classId; i++)
    {
        if(table[i][funcId])
            result.push_back(i);
    }
    if(correct)
    {
        vector<int> correctRes;
        correctRes.push_back(result.back());
        return correctRes;
    }
    else
        return result;
}

/// Prints the dump-file (adjacency list representation) for one testfunction
void printDumpFile(ostream & os, int classId, int funcId)
{
    os << "test_" << classNames[classId] << "_" << functionNames[funcId] << " ->";
    vector<int> res = resolve(classId,funcId,false);
    for(size_t i = 0; i < res.size(); i++)
        os << " " << classNames[res[i]] << "::" << functionNames[funcId];
    os << endl;
}



string prependPath(const string & path, const string & filename)
{
    string result=path;
    result.append("/");
    result.append(filename);
    return result;
}


int main(int argc, char** argv)
{
   srand((unsigned int)time(NULL));

   size_t classCount;
   size_t functionCount;
   double fillRate;
   string outputFolderName;

    //arg0 progname
    //arg1 classCount
    //arg2 functionCount
    //arg3 fillrate
    //arg4 foldername

    if(argc!=5)
    {
        cerr << "Virtual Call Resolution Testfile Generator:" << endl;
        cerr << "Usage:" << endl;
        cerr << "./virtualTestCaseGen <classCount> <functionCount> <fillRate> <outputFolderName>" << endl;
        return 1;
    }

    classCount = atoi (argv[1]);
    functionCount = atoi(argv[2]);
    fillRate = atof(argv[3]);
    outputFolderName = string(argv[4]);

    create_directory(outputFolderName);


    classCount=10;
    functionCount=10;
    fillRate=0.5;

    fillTable(functionCount,classCount,fillRate);
    fillFunctionNames(functionCount);
    fillClassNames(classCount);

    //printTable(cout);

    // Print each class to a file
    for(size_t i=0; i<classCount; i++)
    {
        string filename = classNames[i];
        filename.append(".h");
        ofstream fstr ( prependPath(outputFolderName,filename).c_str());
        printClass(fstr,i);

    }

    ofstream mainfile (prependPath(outputFolderName,"testcase.cpp").c_str());
    ofstream dumpfile (prependPath(outputFolderName,"dumfile.cmp").c_str());

    mainfile << "#include \"" << classNames[classCount-1] << ".h\"" << endl;

    mainfile << endl;

    for(size_t c=0; c<table.size(); c++ )
          for(size_t f=0; f<table[c].size(); f++)
          {
              printTestFunc(mainfile,c,f);
              printDumpFile(dumpfile,c,f);
          }

    mainfile << "int main(int argc, char**argv)" << endl;
    mainfile << "{" << endl;
    mainfile << "\treturn 0;"  << endl << "}" << endl;
    mainfile << endl;

    return 0;
}


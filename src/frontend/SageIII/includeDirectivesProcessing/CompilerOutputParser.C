// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include <rose.h>
#include "sage3basic.h"

#include <rose_config.h>

#include "FileHelper.h"
#include "CompilerOutputParser.h"

using namespace std;

const string CompilerOutputParser::topLevelParsePrefix = ". ";

CompilerOutputParser::CompilerOutputParser(SgProject* projectNode) {
    this -> projectNode = projectNode;
}

//This method is a copy of the ROSE's method popenReadFromVector in file processSupport.C. 
//The only two changes are adding an additional option to the compiler arguments, 
//and replacing stdout with stderr in the connector pipe (since both -v and -H options output goes to stderr).

FILE* CompilerOutputParser::getCompilerOutput(const vector<string>& argv, bool isVerbose) 
   {
    ROSE_ASSERT(!argv.empty());
    int pipeDescriptors[2];

#if !ROSE_MICROSOFT_OS
    int pipeErr = pipe(pipeDescriptors);
    if (pipeErr == -1) {
        perror("pipe");
        abort();
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        abort();
    }
    if (pid == 0) { // Child
        vector<const char*> argvC(argv.size() + 2);
        argvC[0] = strdup(argv[0].c_str());
        //Is either verbose or outputs included files (i.e. either -v or -H option). Its ok to duplicate the option if it is already present.
        if (isVerbose) {
            argvC[1] = "-v";
        } else {
            argvC[1] = "-H";
        }
        for (size_t i = 1; i < argv.size(); ++i) {
            argvC[i + 1] = strdup(argv[i].c_str());
        }
        argvC.back() = NULL;

        int closeErr = close(pipeDescriptors[0]);
        if (closeErr == -1) {
            perror("close (in child)");
            abort();
        }
        int dup2Err = dup2(pipeDescriptors[1], fileno(stderr));
        if (dup2Err == -1) {
            perror("dup2");
            abort();
        }
        execvp(argvC[0], (char* const*) &argvC[0]);
        perror(("execvp in popenReadFromVector: " + argv[0]).c_str());
        exit(1); // Should not get here normally
    } else { // Parent
        int closeErr = close(pipeDescriptors[1]);
        if (closeErr == -1) {
            perror("close (in parent)");
            abort();
        }
        return fdopen(pipeDescriptors[0], "r");
    }
#else
    // tps: does not work right now. Have not hit this assert yet.
    printf("Error: no MSVS implementation available popenReadFromVector() (not implemented) \n");
    assert(false);

    return NULL;
#endif
}

void CompilerOutputParser::addIncludedFilesToMap(const string& includingFileName, const set<string>& includedFiles) {
    map<string, set<string> >::iterator mapEntry = includedFilesMap.find(includingFileName);
    if (mapEntry != includedFilesMap.end()) {
        (mapEntry -> second).insert(includedFiles.begin(), includedFiles.end());
    }else{
        set<string> includedFilesSet;
        includedFilesSet.insert(includedFiles.begin(), includedFiles.end());
        includedFilesMap.insert(pair<string, set<string> >(includingFileName, includedFilesSet));
    }
//        //If the list of included files for this including file already exists, then check that it is the same as the newly collected list. 
//        list<string> existingList = existingMapEntry -> second;
//
//        ROSE_ASSERT(existingList.size() == includedFiles.size());
//
//        list<string>::iterator existingListIterator = existingList.begin();
//        list<string>::iterator newListIterator = includedFiles.begin();
//        while (existingListIterator != existingList.end()) {
//            ROSE_ASSERT((*existingListIterator).compare(*newListIterator) == 0);
//            existingListIterator++;
//            newListIterator++;
//        }
//    } else {
//        includedFilesMap.insert(pair<string, list<string> >(includingFileName, includedFiles));
//    }
}

void CompilerOutputParser::parseIncludedFilesFromCompilerOutput(const string& includingFileName, const string& parsePrefix) {
    ROSE_ASSERT(FileHelper::fileExists(includingFileName)); //should be an existing file

    size_t parsePrefixSize = parsePrefix.size();
    string oneLevelDeeperParsePrefix = "." + parsePrefix;
    string lastIncludedFile;
    set<string> includedFiles;

    while (compilerOutputReader -> hasLine()) {
        string currentLine = compilerOutputReader -> getLine();
        if (currentLine.compare(0, parsePrefixSize, parsePrefix) == 0) {
            //The current line belongs to this including file.
            lastIncludedFile = FileHelper::makeAbsoluteNormalizedPath(currentLine.substr(parsePrefixSize), workingDirectory);                    
            includedFiles.insert(lastIncludedFile);
            compilerOutputReader -> readNextLine(); //read the next line, because the current line was processed
        } else {
            if (currentLine.compare(0, oneLevelDeeperParsePrefix.size(), oneLevelDeeperParsePrefix) == 0) {
                //The current line belongs to the last processed included file.
                parseIncludedFilesFromCompilerOutput(lastIncludedFile, oneLevelDeeperParsePrefix);
            } else {
                //The current line is neither of this including file nor of the last processed included file, 
                //so stop and let the caller process it (and the following lines, if any), unless this is the top level.
                if (parsePrefix.compare(topLevelParsePrefix) != 0) {
                    break;
                } else {
                    compilerOutputReader -> readNextLine(); //read the next line, because the current line was skipped
                }
            }
        }
    }
    addIncludedFilesToMap(includingFileName, includedFiles);
}

void CompilerOutputParser::parseIncludedFilesSearchPathsFromCompilerOutput() {
    const string quotedIncludesStart = "#include \"...\" search starts here:";
    const string bracketedIncludesStart = "#include <...> search starts here:";
    const string includesEnd = "End of search list.";
    bool isInsideQuotedIncludes = false;
    bool isInsideBracketedIncludes = false;

    while (compilerOutputReader -> hasLine()) {
        string currentLine = compilerOutputReader -> getLine();
        compilerOutputReader -> readNextLine();
        if (currentLine.compare(quotedIncludesStart) == 0) {
            isInsideQuotedIncludes = true;
            continue;
        }
        if (currentLine.compare(bracketedIncludesStart) == 0) {
            isInsideQuotedIncludes = false;
            isInsideBracketedIncludes = true;
            continue;
        }
        if (currentLine.compare(includesEnd) == 0) {
            //done
            return;
        }

        if (isInsideQuotedIncludes || isInsideBracketedIncludes) {
            string path = FileHelper::makeAbsoluteNormalizedPath(/*the first character is a white space, so ignore it*/currentLine.substr(1), workingDirectory);
            if (isInsideQuotedIncludes) {
                quotedIncludesSearchPaths.push_back(path);
            } else {
                bracketedIncludesSearchPaths.push_back(path);
            }
        }
    }
}

void CompilerOutputParser::processFile(SgFile* inputFile, bool isVerbose) 
   {
  // This code duplicates parts of methods int SgFile::compileOutput ( int fileNameIndex ) from file Cxx_Grammar.C and 
  // int SgFile::compileOutput ( vector<string>& argv, int fileNameIndex ) from file sageSupport.C

  // This part is from int SgFile::compileOutput ( int fileNameIndex )
     vector<string> argv = inputFile -> get_originalCommandLineArgumentList();
     assert(!argv.empty());
     assert(argv.size() > 1);
     inputFile -> stripRoseCommandLineOptions(argv);
     if (inputFile -> get_C_only() == true || inputFile -> get_C99_only() == true || inputFile -> get_Cxx_only() == true) 
        {
          inputFile -> stripEdgCommandLineOptions(argv);
        } 
       else
        {
          if (inputFile -> get_Fortran_only() == true) 
             {
            // DQ (4/2/2011): some Fortran tests pass in tests/nonsmoke/functional/roseTests/astInterfaceTests pass in EDG options, we
            // might want to correct this (for now we will clean up the command line as part of command line handling).
               inputFile -> stripEdgCommandLineOptions(argv);
               inputFile -> stripFortranCommandLineOptions(argv);
             } 
            else
             {
            // This is the case of binary analysis...(nothing to do there)
             }
        }

  // This part is from int SgFile::compileOutput ( vector<string>& argv, int fileNameIndex )
     string compilerNameOrig = BACKEND_CXX_COMPILER_NAME_WITH_PATH;
     if (inputFile -> get_Java_only() == true) 
        {
       // TODO: We do not handle this, stop gracefully.
          compilerNameOrig = BACKEND_JAVA_COMPILER_NAME_WITH_PATH;
        }
     if (inputFile -> get_Fortran_only() == true) 
        {
          compilerNameOrig = BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH;
        }
    
     string compilerName = compilerNameOrig + " ";

  // Simulate the presence of -H option and then restore back
     bool originalSkipUnparse = inputFile -> get_skip_unparse();
     string originalUnparseOutputFileName = inputFile -> get_unparse_output_filename();

     inputFile -> set_skip_unparse(true);
     inputFile -> set_unparse_output_filename(inputFile -> get_sourceFileNameWithPath());

#if 0
  // DQ (3/15/2020): There are only two places where this is called (here and in the SgFile::compileOutput() function).
  // When it is called here it seems to be unnessasary, and to early when the get_extraIncludeDirectorySpecifierList
  // is supposed to be computed before hand.
     printf ("In CompilerOutputParser::processFile(): Calling buildCompilerCommandLineOptions(): inputFile->getFileName() = %s \n",inputFile->getFileName().c_str());
     printf (" --- isVerbose = %s \n",isVerbose ? "true" : "false");
#endif

#if 0
  // DQ (3/14/2020): I don't think this is required anymore within the improved design of the header file unparsing support.
     vector<string> compilerNameString = inputFile -> buildCompilerCommandLineOptions(argv, 0, compilerName);
     compilerOutputReader = new CompilerOutputReader(getCompilerOutput(compilerNameString, isVerbose));
     workingDirectory = inputFile -> getWorkingDirectory();
     if (isVerbose) 
        {
          parseIncludedFilesSearchPathsFromCompilerOutput();
        } 
       else
        {
          parseIncludedFilesFromCompilerOutput(FileHelper::normalizePath(inputFile -> getFileName()), topLevelParsePrefix);
        }
#else
  // DQ (3/14/2020): Output a message until I verify this is no longer required.
  // DQ (4/6/2020): Added header file unparsing feature specific debug level.
     if (SgProject::get_unparseHeaderFilesDebug() >= 1)
        {
          printf ("In CompilerOutputParser::processFile(): skipping call to buildCompilerCommandLineOptions() and parseIncludedFilesFromCompilerOutput() \n");
        }
#endif

#if 0
  // DQ (11/5/2018): Output as part of debugging unparsing of header files (filename and directory selection).
     printf ("In CompilerOutputParser::processFile(): inputFile->getFileName()      = %s \n",inputFile->getFileName().c_str());
     printf ("In CompilerOutputParser::processFile(): originalUnparseOutputFileName = %s \n",originalUnparseOutputFileName.c_str());
#endif

  // Restore back original settings
     inputFile -> set_skip_unparse(originalSkipUnparse);
     inputFile -> set_unparse_output_filename(originalUnparseOutputFileName);
  // inputFile -> compileOutput(0);
   }


map<string, set<string> > 
CompilerOutputParser::collectIncludedFilesMap() 
   {
     SgFilePtrList inputFileList = projectNode -> get_fileList();

     for (SgFilePtrList::const_iterator it = inputFileList.begin(); it != inputFileList.end(); it++) 
        {
          processFile(*it, false);
        }
     return includedFilesMap;
   }


pair<list<string>, list<string> > 
CompilerOutputParser::collectIncludedFilesSearchPaths() 
   {
     processFile(*(projectNode -> get_fileList().begin()), true); //it is sufficient to get verbose output for a single input file
     return pair<list<string>, list<string> >(quotedIncludesSearchPaths, bracketedIncludesSearchPaths);
   }

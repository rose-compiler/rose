#ifndef GRAMMAR_FILE_H
#define GRAMMAR_FILE_H

// DQ (3/12/2006): We want to remove config.h from being placed in every source file
#include <rose_config.h>
#include "fileoffsetbits.h"

// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"
#include "rose_paths.h"

#include <stdio.h>
#include <stdlib.h>

// We need to separate the construction of the code for ROSE from the ROSE code
// #include FSTREAM_HEADER_FILE
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#if !ROSE_MICROSOFT_OS
#include <dirent.h>
#include <unistd.h>
#endif
#include "string_functions.h"

// This class is used to cache files read in as part of the processing of a grammar
class GrammarFile {
     public:
       GrammarFile( std::string, const StringUtility::FileWithLineNumbers inputBuffer);
       GrammarFile( const GrammarFile & X );
       ~GrammarFile();
       GrammarFile& operator=( const GrammarFile & X );
       void setFilename( const std::string& inputFilename );
       std::string getFilename() const;
       void setBuffer ( const StringUtility::FileWithLineNumbers& inputBuffer );
       const StringUtility::FileWithLineNumbers& getBuffer() const;
     private:
       std::string filename;
       StringUtility::FileWithLineNumbers buffer;

    };
#endif

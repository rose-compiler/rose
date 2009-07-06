#include "FileManager.h"

#include <iostream>
#include <sstream>

#include "CppRuntimeSystem.h"

using namespace std;



// -----------------------    FileInfo  --------------------------------------


FileInfo::FileInfo(FileHandle fh) :
    handle(fh),
    name("Invalid"),
    openMode(INVALID_OPEN_MODE)
{
}

FileInfo::FileInfo(FileHandle fp,
                   const std::string & _name,
                   int _openMode,
                   const SourcePosition & pos):
     handle(fp),
     name(_name),
     openMode(_openMode),
     openPos(pos)
{

}


void FileInfo::print(ostream & os) const
{
    os << hex << handle << "\t" << name ;
    os << " Mode:";
    if(openMode & READ)    os << "R";
    if(openMode & WRITE)   os << "W";
    if(openMode & APPEND)  os << "A";
    os << "\t Opened at " << openPos;
}


ostream& operator<< ( ostream &os, const FileInfo & m)
{
    m.print(os);
    return os;
}

// -----------------------    FileManager  --------------------------------------


void FileManager::openFile(FileHandle handle,
                           const std::string & fileName,
                           int mode,
                           const SourcePosition & pos)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    FileInfo compareObj (handle);
    if( openFiles.find(compareObj) != openFiles.end() )
    {
        rs->violationHandler(RuntimeViolation::DOUBLE_FILE_OPEN,
                             "Tried to register the same file-handle twice");
        return;
    }

    openFiles.insert( FileInfo(handle,fileName,mode,pos));
}

void FileManager::closeFile(FileHandle handle)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    FileInfo compareObj(handle);

    set<FileInfo>::iterator iter = openFiles.find(compareObj);
    if( iter == openFiles.end() )
    {
        rs->violationHandler(RuntimeViolation::INVALID_FILE_CLOSE,
                             "Tried to close a non opened File-Handle");
        return;
    }
    openFiles.erase(iter);
}

void FileManager::checkFileAccess(FileHandle handle, bool read)
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    //TODO check for read write

    FileInfo compareObj(handle);
    if( openFiles.find(compareObj) != openFiles.end() )
    {
        rs->violationHandler(RuntimeViolation::INVALID_FILE_ACCESS,
                             "Tried to access invalid file");
        return;
    }
}


void FileManager::checkForOpenFiles()
{
    RuntimeSystem * rs = RuntimeSystem::instance();

    // Check if all files were closed
    if( openFiles.size() > 0 )
    {
        stringstream desc;
        print(desc);
        rs->violationHandler(RuntimeViolation::UNCLOSED_FILES,desc.str());
    }
}



void FileManager::print(ostream & os) const
{
    os << endl;
    os << "----------------------------- Filemanager: Open Files -----------------------------" << endl << endl;

    typedef set<FileInfo>::const_iterator FileIter;
    for( FileIter i = openFiles.begin(); i != openFiles.end(); ++i)
        os << *i << endl;



    os << "-----------------------------------------------------------------------------------" << endl;

}

std::ostream& operator<< (std::ostream &os, const FileManager & m)
{
    m.print(os);
    return os;
}



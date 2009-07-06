
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <set>

#include "Util.h"


class FileManager;

typedef FILE *  FileHandle;


class FileInfo
{
    public:
        /// Constructor
        /// @param openMode  OR combination out of RuntimeSystem::FileOpenMode constants
        FileInfo(FileHandle fp,
                 const std::string & name,
                 int openMode,
                 const SourcePosition & pos);



        /// overloaded operator because FileInfo's are managed in a std::set
        bool operator< (const FileInfo & other) const { return handle < other.handle; }

        void print(std::ostream & os) const;

        FileHandle             getHandle()   const      { return handle;   }
        const std::string &    getFileName() const      { return name;     }
        int                    getOpenMode() const      { return openMode; }
        const SourcePosition & getPos()      const      { return openPos;  }

    protected:
        friend class FileManager;

        /// Creates an invalid FileInfo, only used to get comparison objects in FileManager
        FileInfo(FileHandle f);


        FileHandle     handle;   ///< the pointer returned by fopen
        std::string    name;      ///< filename or filepath
        int            openMode;  ///< combination of OpenMode flags
        SourcePosition openPos;   ///< position in sourcecode where file was opened
};

std::ostream& operator<< (std::ostream &os, const FileInfo & m);




enum FileOpenMode   {  INVALID_OPEN_MODE=0, READ=1,WRITE=2,APPEND=4  };


class FileManager
{
    public:
        typedef FileOpenMode OpenMode;

        FileManager()  {};
        ~FileManager() {};


        /// Registers that a file was opened
        void openFile(FileHandle handle,
                      const std::string & fileName,
                      int openMode,
                      const SourcePosition & pos);

        /// Registers that a file was closed
        void closeFile(FileHandle  handle);

        /// Checks if a certain file-access is valid
        /// @param handle  the handle on which the file operation is performed
        /// @param read    true if read-access, false if write-access
        void checkFileAccess(FileHandle handle, bool read);

        /// Should be called at end of program, to check if there are any open files
        void checkForOpenFiles();

        /// Deletes all collected data
        /// normally only needed for debug purposes
        void clearStatus() { openFiles.clear(); }

        /// Prints the status to a stream
        void print(std::ostream & os) const;

    private:
        std::set<FileInfo> openFiles;

};
std::ostream& operator<< (std::ostream &os, const FileManager & m);




#endif

#ifndef LIBRARY_INFO_H
#define LIBRARY_INFO_H

#include <BinaryAstHash.h>
#include <time.h>
#include "Combinatorics.h"

/** LibraryIdentification.
 *
 *  This namespace encapsulates library for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like libraryality for
 *  ROSE binary analysis.
 *  Currently only uses the Fowler-Noll-Vo HasherFnv class in
 *  Combinatorics.h.  We should add options for others.
 **/
namespace LibraryIdentification
{
    class LibraryInfo
    {
    public:
        /**
         *  LibraryInfo
         *
         *  Constructor.  Combines all the information required to
         *  identify a library. 
         *  This constructor allows the user to define everything.
         *
         * @param[in] libName  Name of the library to add
         * @param[in] libVersion  Version of the library
         * (Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h)
         * @param[in] libHash  Unique Hash of the library to add
         * (Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h)
         * @param[in] architecture  architecture library was built for
         **/
    LibraryInfo(const std::string& ilibName, const std::string& ilibVersion, const std::string& ilibHash, SgAsmExecutableFileFormat::InsSetArchitecture iarchitecture) :
        libName(ilibName), libVersion(ilibVersion), libHash(ilibHash), architecture(iarchitecture) {
            analysisTime = time(NULL);
        };
        
        
        /**
         *  LibraryInfo
         *
         *  Constructor.  Combines all the information required to
         *  identify a library. 
         *  This constructor constructs the hash, and all other
         *  library info, from a SgAsmGenericFile.  Unfortunately, the
         *  name and version of the library is only availible from
         *  dynamic libraries, so it must be passed in here.
         *  WARNING: Currently Elf files only!
         *
         * @param[in] libName  Name of the library to add
         * @param[in] libVersion  Version of the library       
         * @param[in] libraryFile The library file we are storing
         **/
    LibraryInfo(const std::string& ilibName, const std::string& ilibVersion, SgAsmGenericFile* libraryFile) :
        libName(ilibName), libVersion(ilibVersion)
        {
            SgAsmGenericHeader* genericHeader = libraryFile->get_header(SgAsmExecutableFileFormat::ExecFamily::FAMILY_ELF);
            SgAsmElfFileHeader* elfHeader = dynamic_cast<SgAsmElfFileHeader*>(genericHeader);
            ASSERT_require(elfHeader != NULL);
            
            //TODO: Check that we have a name and version, if not, see if
            //the library is dynamic and they can be discovered
            

            //Get the architecture
            architecture = elfHeader->get_isa();
            
            //Generate hash of library contents
            //Should make the hash optional via factory method in Combinatorics.h
            Rose::Combinatorics::HasherFnv fnvHash;
            
            SgFileContentList content = libraryFile->content();
            fnvHash.append(&content[0], content.size());
            libHash = fnvHash.toString();
        }

        /**
         *  LibraryInfo
         *
         *  Constructor.  Only takes the hash, Rest to be filled in from
         *  matching in the database. 
         *
         * @param[in] libHash  Unique Hash of the library to add
         **/
    LibraryInfo(const std::string& ilibHash) : libHash(ilibHash) { };
        

        friend bool operator<(const LibraryInfo& lhs,const LibraryInfo& rhs) 
        {
            return lhs.libHash < rhs.libHash;
        }
        

        /**
         *  getUnknownLibraryInfo
         *
         *  Constructs and returns the special sentinal "Unknown
         *  Library" instance for functions that couldn't be identified.
         **/
        static LibraryInfo getUnknownLibraryInfo() 
        {
            return LibraryInfo("UNKNOWN", "UNKNOWN", "UNKNOWN",   SgAsmExecutableFileFormat::InsSetArchitecture::ISA_UNSPECIFIED);
        };
        
        
        

        //   TODO:  add accessors
        //    private:
        //@brief The name of the library
        std::string libName;

        //@brief The version of the library
        std::string libVersion;

        //@brief A hash that should uniquely identify the library
        std::string libHash;

        //@brief Enumeration of instruction set library was build for
        SgAsmExecutableFileFormat::InsSetArchitecture architecture;

        //@brief The time when this library was processed
        time_t analysisTime;

    };
    
}



#endif //LIBRARY_INFO

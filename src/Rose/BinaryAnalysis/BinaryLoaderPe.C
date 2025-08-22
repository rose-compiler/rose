#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // for SgBinaryComposite
#include <Rose/BinaryAnalysis/BinaryLoaderPe.h>

#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/Diagnostics.h>

#include <SgAsmGenericFile.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericString.h>
#include <SgAsmInterpretation.h>
#include <SgAsmPEExportDirectory.h>
#include <SgAsmPEExportEntry.h>
#include <SgAsmPEExportEntryList.h>
#include <SgAsmPEExportSection.h>
#include <SgAsmPEFileHeader.h>
#include <SgAsmPEImportDirectory.h>
#include <SgAsmPEImportDirectoryList.h>
#include <SgAsmPEImportItem.h>
#include <SgAsmPEImportItemList.h>
#include <SgAsmPEImportSection.h>
#include <Cxx_GrammarDowncast.h>

using namespace Sawyer::Message;
using namespace std;

namespace Rose {
namespace BinaryAnalysis {

/* This binary loader can handle all PE files. */
bool
BinaryLoaderPe::canLoad(SgAsmGenericHeader *hdr) const {
    return isSgAsmPEFileHeader(hdr)!=NULL;
}

/* For any given file header, start mapping at a particular location in the address space. */
Address
BinaryLoaderPe::rebase(const MemoryMap::Ptr &map, SgAsmGenericHeader *header, const SgAsmGenericSectionPtrList &sections) {
    SgAsmPEFileHeader* pe_header = isSgAsmPEFileHeader(header);
    ROSE_ASSERT(pe_header != NULL);
    const size_t maximum_alignment = pe_header->get_e_section_align();
    Address original = pe_header->get_baseVa();

    // Find the minimum address desired by the sections to be mapped.
    Address min_preferred_rva = (uint64_t)(-1);
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si)
        min_preferred_rva = std::min(min_preferred_rva, (*si)->get_mappedPreferredRva());
    Address min_preferred_va = header->get_baseVa() + min_preferred_rva;

    // Minimum address at which to map
    //AddressInterval freeSpace = map->unmapped(mappableArea.greatest(), Sawyer::Container::MATCH_BACKWARD);
    AddressInterval valid_range = AddressInterval::hull(pe_header->get_baseVa(),0x7FFFFFFFFFFF);
    ROSE_ASSERT(!valid_range.isEmpty());
    Address map_base_va = map->findFreeSpace(pe_header->get_e_image_size(), maximum_alignment, valid_range).get();
    map_base_va = alignUp(map_base_va, (Address)maximum_alignment);
    
    if(map_base_va != original){
        mlog[INFO]<<"Rebasing File "<<pe_header->get_file()->get_name()<<", From:0x"<<hex<<original<<" To:0x"<<map_base_va<<dec<<endl;
    }

    // If the minimum preferred virtual address is less than the floor of the page-aligned mapping area, then
    // return a base address which moves the min_preferred_va to somewhere in the page pointed to by map_base_va.
    if (min_preferred_va < map_base_va) {
        size_t min_preferred_page = min_preferred_va / maximum_alignment;
        if (map_base_va < min_preferred_page * maximum_alignment)
            return 0;
        return map_base_va;
    }

    return header->get_baseVa();
}

/* Returns sections to be mapped */
SgAsmGenericSectionPtrList
BinaryLoaderPe::getRemapSections(SgAsmGenericHeader *header) {
    SgAsmGenericSectionPtrList retval;

    /* The NT loader always loads the PE header, so we include that first in the list. */
    retval.push_back(header);

    /* Add the sections in the order they appear in the section table */
    const SgAsmGenericSectionPtrList &sections = header->get_sections()->get_sections();
    std::map<int, SgAsmGenericSection*> candidates;
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            ASSERT_require(candidates.find(sections[i]->get_id())==candidates.end());
            candidates.insert(std::make_pair(sections[i]->get_id(), sections[i]));
        }
    }
    for (std::map<int, SgAsmGenericSection*>::iterator ci=candidates.begin(); ci!=candidates.end(); ++ci) {
        retval.push_back(ci->second);
    }
    return retval;
}

/* This algorithm was implemented based on an e-mail from Cory Cohen at CERT and inspection of PE::ConvertRvaToFilePosition()
 * as defined in "PE.cpp 2738 2009-06-05 15:09:11Z murawski_dev". [RPM 2009-08-17] */
BinaryLoader::MappingContribution
BinaryLoaderPe::alignValues(SgAsmGenericSection *section, const MemoryMap::Ptr&,
                            Address *malign_lo_p, Address *malign_hi_p,
                            Address *va_p, Address *mem_size_p,
                            Address *offset_p, Address *file_size_p, bool *map_private_p,
                            Address *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                            ConflictResolution *resolve_p) {
    SgAsmGenericHeader *header = isSgAsmPEFileHeader(section);
    if (!header) header = section->get_header();
    ASSERT_not_null(header);

    if (!section->isMapped())
        return CONTRIBUTE_NONE;

    /* File and memory alignment must be between 1 and 0x200 (512), inclusive */
    Address file_alignment = section->get_fileAlignment();
    if (file_alignment>0x200 || 0==file_alignment)
        file_alignment = 0x200;
    Address mapped_alignment = section->get_mappedAlignment();
    if (0==mapped_alignment)
        mapped_alignment = 0x200;

    /* Align file size upward even before we align the file offset downward. */
    Address file_size = alignUp(section->get_size(), file_alignment);

    /* Map the entire section's file content (aligned) or the requested map size, whichever is larger. */
    Address mapped_size = std::max(section->get_mappedSize(), file_size);

    /* Align file offset downward but do not adjust file size. */
    Address file_offset = alignDown(section->get_offset(), file_alignment);

    /* Align the preferred relative virtual address downward without regard for the base virtual address, and do not adjust
     * mapped size. */
    Address mapped_va = header->get_baseVa() + alignDown(section->get_mappedPreferredRva(), mapped_alignment);

    *malign_lo_p = mapped_alignment;
    *malign_hi_p = 1;
    *va_p = mapped_va;
    *mem_size_p = mapped_size;
    *offset_p = file_offset;
    *file_size_p = file_size;
    *map_private_p = false;
    *va_offset_p = 0;
    *anon_lo_p = *anon_hi_p = true;
    *resolve_p = RESOLVE_OVERMAP;
    return CONTRIBUTE_ADD;
}

//isLinked is case insensitive as is the windows filesystem.
bool
BinaryLoaderPe::isLinked(SgBinaryComposite *composite, const std::string &filename) {
    std::string lowerFileName = filename;
    lowerFileName = lowerFileName.substr(lowerFileName.find_last_of("/") + 1);
    boost::to_lower(lowerFileName);
    const SgAsmGenericFilePtrList &files = composite->get_genericFileList()->get_files();
    for (SgAsmGenericFilePtrList::const_iterator fi=files.begin(); fi!=files.end(); ++fi) {
        std::string headerName = (*fi)->get_name();
        headerName = headerName.substr(headerName.find_last_of("/") + 1);
        boost::to_lower(headerName);
        if (headerName==lowerFileName)
            return true;
    }
    return false;
}

bool
BinaryLoaderPe::isLinked(SgAsmInterpretation *interp, const std::string &filename) {
    std::string lowerFileName = filename;
    lowerFileName = lowerFileName.substr(lowerFileName.find_last_of("/") + 1);
    boost::to_lower(lowerFileName);
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        ASSERT_not_null((*hi)->get_file());
        std::string headerName = (*hi)->get_name()->get_string();
        headerName = headerName.substr(headerName.find_last_of("/") + 1);
        boost::to_lower(headerName);
        if (headerName==lowerFileName)
            return true;
    }
    return false;
}

void
BinaryLoaderPe::addLibDefaults(SgAsmGenericHeader* /*hdr=nullptr*/) {
    /* The LD_PRELOAD environment variable may contain space-separated library names */
    const char* ld_preload_env = getenv("LD_PRELOAD");
    if (ld_preload_env) {
        std::string s = ld_preload_env;
        boost::regex re;
        re.assign("\\s+");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            preloads().push_back(*iter);
    }

    /* Add the paths from the LD_LIBRARY_PATH environment variable */
    const char *ld_library_path_env = getenv("LD_LIBRARY_PATH");
    if (ld_library_path_env) {
        std::string s = ld_library_path_env;
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            directories().push_back(*iter);
    }
}

std::string
BinaryLoaderPe::findSoFile(const std::string &libname) const {
    static std::map<std::string,bool> notFound;
    if(notFound.count(libname) == 1) return "";
    mlog[TRACE] <<"find library=" <<libname <<"\n";
    if (!libname.empty() && '/'==libname[0])
        return libname;
    std::string lowerlibname = libname;
    boost::to_lower(lowerlibname);
    for (std::vector<std::string>::const_iterator di=directories().begin(); di!=directories().end(); ++di) {
        mlog[TRACE] <<"  looking in " <<*di <<"\n";
        std::string libpath = *di + "/" + libname;
        std::string lowerlibpath = *di + "/" + lowerlibname;
        struct stat sb;
#ifndef _MSC_VER
        if (stat(libpath.c_str(), &sb)>=0 && S_ISREG(sb.st_mode) && access(libpath.c_str(), R_OK)>=0) {
            mlog[TRACE] <<"    found.\n";
            return libpath;
        }else if(stat(lowerlibpath.c_str(), &sb)>=0 ){
            mlog[TRACE] <<"    found.\n";
            return lowerlibpath;
        }
#endif
    }
    notFound[libname] = false;
    if (mlog[TRACE]) {
        if (directories().empty())
            mlog[TRACE] <<"no search directories\n";
    }
    mlog[WARN]<<"cannot find file for library: " + libname + "\n";
    return "";
}

/* once called loadInterpLibraries */
void
BinaryLoaderPe::link(SgAsmInterpretation* interp) {
    addLibDefaults();
    ASSERT_not_null(interp);
    SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    ASSERT_not_null(composite);

    /* Make sure the pre-load objects are parsed and linked into the AST. */
    for (std::vector<std::string>::const_iterator pi=preloads().begin(); pi!=preloads().end(); ++pi) {
        mlog[TRACE] <<"preload object " <<*pi <<"\n";
        std::string filename = findSoFile(*pi);
        if (filename == "") continue;
        if (isLinked(composite, filename)) {
            mlog[TRACE] <<filename <<" is already parsed.\n";
        } else {
            Stream m1(mlog[TRACE] <<"parsing " <<filename);
            createAsmAST(composite, filename);
            m1 <<"... done.\n";
        }
    }

    /* Bootstrap */
    std::list<SgAsmGenericHeader*> unresolved_hdrs;
    unresolved_hdrs.insert(unresolved_hdrs.end(),
                           interp->get_headers()->get_headers().begin(),
                           interp->get_headers()->get_headers().end());

    /* Process unresolved headers from the beginning of the queue and push new ones onto the end. */
    while (!unresolved_hdrs.empty()) {
        SgAsmGenericHeader *header = unresolved_hdrs.front();
        unresolved_hdrs.pop_front();
        std::string header_name = header->get_file()->get_name();
        std::vector<std::string> deps = dependencies(header);
        for (std::vector<std::string>::iterator di=deps.begin(); di!=deps.end(); ++di) {
            mlog[TRACE] <<"library " <<*di <<" needed by " <<header_name <<"\n";
            std::string filename = findSoFile(*di);
            if (filename == "") continue;
            if (isLinked(composite, filename)) {
                mlog[TRACE] <<filename <<" is already parsed.\n";
            } else {
                Stream m1(mlog[TRACE] <<"parsing " <<filename);
                SgAsmGenericFile *new_file = createAsmAST(composite, filename);
                m1 <<"... done.\n";
                ASSERT_not_null2(new_file, "createAsmAST failed");
                SgAsmGenericHeaderPtrList new_hdrs = findSimilarHeaders(header, new_file->get_headers()->get_headers());
                unresolved_hdrs.insert(unresolved_hdrs.end(), new_hdrs.begin(), new_hdrs.end());
                mlog[INFO] <<"found library "<<filename<<std::endl;
            }
        }
    }
}

void
BinaryLoaderPe::fixup(SgAsmInterpretation* interp, FixupErrors* /*errors*/) {
    SgAsmGenericHeaderPtrList& headers = interp->get_headers()->get_headers();
    map<pair<string,string>,SgAsmPEExportEntry*> exportEntryMap;
    map<pair<string,unsigned>,SgAsmPEExportEntry*> exportEntryOrdinalMap;
    
    MemoryMap::Ptr memoryMap = interp->get_map();
    if(memoryMap == nullptr) interp->set_map(memoryMap = MemoryMap::instance());
    
    //Traverse sections to ensure mapped address and sections are properly set.
    for(auto h = headers.begin(); h != headers.end(); ++h) {
        SgAsmGenericSectionPtrList& sections = (*h)->get_sections()->get_sections();
        Address headerOffset = (*h)->get_mappedActualVa() - (*h)->get_baseVa();
        for(auto s = sections.begin(); s != sections.end(); ++s){
            SgAsmGenericSection* section = *s;
            if((headerOffset + section->get_baseVa()) > section->get_mappedActualVa()){
                section->set_mappedActualVa(section->get_mappedActualVa() + headerOffset);
            }
        }
    }
    
    //Build a map with the keys DDL Name and entry name that points to the export entry.
    //Seperate map for all loaded dll for determining when a dll was not found.
    for(auto h = headers.begin(); h != headers.end(); ++h) {
        SgAsmGenericSectionPtrList& sections = (*h)->get_sections()->get_sections();
        for(auto s = sections.begin(); s != sections.end(); ++s){
            if(SgAsmPEExportSection* exportSection = isSgAsmPEExportSection(*s)){
                SgAsmPEExportDirectory* exportDir = exportSection->get_exportDirectory();
                SgAsmPEExportEntryPtrList& exports = exportSection->get_exports()->get_exports();
                string dirName = exportDir->get_name()->get_string();
                boost::to_lower(dirName);

                for(auto e = exports.begin(); e != exports.end(); ++e){
                    string entryName = (*e)->get_name()->get_string();
                    boost::to_lower(entryName);
                    unsigned entryOrdinal = (*e)->get_ordinal();
                    exportEntryOrdinalMap[make_pair(dirName,entryOrdinal)] = (*e);
                    if(entryName != "") exportEntryMap[make_pair(dirName,entryName)] = (*e);
                }
            }
        }
    }

    //Find all the import sections and update the IAT
    for(auto h = headers.begin(); h != headers.end(); ++h) {
        SgAsmGenericSectionPtrList& sections = (*h)->get_sections()->get_sections();
        for(auto s = sections.begin(); s != sections.end(); ++s){
            if(SgAsmPEImportSection* importSection = isSgAsmPEImportSection(*s)){
                SgAsmPEImportDirectoryPtrList& directories = (importSection)->get_importDirectories()->get_vector();
                for(auto d = directories.begin(); d != directories.end(); ++d){
                    SgAsmPEImportItemPtrList& imports = (*d)->get_imports()->get_vector();
                    string dirName = (*d)->get_dllName()->get_string();
                    boost::to_lower(dirName);
                    for(auto i = imports.begin(); i != imports.end(); ++i){
                        SgAsmPEImportItem* importEntry = *i;
                        SgAsmPEExportEntry* exportEntry = nullptr;
                        string importName = "";
                        unsigned entryOrdinal = 0;
                        if(importEntry->get_by_ordinal()){
                            entryOrdinal = importEntry->get_ordinal();
                            auto found = exportEntryOrdinalMap.find(make_pair(dirName,entryOrdinal));
                            if (found != exportEntryOrdinalMap.end()) exportEntry = found->second;
                        }else{
                            importName = importEntry->get_name()->get_string();
                            boost::to_lower(importName);
                            auto found = exportEntryMap.find(make_pair(dirName,importName));
                            if (found != exportEntryMap.end()) exportEntry = found->second;
                        }
                        if(exportEntry != nullptr){
                            //The entry could be forwarded to another dll.
                            //Recurse down forwards till the final entry is found
                            set<SgAsmGenericString*> forwardList; 
                            while(SgAsmGenericString* forward = exportEntry->get_forwarder()){
                                //This could be a circular depedency. The windows loader has special behavior
                                //for some functions that are OS functions and do not exist in dll
                                //in these cases no source code is loaded
                                if(forwardList.count(forward) == 0) forwardList.insert(forward);
                                else{
                                    exportEntry = nullptr;
                                    break;
                                }
                                string forwardString = forward->get_string();
                                size_t splitPos = forwardString.find(".");
                                string forwardDirName = forwardString.substr(0,splitPos+1) + "dll";
                                string forwardImportName = forwardString.substr(splitPos+1);
                                boost::to_lower(forwardDirName);
                                boost::to_lower(forwardImportName);
                                exportEntry = nullptr;
                                if(forwardImportName[0] == '#'){
                                    //Forward based upon ordinal
                                    string ordinalString = forwardImportName.substr(1);
                                    unsigned forwardOrdinal = strtoul(ordinalString.c_str(),nullptr,0);
                                    auto found = exportEntryOrdinalMap.find(make_pair(forwardDirName,forwardOrdinal));
                                    if (found != exportEntryOrdinalMap.end()) exportEntry = found->second;
                                }else{
                                    auto found = exportEntryMap.find(make_pair(forwardDirName,forwardImportName));
                                    if (found != exportEntryMap.end()) exportEntry = found->second;
                                }
                                if(exportEntry == nullptr){
                                    //Forwards to an unloaded dll
                                    break;
                                }
                            }
                        }
                        //Export entry found. Set address in the IAT
                        if(exportEntry != nullptr){
                            Address exportAddr   = *exportEntry->get_exportRva().va();
                            Address iatEntryAddr = importEntry->get_iatEntryVa();
                            size_t written = memoryMap->writeUnsigned(exportAddr,iatEntryAddr);
                            if(written > 0) importEntry->set_iat_written(true);
                            mlog[TRACE]<<"Setting value in IAT: "<<dirName<<"."<<importName<<"-"<<entryOrdinal<<": IAT 0x"<<hex<<iatEntryAddr<<" Export: 0x"<<exportAddr<<dec<<endl;
                        }
                    }//End import list iterator
                }//End import directory iterator
            }//End is import section
        }//End section iterator
    }//End Header iterator
}

} // namespace
} // namespace

#endif

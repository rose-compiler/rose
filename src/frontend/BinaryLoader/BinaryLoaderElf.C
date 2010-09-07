#include "sage3basic.h"
#include "BinaryLoaderElf.h"

#include <fstream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* This binary loader can handle all ELF files. */
bool
BinaryLoaderElf::can_load(SgAsmGenericHeader *hdr) const
{
    return isSgAsmElfFileHeader(hdr)!=NULL;
}

static bool
section_cmp(SgAsmGenericSection *_a, SgAsmGenericSection *_b)
{
    SgAsmElfSection *a = isSgAsmElfSection(_a);
    SgAsmElfSection *b = isSgAsmElfSection(_b);
    ROSE_ASSERT(a!=NULL && b!=NULL);
    SgAsmElfSegmentTableEntry *a_seg = a->get_segment_entry();
    SgAsmElfSegmentTableEntry *b_seg = b->get_segment_entry();
    if (a_seg && b_seg) {
#if 0
        /* Sort ELF Segments by position in ELF Segment Table */
        return a_seg->get_index() < b_seg->get_index();
#else
        /* Sort ELF Segments by preferred virtual address */
        return a->get_mapped_preferred_rva() < b->get_mapped_preferred_rva();
#endif
    } else if (a_seg) {
        /* ELF Segments come before ELF Sections */
        return true;
    } else {
        /* Sort ELF Sections by position in ELF Section Table */
        return a->get_id() < b->get_id();
    }
}

SgAsmGenericSectionPtrList
BinaryLoaderElf::get_remap_sections(SgAsmGenericHeader *header)
{
    SgAsmGenericSectionPtrList retval = header->get_mapped_sections();
    std::sort(retval.begin(), retval.end(), section_cmp);
    return retval;
}

/* Same as superclass, but if we are mapping/unmapping an ELF Section (that is not an ELF Segment) then don't bother to align
 * it. This is used for ELF code mapping because code is mapped by ELF Segments and then the ELF Sections fine tune it. */
BinaryLoader::MappingContribution
BinaryLoaderElf::align_values(SgAsmGenericSection *_section, MemoryMap *map,
                              rose_addr_t *malign_lo, rose_addr_t *malign_hi,
                              rose_addr_t *va, rose_addr_t *mem_size,
                              rose_addr_t *offset, rose_addr_t *file_size,
                              rose_addr_t *va_offset, bool *anon_lo, bool *anon_hi, 
                              ConflictResolution *resolve)
{
    MappingContribution retval;
    SgAsmElfSection *section = isSgAsmElfSection(_section);
    rose_addr_t old_malign = section->get_mapped_alignment();
    rose_addr_t old_falign = section->get_file_alignment();

    try {
        if (NULL==section->get_segment_entry()) {
            if (get_debug())
                fprintf(get_debug(), "    Temporarily relaxing memory alignment constraints.\n");
            section->set_mapped_alignment(1);
            section->set_file_alignment(1);
        }
        retval = BinaryLoader::align_values(_section, map, malign_lo, malign_hi, va, mem_size, offset, file_size,
                                            va_offset, anon_lo, anon_hi, resolve);
    } catch (...) {
        section->set_mapped_alignment(old_malign);
        section->set_file_alignment(old_falign);
        throw;
    }
    section->set_mapped_alignment(old_malign);
    section->set_file_alignment(old_falign);

    *anon_lo = false;
    *anon_hi = true;
    *resolve = RESOLVE_OVERMAP;

    return retval;
}
    









#if 0
void
BinaryLoaderElf::handleSectionMapped(SgAsmGenericSection* section)
{
    if (isSgAsmElfSection(section)) {
        SgAsmElfSection* elfSection = isSgAsmElfSection(section);
        // needed to fix up SHT_NOBITS sections (like bss)
        if (elfSection->get_section_entry() && 
            (elfSection->get_section_entry()->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS)) {
            uint64_t nbytes=elfSection->get_section_entry()->get_sh_size();
            if (nbytes) {
                section->set_mapped_size(nbytes);
                unsigned char* zeroBits = section->local_content(nbytes);
                memset(zeroBits, 0, nbytes);
            }
        }
    }
}
#endif 

/* NOTE: We assume that the value in DT_STRTAB matches dynamic->get_linked_section */
static void
getDynamicVars(SgAsmElfFileHeader* elfHeader, std::string& rpath, std::string& runpath)
{
    SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();

    SgAsmElfDynamicSection* dynamic=NULL;
    for (size_t i=0; i < sections.size(); ++i) {
        if (isSgAsmElfDynamicSection(sections[i])) {
            dynamic = isSgAsmElfDynamicSection(sections[i]);
            break;
        }
    }
    if(NULL == dynamic)
        return;

    SgAsmElfDynamicEntryPtrList& entries = dynamic->get_entries()->get_entries();
    //SgAsmElfStringSection* strtab = isSgAsmElfStringSection(dynamic->get_linked_section());
    for (size_t i=0; i < entries.size();++i) {
        if (SgAsmElfDynamicEntry::DT_RPATH == entries[i]->get_d_tag()) {
            rpath = entries[i]->get_d_val().to_string();
        } else if (SgAsmElfDynamicEntry::DT_RUNPATH == entries[i]->get_d_tag()) {
            runpath = entries[i]->get_d_val().to_string();
        }
    }
}

static Rose_STL_Container<std::string>
getLdPreload(SgAsmElfFileHeader* /*header*/)
{
    Rose_STL_Container<std::string> ret;
    const char* ld_preload_env=getenv("LD_PRELOAD");
    if (NULL == ld_preload_env)
        return ret;
    std::string ldPrelodStr;
    boost::regex re;
    re.assign("\\s+");
    boost::sregex_token_iterator iter(ldPrelodStr.begin(), ldPrelodStr.end(), re, -1);
    boost::sregex_token_iterator iterEnd;
    for (; iter != iterEnd; ++iter)
        ret.push_back(*iter);
    return ret;
}

static Rose_STL_Container<std::string>
parseLdPath(const std::string& rawPath)
{
    Rose_STL_Container<std::string> ret;
    if (rawPath.empty())
        return ret;

    boost::regex re;
    re.assign("[:;]");
    boost::sregex_token_iterator iter(rawPath.begin(), rawPath.end(), re, -1);
    boost::sregex_token_iterator iterEnd;
    for (; iter != iterEnd; ++iter)
        ret.push_back(*iter);
    return ret;
}

static Rose_STL_Container<std::string>
getLdLibraryPaths(SgAsmElfFileHeader* /*header*/)
{                                     
    Rose_STL_Container<std::string> ret;
    const char* ld_preload_env =  getenv("LD_PRELOAD");
    if (ld_preload_env) {
        return parseLdPath(ld_preload_env);
    } else {
        return ret;
    }
}


#if 0 /*replaced by dependencies() but also need a function to set up the search path*/
// Overall Caveats - 
//      setuid       - the loader should do different stuff if setuid is used, we don't do that
//      /etc/ld.so.* - there are a bunch of files that can alter behavior - they are not handled
//      -z nodeflib  - aparently different behavior is warrented when linked w/ -z nodeflib, 
//                       I'm not sure how to detect this, so it is ignored
//      vdso         - the linux virtual dynamic shared object is not loaded, but we might want
//                     a way to simulate this in the future. [RPM 2010-08-31]
//      interference - the methods used by ROSE to load an executable (namely, environment variables)
//                     interfere with the loading of ROSE itself. [RPM 2010-08-31]
//      diagnostics  - we should have some way to produce diagnostics similar to those that can be
//                     produced by the readl loader.  E.g., LD_SHOW_AUXV, etc. [RPM 2010-08-31]
//  
// LD_PRELOAD - whitespace separated list of additional libraries (complex qualification for setuid)
// [NOT SUPPORTED] /etc/ld.so.preload 
// DT_RPATH from .dynamic (only if DT_RUNPATH does NOT exist)
// LD_LIBRARY_PATH from environment (no setuid)
// DT_RUNPATH from .dynamic
// [NOT SUPPORTED] from /etc/ld.so.cache (unless compiled w/ -z nodeflib)
// '/lib'                                (unless compiled w/ -z nodeflib)
// '/usr/lib'                            (unless copmiled w/ -z nodeflib)
Rose_STL_Container<std::string>
BinaryLoaderElf::getDLLs(SgAsmGenericHeader* header, const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded)
{
    // not sure what nodeflib does...
    SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(header);
    ROSE_ASSERT(NULL != elfHeader);

    // LD_PRELOAD - list of libraries to be loaded
    Rose_STL_Container<std::string> ldLibs=getLdPreload(elfHeader);

    std::string rpath, runpath;
    getDynamicVars(elfHeader,rpath,runpath);
  
    Rose_STL_Container<std::string> libPaths;
    if (!rpath.empty() && runpath.empty()) {
        // if DT_RPATH is exists AND LD_RUNPATH does NOT exist, seach DT_RPATHs first
        libPaths = parseLdPath(rpath);
    }
    // examine LD_LIBRARY_PATH environment variable
    Rose_STL_Container<std::string> ldLibPaths = getLdLibraryPaths(elfHeader);

    libPaths.insert(libPaths.end(), ldLibPaths.begin(), ldLibPaths.end());
    if (!runpath.empty()) {
        // if DT_RUNPATH exist, search those path's after LD_LIBRARY_PATH
        Rose_STL_Container<std::string> runPaths = parseLdPath(runpath);
        libPaths.insert(libPaths.end(), runPaths.begin(), runPaths.end());
    }
  
    if ((header->get_isa() & SgAsmGenericHeader::ISA_FAMILY_MASK) == SgAsmGenericHeader::ISA_X8664_Family) {
        // Not sure what actually says this is standard, but redhat 5.3 seems to require it
        libPaths.push_back("/lib64");
        libPaths.push_back("/usr/lib64");
    }

    libPaths.push_back("/lib");
    libPaths.push_back("/usr/lib");

    Rose_STL_Container<std::string> files;

    ROSE_ASSERT(header != NULL);
    std::set<std::string> alreadyLoadedSet;
    for (size_t i=0; i < dllFilesAlreadyLoaded.size(); ++i) {
        boost::filesystem::path p(dllFilesAlreadyLoaded[i]);
        //FIXME !! tps (12/18/2009) : Commented out for now since it assumes BOOST 1.4 and does not compile for anyone else 
        // Re-enabling it to see what happens since this is an integral part of the algorithm. [RPM 2010-08-31]
        alreadyLoadedSet.insert(p.filename());
    }

    const SgAsmGenericDLLPtrList &rawdlls = header->get_dlls();
    printf("handling dlls for '%s'\n", header->get_file()->get_name().c_str());

    for (size_t j=0; j < rawdlls.size(); ++j) {
        std::string dll = rawdlls[j]->get_name()->c_str();
        printf("  looking for dll '%s' : ", dll.c_str());
        if (alreadyLoadedSet.find(dll) != alreadyLoadedSet.end()) {
            printf("already loaded\n");
            // already loaded TODO - check to make sure [MCB]
            continue;
        }
        printf(" searching paths...\n");

        for(size_t p=0; p < libPaths.size(); ++p){
            boost::filesystem::path libPath(libPaths[p]);
            libPath /= dll;
            std::string file=libPath.file_string();

            //FIXME !! tps (12/18/2009) : Commented out for now since it assumes BOOST 1.4 and does not compile for anyone else 
            // Re-enabling it to see what happens since this is an integral part of the algorithm. [RPM 2010-08-31]
            printf("      trying '%s':", file.c_str());
            if (!boost::filesystem::is_regular_file(libPath)) {
                printf(" file not found\n");
                continue;
            }

            // have to make sure it actually opens
            std::ifstream test(file.c_str());
            if (test.bad()) {
                printf(" could not be opened\n");
                continue;
            }

            printf(" good\n");
            files.push_back(file);
            break;
        }
    }
    return files;
}
#endif

/*************************************************************************************************************************
 * Low-level ELF stuff begins here.  More BinaryLoaderElf methods follow this stuff.
 *************************************************************************************************************************/

/* FIXME: Move this to src/ROSETTA where it belongs. [RPM 2010-08-31] */
typedef Rose_STL_Container<SgAsmElfSection*> SgAsmElfSectionPtrList;

namespace BinaryLoader_ElfSupport {
enum {
    VER_FLG_BASE=0x1,
    VER_FLG_WEAK=0x2,
    VERSYM_HIDDEN=0x8000
};


int get_verbose()
{
  return SgProject::get_verbose();
}

void prettyPrint(SgAsmElfSymbolSection* elfSection)
{
  SgAsmGenericFile* file = SageInterface::getEnclosingNode<SgAsmGenericFile>(elfSection);
  printf("SECTION: %s : %s\n", file->get_name().c_str(), elfSection->get_name()->c_str());
  for(size_t sym=0; sym< elfSection->get_symbols()->get_symbols().size(); ++sym){
    SgAsmElfSymbol* symbol = elfSection->get_symbols()->get_symbols()[sym];
    
    printf("%s %s %s %s\n",
           symbol->get_name()->c_str(),
	   symbol->stringifyDefState().c_str(),
	   symbol->stringifyType().c_str(),
	   symbol->stringifyBinding().c_str());
  }
}


//typedef std::map<std::string, SgAsmElfSymbol*> SymbolMap;

struct VersionedSymbol
{
public:
  explicit VersionedSymbol(SgAsmElfSymbol* symbol=NULL,
                           SgAsmElfSymbolSection* parent=NULL) : 
    p_symbol(symbol),
    p_parent(parent),
    p_version_entry(NULL),
    p_version_def(NULL),
    p_version_need(NULL){}
    

  bool get_is_local() const
  {
    if(p_version_entry && p_version_entry->get_value() == 0){
      return true;
    }
    else if(p_symbol->SgAsmElfSymbol::STB_LOCAL == p_symbol->get_elf_binding()){
      return true;
    }
    return false;
  }

  bool get_is_hidden() const
  {
    if(p_version_entry && p_version_entry->get_value() & VERSYM_HIDDEN){
      return true;
    }
    return false;
  }

  bool get_is_reference() const
  {
    return NULL != p_version_need;
  }

  bool get_is_base_definition() const
  {
    if(NULL == p_version_entry)
      return true;// unversioned entries are always considered "base"
    if(NULL == p_version_def)
      return false;// if its not a definition - its clearly not a base definition
    if(p_version_def && p_version_def->get_flags() & VER_FLG_BASE)
      return true;
    return false;
  }

  SgAsmElfSymbol* get_symbol() const {return p_symbol;}
  SgAsmElfSymbolSection* get_parent() const {return p_parent;}

  /** if empty string - there is no associated version */
  std::string get_version() const
  {
    if(p_version_def)
      return p_version_def->get_entries()->get_entries().front()->get_name()->c_str();
    else if(p_version_need)
      return p_version_need->get_name()->c_str();
    return std::string();
  }

  std::string get_name() const
  {
    return p_symbol->get_name()->c_str();
  }

  std::string dump_versioned_name() const
  {
    std::string name = get_name();
    if(get_is_hidden())
      name += "[HIDDEN]";
    if(p_version_def){
      if(p_version_def->get_flags() & VER_FLG_BASE)
	name += "[BASE]";

      SgAsmElfSymverDefinedAuxPtrList& entries=p_version_def->get_entries()->get_entries();
      name += entries.front()->get_name()->c_str();
      if(entries.size() > 1)
      {
	name += ";";
	for(size_t i=1;i < entries.size(); ++i){
	  name += " ";
	  name += entries[i]->get_name()->c_str();
	}
      }
    }
    if(p_version_need)
    {
      if(p_version_need->get_flags() & VER_FLG_WEAK)
	name += "[WEAK]";
      name += p_version_need->get_name()->c_str();
    }
    return name;
  }

  void set_symbol(SgAsmElfSymbol* symbol,SgAsmElfSymbolSection* parent) {p_symbol=symbol;p_parent=parent;}
  void set_version_entry(SgAsmElfSymverEntry* entry) {p_version_entry=entry;}
  void set_version_def(SgAsmElfSymverDefinedEntry* def) 
  { ROSE_ASSERT(def->get_flags() == 0 || def->get_flags() == VER_FLG_BASE);
    p_version_def=def;}

  void set_version_need(SgAsmElfSymverNeededAux* need) 
  { ROSE_ASSERT(need->get_flags() == 0 || need->get_flags() == VER_FLG_WEAK);
    p_version_need=need;}

  SgAsmElfSymverNeededAux* get_version_need() const{
    return p_version_need;
  }

  SgAsmElfSymverDefinedEntry* get_version_def() const{
    return p_version_def;
  }

private:
  SgAsmElfSymbol* p_symbol;
  SgAsmElfSymbolSection* p_parent;
  SgAsmElfSymverEntry* p_version_entry;
  SgAsmElfSymverDefinedEntry* p_version_def;
  SgAsmElfSymverNeededAux* p_version_need;
};




class SymbolMap
{
public:
  // for some reason the SymbolSection is not an ancestor of the Symbol, so we have to store it
  struct Entry 
  {
  public:
    const VersionedSymbol& get_vsymbol() const {return get_base_version();}

    SgAsmElfSymbol* get_symbol() const {return get_vsymbol().get_symbol();}
    VersionedSymbol get_vsymbol(const VersionedSymbol &version) const 
    {
      if(NULL == version.get_version_need())
	return get_base_version();

      std::string neededVersion=version.get_version_need()->get_name()->c_str();
      for(size_t i=0; i < p_versions.size(); ++i){
	SgAsmElfSymverDefinedEntry* def = p_versions[i].get_version_def();
	if(NULL == def)
	  continue;
	if(neededVersion == def->get_entries()->get_entries().front()->get_name()->c_str())
	  return p_versions[i];
      }
      ROSE_ASSERT(false);// TODO, handle cases where input uses versioning, but definition does not 
    }
    SgAsmElfSymbolSection* get_parent() const {return get_vsymbol().get_parent();}
    
    void addVersion(const VersionedSymbol& vsymbol)
    {
      if(vsymbol.get_is_base_definition())
      {
	// there can be only one "base" version
	ROSE_ASSERT(p_versions.empty() || false == get_vsymbol().get_is_base_definition());
	p_versions.push_back(p_versions.back());// swap the front to the back
	p_versions[0] = vsymbol;
      }
      else
	p_versions.push_back(vsymbol);
    }

    void merge(const Entry& newEntry)
    {
      VersionedSymbol oldSymbol=p_versions.front();
      VersionedSymbol newSymbol=newEntry.p_versions.front();

      int start=0;
      // first handle base
      if(oldSymbol.get_is_base_definition() && newSymbol.get_is_base_definition()){
	if(oldSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK &&
           newSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL){
          // the new symbol becomes the new base
	  p_versions[0] = newSymbol;
	  start=1;
	  if(!oldSymbol.get_version().empty() && 
             oldSymbol.get_version() != newSymbol.get_version()){
            // the old symbol was overridden, but it still has a unique
	    //  version - so we need to keep it
	    p_versions.push_back(oldSymbol);
	  }
	}
      }
      
      for(size_t i=start; i < newEntry.p_versions.size(); ++i){
	newSymbol = newEntry.p_versions[i];
	std::string newVersion=newSymbol.get_version();
	if(newVersion.empty()){
	  // if the version has no version - then it should have been the base entry
	  //  in which case it would have been handled above - since it isn't
	  //  this is some 'weaker' non-versioned entry, and it can be dropped
	  continue;
	}
	size_t found=false;
	for(size_t j=0; j < p_versions.size(); ++j){
	  oldSymbol = p_versions[j];
	  if(oldSymbol.get_version() == newSymbol.get_version() &&                    // matching version
             oldSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK && // old is weak
	     newSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL){// new is strong
            p_versions[j] = newSymbol;// override the old symbol, 
	    found=true;
	    break;
	  }
	}
	if(!found){
	  // this version doesn't exist - stick it on the list
	  p_versions.push_back(newSymbol);
	}
      }
    }

  private:
    const VersionedSymbol& get_base_version() const {ROSE_ASSERT(false == p_versions.empty()); return p_versions.front();}

    // if we have a base version, it will always be at the front - otherwise unsorted
    std::vector<VersionedSymbol> p_versions;
  };

  /* We store a list of Entries.  The first entry is the 'base' version
     the rest are additional versions.  If versioning is not used, the base version
     will be used
   */
  typedef std::map<std::string, Entry> BaseMap;

  const Entry* find(std::string name) const
  {
    BaseMap::const_iterator iter = p_base_map.find(name);
    if(p_base_map.end() ==iter){
      return NULL;
    }
    return &(iter->second);
  }

  const Entry* find(std::string name, std::string version) const
  {
    if(version.empty())
      return find(name);
    else
      return find(name + "("  + version + ")");
  }

  BaseMap& get_base_map() {return p_base_map;}
private:
  BaseMap p_base_map;
};

struct SymverResolver
{
  SymverResolver(SgAsmGenericHeader* header)
  {
    SgAsmElfSymbolSection* dynsym=NULL;
    SgAsmElfSymverSection* symver=NULL;
    SgAsmElfSymverDefinedSection* symver_def=NULL;
    SgAsmElfSymverNeededSection* symver_need=NULL;

    SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();    
    for(size_t sec=0; sec < sections.size(); ++sec){
      SgAsmGenericSection* section = sections[sec];
      if(isSgAsmElfSymbolSection(section) && isSgAsmElfSymbolSection(section)->get_is_dynamic()){
	dynsym = isSgAsmElfSymbolSection(section);
	//section->dump(stdout,"",0);
      }
      else if(isSgAsmElfSymverSection(section)){
	symver = isSgAsmElfSymverSection(section);
	//section->dump(stdout,"",0);
      }
      else if(isSgAsmElfSymverDefinedSection(section)){
	symver_def = isSgAsmElfSymverDefinedSection(section);
	//section->dump(stdout,"",0);
      }else if(isSgAsmElfSymverNeededSection(section)){
	symver_need=isSgAsmElfSymverNeededSection(section);
	//section->dump(stdout,"",0);
      }
    }

    if(NULL != symver_def){
      makeSymbolVersionDefMap(symver_def);
    }
    if(NULL != symver_need){
      makeSymbolVersionNeedMap(symver_need);
    }

    makeVersionedSymbolMap(dynsym, symver);
  }

  VersionedSymbol get_versioned_symbol(SgAsmElfSymbol* symbol) const{
    if(p_versionedSymbolMap.empty()){
      // we don't actually have versioned symbols, so return the identity version
      return VersionedSymbol(symbol);
    }
    VersionedSymbolMap::const_iterator iter = p_versionedSymbolMap.find(symbol);
    // We should have every symbol that might get looked up in here
    ROSE_ASSERT(p_versionedSymbolMap.end() != iter);
    return *(iter->second);
  }
private:

  void makeSymbolVersionDefMap(SgAsmElfSymverDefinedSection* section)
  {
    ROSE_ASSERT(NULL != section);
    SgAsmElfSymverDefinedEntryPtrList& defs = section->get_entries()->get_entries();
    for(size_t def=0; def < defs.size(); ++def){
      SgAsmElfSymverDefinedEntry* defEntry =  defs[def];
      p_symbolVersionDefMap[defEntry->get_index()]=defEntry;
    }
  }

  void makeSymbolVersionNeedMap(SgAsmElfSymverNeededSection* section)
  {
    ROSE_ASSERT(NULL != section);
    SgAsmElfSymverNeededEntryPtrList& needs = section->get_entries()->get_entries();
    for(size_t need=0; need < needs.size(); ++need){
      SgAsmElfSymverNeededEntry* needEntry = needs[need];
      SgAsmElfSymverNeededAuxPtrList& auxes = needEntry->get_entries()->get_entries();
      for(size_t aux=0; aux < auxes.size(); ++aux){
	SgAsmElfSymverNeededAux* auxEntry = auxes[aux];
	p_symbolVersionNeedMap[auxEntry->get_other()]=auxEntry;
      }
    }
  }

  /** Create a map from from each SgAsmElfSymbol* to a VersionedSymbol
      Prereqs: p_symbolVersionDefMap must be initialized before calling this
      Note: symver may be null, in which case VersionedSymbols are basically just
        a wrapper to their SgAsmElfSymbol.

   */
  void makeVersionedSymbolMap(SgAsmElfSymbolSection* dynsym,
			      SgAsmElfSymverSection* symver)
  {
    ROSE_ASSERT(dynsym && dynsym->get_is_dynamic());
    SgAsmElfSymbolPtrList& symbols = dynsym->get_symbols()->get_symbols();

    /// symverSection may be NULL, but if it isn't, it must have the same number of entries as dynsym
    ROSE_ASSERT(NULL == symver || symbols.size() == symver->get_entries()->get_entries().size());
    

    for(size_t sym=0; sym < symbols.size(); ++sym){
      SgAsmElfSymbol *symbol = symbols[sym];

      ROSE_ASSERT(p_versionedSymbolMap.end() == p_versionedSymbolMap.find(symbol));
      
      VersionedSymbol* versionedSymbol = new VersionedSymbol;
      p_versionedSymbolMap.insert(std::make_pair(symbol,versionedSymbol));

      versionedSymbol->set_symbol(symbol,dynsym);
      if(symver){
	SgAsmElfSymverEntry *symverEntry = symver->get_entries()->get_entries()[sym];
	uint16_t value=symverEntry->get_value();
	
	if(0x8000 & value){
	  //  VERSYM_HIDDEN = 0x8000 - if set, we should actually completely ignore the symbol
	  //                  unless this version is specifically requested (which is the normal case anwyay?)
	  //                  at any rate - we need to strip bit-15
	  //                  This appears to be poorly documented by the spec
	  value = value & 0x7fff;
	}

	if(0 == value || 1 == value){
	  // 0 and 1 are special (local and global respectively) they DO NOT correspond to entries on symver_def
	  //  Also, (no documentation for this) if bit 15 is set, this symbol should be ignored?
	  versionedSymbol->set_version_entry(symverEntry);	  
	}
	else{
	 
	  SymbolVersionDefinitionMap::const_iterator defIter = p_symbolVersionDefMap.find(value);
	  SymbolVersionNeededMap::const_iterator needIter = p_symbolVersionNeedMap.find(value);
	  ROSE_ASSERT((p_symbolVersionDefMap.end() == defIter) != 
		      (p_symbolVersionNeedMap.end() == needIter)); // we must have a match from defs or needs, not both,not neither
	  versionedSymbol->set_version_entry(symverEntry);
	  if(p_symbolVersionDefMap.end() != defIter)
	    versionedSymbol->set_version_def(defIter->second);
	  else if(p_symbolVersionNeedMap.end() != needIter)
	    versionedSymbol->set_version_need(needIter->second);
	}
      }
    }
  }


  typedef std::map<SgAsmElfSymbol*, VersionedSymbol*> VersionedSymbolMap;
  typedef std::map<uint16_t, SgAsmElfSymverDefinedEntry*> SymbolVersionDefinitionMap;
  typedef std::map<uint16_t, SgAsmElfSymverNeededAux*> SymbolVersionNeededMap;

  SymbolVersionDefinitionMap p_symbolVersionDefMap;
  SymbolVersionNeededMap p_symbolVersionNeedMap;
  VersionedSymbolMap p_versionedSymbolMap;
};


/** 
    Relocation of Jump Table Entry (e.g. jump slot).
    A call to a library looks like the following :
      .text: [EXECUTABLE READ ONLY]
        ...
A       call 0x000002080 <file relative address in plt>
F       ...

      .plt [EXECUTABLE READ ONLY]
        ... 
B 0x2080 jmp *03030 <file relative address in .got.plt>
C 0x2086 push 0x4 <This is the 4th function in the plt>
D 0x208b jmp *0x00002018 <basically a call to dynamic linker>

      .got.plt [NOT EXECUTABLE WRITABLE]
        ...
Z 0x3030 : 0x2086
	


LIBRARY : 
   .text:
     ...
E 0xA018: FOO <do function stuff here>
  ...
  ret
 

In the above example, in "normal" operation, we would see:
The FIRST time we call the "foo"
A
B <jumps to *Z, which starts pointing at C> 
C
D <change Z to the value of E: 0xA018, call Foo directly>
E <execute Foo>
F 

The SECOND time, we will have called the dynamic linker, and C and D are no inert
A
B <jumps to *Z which is NOW just E>
E <execute Foo>
F

In this relocation, we need to mimic the dynamic linker, and overwrite the 
.got entry (Z) with the address of the function (E i.e. Foo)
Thus, we're performing
*Z = E

*/
SgAsmGenericSection* find_mapped_section(SgAsmGenericHeader* header,
                                         rose_addr_t va)
{
  SgAsmGenericSectionPtrList sections = header->get_sections_by_va(va);
  SgAsmGenericSection* section=NULL;
  for(size_t sec=0; sec < sections.size(); ++sec){
    SgAsmElfSection* elfSection = isSgAsmElfSection(sections[sec]);
    if(NULL == elfSection)
      continue;
    ROSE_ASSERT(true == elfSection->is_mapped());// this should be guaranteed by get_sections_by_rva

    if(NULL == elfSection->get_section_entry())
      continue;
    if((elfSection->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_TLS)
       && (elfSection->get_section_entry()->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS)){
      //TODO handle .tbss correctly
      continue;
    }

    // there should be only one - could also just assume its the case, and break
    ROSE_ASSERT(NULL == section);
    section = elfSection;
  }
  return section;
}

void 
relocate_X86_JMP_SLOT(SgAsmElfRelocEntry* reloc,
		      SgAsmElfRelocSection* parentSection,
		      const SymbolMap &masterSymbolMap,
		      const SymverResolver &resolver,
		      const size_t addrSize)
{
  ROSE_ASSERT(NULL != reloc);
  ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

  SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
  ROSE_ASSERT(NULL != linkedSymbolSection); 

  SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
  ROSE_ASSERT(NULL != relocSymbol);

  VersionedSymbol relocVSymbol = resolver.get_versioned_symbol(relocSymbol);
  if(get_verbose() > 0)
    printf("relocVSymbol: %s\n", relocVSymbol.dump_versioned_name().c_str());

  std::string symbolName=relocVSymbol.get_name();
  const SymbolMap::Entry *symbolEntry = masterSymbolMap.find(symbolName);
  
  if(NULL == symbolEntry){ // TODO check for weak relocsymbol
    printf("Could not find symbol '%s'\n", relocSymbol->get_name()->c_str());
    return;
  }

  VersionedSymbol sourceVSymbol = symbolEntry->get_vsymbol(relocVSymbol);
  SgAsmElfSymbol* sourceSymbol=sourceVSymbol.get_symbol();
  ROSE_ASSERT(NULL != sourceSymbol);

  ROSE_ASSERT(0 != sourceSymbol->get_st_shndx());// test an assumption

  
  SgAsmGenericHeader* symbolHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(sourceVSymbol.get_parent());


  // The symbol contains a virtual address in a section in the same file as the symbol
  //  however, since that section may have been moved, we need to adjust the resulted symbol value (an address)
  //  to do that, we have to find the relavant section via va (i.e. preferred address)
  SgAsmGenericSection* sourceSection=find_mapped_section(symbolHeader,sourceSymbol->get_value());

  // investiage SHN_COMMON


  SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
  SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
  ROSE_ASSERT(NULL != targetSection);

  // reloc->get_r_offset is the va assuming the library was not moved
  //  so we have to adjust it by the same amount the library WAS moved
  const rose_addr_t reloc_va = 
    reloc->get_r_offset() + (targetSection->get_mapped_actual_rva() - targetSection->get_mapped_preferred_rva());

  // offset from start of section 
  rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

  const rose_addr_t symbol_va = 
    sourceSymbol->get_value() + (sourceSection->get_mapped_actual_rva() - sourceSection->get_mapped_preferred_rva());

  SgFileContentList contents = targetSection->get_data();

  if(addrSize == 4){
    ROSE_ASSERT(symbol_va <= UINT_MAX);
    uint32_t reloc_value_32 = symbol_va;
    memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
  }
  else if(addrSize == 8){
    // TODO Throw some asserts around this
    memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
  }
  
  if(get_verbose() > 0){
    const char* targetSectionName = targetSection->get_name()->c_str();
    const char* sourceSectionName = "";
    if(sourceSection)
      sourceSectionName = sourceSection->get_name()->c_str();

    printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
  }


}

void 
relocate_X86_64_RELATIVE(SgAsmElfRelocEntry* reloc,
                         SgAsmElfRelocSection* parentSection,
			 const SymbolMap &masterSymbolMap,
			 const SymverResolver &resolver,
			 const size_t addrSize)
{
  ROSE_ASSERT(NULL != reloc);
  ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

  //SgAsmElfSection* linkedSection = parentSection->get_linked_section();
  //ROSE_ASSERT(NULL != linkedSymbolSection); 

  ROSE_ASSERT(0 == reloc->get_sym());// _RELATIVE relocs must set their symbol index to zero

  SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
  SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
  ROSE_ASSERT(NULL != targetSection);

  rose_addr_t targetSectionShift=(targetSection->get_mapped_actual_rva() - targetSection->get_mapped_preferred_rva());
  if(targetSectionShift == 0){
    // _RELATIVE locations are used to shift address so they are still pointing at the same object
    //  of the section was loaded at the 'preferred' address, nothing needs to be done
    return;
  }

  // reloc->get_r_offset is the va assuming the library was not moved
  //  so we have to adjust it by the same amount the library WAS moved
  const rose_addr_t reloc_va = reloc->get_r_offset() + targetSectionShift;

  // offset from start of section 
  rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

  SgFileContentList contents = targetSection->get_data();

  // The new value is simply the address (plus the addend - if any)
  const rose_addr_t reloc_value = reloc_va + reloc->get_r_addend();

  if(addrSize == 4){
    ROSE_ASSERT(reloc_value <= (uint64_t)(UINT_MAX));
    uint32_t reloc_value_32 = reloc_value;
    memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
  }
  else if(addrSize == 8){
    // TODO Throw some asserts around this
    memcpy(&(contents.at(relocSectionOffset)),&reloc_va,addrSize);
  }
  
  if(get_verbose() > 0){
    const char* targetSectionName = targetSection->get_name()->c_str();

    printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"\n",reloc_va,targetSectionName,reloc_value);
  }
}

void 
relocate_X86_64_64(SgAsmElfRelocEntry* reloc,
                   SgAsmElfRelocSection* parentSection,
		   const SymbolMap &masterSymbolMap,
		   const SymverResolver &resolver,
		   const size_t addrSize)
{
  ROSE_ASSERT(NULL != reloc);
  ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

  SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
  ROSE_ASSERT(NULL != linkedSymbolSection); 

  SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
  ROSE_ASSERT(NULL != relocSymbol);

  VersionedSymbol relocVSymbol = resolver.get_versioned_symbol(relocSymbol);
  if(get_verbose() > 0)
    printf("relocVSymbol: %s\n", relocVSymbol.dump_versioned_name().c_str());

  std::string symbolName=relocVSymbol.get_name();
  const SymbolMap::Entry *symbolEntry = masterSymbolMap.find(symbolName);
  
  if(NULL == symbolEntry){ // TODO check for weak relocsymbol
    printf("Could not find symbol '%s'\n", relocSymbol->get_name()->c_str());
    return;
  }

  VersionedSymbol sourceVSymbol = symbolEntry->get_vsymbol(relocVSymbol);
  SgAsmElfSymbol* sourceSymbol=sourceVSymbol.get_symbol();
  ROSE_ASSERT(NULL != sourceSymbol);

  ROSE_ASSERT(0 != sourceSymbol->get_st_shndx());// test an assumption

  
  SgAsmGenericHeader* symbolHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(sourceVSymbol.get_parent());


  // The symbol contains a virtual address in a section in the same file as the symbol
  //  however, since that section may have been moved, we need to adjust the resulted symbol value (an address)
  //  to do that, we have to find the relavant section via va (i.e. preferred address)
  SgAsmGenericSection* sourceSection=find_mapped_section(symbolHeader,sourceSymbol->get_value());

  // investiage SHN_COMMON


  SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
  SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
  ROSE_ASSERT(NULL != targetSection);

  // reloc->get_r_offset is the va assuming the library was not moved
  //  so we have to adjust it by the same amount the library WAS moved
  const rose_addr_t reloc_va = 
    reloc->get_r_offset() + (targetSection->get_mapped_actual_rva() - targetSection->get_mapped_preferred_rva());

  // offset from start of section 
  rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

  const rose_addr_t symbol_va = 
    sourceSymbol->get_value() + (sourceSection->get_mapped_actual_rva() - sourceSection->get_mapped_preferred_rva());

  SgFileContentList contents = targetSection->get_data();
  rose_addr_t reloc_value = symbol_va + reloc->get_r_addend();

  if(addrSize == 4){
    ROSE_ASSERT(reloc_value <= (uint64_t)(UINT_MAX));
    uint32_t reloc_value_32 = reloc_value;
    memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
  }
  else if(addrSize == 8){
    // TODO Throw some asserts around this
    memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
  }
  
  if(get_verbose() > 0){
    const char* targetSectionName = targetSection->get_name()->c_str();
    const char* sourceSectionName = "";
    if(sourceSection)
      sourceSectionName = sourceSection->get_name()->c_str();

    printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,reloc_value,sourceSectionName);
  }
}


void performRelocation(SgAsmElfRelocEntry* reloc, 
                       SgAsmElfRelocSection* parentSection,
		       const SgAsmElfSectionPtrList& extentSortedSections,
		       const SymverResolver &resolver,
		       const SymbolMap& masterSymbolMap)
{
  ROSE_ASSERT(NULL != reloc);
  ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

  SgAsmGenericHeader* header = parentSection->get_header();
  ROSE_ASSERT(NULL != header);// need the header to determine ISA

  SgAsmGenericHeader::InsSetArchitecture isa = header->get_isa();

  if(get_verbose())
  {
    SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
    ROSE_ASSERT(NULL != linkedSymbolSection);
    
    SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
    ROSE_ASSERT(NULL != relocSymbol);
    
    SgAsmElfSection* targetSection=parentSection->get_target_section();
    ROSE_ASSERT(NULL != targetSection);
    std::string symbolName=relocSymbol->get_name()->c_str();

    std::string relocStr;
    if(header)
      relocStr = reloc->to_string(reloc->get_type(),isa);
    printf("%16s '%25s' 0x%016lx",relocStr.c_str(),symbolName.c_str(),reloc->get_r_offset());
  }

    // TODO support other than x86
    switch(isa & SgAsmGenericHeader::ISA_FAMILY_MASK){
      case SgAsmGenericHeader::ISA_IA32_Family:
	switch(reloc->get_type()){
	  //case SgAsmElfRelocEntry::R_386_JMP_SLOT:relocate_X86_JMP_SLOT(reloc,parentSection,masterSymbolMap,resolver,extentSortedSections,4);break;
	    //case SgAsmElfRelocEntry::R_386_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,4);break;
	    //case SgAsmElfRelocEntry::R_386_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;
	    //	  case SgAsmElfRelocEntry::R_386_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
	  case SgAsmElfRelocEntry::R_386_TLS_TPOFF:
	  case SgAsmElfRelocEntry::R_386_TLS_IE:
	  case SgAsmElfRelocEntry::R_386_TLS_GOTIE:
	  case SgAsmElfRelocEntry::R_386_TLS_LE:
	  case SgAsmElfRelocEntry::R_386_TLS_GD:
	  case SgAsmElfRelocEntry::R_386_TLS_LDM:
	  case SgAsmElfRelocEntry::R_386_TLS_GD_32:
	  case SgAsmElfRelocEntry::R_386_TLS_GD_PUSH:
	  case SgAsmElfRelocEntry::R_386_TLS_GD_CALL:
	  case SgAsmElfRelocEntry::R_386_TLS_GD_POP:
	  case SgAsmElfRelocEntry::R_386_TLS_LDM_32:
	  case SgAsmElfRelocEntry::R_386_TLS_LDM_PUSH:
	  case SgAsmElfRelocEntry::R_386_TLS_LDM_CALL:
	  case SgAsmElfRelocEntry::R_386_TLS_LDM_POP:
	  case SgAsmElfRelocEntry::R_386_TLS_LDO_32:
	  case SgAsmElfRelocEntry::R_386_TLS_IE_32:
	  case SgAsmElfRelocEntry::R_386_TLS_LE_32:
	  case SgAsmElfRelocEntry::R_386_TLS_DTPMOD32:
	  case SgAsmElfRelocEntry::R_386_TLS_DTPOFF32:
	  case SgAsmElfRelocEntry::R_386_TLS_TPOFF32:
	    printf("%s%s\n", "<unresolved>: Thread Local Storage not supported",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str()); break;
	  default:
	    printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
	};
	break;
      case SgAsmGenericHeader::ISA_X8664_Family:
	switch(reloc->get_type()){
	  case SgAsmElfRelocEntry::R_X86_64_JUMP_SLOT:relocate_X86_JMP_SLOT(reloc,parentSection,masterSymbolMap,resolver,8);break;
	  case SgAsmElfRelocEntry::R_X86_64_GLOB_DAT:relocate_X86_JMP_SLOT(reloc,parentSection,masterSymbolMap,resolver,8);break;
	    //case SgAsmElfRelocEntry::R_X86_64_32:      relocate_X86_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should zero extend symbol value
	  //	  case SgAsmElfRelocEntry::R_X86_64_32S:     relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should sign extend symbol value
	  case SgAsmElfRelocEntry::R_X86_64_64:relocate_X86_64_64(reloc,parentSection,masterSymbolMap,resolver,8);break;
	  case SgAsmElfRelocEntry::R_X86_64_RELATIVE:relocate_X86_64_RELATIVE(reloc,parentSection,masterSymbolMap,resolver,8);break;
	  default:
	    printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());	  
	};
	break;
      default:
	printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
    };
}


void performRelocations(SgAsmElfFileHeader* elfHeader, 
			const SgAsmElfSectionPtrList& extentSortedSections,
			const SymbolMap& masterSymbolMap)
{
  SymverResolver resolver(elfHeader);  
  SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();    
  for(size_t sec=0; sec < sections.size(); ++sec){
    SgAsmElfRelocSection* relocSection = isSgAsmElfRelocSection(sections[sec]);
    if(NULL == relocSection)
      continue;

    SgAsmElfRelocEntryPtrList &relocs = relocSection->get_entries()->get_entries();
    for(size_t r=0; r <  relocs.size(); ++r){
      SgAsmElfRelocEntry* reloc = relocs[r];
      performRelocation(reloc, relocSection, extentSortedSections,resolver,masterSymbolMap);
    }
  }
}

#if 0
VersionedSymbol makeVersionedSymbol(SgAsmElfSymbol* symbol)
{
  VersionedSymbol vsymbol(symbol);
  SgAsmElfSymverSection* symver=NULL;
  SgAsmElfSymverDefinedSection* symver_def=NULL;
  SgAsmElfSymverNeededSection* symver_need=NULL;

  SgAsmElfFileHeader* header = SageInterface::getEnclosingNode<SgAsmElfFileHeader>(symbol);
  if(header == NULL){
    // somehow free floating - just return it
    return vsymbol;
  }
  SgAsmElfSymbolSection* dynsym = SageInterface::getEnclosingNode<SgAsmElfSymbolSection>(symbol);
  ROSE_ASSERT(NULL != dynsym);
  if(false == dynsym->get_is_dynamic()){
    // only the dynamic symbol table contains versioned symbols
    return vsymbol;
  }

  SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();    
  for(size_t sec=0; sec < sections.size(); ++sec){
    SgAsmGenericSection* section = sections[sec];
    if(isSgAsmElfSymverSection(section)){
      symver = isSgAsmElfSymverSection(section);
    }
    else if(isSgAsmElfSymverDefinedSection(section)){
      symver_def = isSgAsmElfSymverDefinedSection(section);
    }
    else if(isSgAsmElfSymverDefinedSection(section)){
      symver_need = isSgAsmElfSymverNeededSection(section);
    }
  }
  if(NULL == symver){
    // symbol versioning not present here, return unversioned
    return vsymbol; 
  }
  
}
#endif

// [ Reference Elf TIS Portal Formats Specification, Version 1.1 ] 
bool relocateInterpLibraries(SgAsmInterpretation* interp)
{
  bool verbose = get_verbose() > 1;
  // build map for each symbol table map<std::string,SgElfSymbol>
  // compute global symbol resolution
  //   in the DT_SYMBOLIC case, start w/ local map(s) before proceeding
  //
  // build map from SgAsmGenericSection to its symbolmap map<SgAsmGenericSection*,SymbolMap>
  // biuld list of
  typedef BinaryLoader_ElfSupport::SymbolMap SymbolMap;
  typedef SymbolMap::BaseMap BaseMap;
 

  SgAsmGenericHeaderPtrList& headers = interp->get_headers()->get_headers();

  typedef std::vector<std::pair<SgAsmGenericSection*, BaseMap*> > SymbolMapList;
  SymbolMapList symbolMaps;

  for(size_t h=0; h < headers.size(); ++h){
    SgAsmGenericHeader* header = headers[h];
    SymverResolver versionResolver(header);

    SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();
    
    for(size_t sec=0; sec < sections.size(); ++sec){
      SgAsmGenericSection* section = sections[sec];
      
      SgAsmElfSymbolSection* elfSection = isSgAsmElfSymbolSection(section);
      if(NULL == elfSection){
	continue;
      }
      if(SgAsmElfSectionTableEntry::SHT_DYNSYM != elfSection->get_section_entry()->get_sh_type()){
	continue;
      }
      /** 
	  Manual 1-10 under SHT_SYMTAB and SHT_DYNSYM:
	  ...SHT_DYNSYM section holds a minimal set of dynamic linking symbols
	  NOTE: Technically, the dynsym may be omitted, and we should truly
	  use the .dynamic section's DT_SYMTAB entry(ies)
      */
      BaseMap* symbolMap = new BaseMap;
      // Note: We should be able to safely skip sym==0, which "must" be undefined
      for(size_t sym=0; sym< elfSection->get_symbols()->get_symbols().size(); ++sym){
	SgAsmElfSymbol* symbol = elfSection->get_symbols()->get_symbols()[sym];
	if(verbose)
	  printf("Considering symbol [%zu] - '%s' : ", sym, symbol->get_name()->c_str());

	VersionedSymbol symver = versionResolver.get_versioned_symbol(symbol);
	if(symver.get_is_local()){
	  if(verbose)
	    printf("ignored - local\n");
	  // symbols in the dynsym should never be local (if this is isn't the case, we can ignore them)
	  // symbol versioning also may make symbols "local"
	  continue;
	}
	if(symver.get_is_hidden()){
	  if(verbose) printf("ignored - hidden\n");

	  // symbol versioning can result in 'hidden' symbols that should be ignored
	  continue;
	}
	if(symver.get_is_reference()){
	  if(verbose) printf("ignored - reference\n");
	  // symbol versioning lets us determine which symbols are 'references' i.e. 
	  //  that are only used to help relocating
	  continue;
	}
	if(verbose) printf("\n");
	std::string symName=symver.get_name();
	SymbolMap::Entry &symbolEntry = (*symbolMap)[symName];

	symbolEntry.addVersion(symver);
      }

      symbolMaps.push_back(std::make_pair(elfSection,symbolMap));
    }
  }
  SymbolMap masterSymbolMap;
  BaseMap& masterSymbolMapBase=masterSymbolMap.get_base_map();

  for( size_t i=0; i < symbolMaps.size(); ++i){
    const BaseMap* symbolMap = symbolMaps[i].second;
    BaseMap::const_iterator newSymbolIter = symbolMap->begin();

    for(; newSymbolIter != symbolMap->end(); ++newSymbolIter){
      const SymbolMap::Entry &newEntry = newSymbolIter->second;
      const std::string symName = newSymbolIter->first;

      BaseMap::iterator oldSymbolIter=masterSymbolMapBase.find(symName);

      if(masterSymbolMapBase.end() == oldSymbolIter){
	// no symbol yet, set it [this is the most common case]
	masterSymbolMapBase[symName] = newSymbolIter->second;
	continue;
      }
      else{
	// otherwise, we have to go through version by version to 'merge' 
	//  a complete map
	oldSymbolIter->second.merge(newEntry);
      }
    }
  }



  SgAsmElfSectionPtrList mappedSections;

  for(size_t h=0; h < headers.size(); ++h){
    SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(headers[h]);
    ROSE_ASSERT(elfHeader != NULL);
    
    SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();    
    for(size_t i=0; i < sections.size(); ++i){
      if(sections[i]->is_mapped()){
	ROSE_ASSERT(NULL != isSgAsmElfSection(sections[i]));
	mappedSections.push_back(isSgAsmElfSection(sections[i]));
      }
    }
  }
  SgAsmElfSectionPtrList extentSortedSections;
  //std::sort(mappedSections.begin(),mappedSections.end(),extentSorterActualVA);

  for(size_t h=0; h < headers.size(); ++h){
    SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(headers[h]);
    ROSE_ASSERT(NULL != elfHeader);
    performRelocations(elfHeader, extentSortedSections,masterSymbolMap);
  }  
  return true;
}

}// end namespace BinaryLoader_ElfSupport


/*************************************************************************************************************************
 * End of low-level ELF stuff.  Now back to the BinaryLoaderElf class itself....
 *************************************************************************************************************************/



void
BinaryLoaderElf::fixup(SgAsmInterpretation *interp)
{
    BinaryLoader_ElfSupport::relocateInterpLibraries(interp);

    /*
    // 1. Get section map (name -> list<section*>)
    getSectionNameMap(binaryFile,allSectionsMap,loadableSectionsMap);

    // 2. Create Symbol map from relevant sections (.dynsym) 
    SymbolMap symbolMap;
    getSymbolMap(allSectionsMap,symbolMap);

    // 3. Create Extent sorted list of loadable sections
    SgAsmElfSectionPtrList extentSortedSections;
    sortSections(loadableSectionsMap,extentSortedSections);

    // 4. Collect Relocation Entries.
    RelocationEntryList relocations;
    getRelocationEntries(binaryFile, relocations);

    // 5.  for each relocation entry, perform relocation
    processRelocations(relocations,symbolMap,extentSortedSections);
    */
}


/* This is a big section of commented out code. I'm prefixing all lines with "//" to make it more obvious. [RPM 2010-08-31] */
// #if 0
// 
// typedef std::map<string,SgAsmElfSectionPtrList> SectionNameMap;
// 
// class ElfSectionCollector : public AstSimpleProcessing
// {
// public:
//   SectionNameMap mAllSections;
//   SectionNameMap mLoadableSections;
// 
//   virtual void visit(SgNode* node)
//   {
//     SgAsmElfSection* elfSection = isSgAsmElfSection(node);
//     if(elfSection == NULL)
//       return; // not an elf section
//     
//     // TODO we shouldn't really be using 'name' as a designator, rather, we should use sectionEntry flags
//     string name = elfSection->get_name()->c_str();
//     SgAsmElfSectionPtrList& allEntryList = mAllSections[name];
//     if(!allEntryList.empty() && elfSection == allEntryList.back()){
//       // we're getting duplicates in visit
//       return;
//     }
//     
//     allEntryList.push_back(elfSection);
//     
//     bool isLoadable=true;
//     const SgAsmElfSectionTableEntry* sectionEntry = elfSection->get_section_entry();
//     if(sectionEntry) {
//       const uint64_t flags = sectionEntry->get_sh_flags();
//       if(!(flags & SgAsmElfSectionTableEntry::SHF_ALLOC )){
// 	// Elf lets us completely skip loading a section if its not SHF_ALLOC
// 	isLoadable=false;
//       }
//       if((flags & SgAsmElfSectionTableEntry::SHF_TLS) && 
//          sectionEntry->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS){ // .tbss
//         //TODO support Thread Local Storage.  In particular, .tbss gives us
// 	//  issues because it occupies zero disk and zero LOGICAL space
// 	isLoadable=false;
//       }
//     }
//     if(isLoadable){
//       mLoadableSections[name].push_back(elfSection);
//     }
//   }
// };
// 
// void getSectionNameMap(SgBinaryComposite* binaryFile,
//                        SectionNameMap& allSections,
// 		       SectionNameMap& loadedSections)
// {
//   ElfSectionCollector visitor;
//   visitor.traverse(binaryFile,preorder);
//   allSections = visitor.mAllSections;
//   loadedSections = visitor.mLoadableSections;
// }
// 
// 
// struct ElfSymbolMapEntry
// {
//   ElfSymbolMapEntry() : symbol(NULL){};
// public:
//   rose_addr_t get_va() const
//   { return symbol->get_value() + section->get_base_va();}
// 
//   SgAsmElfSymbol* symbol;
//   SgAsmGenericSection* section;
// };
// 
// /// map from symbol name to a symbol entry
// typedef Rose_STL_Container<ElfSymbolMapEntry> ElfSymbolMapEntryList;
// typedef std::map<string, ElfSymbolMapEntryList> SymbolMap;
// 
// 
// class SymbolCollector : public AstSimpleProcessing
// {
// public:
//   SymbolMap _symbolMap;
//   virtual void visit(SgNode* node)
//   {
//     if (isSgAsmElfSymbol(node)){
//       SgAsmElfSymbol* symbol = isSgAsmElfSymbol(node);
//       SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(symbol);
//       string symbolName = symbol->get_name()->c_str();
//       if(symbolName.empty()){
// 	return;
//       }
//       ElfSymbolMapEntryList& entryList = _symbolMap[symbolName];
//       if(header == NULL){
// 	return;
//       }
//       SgAsmGenericSectionPtrList sections = header->get_sections_by_va(symbol->get_value());
//       
//       ROSE_ASSERT(!sections.empty());
//       SgAsmGenericSectionPtrList::iterator iter = sections.begin();
//       for(; iter != sections.end(); ++iter){
// 	SgAsmGenericSection* section = *iter;
// 	SgAsmElfSection* elfSection = isSgAsmElfSection(section);
// 	
// 	// TODO document this
// 	if((!elfSection) || 
//            (elfSection->get_section_entry() && 
//             !(elfSection->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_ALLOC)))
// 	{
// 	  continue;
// 	}
// 	
// 	ElfSymbolMapEntry addr;
// 	addr.section = *iter;
// 	addr.symbol = symbol;
// 	entryList.push_back(addr);
//       }
//     }
//   }
// };
// 
// 
// void getSymbolMap( const SectionNameMap &allSectionsMap,SymbolMap &symbolMap)
// {
// 
//   SectionNameMap::const_iterator findIter=allSectionsMap.find(".dynsym");
//   ROSE_ASSERT(findIter != allSectionsMap.end());// must have at least one dynamic symbol table
//   
//   const SgAsmElfSectionPtrList& dynsymSections = findIter->second;
//   SymbolCollector symbolCollector;
//   {
//     SgAsmElfSectionPtrList::const_iterator sectionIter=dynsymSections.begin();
//     for(; sectionIter != dynsymSections.end(); ++sectionIter){
//       symbolCollector.traverse(*sectionIter,preorder);
//     }
//   }
//   symbolMap = symbolCollector._symbolMap;
// }
// 
// bool extentSorterActualVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_va(),
// 		    lhs->get_mapped_size()))
//     < 
//     (std::make_pair(rhs->get_mapped_actual_va(),
// 		    rhs->get_mapped_size()));
// }
// 
// bool extentSorterPreferedRVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_rva(),
// 		    lhs->get_mapped_size()))
//     < 
//     (std::make_pair(rhs->get_mapped_actual_rva(),
// 		    rhs->get_mapped_size()));
// }
// 
// void sortSections(const SectionNameMap& sectionMap,SgAsmElfSectionPtrList& sortedSections)
// {
//   SectionNameMap::const_iterator sectionIter=sectionMap.begin();
// 
//   for( ;sectionIter!=sectionMap.end();++sectionIter){
//     sortedSections.insert(sortedSections.end(),sectionIter->second.begin(),sectionIter->second.end());
//   }
//   std::sort(sortedSections.begin(),sortedSections.end(),extentSorterActualVA);
//   sortedSections.erase(std::unique(sortedSections.begin(), sortedSections.end()), sortedSections.end());
// }
// struct RelocationEntry
// {
//   SgAsmElfRelocEntry* reloc;
//   SgAsmElfSymbol* symbol;
// };
// 
// typedef Rose_STL_Container<RelocationEntry> RelocationEntryList;
// typedef Rose_STL_Container<SgAsmElfRelocSection*> SgAsmElfRelocSectionPtrList;
// void getRelocationEntries(SgBinaryComposite *binaryFile,RelocationEntryList& relocs)
// {
// #if 0
// 
//   SgAsmElfRelocSectionPtrList relocSections = 
//     SageInterface::querySubTree<SgAsmElfRelocSection>(binaryFile,V_SgAsmElfRelocSection);
// 
//   SgAsmElfRelocSectionPtrList::const_iterator sectionIter = relocSections.begin();
//   for(;sectionIter != relocSections.end();++sectionIter){
//     SgAsmElfRelocSection* section = *sectionIter;
// 
//     SgAsmElfSymbolSection *symtab=isSgAsmElfSymbolSection(section->get_linked_section());
//     ROSE_ASSERT(symtab != NULL);
// 
//     SgAsmElfRelocEntryPtrList& entries = section->get_entries()->get_entries();    
//     SgAsmElfRelocEntryPtrList::iterator iter = entries.begin();
//     for(; iter != entries.end(); ++iter)
//     {
//       SgAsmElfRelocEntry* reloc = *iter;
//       RelocationEntry entry;
//       entry.reloc = reloc;
//       entry.symbol = symtab->get_symbols()->get_symbols()[reloc->get_sym()];
//       relocs.push_back(entry);
//     }	    
//   }
// #endif
// }
// 
// string sectionString(SgAsmGenericSection* section)
// {
//   string str;
//   SgAsmGenericFile* file = SageInterface::getEnclosingNode<SgAsmGenericFile>(section);
//   if(file)
//     str += file->get_name();
//   else
//     str += "<none>";
//   
//   str += "[";
//   str += section->get_name()->c_str();
//   str += "]";
//   return str;
// }
// 
// // TODO use sh_type instead of section name
// ElfSymbolMapEntry
// findSection(const ElfSymbolMapEntryList& symbolEntry,
// 	    const string& sectionName)
// {
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     if(iter->section->get_name()->c_str() == sectionName)
//       return *iter;
//   }
//   return ElfSymbolMapEntry();
// }
// 
// SgAsmElfSectionPtrList sectionsOfAddress(rose_addr_t va_addr,
//                                          const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
// 
// SgAsmElfSection* chooseSectionAtAddress(rose_addr_t va_addr,
// 					const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   // we always prefer sections to segments (both of which are SgAsmElfSection's 
//   SgAsmElfSection* segment=NULL;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       if(section->get_section_entry())
// 	return section;
//       else
// 	segment = section;
//     }
//   }
//   return segment;
// }					
// 
// // TODO use sh_type instead of sectionName
// SgAsmElfSection* 
// findElfSection(const SgAsmElfSectionPtrList& sections,
//                const string& sectionName)
// {
//   for(size_t i=0; i < sections.size(); ++i){
//     if(sections[i]->get_name()->c_str() == sectionName)
//       return sections[i];
//   }
//   return NULL;
// }
// 
// ElfSymbolMapEntryList removeSegments(const ElfSymbolMapEntryList &symbolEntry)
// {
//   ElfSymbolMapEntryList returnList;
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     SgAsmElfSection* section = isSgAsmElfSection(iter->section);
//     if(section && section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnList.push_back(*iter);
//     }
//   }
//   return returnList;
// }
// 
// SgAsmElfSectionPtrList removeSegments(const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
// 
// 
// string stringOfSectionsOfAddress(rose_addr_t va_addr,
// 				      const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   std::vector<SgAsmElfSection*> sections = sectionsOfAddress(va_addr,extentSortedSections);
//   SgAsmElfSection* bestSection = NULL;
// 
//   for(size_t i=0; i < sections.size(); ++i)
//   {
//     SgAsmElfSection* section = sections[i];
//     if(bestSection == NULL || bestSection->get_segment_entry())
//       bestSection = section;
//   }
//   if(bestSection == NULL)
//   {
//     return "<unknown>";
//   }
//   else
//   {
//     return sectionString(bestSection);
//   }
// }
// 
// void printSymbolMapEntry(const ElfSymbolMapEntryList& entries,
//                          const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   printf("'%s':\n", entries.front().symbol->get_name()->c_str());
// 
//   for(size_t i=0; i < entries.size(); ++i){
//     const ElfSymbolMapEntry& entry = entries[i];
//     rose_addr_t symbol_va = entry.get_va();
//     printf(" %01d 0x%016x %s\n", i, symbol_va, sectionString(entry.section).c_str());
//     printf("   %s\n",stringOfSectionsOfAddress(symbol_va,extentSortedSections).c_str());
//   }
// }
// 
// void 
// printSymbolMap(const SymbolMap& symbolMap,
//                const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SymbolMap::const_iterator iter = symbolMap.begin();
//   SymbolMap::const_iterator iterEnd = symbolMap.end();
//   for(; iter != iterEnd; ++iterEnd){
//     const ElfSymbolMapEntryList &entry = iter->second;
//     printSymbolMapEntry(entry,extentSortedSections);
//   }
// }
// 
// ElfSymbolMapEntry chooseSymbol(const ElfSymbolMapEntryList& entries,
//                                SgAsmElfSectionTableEntry::SectionType sh_type=SgAsmElfSectionTableEntry::SHT_NULL)
// {
//   ROSE_ASSERT(!entries.empty());
//   ElfSymbolMapEntry best = ElfSymbolMapEntry();
//   bool bestGlobalBinding = false;
//   bool bestIsSection = false;
//   // indicates that section type is an important filter
//   bool checkSectionType = (sh_type != SgAsmElfSectionTableEntry::SHT_NULL);
//   for(size_t i=0; i < entries.size(); ++i){
//     SgAsmElfSection* section = isSgAsmElfSection(entries[i].section);
//     SgAsmElfSectionTableEntry* sectionEntry = NULL;
//     if(section)
//       sectionEntry = section->get_section_entry();
// 
//     if(checkSectionType &&
//        (!sectionEntry || sectionEntry->get_sh_type() != sh_type)){
//       // we're asked to check the section type, but it is not a section
//       //  or its the wrong type, so we just drop it completely.
//       continue;
//     }
// 
//     bool globalBinding = entries[i].symbol->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL;
//     bool isSection = (sectionEntry != NULL);// sections must have a section entry
// 
//     if(bestGlobalBinding == globalBinding &&
//        (bestIsSection || !isSection)){
//       // if the global binding is the same 
//       //  and we are not a section (and best IS), we lose
//       continue;
//     }
//     else if(bestGlobalBinding && !globalBinding){
//       // the current best is globally bound, and we are not, we lose
//       continue;
//     }
//     else{
//       // we are globally bound and/or we are a section
//       bestGlobalBinding = globalBinding;
//       bestIsSection = isSection;
//       best = entries[i];
//     }
//   }
//   return best;
// }
// 			       
// 
// rose_addr_t symbolValue(const SgAsmElfSymbol* relocSymbol,
// 			const SymbolMap &symbolMap,
// 			const SgAsmElfSectionPtrList& extentSortedSections,
// 			SgAsmGenericSection* &symbolSection)
// {
//   ROSE_ASSERT(relocSymbol);
//   string symbolName = relocSymbol->get_name()->c_str();
//   SymbolMap::const_iterator findIter = symbolMap.find(symbolName);
//   if(findIter == symbolMap.end() || findIter->second.empty()){
//     if(relocSymbol->get_elf_binding() == SgAsmElfSymbol::STB_WEAK){
//       return 0;// Its okay to have an undefined symbol if the RELOCATION's symbol is weak
//     }
//     else{
//       printf("Unresolved relocation : Symbol not found: %s\n ",symbolName.c_str());
//       return 0;
//     }
//   }
//   else{
//     const ElfSymbolMapEntryList &entryList = findIter->second;
//     ElfSymbolMapEntry sourceSymbolEntry = chooseSymbol(entryList/*,SgAsmElfSectionTableEntry::SHT_PROGBITS.text*/);
//     SgAsmElfSymbol* sourceSymbol = isSgAsmElfSymbol(sourceSymbolEntry.symbol);
// 
//     rose_addr_t symbol_va = sourceSymbol->get_value() + SageInterface::getEnclosingNode<SgAsmElfSection>(sourceSymbol)->get_base_va();
//         
//     symbolSection = sourceSymbolEntry.section;
//     ROSE_ASSERT(symbolSection != NULL);
//     return symbol_va;
//   }
// 
// }
// 			
// 
// void 
// relocate_X86_Symbol(const SgAsmElfRelocEntry* reloc,
// 		    const SgAsmElfSymbol* symbol,
// 		    const SymbolMap &symbolMap,
// 		    const SgAsmElfSectionPtrList& extentSortedSections,
// 		    const rose_addr_t addrSize)
// {
//   ROSE_ASSERT(reloc != NULL);
//   ROSE_ASSERT(symbol != NULL);
//   SgAsmGenericSection *sourceSection = NULL;
//   rose_addr_t symbol_va = symbolValue(symbol,symbolMap,extentSortedSections,sourceSection);
// 
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
// 
//   const rose_addr_t reloc_va = reloc->get_r_offset() + relocSection->get_base_va();
// 
//   SgAsmElfSection* targetSection = chooseSectionAtAddress(reloc_va,extentSortedSections);
//   ROSE_ASSERT(targetSection);
// 
//   // the relocation should come from the same SgAsmFile as the target, we use base_va as a proxy for this
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
// 
//   /// offset from start of section
//   rose_addr_t relocSectionOffset = reloc_va - targetSection ->get_mapped_actual_va();
// 
//   SgFileContentList contents = targetSection->get_data();
//   const rose_addr_t reloc_value = symbol_va;
// 
//   if(addrSize == 4){
//     ROSE_ASSERT(symbol_va <= UINT_MAX);
//     uint32_t reloc_value_32 = symbol_va;
//     memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
//   }
//   else if(addrSize == 8){
//     // TODO Throw some asserts around this
//     memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
//   }
//   
//   if(get_verbose() > 0){
//     const char* targetSectionName = targetSection->get_name()->c_str();
//     const char* sourceSectionName = "";
//     if(sourceSection)
//       sourceSectionName = sourceSection->get_name()->c_str();
// 
//     printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
//   }
// }
// 
// 
// /** 
//     Relocation of Jump Table Entry (e.g. jump slot).
//     A call to a library looks like the following :
//       .text: [EXECUTABLE READ ONLY]
//         ...
// A       call 0x000002080 <file relative address in plt>
// F       ...
// 
//       .plt [EXECUTABLE READ ONLY]
//         ... 
// B 0x2080 jmp *03030 <file relative address in .got.plt>
// C 0x2086 push 0x4 <This is the 4th function in the plt>
// D 0x208b jmp *0x00002018 <basically a call to dynamic linker>
// 
//       .got.plt [NOT EXECUTABLE WRITABLE]
//         ...
// Z 0x3030 : 0x2086
// 	
// 
// 
// LIBRARY : 
//    .text:
//      ...
// E 0xA018: FOO <do function stuff here>
//   ...
//   ret
//  
// 
// In the above example, in "normal" operation, we would see:
// The FIRST time we call the "foo"
// A
// B <jumps to *Z, which starts pointing at C> 
// C
// D <change Z to the value of E: 0xA018, call Foo directly>
// E <execute Foo>
// F 
// 
// The SECOND time, we will have called the dynamic linker, and C and D are no inert
// A
// B <jumps to *Z which is NOW just E>
// E <execute Foo>
// F
// 
// In this relocation, we need to mimic the dynamic linker, and overwrite the 
// .got entry (Z) with the address of the function (E i.e. Foo)
// Thus, we're performing
// *Z = E
// 
// */
// void 
// relocate_X86_JMP_SLOT(const SgAsmElfRelocEntry* reloc,
// 		      const SgAsmElfSymbol* symbol,
// 		      const SymbolMap &symbolMap,
// 		      const SgAsmElfSectionPtrList& extentSortedSections,
// 		      const size_t addrSize)
// {
//   relocate_X86_Symbol(reloc,symbol,symbolMap,extentSortedSections,addrSize);
// }
// 
// /**
//    Pointers to data.  Generally:
//    *reloc_va (.got) = symbol->address (.data);
//  */
// void 
// relocate_X86_GLOB_DAT(const SgAsmElfRelocEntry* reloc,
// 		      const SgAsmElfSymbol* symbol,
// 		      const SymbolMap &symbolMap,
// 		      const SgAsmElfSectionPtrList& extentSortedSections,
// 		      const size_t addrSize)
// {
//   relocate_X86_Symbol(reloc,symbol,symbolMap,extentSortedSections,addrSize);
// }
// 
// void 
// relocate_386_32(const SgAsmElfRelocEntry* reloc,
// 		const SgAsmElfSymbol* symbol,
// 		const SymbolMap &symbolMap,
// 		const SgAsmElfSectionPtrList& extentSortedSections,
// 		const size_t addrSize)
// {
//   ROSE_ASSERT(reloc != NULL);
//   ROSE_ASSERT(symbol != NULL);
//   SgAsmGenericSection *sourceSection = NULL;
//   rose_addr_t symbol_va = symbolValue(symbol,symbolMap,extentSortedSections,sourceSection);
//   
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
// 
//   const rose_addr_t reloc_va = reloc->get_r_offset() + relocSection->get_base_va();
// 
//   SgAsmElfSection* targetSection = chooseSectionAtAddress(reloc_va,extentSortedSections);
//   ROSE_ASSERT(targetSection != NULL);
// 
//   // the relocation should come from the same SgAsmFile as the target, we use base_va as a proxy for this
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
// 
//   /// offset from start of section
//   rose_addr_t relocSectionOffset = reloc_va - targetSection ->get_mapped_actual_va();
// 
//   SgFileContentList contents = targetSection->get_data();
//   rose_addr_t reloc_value = symbol_va + reloc->get_r_addend();
//   if(addrSize == 4){
//     ROSE_ASSERT(symbol_va <= UINT_MAX);
//     uint32_t reloc_value_32 = reloc_value;
//     memcpy(&(contents.at(relocSectionOffset)),&reloc_value,addrSize);
//   }
//   else if(addrSize == 8){
//     // TODO Throw some asserts around this
//     memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
//   }
//   
//   if(get_verbose() > 0){
//     const char* targetSectionName = targetSection->get_name()->c_str();
//     const char* sourceSectionName = "";
//     if(sourceSection)
//       sourceSectionName = sourceSection->get_name()->c_str();
// 
//     printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
//   }
// }
// 
// void 
// relocate_386_RELATIVE(const SgAsmElfRelocEntry* reloc,
// 		      const SgAsmElfSymbol* symbol,//may be null
// 		      const SymbolMap &symbolMap,
// 		      const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
// 
//   const rose_addr_t reloc_addr_va = reloc->get_r_offset() + relocSection->get_base_va();
// 
//   SgAsmElfSectionPtrList targetSections = removeSegments(sectionsOfAddress(reloc_addr_va,extentSortedSections));
//   ROSE_ASSERT(targetSections.size() == 1);
// 
//   SgAsmElfSection* targetSection = targetSections.front();
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
// 
//   // The "addend" is actually a pointer to unrelocated data.  
//   //   So, we find its relocated address, and set *reloc_va = reloc_data
//   const rose_addr_t reloc_data_va = reloc->get_r_addend() + relocSection->get_base_va();
//   std::vector<SgAsmElfSection*> dataSections = removeSegments(sectionsOfAddress(reloc_data_va,extentSortedSections));
//   if(  dataSections.size() != 1)
//   {
//     dataSections = sectionsOfAddress(reloc_data_va,extentSortedSections);
//     dataSections = removeSegments(dataSections);
//   }
//   ROSE_ASSERT(dataSections.size() == 1);
//   SgAsmElfSection* dataSection = dataSections.front();
// 
//   ROSE_ASSERT(relocSection->get_base_va() == dataSection->get_base_va());
// 
//   rose_addr_t relocSectionAddrOffset = reloc_addr_va - targetSection->get_mapped_actual_va();
//   SgFileContentList targetMemBase = targetSection->get_data();
//   
//   unsigned char* targetMem = &(targetMemBase.at(relocSectionAddrOffset));// size is 8?
// 
//   rose_addr_t relocSectionDataOffset = reloc_data_va - dataSection->get_mapped_actual_va();
//   const SgFileContentList dataMemBase = dataSection->get_data();
//   const unsigned char* dataMem = &(dataMemBase.at(relocSectionDataOffset));// size is 8?
// 
//   /*
//   std::cout << " R_386_RELATIVE: ";
//   std::cout << "*" 
// 	    << std::hex << reloc_addr_va << "[" << targetSection->get_name()->c_str() << "] = " 
// 	    << std::hex << reloc_data_va << "[" << dataSection->get_name()->c_str() << "]" ;
//   */
//   //std::cout << std::endl;
// 
// }
// 
// void processOneRelocation(RelocationEntry relocation,
// 			  const SymbolMap& symbolMap,
// 			  const SgAsmElfSectionPtrList& extentSortedSections)
// {
//     SgAsmElfRelocEntry* reloc = relocation.reloc;
//     SgAsmElfSymbol* symbol = relocation.symbol;
//     SgAsmGenericSection* section = SageInterface::getEnclosingNode<SgAsmGenericSection>(reloc);
//     ROSE_ASSERT(reloc != NULL);
//     ROSE_ASSERT(symbol != NULL);
//     ROSE_ASSERT(section != NULL);
// 
//     SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc);
//     ROSE_ASSERT(header != NULL);// need the header to determine ISA
//     SgAsmGenericHeader::InsSetArchitecture isa = header->get_isa();
// 
//     const rose_addr_t reloc_va = reloc->get_r_offset() + section->get_base_va();
// 
//     string symbolName = symbol->get_name()->c_str();// symbolName may be empty
//     if(get_verbose())
//     {
//       std::string relocStr;
//       if(header)
// 	relocStr = reloc->to_string(reloc->get_type(),isa);
//       printf("%16s '%25s' 0x%016x",relocStr.c_str(),symbolName.c_str(),reloc_va);
//     }
// 
//     // TODO support other than x86
//     switch(isa & SgAsmGenericHeader::ISA_FAMILY_MASK){
//       case SgAsmGenericHeader::ISA_IA32_Family:
// 	switch(reloc->get_type()){
// 	  case SgAsmElfRelocEntry::R_386_JMP_SLOT:relocate_X86_JMP_SLOT(reloc,symbol,symbolMap,extentSortedSections,4);break;
// 	  case SgAsmElfRelocEntry::R_386_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,4);break;
// 	  case SgAsmElfRelocEntry::R_386_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;
// 	  case SgAsmElfRelocEntry::R_386_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
// 	  case SgAsmElfRelocEntry::R_386_TLS_TPOFF:
// 	  case SgAsmElfRelocEntry::R_386_TLS_IE:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GOTIE:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LE:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GD:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDM:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GD_32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GD_PUSH:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GD_CALL:
// 	  case SgAsmElfRelocEntry::R_386_TLS_GD_POP:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDM_32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDM_PUSH:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDM_CALL:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDM_POP:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LDO_32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_IE_32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_LE_32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_DTPMOD32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_DTPOFF32:
// 	  case SgAsmElfRelocEntry::R_386_TLS_TPOFF32:
// 	    printf("%s%s\n", "<unresolved>: Thread Local Storage not supported",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str()); break;
// 	  default:
// 	    printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
// 	};
// 	break;
//       case SgAsmGenericHeader::ISA_X8664_Family:
// 	switch(reloc->get_type()){
// 	  case SgAsmElfRelocEntry::R_X86_64_JUMP_SLOT:relocate_X86_JMP_SLOT(reloc,symbol,symbolMap,extentSortedSections,8);break;
// 	  case SgAsmElfRelocEntry::R_X86_64_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,8);break;
// 	  case SgAsmElfRelocEntry::R_X86_64_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should zero extend symbol value
// 	  case SgAsmElfRelocEntry::R_X86_64_32S:     relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should sign extend symbol value
// 	  case SgAsmElfRelocEntry::R_X86_64_64:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,8);break;
// 	  case SgAsmElfRelocEntry::R_X86_64_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
// 	  default:
// 	    printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());	  
// 	};
// 	break;
//       default:
// 	printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
//     };
// }
// 
// void processRelocations(const RelocationEntryList& relocations,
// 			const SymbolMap& symbolMap,
// 			const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   RelocationEntryList::const_iterator relocationIter = relocations.begin();
//   for(; relocationIter != relocations.end(); ++relocationIter){
//     processOneRelocation(*relocationIter,symbolMap,extentSortedSections);
//   }
// }
// 
// #endif

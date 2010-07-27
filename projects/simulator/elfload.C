#include "rose.h"
#include "simulate.h"

#ifdef ROSE_ENABLE_SIMULATOR /*protects this whole file */

#include "mymemory.h"
#include <sys/mman.h>
#include <elf.h>
#include <dlfcn.h>
#include <asm/ldt.h>
#include <vector>

using namespace std;

static void load_executable_or_library(LinuxMachineState& ms, char* filename, bool& stack_executable, bool* has_interp = 0, uint32_t* phdr = 0, uint32_t* phent = 0, uint32_t* phnum = 0, uint32_t* entry = 0) {
  fprintf(stderr, "loading %s...\n", filename);
  int executable_fd = open(filename, O_RDONLY);
  if (executable_fd == -1) {
    perror("open of executable");
    exit(1);
  }
  struct stat executable_info;
  int fstat_result = fstat(executable_fd, &executable_info);
  if (fstat_result == -1) {
    perror("fstat");
    exit(1);
  }
  char* executable = (char*)mmap(NULL, (executable_info.st_size + 4095) & ~4095, PROT_READ, MAP_PRIVATE, executable_fd, 0);
  if (executable == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  Elf32_Ehdr* header = (Elf32_Ehdr*)executable;
  if ((size_t)executable_info.st_size < sizeof(Elf32_Ehdr) ||
      header->e_ident[EI_MAG0] != ELFMAG0 ||
      header->e_ident[EI_MAG1] != ELFMAG1 ||
      header->e_ident[EI_MAG2] != ELFMAG2 ||
      header->e_ident[EI_MAG3] != ELFMAG3 ||
      header->e_ident[EI_CLASS] != ELFCLASS32 ||
      header->e_ident[EI_DATA] != ELFDATA2LSB ||
      header->e_ident[EI_VERSION] != EV_CURRENT ||
      header->e_machine != EM_386 ||
      header->e_phentsize != sizeof(Elf32_Phdr) ||
      executable_info.st_size < header->e_phoff + header->e_phentsize * header->e_phnum) {
    fprintf(stderr, "Executable does not have correct format\n");
    exit(1);
  }
  Elf32_Phdr* program_headers = (Elf32_Phdr*)(executable + header->e_phoff);
  // phdr is set when a PT_PHDR segment is seen
  if (phent) *phent = header->e_phentsize;
  if (phnum) *phnum = header->e_phnum;
  if (entry) *entry = header->e_entry;
  ms.brk = 0;
  ms.ip = header->e_entry; // Do this first so an interpreter can overwrite it
  stack_executable = true;
  for (size_t i = 0; i < header->e_phnum; ++i) {
    Elf32_Phdr& hdr = program_headers[i];
    switch (hdr.p_type) {
      case PT_NULL: break;
      case PT_LOAD: {
	if (executable_info.st_size < hdr.p_offset + hdr.p_filesz) {
	  fprintf(stderr, "Segment is not in file\n");
	  exit(1);
	}
	if (hdr.p_memsz < hdr.p_filesz) {
	  fprintf(stderr, "Segment memory is too short\n");
	  // exit(1);
	  hdr.p_memsz = hdr.p_filesz;
	}
	// fprintf(stderr, "Found load header -- mapping %p length %u flags 0x%08x into simulated memory at 0x%08X, padded to %u bytes\n", executable + hdr.p_offset, hdr.p_filesz, hdr.p_flags, hdr.p_vaddr, hdr.p_memsz);
	if (hdr.p_vaddr + hdr.p_memsz > ms.brk) ms.brk = hdr.p_vaddr + hdr.p_memsz;
	ms.brk = (ms.brk + PAGE_SIZE - 1) & -PAGE_SIZE;
	uint32_t vaddr_rounded = hdr.p_vaddr & -PAGE_SIZE;
	if (hdr.p_memsz != 0) { // hdr.p_memsz can be 0 to set brk (UPX does this)
          uint32_t end = hdr.p_vaddr + hdr.p_memsz;
          end = (end + PAGE_SIZE - 1) & -PAGE_SIZE;
          char* storage = (char*)calloc(end - vaddr_rounded, 1);
          if (!storage) {
            fprintf(stderr, "Could not allocate memory for partially uninitialized data\n");
            abort();
          }
          memcpy(storage + hdr.p_vaddr % PAGE_SIZE, executable + hdr.p_offset, hdr.p_filesz);
          for (uint32_t j = 0; j < end - vaddr_rounded; j += PAGE_SIZE) {
            ms.memory.findPage(vaddr_rounded + j) = Page((hdr.p_flags & PF_R), (hdr.p_flags & PF_W), (hdr.p_flags & PF_X), (uint8_t*)(storage + j));
          }
	}
	break;
      }
      case PT_DYNAMIC: {
	// fprintf(stderr, "Found dynamic segment\n");
	break;
      }
      case PT_INTERP: {
	if (executable_info.st_size < hdr.p_offset + hdr.p_filesz) {
	  fprintf(stderr, "Segment is not in file\n");
	  exit(1);
	}
	vector<char> interp(executable + hdr.p_offset, executable + hdr.p_offset + hdr.p_filesz);
	interp.push_back(0);
	bool stack_executable2;
	load_executable_or_library(ms, &interp[0], stack_executable2);
	if (has_interp) *has_interp = true;
	break;
      }
      case PT_NOTE: {
	if (executable_info.st_size < hdr.p_offset + hdr.p_filesz) {
	  fprintf(stderr, "Segment is not in file\n");
	  exit(1);
	}
	// fprintf(stderr, "Found note '%.*s'\n", hdr.p_filesz, executable + hdr.p_offset);
	break;
      }
      case PT_PHDR: {
	// fprintf(stderr, "Found program header -- mapping %p length %u flags 0x%08x into simulated memory at 0x%08X, padded to %u bytes\n", executable + hdr.p_offset, hdr.p_filesz, hdr.p_flags, hdr.p_vaddr, hdr.p_memsz);
	if (phdr) *phdr = hdr.p_vaddr;
	break;
      }
      case PT_GNU_EH_FRAME: {
	break;
      }
      case PT_GNU_STACK: {
	stack_executable = (hdr.p_flags & PF_X);
	break;
      }
      default: fprintf(stderr, "Unhandled program header type %u\n", hdr.p_type);
    }
  }
}

void setup(LinuxMachineState& ms, int argc, char** argv) {
  bool stack_executable;
  bool has_interp = false;
  uint32_t phdr = 0, phent = 0, phnum = 0, entry = 0;
  load_executable_or_library(ms, argv[0], stack_executable, &has_interp, &phdr, &phent, &phnum, &entry);
  // Make the stack and brk
  for (size_t i = 0xBF000000; i < 0xC0000000; i += PAGE_SIZE) {
    ms.memory.mapZeroPageIfNeeded(i);
  }

  vector<uint32_t> pointers;
  unsigned int sp = 0xC0000000U;

  pointers.push_back(argc);

  for (int i = 0; i < argc; ++i) {
    sp -= strlen(argv[i]) + 1;
    ms.memory.writeMultiple((const uint8_t*)argv[i], strlen(argv[i]) + 1, sp);
    pointers.push_back(sp);
  }
  pointers.push_back(0);
  fprintf(stderr, "esp after argc/argv = 0x%08x, pointers=%zu\n", sp, pointers.size());

  for (unsigned int i = 0; ; ++i) {
    if (!environ[i]) break;
    sp -= strlen(environ[i]) + 1;
    ms.memory.writeMultiple((const uint8_t*)environ[i], strlen(environ[i]) + 1, sp);
    pointers.push_back(sp);
  }
  pointers.push_back(0);
  fprintf(stderr, "esp after environ = 0x%08x, pointers=%zu\n", sp, pointers.size());

  // Fill in auxv
  // Each entry is two words in the pointers vector
  if (has_interp) {
    pointers.push_back(AT_PHDR); pointers.push_back(phdr);
    pointers.push_back(AT_PHENT); pointers.push_back(phent);
    pointers.push_back(AT_PHNUM); pointers.push_back(phnum);
    pointers.push_back(AT_PAGESZ); pointers.push_back(PAGE_SIZE);
    pointers.push_back(AT_ENTRY); pointers.push_back(entry);
    pointers.push_back(AT_UID); pointers.push_back(getuid());
    pointers.push_back(AT_EUID); pointers.push_back(geteuid());
    pointers.push_back(AT_GID); pointers.push_back(getgid());
    pointers.push_back(AT_EGID); pointers.push_back(getegid());
    pointers.push_back(AT_SECURE); pointers.push_back(false);
  }
  pointers.push_back(AT_NULL); pointers.push_back(0);
  fprintf(stderr, "esp after auxv = 0x%08x, pointers=%zu\n", sp, pointers.size());

  sp &= ~3U;

  sp -= 4 * pointers.size();
  ms.memory.writeMultiple((const uint8_t*)&pointers[0], 4 * pointers.size(), sp);

  ms.gprs[x86_gpr_sp] = sp;

  fprintf(stderr, "esp = 0x%08x\n", sp);
}

#endif /*ROSE_ENABLE_SIMULATOR*/

# -*- makefile -*-

################################################################################################################################
# The finest granularity.  Names follow this format:
#
#   <Container>_<FileType>_<Architecture>_specimens
#
# Where:
#   <Container> is one of
#	dos	-- DOS (Microsoft)
#	elf	-- Executable and Linkable Format (Linux, etc)
#	pe	-- Portable Executable (Microsoft)
#	raw	-- raw data, no container

specimen_containers = dos elf pe raw

#
#  <FileType> is one of
#	exe	-- executable programs (but not necessarily having execute permission)
#	obj	-- object file (names usually ending with ".o" or ".obj")
#	archive	-- archive (static library)
#	so	-- shared object (names usually ending with ".so" or ".dll")
#	core	-- core dump (only used for ELF)
#	insns	-- raw data instructions
#	rot13	-- raw data rot13 encoded

specimen_filetypes = exe obj archive so core insns rot13

#
#  <Architecture> is one of
#       amd64	-- Intel 64-bit
#	arm	-- ARM
#	mips	-- MIPS
#	m68k	-- Motorola m68k
#	ppc	-- PowerPC
#	x86	-- Intel 32-bit
#

specimen_architectures = amd64 arm mips m68k ppc x86

#------------------------------------------------------------------------------------------------------------------------
# dos_*_x86_specimens

dos_exe_x86_specimens =				\
    exefmt.exe

#------------------------------------------------------------------------------------------------------------------------
# elf_*_amd64_specimens

elf_exe_amd64_specimens =			\
    x86-64-ctrlaltdel				\
    x86-64-nologin				\
    x86-64-pivot_root				\
    x86-64-poweroff
elf_obj_amd64_specimens =			\
    gcc-4.0.x-test1.o
elf_so_amd64_specimens =			\
    schulz-sample1-p.so				\
    schulz-sample1.so				\
    schulz-sample2-p.so				\
    schulz-sample2.so
elf_core_amd64_specimens =
elf_archive_amd64_specimens =			\
    libTest-amd64.a

#------------------------------------------------------------------------------------------------------------------------
# elf_*_arm_specimens

elf_exe_arm_specimens =				\
    arm-ctrlaltdel				\
    arm-nologin					\
    arm-pivot_root				\
    arm-poweroff
elf_obj_arm_specimens =
elf_so_arm_specimens =
elf_core_arm_specimens =
elf_archive_arm_specimens =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_m68k_specimens

elf_exe_m68k_specimens =
elf_obj_m68k_specimens =
elf_so_m68k_specimens =
elf_core_m68k_specimens =
elf_archive_m68k_specimens =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_mips_specimens

elf_exe_mips_specimens =			\
    hello.mips
elf_obj_mips_specimens =
elf_so_mips_specimens =
elf_core_mips_specimens =
elf_archive_mips_specimens =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_ppc_specimens

elf_exe_ppc_specimens =				\
    bgl-helloworld-mpicc			\
    bgl-helloworld-mpixlc			\
    fnord.ppc
elf_obj_ppc_specimens =
elf_so_ppc_specimens =
elf_core_ppc_specimens =
elf_archive_ppc_specimens =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_x86_specimens

elf_exe_x86_specimens =				\
    buffer2.bin					\
    fnord.i386					\
    i386-ctrlaltdel				\
    i386-fcalls					\
    i386-fsck.cramfs				\
    i386-fld					\
    i386-fnstsw					\
    i386-fst					\
    i386-int3					\
    i386-mixed					\
    i386-mul					\
    i386-nologin				\
    i386-overlap				\
    i386-pop					\
    i386-pivot_root				\
    i386-pointers				\
    i386-poweroff				\
    i386-pushfd					\
    i386-pxor					\
    i386-stmxcsr				\
    i386-wait					\
    i386-taintflow1				\
    i686-test1.O0.bin				\
    i686-test1.O3-stripped.bin			\
    i686-test1.O3.bin				\
    memreadwrite				\
    pentium-movd				\
    pentium-xmm					\
    usedef
elf_obj_x86_specimens =
elf_so_x86_specimens =				\
    libm-2.3.6.so
elf_core_x86_specimens =
elf_archive_x86_specimens =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_arm_specimens

pe_exe_arm_specimens =				\
    pocketputty.exe				\
    pocketputty2.exe
pe_obj_arm_specimens =
pe_so_arm_specimens =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_ppc_specimens

pe_exe_ppc_specimens =				\
    PFE32.EXE
pe_obj_ppc_specimens =
pe_so_ppc_specimens =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_x86_specimens

pe_exe_x86_specimens =				\
    asm_code_samples_gcc.exe			\
    proxycfg.exe				\
    puttytel.exe
pe_obj_x86_specimens =
pe_so_x86_specimens =

#------------------------------------------------------------------------------------------------------------------------
# raw_*_*_specimens

raw_insns_x86_specimens =			\
    buffer2.raw					\
    vm-final.raw
raw_rot13_x86_specimens =			\
    buffer2.rot13

###############################################################################################################################
# Other distributed files, not necessarily binary specimens

other_distributed =				\
    ARPPRODUCTICON.exe				\
    i386-fld.s					\
    i386-fnstsw.s				\
    i386-fst.s					\
    i386-int3.s					\
    i386-mixed.s				\
    i386-mul.s					\
    i386-overlap.s				\
    i386-pop.s					\
    i386-pushfd.s				\
    i386-pxor.s					\
    i386-stmxcsr.s				\
    i386-wait.s					\
    memreadwrite.s				\
    pentium-movd.s				\
    pentium-xmm.s				\
    vm-final.README				\
    vm-final.ipd

# The name "any_any_any_specimens" is a bit strange, so lets have an alias
all_specimens = $(any_any_any_specimens)

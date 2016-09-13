# -*- makefile -*-

# All variables in this makefile begin with "nonsmoke_specimens_binary_" (since that's where this file is located in the
# directory hierarchy.

nonsmoke_specimens_binary_directory = $(top_srcdir)/tests/nonsmoke/specimens/binary

################################################################################################################################
# The finest granularity.  Names follow this format:
#
#   nonsmoke_specimens_binary_<Container>_<FileType>_<Architecture>
#
# Where:
#   <Container> is one of
#	dos	-- DOS (Microsoft)
#	elf	-- Executable and Linkable Format (Linux, etc)
#	pe	-- Portable Executable (Microsoft)
#	raw	-- raw data, no container

nonsmoke_specimens_binary_containers = dos elf pe raw

#
#  <FileType> is one of
#	exe	-- executable programs (but not necessarily having execute permission)
#	obj	-- object file (names usually ending with ".o" or ".obj")
#	archive	-- archive (static library)
#	so	-- shared object (names usually ending with ".so" or ".dll")
#	core	-- core dump (only used for ELF)
#	insns	-- raw data instructions
#	rot13	-- raw data rot13 encoded

nonsmoke_specimens_binary_filetypes = exe obj archive so core insns rot13

#
#  <Architecture> is one of
#       amd64	-- Intel 64-bit
#	arm	-- ARM
#	mips	-- MIPS
#	m68k	-- Motorola m68k
#	ppc	-- PowerPC
#	x86	-- Intel 32-bit
#

nonsmoke_specimens_binary_architectures = amd64 arm mips m68k ppc x86

#------------------------------------------------------------------------------------------------------------------------
# dos_*_x86_specimens

nonsmoke_specimens_binary_dos_exe_x86 =	\
    exefmt.exe

#------------------------------------------------------------------------------------------------------------------------
# elf_*_amd64_specimens

nonsmoke_specimens_binary_elf_exe_amd64 =	\
    x86-64-adaptiveRegs				\
    x86-64-ctrlaltdel				\
    x86-64-nologin				\
    x86-64-pivot_root				\
    x86-64-poweroff

nonsmoke_specimens_binary_elf_obj_amd64 =	\
    gcc-4.0.x-test1.o

nonsmoke_specimens_binary_elf_so_amd64 =	\
    schulz-sample1-p.so				\
    schulz-sample1.so				\
    schulz-sample2-p.so				\
    schulz-sample2.so

nonsmoke_specimens_binary_elf_core_amd64 =

nonsmoke_specimens_binary_elf_archive_amd64 =	\
    libTest-amd64.a

#------------------------------------------------------------------------------------------------------------------------
# elf_*_arm_specimens

nonsmoke_specimens_binary_elf_exe_arm =		\
    arm-ctrlaltdel				\
    arm-nologin					\
    arm-pivot_root				\
    arm-poweroff

nonsmoke_specimens_binary_elf_obj_arm =
nonsmoke_specimens_binary_elf_so_arm =
nonsmoke_specimens_binary_elf_core_arm =
nonsmoke_specimens_binary_elf_archive_arm =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_m68k_specimens

nonsmoke_specimens_binary_elf_exe_m68k =
nonsmoke_specimens_binary_elf_obj_m68k =
nonsmoke_specimens_binary_elf_so_m68k =
nonsmoke_specimens_binary_elf_core_m68k =
nonsmoke_specimens_binary_elf_archive_m68k =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_mips_specimens

nonsmoke_specimens_binary_elf_exe_mips =	\
    hello.mips

nonsmoke_specimens_binary_elf_obj_mips =
nonsmoke_specimens_binary_elf_so_mips =
nonsmoke_specimens_binary_elf_core_mips =
nonsmoke_specimens_binary_elf_archive_mips =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_ppc_specimens

nonsmoke_specimens_binary_elf_exe_ppc =		\
    bgl-helloworld-mpicc			\
    bgl-helloworld-mpixlc			\
    fnord.ppc

nonsmoke_specimens_binary_elf_obj_ppc =
nonsmoke_specimens_binary_elf_so_ppc =
nonsmoke_specimens_binary_elf_core_ppc =
nonsmoke_specimens_binary_elf_archive_ppc =

#------------------------------------------------------------------------------------------------------------------------
# elf_*_x86_specimens

nonsmoke_specimens_binary_elf_exe_x86 =		\
    buffer2.bin					\
    fnord.i386					\
    i386-ctrlaltdel				\
    i386-fcalls					\
    i386-fsck.cramfs				\
    i386-fld					\
    i386-fnstsw					\
    i386-fst					\
    i386-int3					\
    i386-lea					\
    i386-mixed					\
    i386-mov					\
    i386-mul					\
    i386-nologin				\
    i386-noop					\
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
    i386-thunks-7				\
    i686-test1.O0.bin				\
    i686-test1.O3-stripped.bin			\
    i686-test1.O3.bin				\
    memreadwrite				\
    pentium-movd				\
    pentium-xmm					\
    usedef

nonsmoke_specimens_binary_elf_obj_x86 =

nonsmoke_specimens_binary_elf_so_x86 =		\
    libm-2.3.6.so

nonsmoke_specimens_binary_elf_core_x86 =
nonsmoke_specimens_binary_elf_archive_x86 =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_arm_specimens

nonsmoke_specimens_binary_pe_exe_arm =		\
    pocketputty.exe				\
    pocketputty2.exe

nonsmoke_specimens_binary_pe_obj_arm =
nonsmoke_specimens_binary_pe_so_arm =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_ppc_specimens

nonsmoke_specimens_binary_pe_exe_ppc =		\
    PFE32.EXE

nonsmoke_specimens_binary_pe_obj_ppc =
nonsmoke_specimens_binary_pe_so_ppc =

#------------------------------------------------------------------------------------------------------------------------
# pe_*_x86_specimens

nonsmoke_specimens_binary_pe_exe_x86 =		\
    asm_code_samples_gcc.exe			\
    proxycfg.exe				\
    puttytel.exe

nonsmoke_specimens_binary_pe_obj_x86 =
nonsmoke_specimens_binary_pe_so_x86 =

#------------------------------------------------------------------------------------------------------------------------
# raw_*_*_specimens

nonsmoke_specimens_binary_raw_insns_x86 =	\
    buffer2.raw					\
    i386-initialState				\
    vm-final.raw

nonsmoke_specimens_binary_raw_rot13_x86 =	\
    buffer2.rot13

#------------------------------------------------------------------------------------------------------------------------
# Big specimens not intended to be unit tests and which might take too long on slow hardware. Anything over about 500k.

nonsmoke_specimens_binary_large =		\
    PFE32.EXE					\
    bgl-helloworld-mpicc			\
    bgl-helloworld-mpixlc			\
    hello.mip					\
    libTest-amd64.a				\
    pocketputty.exe				\
    pocketputty2.exe				\
    puttytel.exe				\
    vm-final.raw

###############################################################################################################################
# Other distributed files, not necessarily binary specimens

nonsmoke_specimens_binary_other_distributed =	\
    ARPPRODUCTICON.exe				\
    i386-fld.s					\
    i386-fnstsw.s				\
    i386-fst.s					\
    i386-initialState.s				\
    i386-int3.s					\
    i386-lea.s					\
    i386-mixed.s				\
    i386-mov.s					\
    i386-mul.s					\
    i386-noop.s					\
    i386-overlap.s				\
    i386-pop.s					\
    i386-pushfd.s				\
    i386-pxor.s					\
    i386-stmxcsr.s				\
    i386-taintedflow1.c				\
    i386-thunks-7.s				\
    i386-wait.s					\
    memreadwrite.s				\
    pentium-movd.s				\
    pentium-xmm.s				\
    vm-final.README				\
    vm-final.ipd				\
    x86-64-adaptiveRegs.s

# The name "nonsmoke_specimens_binary_any_any_any" is a bit strange, so lets have an alias
nonsmoke_specimens_binary_all = $(nonsmoke_specimens_binary_any_any_any)

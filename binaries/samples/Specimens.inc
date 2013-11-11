# -*- makefile -*-

DOS_x86_Specimens =				\
    exefmt.exe

ELF_amd64_Specimens =				\
    gcc-4.0.x-test1.o				\
    schulz-sample1-p.so				\
    schulz-sample1.so				\
    schulz-sample2-p.so				\
    schulz-sample2.so				\
    x86-64-ctrlaltdel				\
    x86-64-nologin				\
    x86-64-pivot_root				\
    x86-64-poweroff

ELF_ARM_Specimens =				\
    arm-ctrlaltdel				\
    arm-nologin					\
    arm-pivot_root				\
    arm-poweroff

ELF_MIPS_Specimens =				\
    hello.mips

ELF_PowerPC_Specimens =				\
    bgl-helloworld-mpicc			\
    bgl-helloworld-mpixlc			\
    fnord.ppc

ELF_x86_Specimens =				\
    buffer2.bin					\
    fnord.i386					\
    i386-ctrlaltdel				\
    i386-fcalls					\
    i386-fsck.cramfs				\
    i386-nologin				\
    i386-pivot_root				\
    i386-pointers				\
    i386-poweroff				\
    i686-test1.O0.bin				\
    i686-test1.O3-stripped.bin			\
    i686-test1.O3.bin				\
    libm-2.3.6.so				\
    memreadwrite				\
    usedef

PE_ARM_Specimens =				\
    pocketputty.exe				\
    pocketputty2.exe

PE_x86_Specimens =				\
    PFE32.EXE					\
    asm_code_samples_gcc.exe			\
    proxycfg.exe				\
    puttytel.exe

Raw_x86_Specimens =				\
    buffer2.raw					\
    buffer2.rot13				\
    vm-final.raw

Archive_amd64_Specimens =			\
    libTest-amd64.a

Other_Files =					\
    ARPPRODUCTICON.exe				\
    memreadwrite.s				\
    vm-final.README				\
    vm-final.ipd

# Specimens by container type
DOS_Specimens = $(DOS_x86_Specimens)
ELF_Specimens = $(ELF_amd64_Specimens) $(ELF_ARM_Specimens) $(ELF_MIPS_Specimens) $(ELF_PowerPC_Specimens) $(ELF_x86_Specimens)
PE_Specimens = $(PE_ARM_Specimens) $(PE_x86_Specimens)

Executable_Specimens = $(DOS_Specimens) $(ELF_Specimens) $(PE_Specimens)
Archive_Specimens = $(Archive_amd64_Specimens)
Raw_Specimens = $(Raw_x86_Specimens)

# Specimens by CPU architecture
amd64_Specimens = $(ELF_amd64_Specimens)
ARM_Specimens = $(ELF_ARM_Specimens) $(PE_ARM_Specimens)
MIPS_Specimens = $(ELF_MIPS_Specimens)
PowerPC_Specimens = $(ELF_PowerPC_Specimens)
x86_Specimens = $(DOS_x86_Specimens) $(ELF_x86_Specimens) $(PE_x86_Specimens) $(Raw_x86_Specimens) $(Archive_amd64_Specimens)

# All specimens
All_Specimens =														\
    $(DOS_x86_Specimens)												\
    $(ELF_amd64_Specimens) $(ELF_ARM_Specimens) $(ELF_MIPS_Specimens) $(ELF_PowerPC_Specimens) $(ELF_x86_Specimens)	\
    $(PE_x86_Specimens)													\
    $(Raw_x86_Specimens)												\
    $(Archive_amd64_Specimens)

# Shell commands to set up the environment for the GCC compiler
# from gnuarm.com, for the ARM7, for all Bound-T test programs.


GAINSTD="/usr/local/gnuarm-3.4.3"
export PATH="${GAINSTD}/bin:${PATH}"
PREFX="arm-elf-"

CC="${PREFX}gcc"

CCOPT='-g -O2 -Wa,-a'
#
# Options for C compilation:
#
# -g           Debug info.
# -O2          Optimization level 2.
# -Wa,a        Use assembler option -a = list.

LDOPT="${CCOPT} -Wl,-M,--verbose"
#
# Options for linking (using CC):
#
# -M           List memory map.
# --verbose    List linker script etc.

LD="${PREFX}ld"

LDLDOPT="-M --verbose"
#
# Options for linking (using LD).

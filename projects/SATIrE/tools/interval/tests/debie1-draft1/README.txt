This is the DEBIE-1 DPU SW, draft 1, for the WCET Tool Challenge 2008

This software is provided by courtesy of its owner, Space Systems
Finland Ltd (SSF). SSF allows you to use this version of the DEBIE-1 DPU
software for the specific purpose and under the specific conditions set
forth in the Terms Of Use document enclosed with or attached to this
software. In particular, the software remains the property of SSF and
you must not distribute the software to third parties without written
and signed authorization from SSF.

The Terms Of Use document should be in a file called terms_of_use.pdf;
if you do not find that file near this README file, ask for it from the
source from where you got this README. Do not use the SW before you have
read the Terms Of Use.

This version of the DEBIE-1 DPU SW has been modified by Tidorum Ltd to
adapt it for use as a benchmark in the WCET Tool Challenge. SSF has
granted Tidorum the right to distribute this version as a benchmark for
WCET analysis, but this right does not extend to recipients, who are not
allowed to distribute the SW further, without permission from SSF (see
the Terms Of Use).


PORTABILITY AND PORTING

The original target processor architecture for DEBIE, the Intel-8051, is
very  unfriendly to standard C. Most 8051 C programs are therefore very
unportable (at least before the C99 standard) and littered with
compiler-specific keywords like "xdata" or "idata" to tell the compiler
where to put variables, where pointers can point, and so on.

The DEBIE-1 SW is no exception, but all the special keywords were and
still are defined through C preprocessor macros, eg. XDATA, which means
that suitable macro definitions (mostly defining the macros as null)
make the code standard C.

These macros, and some other similar target-specific macros, are defined
in the header file keyword.h. This header file has several forms that
reside in target- and compiler-specific subfolders, for example
intel/linux/keyword.h for a native workstation compilation, or
arm7/gnu-ga/keyword.h for the ARM7 target with the GNU ARM
cross-compiler.

The header keyword.h also defines some types, similar to the C99 types
defined in <stdint.h>, that let this version of the DEBIE-1 SW use the
most suitable sizes of integer types for each target. For example, many
variables in the original (flying) DEBIE-1 SW were defined as "unsigned
char" to match the 8-bit nature of the 8051 architecture. In many cases
this is inefficient and clumsy on 32-bit processors such as the ARM7,
because the compiler has to insert extra instructions to implement 8-bit
semantics, for example wrap-around from 255 to 0. In the present version
of the SW, such variables are instead defined to be of type
uint_least8_t, a type defined in keyword.h as an integer type that is
efficient and natural for the given target and can hold at least the
numbers from 0 to 255, but does not necessarily wrap around from 255
to 0.


STRUCTURE OF THE DEBIE-1 SOFTWARE

The original DEBIE-1 SW was designed to run on the 80C32 8-bit processor
(Intel-8051 architecture) under the Keil RTX-51 real-time kernel. The
software contained six independent threads: three interrupt handlers and
three prioritized tasks. The tasks interact through mailboxes and some
shared global data.

This reduced version of the DEBIE-1 SW contains no kernel. A specific
"harness" module (harness.c) simulates the kernel functions and some of
the kernel services, for example the service that stores mailbox
messages until read by the receiving task. In this version, each of the
six threads is represented as a normal C function, callable from other C
functions. The tasks contain eternal "wait for next activation" loops,
as required by RTX-51 in the original software. In those cases, the
"guts" of the task -- the code for one activation of the task -- has
been extracted from the task function, and is represented as an
independent C function which does return to its caller. The code that
initializes the global state of the task has also been extracted into an
independent function.


INPUTS AND OUTPUTS

The original DEBIE-1 SW had the following input and output channels:

- A telecommand and telemetry interface, technically a synchronous,
  serial, half-duplex line, but presented to the SW as a parallel 16-bit
  interface that sends or receives one 16-bit word at a time. Reception
  of a word triggers the "TC interrupt". Writing a word into the 16-bit
  interface register initiates transmission of the word. Completion of
  the transmission of a word triggers the "TM interrupt".

- An interrupt that signals the presence of a "hit" on one of the
  four DEBIE Sensor Units (SUs).

- An multi-channel Analog-To-Digital Converter (ADC). To measure the
  voltage on a channel, the DEBIE-1 SW sets a channel-selection I/O
  register, sets another I/O register to start A/D conversion, and polls
  an I/O bit until conversion is completed. Interrupts are not used.
  The ADC can operate in unipolar mode (only non-negative voltages)
  or in bipolar mode (negative or positive voltages).

- Various other input registers connected to the sensors devices in the
  DEBIE Sensor Units. These are simple input registers that supply the
  sensor value when the SW reads the register.

- A multi-channel Digital-To-Analog Converter (DAC) to control some
  calibration signals for the measurement devices. This is an output-
  only device controlled by a number of writable channel-voltage
  output registers.

- Various other output registers that operate the sensor units and
  interfaces by sending reset pulses etc. These are simple output
  registers that the DEBIE-1 SW writes to execute the function of
  the register.

All of these interfaces are simulated in the harness module, more or
less realistically. (SIMULATION IS INCOMPLETE IN THIS DRAFT VERSION.)


DEBIE-1 AS A WCET ANALYSIS BENCHMARK

The accompanying folder problem-defs contains a number of text files
that define the analysis problems that participants in the WCET Tool
Challenge are expected to solve, for the DEBIE-1 SW. However, the Terms
Of Use permit any kind of WCET analysis of DEBIE-1, not limited to the
specific problems defined for the Challenge.


COMPILING DEBIE-1

The folder debie1 contains the 'C' source-code of the debie1 benchmark.
There are subfolders for each target and cross-compiler as follows:

intel/linux
   Compilation for an Intel/Linux system using the native
   GCC compiler.

arm7/gcc-ga
   Cross-compilation for an ARM7 using the GNU ARM cross-compiler.

Each subfolder contains the following target- and compiler-specific
files:

   keyword.h
      Type and macro definitions for this target and cross-compiler.

   build.sh
      A unix-style script to compile and link the benchmark.

To generate the binary executable for a given target and cross-compiler,
"cd" to the corresponding subfolder, check that the PATH definitions in
the build script (build.sh) are suitable for your workstation, and
execute the build script.

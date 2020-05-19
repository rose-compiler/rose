![Megachiropteran](logo.png)

# Tools

🦇

This repository contains the "Megachiroptean" tools.  Their general design is that each tool does one simple thing, and thus
they can be chained together to do more complex things.  Commonly used tools have short names, and less common tools have
longer, more descriptive names.  Every tool supports certain common features, among which is a "--help" switch that provides
detailed information about the tool.

## Common tools

* `bat-ana` analyzes a specimen and produces an analysis state file.  Most other tools read these state files.

* `bat-cc` calling convention analysis.

* `bat-cfg` dumps control flow graphs of various types to GraphViz files, which can be converted to various
  image and document formats (including SVG, JPEG, and PDF) or viewed interactively.

* `bat-cg` is similar to bat-cfg, but for function call graphs.

* `bat-container` dumps all details about an ELF or PE container.

* `bat-dis` disassembles a specimen to produce an assembly listing. This tool has many command-line switches to
  control its output. The `bat-dis-simple` executable is a quick way to specify switches that cause the
  output to be quite minimal.

* `bat-linear` is a simple linear disassembler that doesn't account for any control-flow.

* `bat-lsb` lists information about each basic block.

* `bat-lsd` lists information about each static data block.

* `bat-lsf` lists information about each function.

* `bat-mem` lists the specimen's memory map or extracts parts of it into files of various formats.

* `bat-prop` is used to query various simple properties about a specimen, such as the number of functions.

* `bat-trace` runs a program natively under a debugger to generate a trace or to compare a program with a previous trace.

## Less common tools

* `bat-stack-deltas` produces a list of instruction addresses and the difference between the stack pointer at the start
  and end of that instruction with respect to the stack pointer at the beginning of the function.

## Tools under development

# Kinds of input files

The `bat-ana` tool preprocesses various specimens to create an RBA file which is then read by most other
tools. The following types of specimens can be processed (for the most up-to-date information see the `bat-ana --help`.

* ELF (Linux, Unix, etc) executables, shared libraries, and core dumps.

* PE (Windows) executables and shared libraries.

* ELF object files. These have limited utility for analysis since they're not yet mapped to virtual memory and they may
  have missing or misleading data.

* Archives of ELF object files, also known as statically linked libraries.  As with object files, an archive of object
  files has limited information for analysis.

* Motorola S-Records, a common text format for firmware.

* Intel-HEX, a common text format for firmware.

* Running processes on Linux.

* Linux native executables can be partially run to cause their built-in dynamic linker to link shared libraries, then
  the executable is stopped, ROSE grabs its memory, and the process is killed.

* Files containing raw memory dumps. These are just files that contain the contents of some part of virtual memory.

* A custom format designed for analyzing a snapshot of the firmware running on hardware.

* Any combination of the above formats, although some combinations don't make much sense. For instance, you can combine
  an ELF executable file with the memory obtained from a running process and augmented with additional memory dumps such
  as the Linux VDSO.

# Instruction set architectures

ROSE supports the following instruction set architectures. Please refer to the ROSE documentation for the most
up-to-date list.

* Intel x86 family such as i386, and Pentium.  ROSE can disassemble these instructions and knows their semantics.

* AMD64 family such as x86-64, x86_64, x64 and Intel 64.  ROSE can disassemble these instructions and knows their
  semantics.

* Motorola 68000 family. ROSE can disassemble these instructions and knows their semantics.

* PowerPC. ROSE can disassemble these instructions and knows their semantics.

* PowerPC-64. ROSE can disassemble these instructions and knows their semantics.

* MIPS. ROSE can disassemble these instructions but does not know what they do.

* ARM AArch A64.


# Dependencies

ROSE must be configured with at least binary analysis support (`--enable-languages=binaries`). Although not required,
turning on the following software makes more binary analysis features available. The parenthesized version numbers are
those used and tested during development.

* boost (1.57.0 through 1.73.0 except 1.65.x). A subset of the following libraries are needed depending on how ROSE is
  configured: chrono, date_time, filesystem, iostreams, random, regex, serialization, system, thread.

* dlib (18.x, but especially 18.17) for some graph altorithms

* libgcrypt (any recent version) for calculating hashes for some binary analysis.

* libsqlite (any recent version, especially 3.23.1) for concolic testing databases.

* z3 (4.8.4 or later) for model checking

# Tutorial

## Documentation

All tools understand a `--help` (also `-h`) switch which causes them to print documentation similar to a man page.

## RBA files

Most of the tools operate on (or produce) a ROSE binary analysis (RBA) state file, usually named "*.rba". If no RBA file
name is specified on the command-line, or the name "-" (single hyphen) is specified, then the RBA file is read from
standard input (or written to standard output) provided the operating system and C++ library supports binary I/O on this
stream.

An RBA file is basically a serialization of the entire ROSE binary analysis state.  RBA files come in three formats
chosen by the `--state-format` switch. For technical reasons, the format of an RBA input file cannot be detected
automatically, but must be specified with this switch.  The formats are binary (the default), text, and XML. The binary
format is the smallest and fastest and can be compressed for even more space savings.  The text format is ASCII text and
therefore somewhat larger and slower. The XML format is orders of magnitude larger and slower but has the benefit of
being convertable to JSON and understood by non-ROSE tools.

Although not officially supported and tested, RBA files are generally portable between different versions of ROSE and
these tools. They're also portable between tools compiled with different compilers, compiler optimizations, and C++
language standards. They're not portable across different architectures (e.g., a tool running on a 32-bit machine vs. a
tool running on a 64-bit machine), although the text formats are portable between different byte orders.

## Generating RBA files

These tools read RBA files for one main reason: initializing a tool's analysis state from an RBA file is usually many
times faster than initializing it from scratch.  Therefore, one can produce an expensive RBA file once and then use it
with many other tools.  Some tools can even add additional information to the state and produce a new RBA file.

The "bat-ana" tool is the primary tool for generating RBA files. It takes as input a binary specimen such as an ELF
executable and analyzes it by parsing the ELF container, mapping file regions into virtual memory, finding and decoding
instructions and static data, organizing instructions into basic blocks and functions, determining calling conventions,
analyzing stack behavior, analyzing whether functions return, finding no-op sequences, etc., and finally producing an
RBA file.

For example, here's a command that creates an RBA file by analyzing the /bin/bash ELF executable:

    bat-ana -o bash.rba /bin/bash

## Obtaining a disassembly listing

Once you have an RBA file, you can obtain a disassembly listing of the entire file or some function(s) within the
file. The "bat-dis" tool has a multitude of command-line options for controlling the style of output and defaults to
showing quite a bit of information. An easy way to get a simplified listing is with the "bat-dis-simple" tool, which is
identical to "bat-dis" except its switches default to produce less verbose output.

        bat-dis bash.rba

To list one function:

        bat-dis --function=main bash.rba
        bat-dis --function=0x08041000 bash.rba

The output from "bat-dis" and "bat-dis-simple" is not intended for re-assembly, but rather human consumption.

## Obtaining a control flow graph

The "bat-cfg" tool generates control flow graphs. It can produce global CFGs that describe the control flow for an
entire specimen, or a single function. Output is adjustable and can be text files describing all the information stored
in the CFG, or GraphViz files that can be converted to various formats or viewed interactively.

        bat-cfg --function=main --format=gv bash.rba |dot -Tpng > main-cfg.png

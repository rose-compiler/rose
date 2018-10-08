description = """
This tool loads and parses a binary specimen, then disassembles its instructions and partitions them into
basic blocks, functions, and data.  Various analyses are performed during and after the partitioning step.
Results are printed in a human-readable assembly format (not intended to be re-assembled).  The specimen
can be a binary container such as an ELF or PE executable, Motorola S-Records, raw binary memory dumps,
a running Linux process, a process stopped in a debugger, or certain combinations thereof. This tool is
cross-platform, allowing specimens to be disassembled which are not native to the platform on which this
tool is running."""

import sys
from Rose.BinaryAnalysis.Partitioner2 import *

Engine().parse(sys.argv[1:], "disassembler", description).unparse()

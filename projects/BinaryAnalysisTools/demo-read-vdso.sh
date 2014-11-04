#!/bin/bash
# This is a demo showing how to disassemble the VDSO that is
# mapped into process address space on Linux.

: ${RG_SRC:=$ROSE_SRC}
: ${RG_BLD:=$ROSE_BLD}
export RG_SRC RG_BLD
if [ "$RG_SRC" = "" -o "$RG_BLD" = "" ]; then
    echo "The ROSE_SRC or RG_SRC environment variable must be" >&2
    echo "set to point to the top of the ROSE source tree, and" >&2
    echo "the ROSE_BLD or RG_BLD environment variable must" >&2
    echo "be the top of the ROSE build tree." >&2
    exit 1
fi

banner() {
    local banner="Run command..."
    echo -ne '\033[32m'
    echo -n "$banner"
    echo -e '\033[0m'
}

runCommand() {
    local cmd="$*"
    read -p "$ " -e -i "$cmd" cmd
    eval "$cmd"
}

pause() {
    local prompt="${1-Press ENTER to continue.} "
    read -p "$prompt"
}

cat <<EOF
  ================================================================================
  == Introduction
  ================================================================================

  This is a demo showing how to disassemble the VDSO that is mapped
  into process address space on Linux.

  The Linux kernel maps a virtual dynamic shared object (vdso) into
  the address space of every process.  This library contains the
  instructions for how a process should make system calls and is not
  present in any file on the system.  A good description can be found
  at [[http://www.trilithium.com/johan/2005/08/linux-gate/]].

EOF

pause

cat <<EOF

  ================================================================================
  == Preparation
  ================================================================================
      
  The first problem is how to get a copy of the VDSO if it doesn't
  exist in the filesystem.  The "dumpMemory" tool and "run:" specimen
  name scheme are useful for this purpose.  First, get information
  about how to use this tool:

EOF

banner
runCommand ./dumpMemory --help
pause

cat <<EOF

  We'll use the $ROSE_SOURCE/binaries/samples/i386-mixed executable
  for this demo because it's a small, statically linked, 32-bit, ELF
  executable that does nothing but segfault (via HLT instruction). But
  you could use any program (even /bin/ls). We'll need it to be
  executable for this demo, so copy it to the CWD and change its
  permissions:

EOF

banner
runCommand cp $RG_SRC/binaries/samples/i386-mixed .
runCommand chmod 700 i386-mixed
pause

cat <<EOF

  ================================================================================
  == Finding the VDSO address
  ================================================================================

  We'll need to know where the VDSO segment is in memory first. One
  way to get this is by running:

      $ i386 -R ./dumpMemory run:i386-mixed

  The "i386 -R" command turns off address randomization -- we want
  Linux to load things at the same address every time we run,
  otherwise we'll have a hard time asking for the VDSO memory.

  The "run:" means that ROSE will parse the ELF container like
  normal, and then it will execute the specimen and copy the
  process memory.  The specimen is only allowed to execute until
  it reaches any executable address known to ROSE -- in other
  words, the dynamic linker will be allowed to run and then the
  process is stopped, its memory read, and then it is killed.

EOF

banner
runCommand i386 -3R ./dumpMemory run:i386-mixed

cat <<EOF

  The warning is okay -- dumpMemory is just informing us that it
  didn't dump anything. But its side effect is to show us a map of
  virtual addresses (ROSE's MemoryMap data structure). We can see
  that the VDSO occupies 4096 (0x1000) bytes starting at
  0xb7fff000 (that is, if your loader is operating the same way
  mine is).

  You might want to run the command another time to make sure that
  the VDSO is always at the same address.

EOF

banner
runCommand i386 -3R ./dumpMemory run:i386-mixed

echo
VDSO_ADDR=b7fff000
read -p "What is the address (use hexadecimal w/out leading 0x)? " -e -i $VDSO_ADDR VDSO_ADDR

cat <<EOF

  ================================================================================
  == Getting a copy of the VDSO
  ================================================================================

  Next, we need to get a copy of the VDSO into the filesystem.
  The switches we're using are:

    --binary        Copy memory directly to a binary file.
    --prefix        Prefix for output file names
    --where         Address interval containing the VDSO

EOF

banner
runCommand i386 -3R ./dumpMemory --binary --prefix=i386-mixed- --where=0x$VDSO_ADDR+0x1000 run:i386-mixed

cat <<EOF

  This should have created a file named "i386-mixed-$VDSO_ADDR.raw"
  that contains the contents of the VDSO section.  The file is an ELF
  shared library:

EOF

banner
runCommand file i386-mixed-$VDSO_ADDR.raw

cat <<EOF

  It also created a file named "i386-mixed-load" that contains some
  additional information about what was dumped:

EOF

banner
runCommand cat i386-mixed-load

cat <<EOF

  ================================================================================
  == Disassembling the VDSO
  ================================================================================

  And finally, we can disassemble the VDSO. This tool has a lot of
  options:

EOF

banner
runCommand ./recursiveDisassemble --help
pause

cat <<EOF

  But all we really need is the file name:

EOF

TMP_FILE=$(tempfile)
banner
runCommand ./recursiveDisassemble i386-mixed-$VDSO_ADDR.raw >$TMP_FILE
pause "Press ENTER to see standard output using ${PAGER-less}."
${PAGER-less} $TMP_FILE
pause
rm -f $TMP_FILE

cat <<EOF

  That's all folks.  Although the disassembler output has been removed,
  the executable that we copied early in this demo and the VDSO file
  that we extracted from the process are still present if you feel like
  playing more with the disassembler.
EOF

exit 0
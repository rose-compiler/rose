# Tool purpose #

This tool doesn't really do anything other than serve as a simple
example demonstrating how a tool can be written. It aims to support
and demonstrate the items in the [ROSE Tool Release Checklist][1].

Like all projects, it is not intended that this project be compiled as
part of ROSE, but rather that this directory is compiled only after
ROSE is already installed. Instructions are contained below.

Being a working example, this tool is also used as part of ROSE
Portability Testing.

# Runtime Dependencies #

These are the dependencies that must be present for the tool to
run. Building the tool may require different/additional dependencies.

* The ROSE library, version 0.11.145.133 or later.

# Building the tool #

To build the tool, first install the ROSE library. Then configure this
tool using the `configure` command, like this:

```
$ ./configure /path/to/ROSE/installation/root install
```

The tool will be installed in the same directory as ROSE.

The `configure` command for this project is a simple shell script, and
more information about its usage can be found by running it with
"--help" or by looking at the documentation in the script.

# Running the tool #

You should not need to set the `LD_LIBRARY_PATH` environment variable
to run this tool from its final installation point (the build system
and/or the binary release mechanism should have taken care of that for
you).  You may set the shell's `PATH` environment variable so it
contains the "bin" directory found in the ROSE installation root, but
this is not necessary.

The tool's documentation can be obtained by running the tool with the
"--help" or "-h" switch.

[1]: https://rosecompiler2.llnl.gov/gitlab/main/rose-compiler/rose/-/wikis/Home/ROSE-Tool-Release-Checklist

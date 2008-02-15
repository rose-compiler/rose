=========================================
PDFlib Lite Source for Mac OS 9 (Classic)
=========================================

Note:
please see readme-source-unix.txt for information on using PDFlib on Mac OS X.

PDFlib Lite supports Carbon and Classic builds.

To compile PDFlib with Metrowerks CodeWarrior, open the supplied
project file PDFlib.mcp with the Metrowerks IDE. The project file
contains targets for building a static library in Classic and Carbon
flavors, as well as a target for a shared Carbonized version.

Don't worry about "access path not found" warnings when building PDFlib.

Separate project files for building various C and C++ sample programs
can be found in bind:pdflib:c:samples.mcp and bind:pdflib:cpp:samples.mcp.
These can be used to test the newly created library. The tests create simple
command-line programs without any fancy user interface.

Note that not all tests will succeed because they need features which
require commercial PDFlib products. Also, don't worry about warnings
regarding some access path not being found.

In order to make the C and C++ samples work with Classic you must change the
SearchPath entries in the C to use Mac volume syntax instead of Unix-style
path names, e.g. in image.c change the line

    char *searchpath = "../data";

    to

    char *searchpath = ":data";

Note that on OS 9 only C and C++ are available; other language wrappers are
no longer supported. All language wrappers are fully supported on Mac OS X,
though.

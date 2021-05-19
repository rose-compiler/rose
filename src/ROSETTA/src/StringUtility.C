// ROSETTA needs Rose::StringUtility functions before librose is generated. There's are two broad ways to make this happen:
//
//  1. Have special rules in all our build systems that ensure that ROSETTA's depedencies, regardless of where they're located
//     in the source tree, are compiled into libraries that ROSETTA can use before the rest of librose objects are built.
//
//     a. Originally, ROSETTA's dependencies that were also dependencies of librose were placed in the the src/util directory.
//        Unfortunately this was a bad choice of name because developers thought this directory was for ROSE utilities (not
//        just ROSETTA utilities) and the directory became confusing and redundant with src/roseSupport.
//
//     b. Some of the features that are useful to ROSETTA are also useful to users of librose (end users, tools, testing,
//        analyses, etc) and are therefore rightfully under the ::Rose namespace since they're publically visible. This means
//        that the .h files must be organized by namespace, and naturally we want to keep the .C files next to them for ease of
//        development. This means the build systems have to know that some .C files under ::Rose need to be compiled early for
//        ROSETTA's sake, but the neighboring files (which often depend on ROSETTA's output) cannot be compiled at the same
//        time.
//
//     Needless to say, using the build systems to work around these nuances in compiling order makes for confusing build
//     systems when the rest of the build configuration doesn't need to take any of this into account.
//
//  2. Put all the ROSETTA dependencies next to ROSETTA (as in same directory or below).  This is how build systems like
//     sources to be set up, so no surprising workarounds necessary in any of the build systems.
//
//     a. We could place the ROSETTA dependency .C files in the ROSETTA directory but leave the corresponding .h files at their
//        normal location under Rose/ so they're still included as #include <Rose/....h> (when we build librose, and when users
//        build their own tools using installed ROSE headers that also have #include's). Splitting the corresponding .h and .C
//        file into different directories is less than ideal.
//
//     b. We could have two copies of the .C file: one used by ROSETTA and one used by librose. This is a nightmare, but at
//        least it solves the naming issues: the file lives in two places because it's both a ROSETTA dependency and a librose
//        dependency. Besides having two copies of the code (and having to maintain both), we're compiling these shared
//        dependencies twice.
//
//     c. We could create .C files for ROSETTA that are copied by the C preprocessor from their normal location using #include.
//        Like the previous item, this also solves the naming issue but not the compiling twice issue.
//
// I'm deciding to go with 2.c, realizing that none of the shared dependencies include the huge ROSETTA output headers and
// therefore should compile quickly.

#include <Rose/StringUtility/StringUtility.C>

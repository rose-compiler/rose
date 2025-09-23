//===-- tools/f18/f18-parse-demo.cpp --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// F18 parsing demonstration.
//   f18-parse-demo [ -E | -fdump-parse-tree | -funparse-only ]
//     foo.{f,F,f77,F77,f90,F90,&c.}
//
// By default, runs the supplied source files through the F18 preprocessing and
// parsing phases, reconstitutes a Fortran program from the parse tree, and
// passes that Fortran program to a Fortran compiler identified by the $F18_FC
// environment variable (defaulting to gfortran).  The Fortran preprocessor is
// always run, whatever the case of the source file extension.  Unrecognized
// options are passed through to the underlying Fortran compiler.
//
// This program is actually a stripped-down variant of f18.cpp, a temporary
// scaffolding compiler driver that can test some semantic passes of the
// F18 compiler under development.

//----------------------------------------------------------------------------//

//TODO: This should be an command line option
#define DUMP_PARSE_TREE 0

// Fortran front end driver main program for ROSE scaffolding.

#include "sage3basic.h"
#include "../../frontend/Experimental_Flang_ROSE_Connection/sage-build.h"
#include "FlangParseArgs.hh"

std::vector<std::string> filesToDelete;

static void CleanUpAtExit() {
  for (const auto &path : filesToDelete) {
    if (!path.empty()) {
      llvm::sys::fs::remove(path);
    }
  }
}

// Turn on CPU timing
#if _POSIX_C_SOURCE >= 199309L && _POSIX_TIMERS > 0 && _POSIX_CPUTIME && \
    defined CLOCK_PROCESS_CPUTIME_ID
static constexpr bool canTime{true};
double CPUseconds() {
  struct timespec tspec;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tspec);
  return tspec.tv_nsec * 1.0e-9 + tspec.tv_sec;
}
#else
static constexpr bool canTime{false};
double CPUseconds() { return 0; }
#endif

void Exec(std::vector<llvm::StringRef> &argv, bool verbose = false) {
  if (verbose) {
    for (size_t j{0}; j < argv.size(); ++j) {
      llvm::errs() << (j > 0 ? " " : "") << argv[j];
    }
    llvm::errs() << '\n';
  }
  std::string ErrMsg;
  llvm::ErrorOr<std::string> Program = llvm::sys::findProgramByName(argv[0]);
  if (!Program)
    ErrMsg = Program.getError().message();
  if (!Program ||
      llvm::sys::ExecuteAndWait(
          Program.get(), argv, std::nullopt, {}, 0, 0, &ErrMsg)) {
    llvm::errs() << "execvp(" << argv[0] << ") failed: " << ErrMsg << '\n';
    exit(EXIT_FAILURE);
  }
}

void RunOtherCompiler(DriverOptions &driver, char *source, char *relo) {
  std::vector<llvm::StringRef> argv;
  for (size_t j{0}; j < driver.fcArgs.size(); ++j) {
    argv.push_back(driver.fcArgs[j]);
  }
  char dashC[3] = "-c", dashO[3] = "-o";
  argv.push_back(dashC);
  argv.push_back(dashO);
  argv.push_back(relo);
  argv.push_back(source);
  Exec(argv, driver.verbose);
}

std::string RelocatableName(const DriverOptions &driver, std::string path) {
  if (driver.compileOnly && !driver.outputPath.empty()) {
    return driver.outputPath;
  }
  std::string base{path};
  auto slash{base.rfind("/")};
  if (slash != std::string::npos) {
    base = base.substr(slash + 1);
  }
  std::string relo{base};
  auto dot{base.rfind(".")};
  if (dot != std::string::npos) {
    relo = base.substr(0, dot);
  }
  relo += ".o";
  return relo;
}

int exitStatus{EXIT_SUCCESS};

std::string CompileFortran(
    std::string path, Fortran::parser::Options options, DriverOptions &driver) {
  if (!driver.forcedForm) {
    auto dot{path.rfind(".")};
    if (dot != std::string::npos) {
      std::string suffix{path.substr(dot + 1)};
      options.isFixedForm = suffix == "f" || suffix == "F" || suffix == "ff";
    }
  }
  options.searchDirectories = driver.searchDirectories;
  Fortran::parser::AllSources allSources;
  Fortran::parser::AllCookedSources allCookedSources{allSources};
  Fortran::parser::Parsing parsing{allCookedSources};

  auto start{CPUseconds()};
  parsing.Prescan(path, options);
  if (!parsing.messages().empty() &&
      (driver.warningsAreErrors || parsing.messages().AnyFatalError())) {
    llvm::errs() << driver.prefix << "could not scan " << path << '\n';
    parsing.messages().Emit(llvm::errs(), parsing.allCooked());
    exitStatus = EXIT_FAILURE;
    return {};
  }
  if (driver.dumpProvenance) {
    parsing.DumpProvenance(llvm::outs());
    return {};
  }
  if (options.prescanAndReformat) {
    parsing.messages().Emit(llvm::errs(), allCookedSources);
    if (driver.noReformat) {
      parsing.DumpCookedChars(llvm::outs());
    } else {
      parsing.EmitPreprocessedSource(llvm::outs(), driver.lineDirectives);
    }
    return {};
  }
  parsing.Parse(llvm::outs());
  auto stop{CPUseconds()};
  if (driver.timeParse) {
    if (canTime) {
      llvm::outs() << "parse time for " << path << ": " << (stop - start)
                   << " CPU seconds\n";
    } else {
      llvm::outs() << "no timing information due to lack of clock_gettime()\n";
    }
  }

  parsing.ClearLog();
  parsing.messages().Emit(llvm::errs(), parsing.allCooked());
  if (!parsing.consumedWholeFile()) {
    parsing.EmitMessage(llvm::errs(), parsing.finalRestingPlace(),
        "parser FAIL (final position)", "error: ", llvm::raw_ostream::RED);
    exitStatus = EXIT_FAILURE;
    return {};
  }
  if ((!parsing.messages().empty() &&
          (driver.warningsAreErrors || parsing.messages().AnyFatalError())) ||
      !parsing.parseTree()) {
    llvm::errs() << driver.prefix << "could not parse " << path << '\n';
    exitStatus = EXIT_FAILURE;
    return {};
  }

  auto &parseTree{*parsing.parseTree()};

  // Transform the parse tree using Rose::builder
  if (driver.externalBuilder) {
    auto start{CPUseconds()};
    Rose::builder::Build(parseTree, allCookedSources);
    auto stop{CPUseconds()};
    if (canTime) {
      llvm::outs() << "Rose::Build time for " << path << ": " << (stop - start)
                   << " CPU milliseconds\n";
    }

    // Always dump the parse tree for now
#if DUMP_PARSE_TREE
    Fortran::parser::DumpTree(llvm::outs(), parseTreeCopy);
#endif
    return {};
  }

  if (driver.dumpParseTree) {
#if DUMP_PARSE_TREE
    Fortran::parser::DumpTree(llvm::outs(), parseTree);
#endif
    return {};
  }
  if (driver.dumpUnparse) {
    Unparse(llvm::outs(), parseTree, driver.langOpts, driver.encoding,
        true /*capitalize*/,
        options.features.IsEnabled(
            Fortran::common::LanguageFeature::BackslashEscapes));
    return {};
  }
  if (driver.syntaxOnly) {
    return {};
  }

  std::string relo{RelocatableName(driver, path)};

  llvm::SmallString<32> tmpSourcePath;
  {
    int fd;
    std::error_code EC =
        llvm::sys::fs::createUniqueFile("f18-%%%%.f90", fd, tmpSourcePath);
    if (EC) {
      llvm::errs() << EC.message() << "\n";
      std::exit(EXIT_FAILURE);
    }
    llvm::raw_fd_ostream tmpSource(fd, /*shouldClose*/ true);
    Unparse(tmpSource, parseTree, driver.langOpts, driver.encoding,
        true /*capitalize*/,
        options.features.IsEnabled(
            Fortran::common::LanguageFeature::BackslashEscapes));
  }

  RunOtherCompiler(driver, tmpSourcePath.data(), relo.data());
  filesToDelete.emplace_back(tmpSourcePath);
  if (!driver.compileOnly && driver.outputPath.empty()) {
    filesToDelete.push_back(relo);
  }
  return relo;
}

std::string CompileOtherLanguage(std::string path, DriverOptions &driver) {
  std::string relo{RelocatableName(driver, path)};
  RunOtherCompiler(driver, path.data(), relo.data());
  if (!driver.compileOnly && driver.outputPath.empty()) {
    filesToDelete.push_back(relo);
  }
  return relo;
}

void Link(std::vector<std::string> &relocatables, DriverOptions &driver) {
  std::vector<llvm::StringRef> argv;
  for (size_t j{0}; j < driver.fcArgs.size(); ++j) {
    argv.push_back(driver.fcArgs[j].data());
  }
  for (auto &relo : relocatables) {
    argv.push_back(relo.data());
  }
  if (!driver.outputPath.empty()) {
    char dashO[3] = "-o";
    argv.push_back(dashO);
    argv.push_back(driver.outputPath.data());
  }
  Exec(argv, driver.verbose);
}

class SgSourceFile;
int flang_external_builder_main(int argc, char *const argv[], SgSourceFile* roseSourceFile)
{
  atexit(CleanUpAtExit);

  // The SageTreeBuilder must be initialized before using it via getSageTreeBuilder()
  initSageTreeBuilder(roseSourceFile, Rose::builder::SageTreeBuilder::LanguageEnum::Fortran);

  // Initialize Flang command-line arguments
  DriverContext ctx{};
  exitStatus = ParseFlangArgs(argc, argv, ctx);
  if (exitStatus != EXIT_SUCCESS) return exitStatus;

  if (!ctx.anyFiles) {
    ctx.driver.dumpUnparse = true;
    CompileFortran("-", ctx.options, ctx.driver);
    return exitStatus;
  }

  for (const auto &path : ctx.fortranSources) {
    std::string relo{CompileFortran(path, ctx.options, ctx.driver)};
    if (!ctx.driver.compileOnly && !relo.empty()) {
      ctx.relocatables.push_back(relo);
    }
  }

  for (const auto &path : ctx.otherSources) {
    std::string relo{CompileOtherLanguage(path, ctx.driver)};
    if (!ctx.driver.compileOnly && !relo.empty()) {
      ctx.relocatables.push_back(relo);
    }
  }

  if (!ctx.relocatables.empty()) {
    Link(ctx.relocatables, ctx.driver);
  }

  return exitStatus;
}

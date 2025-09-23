#pragma once

#include "flang/Parser/dump-parse-tree.h"
#include "flang/Parser/message.h"
#include "flang/Parser/parse-tree-visitor.h"
#include "flang/Parser/parse-tree.h"
#include "flang/Parser/parsing.h"
#include "flang/Parser/provenance.h"
#include "flang/Parser/unparse.h"
#include "flang/Support/Fortran-features.h"
#include "flang/Support/LangOptions.h"
#include "flang/Support/default-kinds.h"
#include "llvm/Support/Errno.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <list>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

struct DriverOptions {
  DriverOptions() {}
  bool verbose{false}; // -v
  bool compileOnly{false}; // -c
  std::string outputPath; // -o path
  std::vector<std::string> searchDirectories{"."s}; // -I dir
  Fortran::common::LangOptions langOpts;
  bool forcedForm{false}; // -Mfixed or -Mfree appeared
  bool warnOnNonstandardUsage{false}; // -Mstandard
  bool warnOnSuspiciousUsage{false}; // -pedantic
  bool warningsAreErrors{false}; // -Werror
  Fortran::parser::Encoding encoding{Fortran::parser::Encoding::LATIN_1};
  bool lineDirectives{true}; // -P disables
  bool syntaxOnly{false};
  bool dumpProvenance{false};
  bool noReformat{false}; // -E -fno-reformat
  bool dumpUnparse{false};
  bool dumpParseTree{false};
  bool timeParse{false};
  bool externalBuilder{false}; // ROSE
  std::vector<std::string> fcArgs;
  const char *prefix{nullptr};
};

struct DriverContext {
  DriverOptions driver{};
  Fortran::parser::Options options{};
  std::vector<std::string> fortranSources;
  std::vector<std::string> otherSources;
  std::vector<std::string> relocatables;
  bool anyFiles{false};
};

inline int ParseFlangArgs(int argc, char *const argv[], DriverContext &ctx) {
  int exitStatus{EXIT_SUCCESS};

  // Transfer argv to a list of strings
  std::list<std::string> args{};
  for (int ii = 0; ii < argc; ++ii) {
    args.emplace_back(argv[ii]);
  }

  std::string prefix{args.front()};
  args.pop_front();
  prefix += ": ";
  ctx.driver.prefix = prefix.data();

  ctx.options.predefinitions.emplace_back("__F18", "1");
  ctx.options.predefinitions.emplace_back("__F18_MAJOR__", "1");
  ctx.options.predefinitions.emplace_back("__F18_MINOR__", "1");
  ctx.options.predefinitions.emplace_back("__F18_PATCHLEVEL__", "1");

  Fortran::common::IntrinsicTypeDefaultKinds defaultKinds;

  while (!args.empty()) {
    std::string arg{std::move(args.front())};
    args.pop_front();
    if (arg.empty() || arg == "-Xflang") {
    } else if (arg.at(0) != '-') {
      ctx.anyFiles = true;
      auto dot{arg.rfind(".")};
      if (dot == std::string::npos) {
        ctx.driver.fcArgs.push_back(arg);
      } else {
        std::string suffix{arg.substr(dot + 1)};
        if (suffix == "f" || suffix == "F" || suffix == "ff" ||
            suffix == "f90" || suffix == "F90" || suffix == "ff90" ||
            suffix == "f95" || suffix == "F95" || suffix == "ff95" ||
            suffix == "cuf" || suffix == "CUF" || suffix == "f18" ||
            suffix == "F18" || suffix == "ff18") {
          ctx.fortranSources.push_back(arg);
        } else if (suffix == "o" || suffix == "a") {
          ctx.relocatables.push_back(arg);
        } else {
          ctx.otherSources.push_back(arg);
        }
      }
    } else if (arg == "-") {
      ctx.fortranSources.push_back("-");
    } else if (arg == "--") {
      while (!args.empty()) {
        ctx.fortranSources.emplace_back(std::move(args.front()));
        args.pop_front();
      }
      break;
    } else if (arg == "-Mextend") {
      ctx.options.fixedFormColumns = 132;
    } else if (arg == "-Mstandard") {
      ctx.driver.warnOnNonstandardUsage = true;
    } else if (arg == "-pedantic") {
      ctx.driver.warnOnNonstandardUsage = true;
      ctx.driver.warnOnSuspiciousUsage = true;
    } else if (arg == "-Werror") {
      ctx.driver.warningsAreErrors = true;
    } else if (arg == "-ed") {
      ctx.options.features.Enable(Fortran::common::LanguageFeature::OldDebugLines);
    } else if (arg == "-E") {
      ctx.options.prescanAndReformat = true;
    } else if (arg == "-P") {
      ctx.driver.lineDirectives = false;
    } else if (arg == "-fno-reformat") {
      ctx.driver.noReformat = true;
    } else if (arg == "-fbackslash") {
      ctx.options.features.Enable(
          Fortran::common::LanguageFeature::BackslashEscapes);
    } else if (arg == "-fno-backslash") {
      ctx.options.features.Enable(
          Fortran::common::LanguageFeature::BackslashEscapes, false);
    } else if (arg == "-fdump-provenance") {
      ctx.driver.dumpProvenance = true;
    } else if (arg == "-fdump-parse-tree") {
      ctx.driver.dumpParseTree = true;
    } else if (arg == "-funparse") {
      ctx.driver.dumpUnparse = true;
    } else if (arg == "-ftime-parse") {
      ctx.driver.timeParse = true;
    } else if (arg == "-fparse-only" || arg == "-fsyntax-only") {
      ctx.driver.syntaxOnly = true;
    } else if (arg == "-fexternal-builder") {
      ctx.driver.externalBuilder = true;
    } else if (arg == "-c") {
      ctx.driver.compileOnly = true;
    } else if (arg == "-o") {
      ctx.driver.outputPath = args.front();
      args.pop_front();
    } else if (arg.substr(0, 2) == "-D") {
      auto eq{arg.find('=')};
      if (eq == std::string::npos) {
        ctx.options.predefinitions.emplace_back(arg.substr(2), "1");
      } else {
        ctx.options.predefinitions.emplace_back(
            arg.substr(2, eq - 2), arg.substr(eq + 1));
      }
    } else if (arg.substr(0, 2) == "-U") {
      ctx.options.predefinitions.emplace_back(
          arg.substr(2), std::optional<std::string>{});
    } else if (arg == "-r8" || arg == "-fdefault-real-8") {
      defaultKinds.set_defaultRealKind(8);
    } else if (arg == "-i8" || arg == "-fdefault-integer-8") {
      defaultKinds.set_defaultIntegerKind(8);
      defaultKinds.set_defaultLogicalKind(8);
    } else if (arg == "-help" || arg == "--help" || arg == "-?") {
      llvm::errs()
          << "f18-parse-demo options:\n"
          << "  -Mfixed | -Mfree     force the source form\n"
          << "  -Mextend             132-column fixed form\n"
          << "  -f[no-]backslash     enable[disable] \\escapes in literals\n"
          << "  -M[no]backslash      disable[enable] \\escapes in literals\n"
          << "  -Mstandard           enable conformance warnings\n"
          << "  -r8 | -fdefault-real-8 | -i8 | -fdefault-integer-8  "
             "change default kinds of intrinsic types\n"
          << "  -Werror              treat warnings as errors\n"
          << "  -ed                  enable fixed form D lines\n"
          << "  -E                   prescan & preprocess only\n"
          << "  -ftime-parse         measure parsing time\n"
          << "  -fsyntax-only        parse only, no output except messages\n"
          << "  -funparse            parse & reformat only, no code "
             "generation\n"
          << "  -fdump-provenance    dump the provenance table (no code)\n"
          << "  -fdump-parse-tree    dump the parse tree (no code)\n"
          << "  -v -c -o -I -D -U    have their usual meanings\n"
          << "  -help                print this again\n"
          << "Other options are passed through to the $F18_FC compiler.\n";
      return exitStatus;
    } else if (arg == "-V") {
      llvm::errs() << "\nf18-parse-demo\n";
      return exitStatus;
    } else {
      ctx.driver.fcArgs.push_back(arg);
      if (arg == "-v") {
        ctx.driver.verbose = true;
      } else if (arg == "-I") {
        ctx.driver.fcArgs.push_back(args.front());
        ctx.driver.searchDirectories.push_back(args.front());
        args.pop_front();
      } else if (arg.substr(0, 2) == "-I") {
        ctx.driver.searchDirectories.push_back(arg.substr(2));
      }
    }
  }

  if (ctx.driver.warnOnNonstandardUsage) {
    ctx.options.features.WarnOnAllNonstandard();
  }
  if (ctx.driver.warnOnSuspiciousUsage) {
    ctx.options.features.WarnOnAllUsage();
  }
  if (!ctx.options.features.IsEnabled(
          Fortran::common::LanguageFeature::BackslashEscapes)) {
    ctx.driver.fcArgs.push_back("-fno-backslash"); // PGI "-Mbackslash"
  }

  return exitStatus;
}

#ifndef ROSE_Logger_H
#define ROSE_Logger_H

#include <map>
#include <string>

namespace Rose {

#define DEBUG__Rose__Loggers 0

//! A nested logging mechanism to facilitate debbuging of tree traversal.
class Logger {
  public:
    enum class Level {
      debug   = 0,
      info    = 1,
      warning = 2,
      error   = 3,
      fatal   = 4,
      enter   = 5, //< Used internally for "Enter: mnenonic(ptr_id)"
      leave   = 6  //< Used internally for "Leave: mnenonic(ptr_id)"
    };

  private:
    /// Permit the runtime (CLI) to control the verbosity of any logger. It override the compile-time value (provided by constructor).
    static std::map<std::string, Logger::Level> mnemonic_to_level;

    static Logger root;
    static Logger * current;

    static std::string indent;
    static bool indent_first;

#if DEBUG__Rose__Loggers
    /// Used to ensure that mnemonic (identifier) are used only once.
    static std::map<std::string, std::string> mnemonic_to_function;
#endif

    /// The exact name of the logger (using __PRETTY_FUNCTION__)
    std::string function;
    /// The human readable identifier of the function. Used for display and to set logging level from CLI.
    std::string mnemonic;
    /// A unique pointer identifying the current instance 
    void * ptr_id;
    Level level;

    Logger * parent;
    int depth;

  private:
    void display(Level const lvl, std::string const & message);
    void display(Level const lvl, const char * format, ...);

  public:
    Logger();
    Logger(const char * function_, const char * mnemonic_, void * const ptr_id_, Level const level_);

    ~Logger();

    void debug(const char * format, ...);
    void info(const char * format, ...);
    void warning(const char * format, ...);
    void error(const char * format, ...);
    void fatal(const char * format, ...);
};

}

#endif /* ROSE_Logger_H */


#include "Rose/Logger.h"

#include <iostream>

#include <cassert>

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace Rose {

static const char * print_to_string(const char * format, va_list va_args) {
  size_t req_sz = vsnprintf(nullptr, 0, format, va_args);
  char * buffer = (char*)malloc(req_sz+1);
  assert(buffer != nullptr);
  buffer[req_sz] = '\0';
  vsnprintf(buffer, req_sz+1, format, va_args);
  return buffer;
}

static const char * level_to_string(Logger::Level lvl) {
  switch (lvl) {
    case Logger::Level::debug:   return "Debug   : ";
    case Logger::Level::info:    return "Info    : ";
    case Logger::Level::warning: return "Warning : ";
    case Logger::Level::error:   return "Error   : ";
    case Logger::Level::enter:   return "Enter   : ";
    case Logger::Level::leave:   return "Leave   : ";
    default: abort();
  }
  abort();
}

Logger Logger::root{};
Logger * Logger::current{nullptr};

std::string Logger::indent{"  "};
bool Logger::indent_first{false};

std::map<std::string, Logger::Level> Logger::mnemonic_to_level{};

#if DEBUG__Rose__Loggers
std::map<std::string, std::string> Logger::mnemonic_to_function{};
#endif

void Logger::display(Level const lvl, std::string const & message) {
  if (lvl == Level::fatal) {
    std::cerr << "FATAL: " << mnemonic << "(" << std::hex << ptr_id << "):" << message << std::endl;
  } else {
    bool indentable = indent.size() > 0 && depth > 0;
    if (indent_first && indentable)
      for (size_t i = 0; i < depth; i++)
        std::cerr << indent;
    std::cerr << level_to_string(lvl);
    if (!indent_first && indentable)
      for (size_t i = 0; i < depth; i++)
        std::cerr << indent;
    std::cerr << message << std::endl;
  }
}

void Logger::display(Level const lvl, const char * format, ...) {
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(lvl, message);
}

Logger::Logger() :
  Logger("root", "root", nullptr, Logger::Level::fatal)
{}

Logger::Logger(const char * function_, const char * mnemonic_, void * const ptr_id_, Level const level_) :
  function(function_),
  mnemonic(mnemonic_),
  ptr_id(ptr_id_),
  level(level_),
  parent(current),
  depth(-1)
{
  assert(level <= Level::fatal); // Cannot use Level::enter or Level::leave when constructing a Logger.
  current = this;
  if (parent != nullptr) {
    depth = parent->depth + 1;
  }
#if DEBUG__Rose__Loggers
  auto it_m2f = mnemonic_to_function.find(mnemonic);
  if (it_m2f == mnemonic_to_function.end()) {
    mnemonic_to_function.insert(std::pair<std::string const, std::string const>(mnemonic, function))
  } else {
    assert(it_m2f->second == function);
  }
#endif

  auto it_lvl = mnemonic_to_level.find(mnemonic);
  if (it_lvl != mnemonic_to_level.end()) {
    level = it_lvl->second;
  }

  if (level < Level::fatal)
    display(Level::enter, "%s(%p)", mnemonic.c_str(), ptr_id);
}

Logger::~Logger() {
  current = parent;

  if (level < Level::fatal)
    display(Level::leave, "%s(%p)", mnemonic.c_str(), ptr_id);
}

void Logger::debug(const char * format, ...) {
  if (level > Level::debug) return; 
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(Level::debug, message);
}

void Logger::info(const char * format, ...) {
  if (level > Level::info) return;
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(Level::info, message);
}

void Logger::warning(const char * format, ...) {
  if (level > Level::warning) return;
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(Level::warning, message);
}

void Logger::error(const char * format, ...) {
  if (level > Level::error) return;
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(Level::error, message);
}

void Logger::fatal(const char * format, ...) {
  va_list va_args;
  va_start(va_args, format);
  std::string message(print_to_string(format, va_args));
  va_end(va_args);
  display(Level::fatal, message);
}


}

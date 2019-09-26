
#include "ROSE/proposed/mfb-json.hpp"

#include "MFB/Sage/driver.hpp"

#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>

#include "sage3basic.h"

namespace ROSE { namespace proposed { namespace MFB {

void Environment::parseConfig(int argc, char ** argv) {
  assert(config.is_object());

  {
    // Parse command line
    char * cfgfile = NULL;
    char * basedir = NULL;
    char * resdir = NULL;
    for (size_t i = 1; i < argc; i++) {
      std::string arg(argv[i]);
      if (arg.find("--cfgfile=") == 0) {
        cfgfile = argv[i] + 10;
      } else if (arg.find("--basedir=") == 0) {
        basedir = argv[i] + 10;
      } else if (arg.find("--resdir=") == 0) {
        resdir = argv[i] + 9;
      } else {
        assert(false);
      }
    }

    // Load provided configuration file
    if (cfgfile != NULL) {
      std::cerr << "Loading configuration file." << std::endl;
      std::ifstream file(cfgfile);
      file >> config;
    }
    assert(config.is_object());

    // Potentially override the base directory (appended to all input path that are not absolute, i.e. original headers and sources)
    if (basedir != NULL) {
      if (config.find("basedir") != config.end()) {
        std::cerr << "Overriding base directory provided by the configuration file." << std::endl;
      }
      config["basedir"] = basedir;
    }

    // Potentially override the result directory (appended to all output path that are not absolute, i.e. graphs, analysis, generated code)
    if (resdir != NULL) {
      if (config.find("resdir") != config.end()) {
        std::cerr << "Overriding result directory provided by the configuration file." << std::endl;
      }
      config["resdir"] = resdir;
    }
  }
  
  // Check basedir
  assert(config.find("basedir") != config.end());
  assert(config["basedir"].is_string());
  std::string basedir = config["basedir"].get<std::string>();
  assert(basedir.size() > 0);
  assert(basedir[0] == '/');
  if (basedir[basedir.size()-1] != '/')
    config["basedir"] = basedir + "/";
  // TODO check that path exists
  
  // Check resdir
  assert(config.find("resdir") != config.end());
  assert(config["resdir"].is_string());
  std::string resdir = config["resdir"].get<std::string>();
  assert(resdir.size() > 0);
  assert(resdir[0] == '/');
  if (resdir[resdir.size()-1] != '/')
    config["resdir"] = resdir + "/";
  // TODO check that path exists
}

void Environment::configureRose(const json & rosecfg) {
  assert(rosecfg.is_object());
  assert(rosecfg.find("compiler") != rosecfg.end());
  assert(rosecfg["compiler"].is_string());
  assert(rosecfg.find("options") != rosecfg.end());
  assert(rosecfg["options"].is_array());

  std::vector<std::string> cmdline;

  cmdline.push_back(rosecfg["compiler"]);
  for (auto it = rosecfg["options"].begin(); it != rosecfg["options"].end(); it++) {
    assert(it->is_string());
    cmdline.push_back(it->get<std::string>());
  }

  // TODO add include dirs

  cmdline.push_back("-c");

  driver->project->set_originalCommandLineArgumentList(cmdline);
  driver->project->get_fileList().clear();
}

boost::filesystem::path Environment::resolvePath(boost::filesystem::path path) const {
  if (boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path))
    return path;

  for (auto it = config["incdirs"].begin(); it != config["incdirs"].end(); it++) {
    boost::filesystem::path tmp_path(it->get<std::string>());
    tmp_path += path;
    if (boost::filesystem::exists(tmp_path) && boost::filesystem::is_regular_file(tmp_path))
      return tmp_path;
  }

  assert(false);
}

void Environment::loadIncludeDirs() {
  if (config.find("incdirs") == config.end()) {
    config["incdirs"] = json::array({ config["basedir"].get<std::string>() });
  } else {
    assert(config["incdirs"].is_array());

    std::vector<std::string> incdirs;
    for (auto it = config["incdirs"].begin(); it != config["incdirs"].end(); it++) {
      assert(it->is_string());
      std::string path = it->get<std::string>();
      assert(path.size() > 0);
      if (path[0] != '/') {
        path = config["basedir"].get<std::string>() + path;
      }
      incdirs.push_back(path);
    }
    config["incdirs"] = incdirs;
  }
}

void Environment::loadSource(const json & source) {
  assert(source.is_object());
  assert(source.find("path") != source.end());

  boost::filesystem::path path = resolvePath(source["path"].get<std::string>());
  std::cerr << " -- path = " << path << std::endl;

  ::MFB::file_id_t fid = driver->add(path);
  std::cerr << "     -- fid = " << fid << std::endl;
}

Environment::Environment(int argc, char ** argv) :
  driver(new ::MFB::Driver<::MFB::Sage>(new SgProject())),
  config({})
{
  parseConfig(argc, argv);

  loadIncludeDirs();

  if (config.find("rosecfg") != config.end())
    configureRose(config["rosecfg"]);

  if (config.find("sources") != config.end()) {
    assert(config["sources"].is_array());
    for (nlohmann::json::iterator it = config["sources"].begin(); it != config["sources"].end(); ++it) {
      loadSource(*it);
    }
  }
}

Environment::~Environment() {
  delete driver->project;
  delete driver;
}

}}}


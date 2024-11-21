// C++ code calling an Ada function.

#include "sage3basic.h"
#include "rose_config.h"

#include "cmdline.h"
#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"
#include "processSupport.h"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "FileUtility.h"

#include "Libadalang_to_ROSE.h"

#include "libadalang.h"

namespace boostfs = boost::filesystem;
namespace scl     = Sawyer::CommandLine;
namespace sas     = Sawyer::Assert;


// minimal declarations from Libadalang_to_ROSE.h
namespace Libadalang_ROSE_Translation
{
  Sawyer::Message::Facility mlog;

  /// initialize translation settings
  void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings);
}

/// Function to search the input \ref dir for any ada sources files (files that end in .adb or .ads), and add them to \ref ada_files
void find_ada_files(std::string dir, std::vector<std::string>& ada_files){
  typedef boostfs::path::string_type string_type;
  boostfs::path dir_to_search {dir};
  for(auto& entry : boost::make_iterator_range(boostfs::directory_iterator(dir_to_search), {})){
    //Make sure this entry isn't a directory
    if(boostfs::is_regular_file(entry)){
      boostfs::path current_path {entry};
      //Check if this file ends with .adb or .ads
      if(current_path.extension().string() == ".adb" || current_path.extension().string() == ".ads"){
        ada_files.push_back(current_path.string());
      }
    }
  }
}

/// Checks for any compilation units required by the given \ref lal_root
///   If found, additional units are added to \ref comp_units, and the function is called recursively on the new units
///   Checks the prelude for ada_with_clause & the body for ada_subunit
void find_additional_compilation_units(ada_base_entity* lal_root, ada_analysis_context& ctx, std::vector<ada_base_entity>& comp_units_storage){
  using Libadalang_ROSE_Translation::mlog;

  std::vector<ada_analysis_unit> units_to_check;

  //Get the prelude, & check if it has a with clause
  ada_base_entity lal_prelude;
  ada_compilation_unit_f_prelude(lal_root, &lal_prelude);

  int num_preludes = ada_node_children_count(&lal_prelude);
  for(int i = 0; i < num_preludes; ++i){
    ada_base_entity lal_with_clause;
    if(ada_node_child(&lal_prelude, i, &lal_with_clause) != 0){
      //Make sure this is a with clause, & not a use clause or pragma
      if(ada_node_kind(&lal_with_clause) == ada_with_clause){
        //Get the name of the withed compilation unit
        ada_base_entity lal_package_list;
        ada_with_clause_f_packages(&lal_with_clause, &lal_package_list);
        int num_packages = ada_node_children_count(&lal_package_list);
        for(int j = 0; j < num_packages; ++j){
          ada_base_entity lal_package_name;
          if(ada_node_child(&lal_package_list, j, &lal_package_name) != 0){
            //The name might be composed of multiple nodes, so call getFullName
            std::string full_package_name = Libadalang_ROSE_Translation::getFullName(&lal_package_name);

            //Make an ada_text for the full name
            size_t full_package_name_length = full_package_name.size();
            uint32_t full_package_name_chars[full_package_name_length];
            for(int i = 0; i < full_package_name_length; i++){
              full_package_name_chars[i] = full_package_name.at(i);
            }

            ada_text package_name_text = { full_package_name_chars, full_package_name_length, true };

            //Get the withed unit (if this isn't a system include, get both .adb and .ads)
            if(full_package_name.rfind("ada.", 0) != 0 && full_package_name.rfind("gnat.", 0) != 0 && full_package_name.rfind("system.", 0) != 0){
              units_to_check.push_back(ada_get_analysis_unit_from_provider(ctx, &package_name_text, ADA_ANALYSIS_UNIT_KIND_UNIT_BODY, NULL, 0));
            }
            units_to_check.push_back(ada_get_analysis_unit_from_provider(ctx, &package_name_text, ADA_ANALYSIS_UNIT_KIND_UNIT_SPECIFICATION, NULL, 0));
          }
        }
      }
    }
  }

  //Check if this comp unit is a subunit
  ada_base_entity lal_body;
  ada_compilation_unit_f_body(lal_root, &lal_body);
  if(ada_node_kind(&lal_body) == ada_subunit){
    //Get the name of the parent compilation unit
    ada_base_entity lal_parent_name;
    ada_subunit_f_name(&lal_body, &lal_parent_name);

    //The name might be composed of multiple nodes, so call getFullName
    std::string full_parent_name = Libadalang_ROSE_Translation::getFullName(&lal_parent_name);

    //Make an ada_text for the full name
    size_t full_parent_name_length = full_parent_name.size();
    uint32_t full_parent_name_chars[full_parent_name_length];
    for(int i = 0; i < full_parent_name_length; i++){
      full_parent_name_chars[i] = full_parent_name.at(i);
    }

    ada_text parent_name_text = { full_parent_name_chars, full_parent_name_length, true };

    //Get the parent unit (we can't know whether it's .ads or .adb, so try both)
    units_to_check.push_back(ada_get_analysis_unit_from_provider(ctx, &parent_name_text, ADA_ANALYSIS_UNIT_KIND_UNIT_BODY, NULL, 0));
    units_to_check.push_back(ada_get_analysis_unit_from_provider(ctx, &parent_name_text, ADA_ANALYSIS_UNIT_KIND_UNIT_SPECIFICATION, NULL, 0));
  }

  //Look over the units_to_check, & add any valid ones to comp_units
  int new_units_start = comp_units_storage.size();

  for(int i = 0; i < units_to_check.size(); ++i){
    ada_analysis_unit unit_to_check = units_to_check.at(i);
    //If we got a non-null analysis unit, check if it has been seen before
    if(unit_to_check == nullptr){
      continue;
    }

    char* file_name_string = ada_unit_filename(unit_to_check);

    //First, check if we have any diagnostics
    unsigned int diagnostic_count = ada_unit_diagnostic_count(unit_to_check);
    if(diagnostic_count > 0){
      //Print the diagnostics, then go to the next unit
      mlog[Sawyer::Message::INFO] << "Got " << diagnostic_count << " diagnostic(s) while trying to init " << file_name_string << ":\n";
      free(file_name_string);
      for(unsigned int i = 0; i < diagnostic_count; i++){
        ada_diagnostic current_diagnostic;
        ada_unit_diagnostic(unit_to_check, i, &current_diagnostic);
        ada_source_location_range sloc = current_diagnostic.sloc_range;
        char* diagnostic_message = ada_text_to_locale_string(&(current_diagnostic.message));
        mlog[Sawyer::Message::INFO] << "  " << sloc.start.line << ":" << sloc.start.column << " .. " << sloc.end.line << ":" << sloc.end.column;
        mlog[Sawyer::Message::INFO] << " - " << diagnostic_message << std::endl;
        free(diagnostic_message);
      }
      continue;
    }

    ada_text_type file_name;
    //Get the file name this node is from
    ada_ada_node_full_sloc_image(lal_root, &file_name);
    std::string current_unit_name = Libadalang_ROSE_Translation::dot_ada_text_type_to_string(file_name);
    mlog[Sawyer::Message::INFO] << "From unit " << current_unit_name << ":\n";

    ada_base_entity lal_new_root;
    ada_unit_root(unit_to_check, &lal_new_root);
    if(!ada_node_is_null(&lal_new_root) && ada_node_kind(&lal_new_root) == ada_compilation_unit){
      mlog[Sawyer::Message::INFO] << "Adding " << file_name_string << std::endl;
      comp_units_storage.push_back(std::move(lal_new_root));
    }
    free(file_name_string);
  }

  int new_units_end = comp_units_storage.size();


  //Call find_additional_compilation_units on any newly added comp units
  for(int i = new_units_start; i < new_units_end; i++){
     find_additional_compilation_units(&comp_units_storage.at(i), ctx, comp_units_storage);
  }
}

int libadalang_main(const std::vector<std::string>& args, SgSourceFile* file)
{
     using Libadalang_ROSE_Translation::mlog;

     ROSE_INITIALIZE;

     int status = 0;

     mlog = Sawyer::Message::Facility("Ada2ROSE", Rose::Diagnostics::destination);

     Rose::Cmdline::Ada::CmdlineSettings settings = Rose::Cmdline::Ada::commandlineSettings();
     std::vector<std::string> unparsedArgs = args;

     std::string              ASISIncludeArgs;
     std::string              GNATArgs;
     std::string              ASISArgs;
     std::vector<std::string> includePaths;
     std::vector<std::string> includeFiles;

     for (unsigned int i=1; i < unparsedArgs.size(); ++i)
        {
       // most options appear as -<option>
       // have to process +w2 (warnings option) on some compilers so include +<option>
          if ( unparsedArgs[i].size() >= 2 && (unparsedArgs[i][0] == '-') && (unparsedArgs[i][1] == 'I') )
             {
               std::string includeDirectorySpecifier =  unparsedArgs[i].substr(2);
               includeDirectorySpecifier = Rose::StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
               includePaths.push_back(includeDirectorySpecifier);
             }
          else if (unparsedArgs[i].find("-gnat") != string::npos)
             {
                GNATArgs.append(unparsedArgs[i] + " ");
             }
        }

     for (vector<string>::const_iterator i = includePaths.begin(); i != includePaths.end(); ++i)
        {
          ASISIncludeArgs.append("-I" + *i + " ");
        }
//     if (includePaths.size() != 0)
//        ASISIncludeArgs = ASISIncludeArgs.substr(0, ASISIncludeArgs.length()-1);

     ASISArgs = GNATArgs + ASISIncludeArgs;

     if(!ASISArgs.empty() && ASISArgs.at(ASISArgs.length()-1) == ' ')
        ASISArgs = ASISArgs.substr(0, ASISArgs.length()-1);

     std::string warninglevels = "none, error, fatal";
     Sawyer::Message::Facilities logctrl;

     logctrl.insert(mlog);
     //~ logctrl.control("none, error, warn, fatal");

     if (settings.logWarn)  warninglevels += ", warn";
     if (settings.logTrace) warninglevels += ", trace";
     if (settings.logInfo)  warninglevels += ", info";

     logctrl.control(warninglevels);

     mlog[Sawyer::Message::TRACE] << "In ada_support.C: In libadalang_main(): calling ada support for file = "
                                  << file->getFileName()
                                  << std::endl;

     const char* gnat_home = std::getenv("GNAT_HOME");

     if (!gnat_home)
     {
       mlog[Sawyer::Message::FATAL] << "Environment variable GNAT_HOME is not set.\n"
                                    << "  Aborting ROSE.."
                                    << std::endl;

       return 1;
     }

     // set ROSE assertion behavior to throw, which seems more robust in the context of
     //   the Asis frontend.
     sas::AssertFailureHandler roseFailureHandler = Rose::failedAssertionBehavior();

     Rose::failedAssertionBehavior(Rose::throwOnFailedAssertion);

     mlog[Sawyer::Message::TRACE] << "BEGIN." << std::endl;

     ada_analysis_context ctx;
     std::vector<boostfs::path::string_type> source_files;
     std::vector<ada_analysis_unit> analysis_units;

     {
       typedef boostfs::path::string_type string_type;

    // DQ (9/15/2017): Updated to include output directory.
    // PP (10/31/20): Produce Ada temp+obj files in src-file specific directory.
       boostfs::path currentDir    = boostfs::current_path();
       string_type   srcFile       = file->getFileName();
       string_type   defaultConfig = currentDir.string<string_type>();

       // check if default configuation, gnat.adc, exists
       defaultConfig += boostfs::path::preferred_separator;
       defaultConfig += "gnat.adc";
       struct stat buffer;
       // inject gnatec only when gnat.adc is available, and gnatA switch is not used
       if (stat (defaultConfig.c_str(), &buffer) == 0 && GNATArgs.find("gnatA") == std::string::npos )
       {
         if(!ASISArgs.empty())
            ASISArgs += " ";
         ASISArgs += "-gnatec=";
         ASISArgs += defaultConfig;
       }

       size_t pos = srcFile.rfind(boostfs::path::preferred_separator);

       if (pos == string_type::npos) pos = 0;

       //Get the directory the input file is in, and search it for ada files
       std::string srcDir = srcFile.substr(0, pos);
       find_ada_files(srcDir, includeFiles);

       //Also grab the system include files
       //TODO this should get GNAT_HOME/lib/gcc/x86_64-pc-linux-gnu/10.3.1/rts-native/adainclude/ for GNAT 2021, but "x86_64-pc-linux-gnu" & "10.3.1" might change between installations
       boostfs::path systemIncludeDir = {std::string(gnat_home) + "/lib/gcc"};
       for(auto& subDir : boost::make_iterator_range(boostfs::directory_iterator(systemIncludeDir), {})){
         //If this is a directory, add it to the path and break
         if(boostfs::is_directory(subDir)){
           systemIncludeDir = subDir.path();
           break;
         }
       }

       for(auto& subDir : boost::make_iterator_range(boostfs::directory_iterator(systemIncludeDir), {})){
         //If this is a directory, add it to the path and break
         if(boostfs::is_directory(subDir)){
           systemIncludeDir = subDir.path();
           break;
         }
       }

       systemIncludeDir /= "rts-native/adainclude";
       if(boostfs::is_directory(systemIncludeDir)){
         find_ada_files(systemIncludeDir.string(), includeFiles);
       } else {
         mlog[Sawyer::Message::ERROR] << "Could not find system includes at: " << systemIncludeDir.string() << std::endl;
         mlog[Sawyer::Message::ERROR] << "Check that GNAT_HOME is set correctly\n";
       }

       //Also check for ada files on any include paths
       for(std::string includePath : includePaths){
         find_ada_files(includePath, includeFiles);
       }

       char* cstring_Args = const_cast<char*>(ASISArgs.c_str());

    // PP (11/5/20): Use Charles' new adapter_wrapper_with_flags function
       mlog[Sawyer::Message::TRACE] << "calling libadalang: src: " << srcFile
                                    << "\n gnat: " << gnat_home
                                    << "\n pdunit: " << settings.processPredefinedUnits
                                    << "\n implunit: " << settings.processImplementationUnits
                                    << "\n dbg: " << settings.asisDebug
                                    << std::endl;

       //Create a unit provider using all of the ada files we have found
       int num_input_files = includeFiles.size();
       mlog[Sawyer::Message::TRACE] << "Found " << num_input_files << " ada files.\n";
       const char* lal_input_files[num_input_files + 1];
       for(int i = 0; i < num_input_files; ++i){
         lal_input_files[i] = includeFiles.at(i).c_str();
       }
       lal_input_files[num_input_files] = nullptr;
       const char* lal_charset = nullptr; //TODO Charset defaults to ISO-8859-1, is this ok?
       ada_unit_provider lal_unit_provider = includeFiles.size() > 0 ? ada_create_auto_provider(lal_input_files, lal_charset) : nullptr;

       //TODO Figure out what all these settings mean
       ctx = ada_create_analysis_context(nullptr, nullptr, lal_unit_provider, 1, 8);
       if (ctx == nullptr){
           mlog[Sawyer::Message::FATAL] << "ada_create_analysis_context returned nullptr." << std::endl;
       }
       mlog[Sawyer::Message::INFO] << "Calling ada_get_analysis_unit_from_file on " << srcFile
                                   << std::endl;

       //Get the comp unit from the source file
       char* cstring_SrcFile = const_cast<char*>(srcFile.c_str());
       analysis_units.push_back(ada_get_analysis_unit_from_file(ctx, cstring_SrcFile, nullptr, 0, ada_default_grammar_rule));

       //Check if the source is a .adb file with a corresponding .ads
       size_t suffix_pos = srcFile.find(".adb");
       if(suffix_pos != string_type::npos){
         mlog[Sawyer::Message::INFO] << "Input file is a .adb\n";
         string_type srcFileStem = srcFile.substr(pos + 1, suffix_pos - pos - 1);
         mlog[Sawyer::Message::INFO] << "Attempting to add " << srcFileStem << ".ads" << std::endl;
         //Check if there is a corresponding .ads file
         uint32_t ads_name_chars[srcFileStem.size()];
         for(int i = 0; i < srcFileStem.size(); i++){
           //Handle the difference between how parent packages are prepended (i.e. package parent.child will be in file parent-child)
           char current_char = srcFileStem[i];
           if(current_char == '-'){
             ads_name_chars[i] = '.';
           } else {
             ads_name_chars[i] = current_char;
           }
         }

         //Make an ada_text for the ads name
         ada_text ads_name = { ads_name_chars, srcFileStem.size(), true };

         //Get the .ads (if it exists)
         analysis_units.push_back(ada_get_analysis_unit_from_provider(ctx, &ads_name, ADA_ANALYSIS_UNIT_KIND_UNIT_SPECIFICATION, NULL, 0));
       }

       //Check if we got any diagnostics for the units
       for(int i = 0; i < analysis_units.size(); ++i){
         unsigned int diagnostic_count = ada_unit_diagnostic_count(analysis_units.at(i));
         if(diagnostic_count > 0){
           //Print the diagnostics
           char* file_name_string = ada_unit_filename(analysis_units.at(i));
           mlog[Sawyer::Message::WARN] << "Got " << diagnostic_count << " diagnostic(s) while trying to init " << file_name_string << ":\n";
           free(file_name_string);
           for(unsigned int j = 0; j < diagnostic_count; j++){
             ada_diagnostic current_diagnostic;
             ada_unit_diagnostic(analysis_units.at(i), j, &current_diagnostic);
             ada_source_location_range sloc = current_diagnostic.sloc_range;
             char* diagnostic_message = ada_text_to_locale_string(&(current_diagnostic.message));
             mlog[Sawyer::Message::WARN] << "  " << sloc.start.line << ":" << sloc.start.column << " .. " << sloc.end.line << ":" << sloc.end.column;
             mlog[Sawyer::Message::WARN] << " - " << diagnostic_message << std::endl;
             free(diagnostic_message);
           }
           if(i == 0){
             mlog[Sawyer::Message::FATAL] << "Input file could not be converted to a compilation unit\n";
           }
           //Remove this unit from the vector
           //This would break if the vector was ever more than 2 elements
           analysis_units.erase(analysis_units.begin() + i);
         }
       }
     }

     mlog[Sawyer::Message::TRACE] << "END." << std::endl;

     //Create a set of vectors for all of the compilation units we've found
     int num_source_files = analysis_units.size();
     std::vector<ada_base_entity> root_storage(num_source_files);

     //Add the initial compilation units to the vectors, then recursively add any dependencies
     for(int i = 0; i < num_source_files; ++i){
       ada_unit_root(analysis_units.at(i), &root_storage.at(i));
     }

     for(int i = 0; i < num_source_files; ++i){
       find_additional_compilation_units(&root_storage.at(i), ctx, root_storage);
     }

     std::vector<ada_base_entity*> roots(root_storage.size());
     //Reset the roots to make sure they point to root_storage
     for(int i = 0; i < root_storage.size(); ++i){
       roots.at(i) = &root_storage.at(i);
     }

     try
     {
       Libadalang_ROSE_Translation::initialize(settings);
       Libadalang_ROSE_Translation::convertLibadalangToROSE(roots, file);
     }
     catch (const std::runtime_error& e)
     {
       mlog[Sawyer::Message::FATAL] << "caught runtime_error: " << typeid(e).name() << " " << e.what() << std::endl;
       status = 1;
     }
     catch (const std::logic_error& e)
     {
       mlog[Sawyer::Message::FATAL] << "caught logic_error: " << typeid(e).name() << " " << e.what() << std::endl;
       status = 1;
     }
     catch (...)
     {
       mlog[Sawyer::Message::FATAL] << "An unexpected exception terminated the ASIS to ROSE converter" << std::endl;
       status = 1;
     }

     //asis_adapterfinal();

     // restore ROSE assertion behavior
     Rose::failedAssertionBehavior(roseFailureHandler);
     mlog[Sawyer::Message::TRACE] << "Leaving libadalang_main(): status = " << status << std::endl;
     return status;
}



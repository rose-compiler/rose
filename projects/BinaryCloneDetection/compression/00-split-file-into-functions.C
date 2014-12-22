// Adds functions to the database.
#include "rose.h"
#include "DwarfLineMapper.h"
#include "BinaryFunctionCall.h"
#include "BinaryLoader.h"
#include "Partitioner.h"
#include "CloneDetectionLib.h"

#include <boost/lexical_cast.hpp>


std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] SPECIMEN\n"
              <<"  This command splits a binary into a file per function.\n"
              <<"\n"
              <<"    SPECIMENS\n"
              <<"            Zero or more binary specimen names.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): link(false), save_ast(false) {}
    bool link, save_ast;
};

static struct InstructionSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmInstruction(node)!=NULL; }
} iselector;

static struct DataSelector: SgAsmFunction::NodeSelector {
    virtual bool operator()(SgNode *node) { return isSgAsmStaticData(node)!=NULL; }
} dselector;


int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    int argno = 1;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);


    std::string specimen_name = StringUtility::getAbsolutePathFromRelativePath(argv[argno++], true);

    std::string specimen_path = StringUtility::getPathFromFileName(specimen_name);

    std::cout << "Specimen name is: " << specimen_name << std::endl;

    SgAsmInterpretation *interp = CloneDetection::open_specimen(specimen_name, argv0, false);
    SgBinaryComposite *binfile = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    assert(interp!=NULL && binfile!=NULL);
 
    // Figure out what functions we need to generate files from.
    std::vector<SgAsmFunction*> all_functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    std::cerr <<argv0 <<": " <<all_functions.size() <<" function" <<(1==all_functions.size()?"":"s") <<" found\n";

 
    for (std::vector<SgAsmFunction*>::iterator fi=all_functions.begin(); fi!=all_functions.end(); ++fi) {
        // Save function
        SgAsmFunction *func = *fi;

        std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(func);


        std::string function_name = func->get_name();

       if( function_name.size() == 0 || insns.size() < 100 ||  function_name.find("@plt") != std::string::npos  )
        {
          continue;
        }
        std::cout << "function name is: " << function_name << std::endl;

 
        {

          //std::string file_name = specimen_name+"_"+func->get_name()+"_"+boost::lexical_cast<std::string>(func->get_entry_va());
          std::string file_name = specimen_path + "/" + function_name;

          std::cout << "generating " << file_name << " from " << specimen_name << std::endl; 

          std::ofstream func_file;
          func_file.open(file_name.c_str());

          // Save instructions
          for (std::vector<SgAsmInstruction*>::iterator it = insns.begin(); it != insns.end(); ++it) {

            SgUnsignedCharList array = (*it)->get_raw_bytes();

            std::string str = "";
            for(size_t i=0; i < array.size(); ++i) {
              unsigned char c = array[i];

              str+= c;
            }
            func_file << str;
          }


          func_file.close();
        }

    }

    return 0;
}

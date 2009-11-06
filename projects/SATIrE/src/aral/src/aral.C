#include "aralir.h"
#include "aralparser_decls.h"

int main(int argc, char **argv)
{
    Analysis* root=Aral::Translator::frontEnd();
    if(root) std::cout << Aral::Translator::backEnd(root) << std::endl;
}

#include <aralir.h>

int main(int argc, char **argv)
{
	Aral::AralFile* root=Aral::Translator::frontEnd();
	if(root) std::cout << Aral::Translator::backEnd(root) << std::endl;
}

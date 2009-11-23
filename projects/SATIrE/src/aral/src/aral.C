#include <aralir.h>

int main()
{
	Aral::AralFile* root=Aral::Translator::frontEnd();
	std::cout << "BACKEND:" << std::endl;
	if(root) std::cout << Aral::Translator::backEnd(root) << std::endl;
	return 0;
}

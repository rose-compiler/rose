#include "common.h"

/* #####################################
   ######### F U N C T I O N S #########
   ##################################### */

string itostr(int num)
{
	stringstream converter;
	converter << num;
	return converter.str();
}

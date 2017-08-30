#include "../RefinementTypes.h"

unsigned int R_verify("__value < 100")
branchTest(unsigned int x)
{
	unsigned int z;
	if(x < 100) 
	{
		z = x;
	}
	else 
	{
		z = 50;
	}

	return z;
}
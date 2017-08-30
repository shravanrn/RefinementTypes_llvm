#include "../RefinementTypes.h"

// int gl = 3;

unsigned int R_verify("__value < 100")
branchTest2
(
	unsigned int x
)
{
	unsigned int z = 8;
	if(x < 50) 
	{
		z = x + 4;
		// gl = 4;
	}

	return z;
}
#include "../RefinementTypes.h"

unsigned int R_verify("__value < 100")
branchTest2(
	unsigned int x,
	unsigned int y R_assume("__value < 50")
)
{
	unsigned int z = 8;
	if(x < 50) 
	{
		z = x + 4;
	}
	else 
	{
		z = y + 8;
	}

	return z;
}
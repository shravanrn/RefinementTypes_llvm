#include "../RefinementTypes.h"

unsigned int R_verify("__value % 5 == 0")
pointerTest
(
	unsigned int a R_assume("__value < 1000")
	, unsigned int b R_assume("__value % 5 == 0 && __value < 1000")
)
{
	unsigned int *valToWrite;

	if(a == 5)
	{
		valToWrite = &b;
	}
	else
	{
		valToWrite = &a;
	}

	*valToWrite = 25;

	unsigned int ret = a + b;
	return ret;
}
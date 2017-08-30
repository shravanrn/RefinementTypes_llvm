#include "../RefinementTypes.h"

unsigned int R_verify("__value == 6")
pointerTest
(
	unsigned int a R_assume("__value == 5")
	, unsigned int b
)
{
	unsigned int *p;

	if(a == 5)
	{
		p = &a;
	}
	else
	{
		p = &b;
	}

	*p = 6;
	return a;
}
#include "../RefinementTypes.h"

unsigned int R_verify("__value == a")
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

	return *p;
}
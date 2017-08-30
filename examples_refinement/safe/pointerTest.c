#include "../RefinementTypes.h"

unsigned gl = 4;

unsigned int //R_verify("__value == a")
pointerTest
(
	unsigned int a //R_assume("__value % 10 == 0")
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
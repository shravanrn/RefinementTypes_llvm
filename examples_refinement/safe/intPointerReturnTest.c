#include "../RefinementTypes.h"

unsigned int* //R_verify("*__value == a || *__value == b")
pointerReturnTest
(
	unsigned int a , unsigned int b
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

	return p;
}
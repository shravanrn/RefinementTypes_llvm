#include "../RefinementTypes.h"

unsigned int R_verify("__value == a + 1")
pointerTest
(
	unsigned int a R_assume("__value < 1000")
)
{
	unsigned int *p = &a;
	a++;
	return *p;
}
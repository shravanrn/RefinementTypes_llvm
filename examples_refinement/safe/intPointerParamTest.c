#include "../RefinementTypes.h"

unsigned int R_verify("__value == 1 || __value != 1")
pointerParamTest
(
	unsigned int* p
)
{
	unsigned int r = *p;
	return r;
}
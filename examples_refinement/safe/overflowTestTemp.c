#include "../RefinementTypes.h"

unsigned int R_verify("__value > 0")
increment
(
	unsigned int a R_assume("__value != 4294967295")
)
{
	a = a + 1;
	return a;
}

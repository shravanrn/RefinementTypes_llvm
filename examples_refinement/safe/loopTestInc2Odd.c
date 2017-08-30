#include "../RefinementTypes.h"

int R_verify("__value == 31")
loopTest2
(
	int n R_assume("__value == 30")
)
{
	unsigned int i = 1;
	for(; i < n; i+=2) { }

	return i;
}
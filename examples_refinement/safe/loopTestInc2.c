#include "../RefinementTypes.h"

int R_verify("__value == 30")
loopTest2
(
	int n R_assume("__value == 30")
)
{
	unsigned int i = 0;
	for(; i < n; i+=2) { }

	return i;
}
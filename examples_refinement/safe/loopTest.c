#include "../RefinementTypes.h"

int R_verify("__value == 30")
loopTest
(
	int n R_assume("__value == 30")
)
{
	unsigned int i = 0;
	for(; i < n; i++) {}

	return i;
}
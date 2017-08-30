#include "../RefinementTypes.h"

int R_verify("__value == 45")
loopTest
(
	int n R_assume("__value == 10")
)
{
	unsigned int i = 0;
	unsigned ret = 0;

	while(i < n)
	{
		ret += i;
		i++;
	}

	return ret;
}
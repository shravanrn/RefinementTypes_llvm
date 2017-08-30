#include "../RefinementTypes.h"

unsigned int R_verify("__value >= 1 && __value <= 200")
factorial
(
	unsigned int n R_assume("__value < 5")
)
{
	if(n == 0) { return 1; }
	return n * factorial(n-1);
}
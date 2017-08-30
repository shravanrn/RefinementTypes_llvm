#include "../RefinementTypes.h"

unsigned int R_verify("__value == 1 || __value % 2 == 0")
twoRaisedToN
(
	unsigned int n R_assume("__value < 5")
)
{
	if(n == 0) { return 1; }
	return 2 * twoRaisedToN(n-1);
}

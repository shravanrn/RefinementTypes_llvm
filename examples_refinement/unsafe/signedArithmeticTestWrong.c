#include "../RefinementTypes.h"

unsigned int R_verify("__value < 200")
arithmeticSignedUnsigned
(
	unsigned int a R_assume("__value < 200")
)
{
	int x = a - 1;
	unsigned int y = x;
	return y;
}
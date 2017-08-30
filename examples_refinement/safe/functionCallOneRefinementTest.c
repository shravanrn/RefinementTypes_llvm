#include "../RefinementTypes.h"

unsigned int
foo
(
	unsigned int a
)
{
	unsigned int r = a + 1;
	return r;
}

unsigned int R_verify("__value >= 0")
bar
(
	unsigned int a R_assume("__value < 1000")
)
{
	unsigned int r = foo(a + 100);
	return r;
}

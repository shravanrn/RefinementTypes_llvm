#include "../RefinementTypes.h"

unsigned int //R_verify("__value == a + 1")
foo
(
	unsigned int a //R_verify("__value >= 100 && __value < 5000")
);

unsigned int //R_verify("__value > 100")
bar
(
	unsigned int b //R_assume("__value < 1000")
)
{
	unsigned int r = foo(b + 100);
	return r;
}
#include "../RefinementTypes.h"

unsigned int R_verify("__value > 0")
arithmeticSigned
(
	int s1 R_assume("__value > 0 && __value < 10"),
	int s2 R_assume("__value > 20 && __value < 30")
)
{
	unsigned int ss = s2 > s1;
	return ss;
}

unsigned int R_verify("__value > 0")
arithmeticUnsigned
(
	unsigned s1 R_assume("__value > 0 && __value < 10"),
	unsigned s2 R_assume("__value > 20 && __value < 30")
)
{
	unsigned int ss = s2 > s1;
	return ss;
}
#include "../RefinementTypes.h"

struct TestStruct
{
	unsigned int fieldA;
	unsigned int fieldB;
};

unsigned int //R_verify("__value <= 1000")
structTest
(
	unsigned int a //R_assume("__value < 1000")
)
{
	struct TestStruct field;
	field.fieldA = a;
	field.fieldA = a + 1;
	return field.fieldA;
}
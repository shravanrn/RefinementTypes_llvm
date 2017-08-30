#define R_assume(str) __attribute__ ((refinement_assume (str) ))
#define R_verify(str) __attribute__ ((refinement_verify (str) ))
#define R_qualifier(...) __attribute__ ((refinement_qualifier (__VA_ARGS__) ))
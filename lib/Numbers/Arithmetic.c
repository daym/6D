#include "6D/Values"
#include "6D/Arithmetic"
#include "Numbers/Integer2"
#include "Numbers/Real"
BEGIN_NAMESPACE_6D(Arithmetic)
static NodeT Splus;
static NodeT Sdash;
static NodeT Sstar;
static NodeT Sslash;
static NodeT addA(NodeT a, NodeT b) {
	printf("ADDING\n");
	return a;
}
DEFINE_STRICT_BINARY_FN(add, addA(env, argument))
static NodeT subtractA(NodeT a, NodeT b) {
	printf("SUBTRACTING\n");
	return a;
}
DEFINE_STRICT_BINARY_FN(subtract, subtractA(env, argument))
static NodeT multiplyA(NodeT a, NodeT b) {
	printf("MULTIPLYING\n");
	return a;
}
DEFINE_STRICT_BINARY_FN(multiply, multiplyA(env, argument))
static NodeT divideA(NodeT a, NodeT b) {
	printf("DIVIDING\n");
	return a;
}
DEFINE_STRICT_BINARY_FN(divide, divideA(env, argument))
void initArithmetic(void) {
	Splus = symbolFromStr("+");
	Sdash = symbolFromStr("-");
	Sstar = symbolFromStr("*");
	Sslash = symbolFromStr("/");
	initIntegers();
	initFloats();
	INIT_BINARY_FN(add)
	INIT_BINARY_FN(subtract)
	INIT_BINARY_FN(multiply)
	INIT_BINARY_FN(divide)
}
NODET withArithmetic(NODET body) {
	return close(Splus, add, 
	       close(Sdash, subtract,
	       close(Sstar, multiply,
	       close(Sslash, divide,
	       body))));
}
END_NAMESPACE_6D(Arithmetic)

#include "6D/Values"
#include "6D/Arithmetic"
#include "Numbers/Integer2"
#include "Numbers/Real"
BEGIN_NAMESPACE_6D(Arithmetic)
static NodeT Splus;
static NodeT Sdash;
static NodeT Sstar;
static NodeT Sslash;
static NodeT Sshl;
static NodeT Sshr;
/* TODO promotion */
static NodeT addA(NodeT a, NodeT b) {
	return integerAdd(a, b);
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
/* swapped arguments, sorry */
static NodeT shlA(NodeT amount, NodeT b) {
	/* FIXME */
	return integerShl(b, 1);
}
DEFINE_STRICT_BINARY_FN(shl, shlA(env, argument))
/* swapped arguments, sorry */
static NodeT shrA(NodeT amount, NodeT b) {
	NativeInt value;
	/* TODO clean up */
	if(!toNativeInt(amount, &value) || value < 0)
		return internNativeInt((NativeInt) 0);
	return integerShr(b, value);
}
DEFINE_STRICT_BINARY_FN(shr, shrA(env, argument))
void initArithmetic(void) {
	Splus = symbolFromStr("+");
	Sdash = symbolFromStr("-");
	Sstar = symbolFromStr("*");
	Sslash = symbolFromStr("/");
	Sshl = symbolFromStr("shl");
	Sshr = symbolFromStr("shr");
	initIntegers();
	initFloats();
	INIT_BINARY_FN(add)
	INIT_BINARY_FN(subtract)
	INIT_BINARY_FN(multiply)
	INIT_BINARY_FN(divide)
	INIT_BINARY_FN(shl)
	INIT_BINARY_FN(shr)
}
NODET withArithmetic(NODET body) {
	return close(Splus, add, 
	       close(Sdash, subtract,
	       close(Sstar, multiply,
	       close(Sslash, divide,
	       close(Sshl, shl,
	       close(Sshr, shr,
	       body))))));
}
END_NAMESPACE_6D(Arithmetic)

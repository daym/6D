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
static NodeT Slessequal;
static NodeT Sequal;
static NodeT int0;
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
	return integerMul(a, b);
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
static NodeT lePA(NodeT a, NodeT b) {
	return subtractA(a, b); /* TODO <= 0 */
}
DEFINE_STRICT_BINARY_FN(leP, lePA(env, argument))
static NodeT equalPA(NodeT a, NodeT b) {
	/* TODO make this less brittle */
	return internNativeBool(a == b || subtractA(a, b) == int0);
}
DEFINE_STRICT_BINARY_FN(equalP, equalPA(env, argument))
void initArithmetic(void) {
	Splus = symbolFromStr("+");
	Sdash = symbolFromStr("-");
	Sstar = symbolFromStr("*");
	Sslash = symbolFromStr("/");
	Sshl = symbolFromStr("shl");
	Sshr = symbolFromStr("shr");
	Slessequal = symbolFromStr("<=");
	Sequal = symbolFromStr("=");
	initIntegers();
	initFloats();
	int0 = internNativeUInt(0);
	INIT_BINARY_FN(add)
	INIT_BINARY_FN(subtract)
	INIT_BINARY_FN(multiply)
	INIT_BINARY_FN(divide)
	INIT_BINARY_FN(shl)
	INIT_BINARY_FN(shr)
	INIT_BINARY_FN(equalP)
	INIT_BINARY_FN(leP)
}
NODET withArithmetic(NODET body) {
	return closeOver(Splus, add, 
	       closeOver(Sdash, subtract,
	       closeOver(Sstar, multiply,
	       closeOver(Sslash, divide,
	       closeOver(Sshl, shl,
	       closeOver(Sshr, shr,
	       closeOver(Sequal, equalP,
	       closeOver(Slessequal, leP,
	       body))))))));
}
END_NAMESPACE_6D(Arithmetic)

#include "6D/Values"
#include "6D/Arithmetics"
#include "6D/Evaluators"
#include "Numbers/Integer2"
#include "Numbers/Real"
#include "Builtins/Builtins"
#include "Arithmetics/Arithmetics"
/* TODO: non-integer arithmetic, namely:
	rational arithmetic.
	float arithmetic.
	complex arithmetic.
*/
BEGIN_NAMESPACE_6D(Arithmetics)
static NodeT int0;
static NodeT Splus;
static NodeT Sdash;
static NodeT Sstar;
static NodeT Sslash;
static NodeT Sdivrem;
static NodeT Sshl;
static NodeT Sshr;
static NodeT Slessequal;
static NodeT Sequal;
static NodeT Sstarstar;
/* TODO promotion */
NodeT add(NodeT a, NodeT b) {
	return integerAdd(a, b);
}
NodeT subtract(NodeT a, NodeT b) {
	return integerSub(a, b);
}
NodeT multiply(NodeT a, NodeT b) {
	return integerMul(a, b);
}
NodeT divide(NodeT a, NodeT b) {
	return integerDiv(a, b);
}
NodeT divrem(NodeT a, NodeT b) {
	return integerDivrem(a, b);
}
NodeT power(NodeT a, NodeT b) {
	return integerPow(a, b);
}
/* swapped arguments, sorry */
NodeT shl(NodeT amount, NodeT b) {
	/* FIXME */
	return integerShl(b, 1);
}
/* swapped arguments, sorry */
NodeT shr(NodeT amount, NodeT b) {
	NativeInt value;
	/* TODO clean up */
	if(!toNativeInt(amount, &value) || value < 0)
		return internNativeInt((NativeInt) 0);
	return integerShr(b, value);
}
NodeT leP(NodeT a, NodeT b) {
	return internNativeBool(integerCompare(a, b) <= 0);
}
NodeT equalP(NodeT a, NodeT b) {
	/* TODO make this less brittle */
	return internNativeBool(a == b || subtract(a, b) == int0);
}
static NodeT builtins;
void initArithmetics(void) {
	builtins = initBuiltins();
	Splus = symbolFromStr("+");
	Sdash = symbolFromStr("-");
	Sstar = symbolFromStr("*");
	Sslash = symbolFromStr("/");
	Sdivrem = symbolFromStr("divrem");
	Sshl = symbolFromStr("shl");
	Sshr = symbolFromStr("shr");
	Slessequal = symbolFromStr("<=");
	Sequal = symbolFromStr("=");
	Sstarstar = symbolFromStr("**");
	initIntegers();
	initFloats();
	int0 = internNativeUInt((NativeUInt) 0U);
}
static NodeT builtin(NodeT sym) {
	return dcall(builtins, sym);
}
NODET withArithmetics(NODET body) {
	return closeOver(Splus, builtin(symbolFromStr("add")), 
	       closeOver(Sdash, builtin(symbolFromStr("subtract")), 
	       closeOver(Sstar, builtin(symbolFromStr("multiply")), 
	       closeOver(Sslash, builtin(symbolFromStr("divide")), 
	       closeOver(Sdivrem, builtin(symbolFromStr("divrem")),
	       closeOver(Sstarstar, builtin(symbolFromStr("power")),
	       closeOver(Sshl, builtin(symbolFromStr("shl")),
	       closeOver(Sshr, builtin(symbolFromStr("shr")), 
	       closeOver(Slessequal, builtin(symbolFromStr("le?")),
	       closeOver(Sequal, builtin(symbolFromStr("equal?")),
	       body))))))))));
}
END_NAMESPACE_6D(Arithmetics)
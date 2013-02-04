#include "6D/Values"
#include "6D/Arithmetic"
#include "Numbers/Integer2"
#include "Numbers/Real"
#include "Builtins/Builtins"
#include "Numbers/Arithmetic"
BEGIN_NAMESPACE_6D(Arithmetic)
static NodeT int0;
/* TODO promotion */
NodeT add(NodeT a, NodeT b) {
	return integerAdd(a, b);
}
NodeT subtract(NodeT a, NodeT b) {
	printf("SUBTRACTING\n");
	return a;
}
NodeT multiply(NodeT a, NodeT b) {
	return integerMul(a, b);
}
NodeT divide(NodeT a, NodeT b) {
	return integerDiv(a, b);
}
NodeT divmod(NodeT a, NodeT b) {
	return integerDivmod(a, b);
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
	return subtract(a, b); /* TODO <= 0 */
}
NodeT equalP(NodeT a, NodeT b) {
	/* TODO make this less brittle */
	return internNativeBool(a == b || subtract(a, b) == int0);
}
void initArithmetic(void) {
	initIntegers();
	int0 = internNativeUInt((NativeUInt) 0U);
}
END_NAMESPACE_6D(Arithmetic)

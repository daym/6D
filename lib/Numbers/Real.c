#include <string.h>
#include "Numbers/Real"
#include "6D/FFIs"
#include "Numbers/Integer2"
#include "Values/Values"

BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Values)
NodeT internNativeFloat(NativeFloat value) {
	struct Float* result = NEW(Float);
	result->value = value;
	return refCXXInstance(result);
}
bool toNativeFloat(NodeT node, NativeFloat* result) {
	*result = 0.0;
	//node = evaluate(node);
	if(floatP(node)) {
		const struct Float* floatNode = (const struct Float*) getCXXInstance(node);
		*result = floatNode->value;
		return(true);
	} else {
		// only coerce integers to float if there is no information loss
		NativeInt value = 0;
		if(!toNativeInt(node, &value))
			return(false);
		*result = (NativeFloat) value;
		return((NativeInt) *result == value);
	}
}
END_NAMESPACE_6D(FFIs)
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Values)
bool floatP(NodeT node) {
	return tagOfNode(node) == TAG_Float;
}
REGISTER_STR(Float, {
	std::stringstream sst;
	sst.precision(std::numeric_limits<NativeFloat>::digits10 + 1);
	sst << node->value;
	std::string v = sst.str();
	const char* vc = v.c_str();
	if(*vc == '-')
		++vc;
	if(strpbrk(vc, ".eE") == NULL && (vc[0] >= '0' && vc[0] <= '9')) { /* not a special value like inf or nan */
		sst << ".0";
		v = sst.str();
	}
	fprintf(destination, "%s", v.c_str());
})

DEFINE_STRICT_FN(FloatP, internNativeBool(floatP(argument)))

REGISTER_BUILTIN(FloatP, 1, 0, symbolFromStr("float?"))

static struct Float nanFloat;
static struct Float infinityFloat;
static NodeT nanFloatN;
static NodeT infinityFloatN;

#ifdef _MSC_VER
double nanxx(void) {
	uint32_t nan[2]={0xffffffff, 0x7fffffff};
	double g = *( double* )nan;
	return g;
}
double infxx(void) {
	uint32_t inf[2]={0x7FF00000, 0};
	double g = *( double* )inf;
	return g;
}
#else
double nanxx(void) {
	return __builtin_nan(""); /* 0.0/0.0 */
}
double infxx(void) {
	return __builtin_inf(); /* 1.0/0.0 */
}
#endif
void initFloats(void) {
	Node_initTag((struct Node*) &nanFloat, TAG_Float);
	nanFloat.value = nanxx();
	Node_initTag((struct Node*) &infinityFloat, TAG_Float);
	infinityFloat.value = infxx();
	nanFloatN = refCXXInstance(&nanFloat);
	infinityFloatN = refCXXInstance(&infinityFloat);
}
NodeT nanA(void) {
	return(nanFloatN);
}
NodeT infinityA(void) {
	return(infinityFloatN);
}
#define ARITH(op) \
	if(!floatP(aP)) \
		return evalError(strC("<float>"), strC("<junk>"), aP); \
	if(!floatP(bP)) \
		return evalError(strC("<float>"), strC("<junk>"), bP); \
	{ \
		const struct Float* a = (const struct Float*) getCXXInstance(aP); \
		const struct Float* b = (const struct Float*) getCXXInstance(bP); \
		return internNativeFloat(a->value op b->value); \
	}

NODET floatAdd(NODET aP, NODET bP) {
	ARITH(+)
}
NODET floatSub(NODET aP, NODET bP) {
	ARITH(-)
}
NODET floatMul(NODET aP, NODET bP) {
	ARITH(*)
}
NODET floatDiv(NODET aP, NODET bP) {
	ARITH(/)
}
NODET floatDivrem(NODET aP, NODET bP) {
	if(!floatP(aP))
		return evalError(strC("<float>"), strC("<junk>"), aP);
	if(!floatP(bP))
		return evalError(strC("<float>"), strC("<junk>"), bP);
	{
		const struct Float* a = (const struct Float*) getCXXInstance(aP);
		const struct Float* b = (const struct Float*) getCXXInstance(bP);
		NativeInt quot = (NativeInt) (a->value/b->value); /* FIXME bigger integers? */
		NativeFloat rem = a->value - quot*b->value;
		return pair(internNativeInt(quot), internNativeFloat(rem));
	}
}
END_NAMESPACE_6D(Values)

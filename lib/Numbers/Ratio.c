#include <stdlib.h>
#include <assert.h>
#include "Values/Values"
#include "Numbers/Ratio"
#include "6D/FFIs"
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
NODET ratio(NodeT aa, NodeT bb) {
	struct Ratio* result = NEW(Ratio);
	result->a = aa;
	result->b = bb;
	return(refCXXInstance(result));
}
bool ratioP(NodeT n) {
	return(tagOfNode(n) == TAG_Ratio);
}
NodeT ratioA(NodeT n) {
	const struct Ratio* r = (const struct Ratio*) getCXXInstance(n);
	return r->a;
}
NodeT ratioB(NodeT n) {
	const struct Ratio* r = (const struct Ratio*) getCXXInstance(n);
	return r->b;
}

DEFINE_STRICT_FN(RatioP, internNativeBool(ratioP(argument)))
DEFINE_STRICT_FN(RatioNumeratorGetter, ratioA(argument))
DEFINE_STRICT_FN(RatioDenominatorGetter, ratioB(argument))

/*
a/b + c/d = (a*d + c*b)/(b*d)
a/b - c/d = (a*d - c*b)/(b*d)
a/b ⋅ c/d = a*c/(b*d)       
a/b / c/d = a/b ⋅ d/c = a*d/(b*c)
*/
REGISTER_BUILTIN(RatioMaker, 2, 0, symbolFromStr("makeRatio"))
REGISTER_BUILTIN(RatioP, 1, 0, symbolFromStr("ratio?"))
REGISTER_BUILTIN(RatioNumeratorGetter, 1, 0, symbolFromStr("ratioNum"))
REGISTER_BUILTIN(RatioDenominatorGetter, 1, 0, symbolFromStr("ratioDenom"))
END_NAMESPACE_6D(Values)

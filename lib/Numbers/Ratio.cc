#include <stdlib.h>
#include <stdexcept>
#include <assert.h>
#include "Values/Values"
#include "Numbers/Ratio"
#include "6D/FFIs"
BEGIN_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(FFIs)
Ratio* ratio(NodeT aa, NodeT bb) {
	Ratio* result = new Ratio;
	result->a = aa;
	result->b = bb;
	return(result);
}
bool ratioP(NodeT n) {
	return(tagOfNode(n) == TAG_RATIO);
}
static inline NodeT ensureRatio(NodeT node) {
	if(!ratioP(node))
		throw std::invalid_argument("argument is not a Ratio");
	return(node);
}
NodeT getRatioA(NodeT n) {
	const Ratio* r = (const Ratio*) getCXXInstance(n);
	return r->a;
}
NodeT getRatioB(NodeT n) {
	const Ratio* r = (const Ratio*) getCXXInstance(n);
	return r->b;
}

DEFINE_STRICT_FN(RatioP, ratioP(argument))
DEFINE_STRICT_FN(RatioNumeratorGetter, getRatioA(ensureRatio(argument)))
DEFINE_STRICT_FN(RatioDenominatorGetter, getRatioB(ensureRatio(argument)))

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

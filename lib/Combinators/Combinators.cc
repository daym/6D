#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"

BEGIN_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
NodeT Identity;
NodeT Konstant;
/* TODO Substitution */
void initCombinators(void) {
	NodeT a = symbolFromStr("a");
	NodeT b = symbolFromStr("b");
	Identity = annotate(nil, fn(a, a));
	Konstant = annotate(nil, fn(a, fn(b, a)));
}

END_NAMESPACE_6D(Combinators)

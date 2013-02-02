#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"

BEGIN_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
Values::NodeT Identity;
Values::NodeT Konstant;
/* TODO Substitution */
void initCombinators(void) {
	Values::NodeT a = symbolFromStr("a");
	Values::NodeT b = symbolFromStr("b");
	Identity = annotate(nil, fn(a, a));
	Konstant = annotate(nil, fn(a, fn(b, a)));
}

END_NAMESPACE_6D(Combinators)

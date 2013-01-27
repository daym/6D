#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"

namespace Combinators {
using namespace Values;
using namespace Evaluators;
Values::NodeT Identity;
Values::NodeT Konstant;
/* TODO Substitution */
void initCombinators(void) {
	Values::NodeT a = symbolFromStr("a");
	Values::NodeT b = symbolFromStr("b");
	Identity = annotate(nil, fn(a, a));
	Konstant = annotate(nil, fn(a, fn(b, a)));
}

}

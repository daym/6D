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
	Identity = annotate(fn(a, a));
	Konstant = annotate(fn(a, fn(b, a)));
}

}

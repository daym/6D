#include "6D/Values"
#include "6D/Operations"
#include "6D/FFIs"
#include "Logic/Logic"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"

namespace Logic {
using namespace Values;
using namespace SpecialForms;
using namespace Combinators;
using namespace FFIs;
NodeT trueN;
NodeT falseN;
static NodeT t;
static NodeT f;
static NodeT a;
static NodeT b;
static NodeT wrapL(NodeT n) {
	return n;
}
DEFINE_STRICT_FN(Conjunction, argument == f ? SpecialForms::constanter2(f) : Identity)
DEFINE_STRICT_FN(Disjunction, argument != f ? SpecialForms::constanter2(uargument) : Identity)
DEFINE_STRICT_FN(Negation, argument == f ? t : f)
void initLogic(void) {
	t = symbolFromStr("t");
	f = nil; // TODO symbolFromStr("f");
	a = symbolFromStr("a");
	b = symbolFromStr("b");
	trueN = wrapL(t);
	falseN = wrapL(f);
}

}
namespace FFIs {
using namespace Logic;
NodeT internNative(bool value) {
	return value ? t : f;
}
}

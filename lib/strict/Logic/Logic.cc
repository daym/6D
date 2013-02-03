#include "6D/Values"
#include "6D/Operations"
#include "6D/FFIs"
#include "Logic/Logic"
#include "SpecialForms/SpecialForms"
#include "Combinators/Combinators"

BEGIN_NAMESPACE_6D(Logic)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Combinators)
USE_NAMESPACE_6D(FFIs)
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

END_NAMESPACE_6D(Logic)
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Logic)
NodeT internNative(bool value) {
	return value ? t : f;
}
bool booleanFromNode(NodeT n) {
	return(n != f);
}
END_NAMESPACE_6D(FFIs)

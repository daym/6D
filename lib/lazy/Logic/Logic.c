#include "6D/Values"
#include "6D/Operations"
#include "6D/Evaluators"
#include "Logic/Logic"

BEGIN_NAMESPACE_6D(Logic)
USE_NAMESPACE_6D(Values)
USE_NAMESPACE_6D(Evaluators)
NodeT trueN;
NodeT falseN;
NodeT Disjunction;
NodeT Conjunction;
NodeT Negation;
static NodeT t;
static NodeT f;
static NodeT wrapL(NodeT n) {
	return fn(t, fn(f, n));
}
void initLogic(void) {
	t = symbolFromStr("t");
	f = symbolFromStr("f");
	true = wrapL(t);
	false = wrapL(f);
	Disjunction = annotate(fn(a, fn(b, wrapL(call2(a, t, call2(b, t, f)))))); // \a \b \t \f a t (b t f)
	Conjunction = annotate(fn(a, fn(b, wrapL(call2(a, call2(b, t, f), f))))); // \a \b \t \f a (b t f) f
	Negation = annotate(fn(a, wrapL(call2(a, f, t)))); // \a \t \f a f t
}

END_NAMESPACE_6D(Logic)
BEGIN_NAMESPACE_6D(FFIs)
USE_NAMESPACE_6D(Logic)
NodeT internNativeBool(bool value) {
        return value ? t : f;   
}
END_NAMESPACE_6D(FFIs)

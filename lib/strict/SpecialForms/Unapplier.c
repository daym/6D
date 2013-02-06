#include "6D/Values"
#include "6D/Evaluators"
#include "SpecialForms/Unapplier"
#include "Numbers/Integer2"
BEGIN_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Values)
static NodeT unapplyN(NodeT accum, NodeT argument) {
	NodeT count = getConsHead(accum);
	NodeT t1 = getConsTail(accum);
	NodeT fn = getConsHead(t1);
	NodeT t2 = getConsTail(t1);
	if(integerCompareU(count, 0) <= 0)
		return dcall(fn, t2); /* order is reversed */
	else {
		return CLOSED_FN(Unapplier3, cons(integerSubU(count, 1U), cons(fn, cons(argument, t2))));
	}
}
static NodeT unapply(NodeT count, NodeT fn) {
	return CLOSED_FN(Unapplier3, cons(count, cons(fn, nil)));
}
DEFINE_SPECIAL_FORM(Unapplier3, unapplyN(env, argument))
DEFINE_SPECIAL_FORM(Unapplier2, unapply(env, argument))
DEFINE_SPECIAL_FORM(Unapplier, CLOSED_FN(Unapplier2, argument))
END_NAMESPACE_6D(SpecialForms)

/* unapply 4 fn a b c d => fn [a b c d] */

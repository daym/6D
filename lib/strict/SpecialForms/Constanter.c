#include "6D/Values"
#include "SpecialForms/Constanter"
BEGIN_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Values)
DEFINE_SPECIAL_FORM(Constanter2, env)
NodeT constanter2(NodeT argument) {
	return CLOSED_FN(Constanter2, argument); // call(CLOSED_FN(), argument);
}
/* mostly for debugging */
DEFINE_SPECIAL_FORM(Constanter, constanter2(argument))

END_NAMESPACE_6D(SpecialForms)

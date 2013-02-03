#include "6D/Values"
#include "SpecialForms/Identer"
BEGIN_NAMESPACE_6D(SpecialForms)
USE_NAMESPACE_6D(Values)
DEFINE_SPECIAL_FORM(Identer2, env)
NodeT identer2(NodeT argument) {
	return CLOSED_FFI_FN(Identer2, argument); // call(CLOSED_FFI_FN(), argument);
}
/* mostly for debugging */
DEFINE_SPECIAL_FORM(Identer, identer2(argument))

END_NAMESPACE_6D(SpecialForms)

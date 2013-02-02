#include "6D/Values"
#include "SpecialForms/Identer"
BEGIN_NAMESPACE_6D(SpecialForms)

DEFINE_SPECIAL_FORM(Identer2, env)
Values::NodeT identer2(Values::NodeT argument) {
	return CLOSED_FFI_FN(Identer2, argument); // call(CLOSED_FFI_FN(), argument);
}
/* mostly for debugging */
DEFINE_SPECIAL_FORM(Identer, identer2(argument))

END_NAMESPACE_6D(SpecialForms)

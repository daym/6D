#include "6D/Values"
#include "SpecialForms/Constanter"
BEGIN_NAMESPACE_6D(SpecialForms)

DEFINE_SPECIAL_FORM(Constanter2, env)
Values::NodeT constanter2(Values::NodeT argument) {
	return CLOSED_FFI_FN(Constanter2, argument); // call(CLOSED_FFI_FN(), argument);
}
/* mostly for debugging */
DEFINE_SPECIAL_FORM(Constanter, constanter2(argument))

END_NAMESPACE_6D(SpecialForms)

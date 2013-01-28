#include <stdio.h>
#include "6D/Builtins"
#include "6D/Values"
#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
namespace Builtins {
using namespace Values;
using namespace FFIs;
static NodeT SConstanter;
static NodeT builtin(NodeT node) {
	if(node == SConstanter)
		return SpecialForms::Constanter;
	else
		return nil; // FIXME error
}
DEFINE_STRICT_FN(Builtins, builtin(argument))
NodeT initBuiltins(void) {
	SConstanter = symbolFromStr("Constanter");
	return Builtins;
}
};

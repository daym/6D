#include <stdio.h>
#include "6D/Builtins"
#include "6D/Values"
#include "6D/FFIs"
#include "SpecialForms/SpecialForms"
#include "Modulesystem/Memoizer"
#include "6D/Modulesystem"
namespace Builtins {
using namespace Values;
using namespace FFIs;
static NodeT SConstanter;
static NodeT Srem;
static NodeT Sexports;
static NodeT Smemoize;
static NodeT builtin(NodeT node) {
	//Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer);
	if(node == SConstanter)
		return SpecialForms::Constanter;
	else if(node == Srem)
		return SpecialForms::Identer;
	else if(node == Smemoize)
		return Modulesystem::Memoizer;
	else if(node == Sexports)
		return cons(SConstanter, cons(Srem, cons(Smemoize, nil)));
	else
		return nil; // FIXME error
}
// TODO Modulesystem::memoize
//DEFINE_STRICT_FN(Builtins, dispatch(Modulesystem::exportsQ("Constanter rem memoize", SpecialForms::Constanter, SpecialForms::Identer, Modulesystem::Memoizer)))
NodeT initBuiltins(void) {
	SConstanter = symbolFromStr("Constanter");
	Srem = symbolFromStr("rem");
	return Builtins;
}
};
